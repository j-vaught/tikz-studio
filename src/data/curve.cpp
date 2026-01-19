#include "curve.h"
#include "point.h"
#include "common.h"

Curve::Curve(QObject *parent)
    : QObject(parent)
    , m_color(UofSC::Black())
{
}

QVector<QPointF> Curve::controlPoints() const {
    QVector<QPointF> result;
    for (int i = 0; i < m_pointRefs.size(); i++) {
        if (m_isRef[i] && m_pointRefs[i]) {
            result.append(m_pointRefs[i]->pos());
        } else {
            result.append(m_ownedPoints[i]);
        }
    }
    return result;
}

void Curve::addControlPoint(QPointF pos) {
    m_pointRefs.append(nullptr);
    m_ownedPoints.append(pos);
    m_isRef.append(false);
    emit changed();
}

void Curve::addControlPoint(Point *point) {
    m_pointRefs.append(point);
    m_ownedPoints.append(point ? point->pos() : QPointF());
    m_isRef.append(true);
    if (point) {
        connect(point, &Point::changed, this, &Curve::onPointChanged);
    }
    emit changed();
}

void Curve::insertControlPoint(int index, QPointF pos) {
    if (index < 0 || index > m_pointRefs.size()) return;
    m_pointRefs.insert(index, nullptr);
    m_ownedPoints.insert(index, pos);
    m_isRef.insert(index, false);
    emit changed();
}

void Curve::removeControlPoint(int index) {
    if (index < 0 || index >= m_pointRefs.size()) return;
    if (m_isRef[index] && m_pointRefs[index]) {
        disconnect(m_pointRefs[index], &Point::changed, this, &Curve::onPointChanged);
    }
    m_pointRefs.remove(index);
    m_ownedPoints.remove(index);
    m_isRef.remove(index);
    emit changed();
}

void Curve::setControlPoint(int index, QPointF pos) {
    if (index < 0 || index >= m_pointRefs.size()) return;
    if (!m_isRef[index]) {
        m_ownedPoints[index] = pos;
        emit changed();
    }
}

int Curve::controlPointCount() const {
    return m_pointRefs.size();
}

void Curve::clear() {
    for (int i = 0; i < m_pointRefs.size(); i++) {
        if (m_isRef[i] && m_pointRefs[i]) {
            disconnect(m_pointRefs[i], &Point::changed, this, &Curve::onPointChanged);
        }
    }
    m_pointRefs.clear();
    m_ownedPoints.clear();
    m_isRef.clear();
    emit changed();
}

void Curve::setCurveType(CurveType type) {
    if (m_curveType != type) {
        m_curveType = type;
        emit changed();
    }
}

void Curve::setTension(float tension) {
    if (m_tension != tension) {
        m_tension = tension;
        emit changed();
    }
}

void Curve::setColor(const QColor &color) {
    if (m_color != color) {
        m_color = color;
        emit changed();
    }
}

void Curve::setLineWidth(float width) {
    if (m_lineWidth != width) {
        m_lineWidth = width;
        emit changed();
    }
}

void Curve::setDashed(bool dashed) {
    if (m_dashed != dashed) {
        m_dashed = dashed;
        emit changed();
    }
}

void Curve::setClosed(bool closed) {
    if (m_closed != closed) {
        m_closed = closed;
        emit changed();
    }
}

void Curve::onPointChanged() {
    emit changed();
}

// Convert Catmull-Rom segment to Bezier control points
QPointF Curve::catmullRomToBezier(int segment, int controlIndex) const {
    QVector<QPointF> pts = controlPoints();
    int n = pts.size();
    if (n < 2) return QPointF();

    // Get 4 points for Catmull-Rom: p0, p1, p2, p3
    // Segment goes from p1 to p2
    QPointF p0, p1, p2, p3;

    p1 = pts[segment];
    p2 = pts[(segment + 1) % n];

    if (m_closed) {
        p0 = pts[(segment - 1 + n) % n];
        p3 = pts[(segment + 2) % n];
    } else {
        p0 = (segment > 0) ? pts[segment - 1] : pts[segment];
        p3 = (segment < n - 2) ? pts[segment + 2] : pts[segment + 1];
    }

    // Convert to cubic Bezier control points
    float t = m_tension;
    switch (controlIndex) {
    case 0: return p1;
    case 1: return p1 + (p2 - p0) * t / 3.0;
    case 2: return p2 - (p3 - p1) * t / 3.0;
    case 3: return p2;
    default: return QPointF();
    }
}

QPainterPath Curve::painterPath() const {
    QPainterPath path;
    QVector<QPointF> pts = controlPoints();

    if (pts.size() < 2) return path;

    path.moveTo(toScreen(pts[0]));

    if (m_curveType == Straight) {
        for (int i = 1; i < pts.size(); i++) {
            path.lineTo(toScreen(pts[i]));
        }
        if (m_closed) path.closeSubpath();
    }
    else if (m_curveType == Smooth) {
        int segments = m_closed ? pts.size() : pts.size() - 1;
        for (int i = 0; i < segments; i++) {
            QPointF c1 = catmullRomToBezier(i, 1);
            QPointF c2 = catmullRomToBezier(i, 2);
            QPointF end = catmullRomToBezier(i, 3);
            path.cubicTo(toScreen(c1), toScreen(c2), toScreen(end));
        }
    }
    else if (m_curveType == Bezier && pts.size() >= 4) {
        // Interpret as explicit bezier: start, ctrl1, ctrl2, end, ...
        for (int i = 0; i + 3 < pts.size(); i += 3) {
            path.cubicTo(toScreen(pts[i+1]), toScreen(pts[i+2]), toScreen(pts[i+3]));
        }
    }

    return path;
}

static QString colorToTikz(const QColor &color) {
    if (color == UofSC::Black()) return "black";
    if (color == UofSC::White()) return "white";
    if (color == UofSC::Garnet()) return "garnet";
    if (color == UofSC::Rose()) return "rose";
    if (color == UofSC::Atlantic()) return "atlantic";
    if (color == UofSC::Congaree()) return "congaree";
    if (color == UofSC::Horseshoe()) return "horseshoe";
    if (color == UofSC::Grass()) return "grass";
    if (color == UofSC::Honeycomb()) return "honeycomb";

    return QString("{rgb,255:red,%1;green,%2;blue,%3}")
        .arg(color.red()).arg(color.green()).arg(color.blue());
}

QString Curve::tikz() const {
    QVector<QPointF> pts = controlPoints();
    if (pts.size() < 2) return QString();

    QStringList opts;

    // Color
    if (m_color != UofSC::Black()) {
        opts << colorToTikz(m_color);
    }

    // Line width
    if (m_lineWidth != 0.8f) {
        opts << QString("line width=%1pt").arg(m_lineWidth, 0, 'f', 1);
    }

    // Dashed
    if (m_dashed) {
        opts << "dashed";
    }

    QString optStr = opts.isEmpty() ? "" : QString("[%1]").arg(opts.join(", "));

    if (m_curveType == Smooth) {
        // Use plot [smooth] for Catmull-Rom style
        QString coords;
        for (const QPointF &p : pts) {
            coords += QString("(%1,%2) ")
                .arg(p.x(), 0, 'f', 3)
                .arg(p.y(), 0, 'f', 3);
        }

        QString cycleStr = m_closed ? " -- cycle" : "";
        return QString("\\draw%1 plot [smooth%2, tension=%3] coordinates {%4}%5;")
            .arg(optStr)
            .arg(m_closed ? ", cycle" : "")
            .arg(m_tension, 0, 'f', 2)
            .arg(coords.trimmed())
            .arg(m_closed ? "" : "");
    }
    else if (m_curveType == Bezier && pts.size() >= 4) {
        // Explicit bezier curves
        QString pathStr = QString("(%1,%2)")
            .arg(pts[0].x(), 0, 'f', 3)
            .arg(pts[0].y(), 0, 'f', 3);

        for (int i = 0; i + 3 < pts.size(); i += 3) {
            pathStr += QString(" .. controls (%1,%2) and (%3,%4) .. (%5,%6)")
                .arg(pts[i+1].x(), 0, 'f', 3).arg(pts[i+1].y(), 0, 'f', 3)
                .arg(pts[i+2].x(), 0, 'f', 3).arg(pts[i+2].y(), 0, 'f', 3)
                .arg(pts[i+3].x(), 0, 'f', 3).arg(pts[i+3].y(), 0, 'f', 3);
        }

        if (m_closed) pathStr += " -- cycle";

        return QString("\\draw%1 %2;").arg(optStr, pathStr);
    }
    else {
        // Straight line segments
        QString pathStr;
        for (int i = 0; i < pts.size(); i++) {
            if (i > 0) pathStr += " -- ";
            pathStr += QString("(%1,%2)")
                .arg(pts[i].x(), 0, 'f', 3)
                .arg(pts[i].y(), 0, 'f', 3);
        }
        if (m_closed) pathStr += " -- cycle";

        return QString("\\draw%1 %2;").arg(optStr, pathStr);
    }
}
