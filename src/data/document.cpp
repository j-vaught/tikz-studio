#include "document.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"
#include "common.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Document::Document(QObject *parent)
    : QObject(parent)
{
}

Document::~Document() {
    clear();
}

Point *Document::addPoint(QPointF pos) {
    Point *p = new Point(pos, this);
    p->setName(freshPointName());
    m_points.append(p);
    connect(p, &Point::changed, this, &Document::changed);
    emit changed();
    return p;
}

void Document::removePoint(Point *point) {
    int idx = m_points.indexOf(point);
    if (idx >= 0) {
        m_points.remove(idx);
        point->deleteLater();
        emit changed();
    }
}

Line *Document::addLine(QPointF start, QPointF end) {
    Line *l = new Line(start, end, this);
    m_lines.append(l);
    connect(l, &Line::changed, this, &Document::changed);
    emit changed();
    return l;
}

Line *Document::addLine(Point *start, Point *end) {
    Line *l = new Line(start, end, this);
    m_lines.append(l);
    connect(l, &Line::changed, this, &Document::changed);
    emit changed();
    return l;
}

void Document::removeLine(Line *line) {
    int idx = m_lines.indexOf(line);
    if (idx >= 0) {
        m_lines.remove(idx);
        line->deleteLater();
        emit changed();
    }
}

Polygon *Document::addPolygon() {
    Polygon *p = new Polygon(this);
    m_polygons.append(p);
    connect(p, &Polygon::changed, this, &Document::changed);
    emit changed();
    return p;
}

void Document::removePolygon(Polygon *polygon) {
    int idx = m_polygons.indexOf(polygon);
    if (idx >= 0) {
        m_polygons.remove(idx);
        polygon->deleteLater();
        emit changed();
    }
}

Curve *Document::addCurve() {
    Curve *c = new Curve(this);
    m_curves.append(c);
    connect(c, &Curve::changed, this, &Document::changed);
    emit changed();
    return c;
}

void Document::removeCurve(Curve *curve) {
    int idx = m_curves.indexOf(curve);
    if (idx >= 0) {
        m_curves.remove(idx);
        curve->deleteLater();
        emit changed();
    }
}

Ellipse *Document::addEllipse() {
    Ellipse *e = new Ellipse(this);
    m_ellipses.append(e);
    connect(e, &Ellipse::changed, this, &Document::changed);
    emit changed();
    return e;
}

Ellipse *Document::addCircle(QPointF center, float radius) {
    Ellipse *e = addEllipse();
    e->setCenter(center);
    e->setRadius(radius);
    return e;
}

Ellipse *Document::addEllipse(QPointF center, float rx, float ry) {
    Ellipse *e = addEllipse();
    e->setCenter(center);
    e->setRadiusX(rx);
    e->setRadiusY(ry);
    return e;
}

void Document::removeEllipse(Ellipse *ellipse) {
    int idx = m_ellipses.indexOf(ellipse);
    if (idx >= 0) {
        m_ellipses.remove(idx);
        ellipse->deleteLater();
        emit changed();
    }
}

void Document::clear() {
    for (Point *p : m_points) p->deleteLater();
    for (Line *l : m_lines) l->deleteLater();
    for (Polygon *p : m_polygons) p->deleteLater();
    for (Curve *c : m_curves) c->deleteLater();
    for (Ellipse *e : m_ellipses) e->deleteLater();

    m_points.clear();
    m_lines.clear();
    m_polygons.clear();
    m_curves.clear();
    m_ellipses.clear();
    m_pointCounter = 0;
    m_backgroundImage.clear();

    emit changed();
}

void Document::setBackgroundImage(const QString &path) {
    if (m_backgroundImage != path) {
        m_backgroundImage = path;
        emit backgroundImageChanged();
    }
}

void Document::setBackgroundOpacity(float opacity) {
    m_backgroundOpacity = qBound(0.0f, opacity, 1.0f);
    emit backgroundImageChanged();
}

QString Document::freshPointName() {
    return QString("p%1").arg(m_pointCounter++);
}

QString Document::tikzPreamble() const {
    return R"(\documentclass[tikz,border=5pt]{standalone}
\usepackage{tikz}
\usetikzlibrary{patterns}

% UofSC Color definitions
\definecolor{garnet}{RGB}{115,0,10}
\definecolor{rose}{RGB}{204,46,64}
\definecolor{atlantic}{RGB}{70,106,159}
\definecolor{congaree}{RGB}{31,65,77}
\definecolor{horseshoe}{RGB}{101,120,11}
\definecolor{grass}{RGB}{206,211,24}
\definecolor{honeycomb}{RGB}{164,145,55}
\definecolor{darkgarnet}{RGB}{87,0,8}
\definecolor{azalea}{RGB}{132,66,71}

\begin{document}
)";
}

QString Document::tikz() const {
    QString result;
    QTextStream out(&result);

    out << "\\begin{tikzpicture}\n";

    // Background image (if any)
    if (!m_backgroundImage.isEmpty()) {
        out << "  % Background image\n";
        out << "  \\node[anchor=south west, inner sep=0] at (0,0) "
            << "{\\includegraphics{" << m_backgroundImage << "}};\n\n";
    }

    // Ellipses (draw first so they're behind)
    if (!m_ellipses.isEmpty()) {
        out << "  % Ellipses and Circles\n";
        for (const Ellipse *e : m_ellipses) {
            QString tikzStr = e->tikz();
            if (!tikzStr.isEmpty()) {
                out << "  " << tikzStr << "\n";
            }
        }
        out << "\n";
    }

    // Polygons
    if (!m_polygons.isEmpty()) {
        out << "  % Polygons\n";
        for (const Polygon *p : m_polygons) {
            QString tikzStr = p->tikz();
            if (!tikzStr.isEmpty()) {
                out << "  " << tikzStr << "\n";
            }
        }
        out << "\n";
    }

    // Lines
    if (!m_lines.isEmpty()) {
        out << "  % Lines\n";
        for (const Line *l : m_lines) {
            QString tikzStr = l->tikz();
            if (!tikzStr.isEmpty()) {
                out << "  " << tikzStr << "\n";
            }
        }
        out << "\n";
    }

    // Curves
    if (!m_curves.isEmpty()) {
        out << "  % Curves\n";
        for (const Curve *c : m_curves) {
            QString tikzStr = c->tikz();
            if (!tikzStr.isEmpty()) {
                out << "  " << tikzStr << "\n";
            }
        }
        out << "\n";
    }

    // Points (draw last so they're on top)
    if (!m_points.isEmpty()) {
        out << "  % Points\n";
        for (const Point *p : m_points) {
            QString tikzStr = p->tikz();
            if (!tikzStr.isEmpty()) {
                out << "  " << tikzStr << "\n";
            }
        }
    }

    out << "\\end{tikzpicture}\n";

    return result;
}

QString Document::tikzFull() const {
    return tikzPreamble() + tikz() + "\\end{document}\n";
}

bool Document::save(const QString &path) {
    QJsonObject root;

    // Points
    QJsonArray pointsArr;
    for (const Point *p : m_points) {
        QJsonObject obj;
        obj["name"] = p->name();
        obj["x"] = p->pos().x();
        obj["y"] = p->pos().y();
        obj["color"] = p->color().name();
        obj["radius"] = p->radius();
        obj["visible"] = p->isVisible();
        pointsArr.append(obj);
    }
    root["points"] = pointsArr;

    // Lines
    QJsonArray linesArr;
    for (const Line *l : m_lines) {
        QJsonObject obj;
        obj["startX"] = l->startPos().x();
        obj["startY"] = l->startPos().y();
        obj["endX"] = l->endPos().x();
        obj["endY"] = l->endPos().y();
        obj["color"] = l->color().name();
        obj["lineWidth"] = l->lineWidth();
        obj["dashed"] = l->isDashed();
        obj["cornerRadius"] = l->cornerRadius();
        linesArr.append(obj);
    }
    root["lines"] = linesArr;

    // Polygons
    QJsonArray polysArr;
    for (const Polygon *p : m_polygons) {
        QJsonObject obj;
        QJsonArray verts;
        for (const Vertex &v : p->vertices()) {
            QJsonObject vobj;
            vobj["x"] = v.pos.x();
            vobj["y"] = v.pos.y();
            vobj["cornerRadius"] = v.cornerRadius;
            verts.append(vobj);
        }
        obj["vertices"] = verts;
        obj["fillColor"] = p->fillColor().name();
        obj["strokeColor"] = p->strokeColor().name();
        obj["lineWidth"] = p->lineWidth();
        obj["opacity"] = p->opacity();
        obj["defaultCornerRadius"] = p->defaultCornerRadius();
        polysArr.append(obj);
    }
    root["polygons"] = polysArr;

    // Curves
    QJsonArray curvesArr;
    for (const Curve *c : m_curves) {
        QJsonObject obj;
        QJsonArray pts;
        for (const QPointF &pt : c->controlPoints()) {
            QJsonObject v;
            v["x"] = pt.x();
            v["y"] = pt.y();
            pts.append(v);
        }
        obj["controlPoints"] = pts;
        obj["curveType"] = static_cast<int>(c->curveType());
        obj["tension"] = c->tension();
        obj["color"] = c->color().name();
        obj["lineWidth"] = c->lineWidth();
        obj["dashed"] = c->isDashed();
        obj["closed"] = c->isClosed();
        curvesArr.append(obj);
    }
    root["curves"] = curvesArr;

    // Ellipses
    QJsonArray ellipsesArr;
    for (const Ellipse *e : m_ellipses) {
        QJsonObject obj;
        obj["centerX"] = e->center().x();
        obj["centerY"] = e->center().y();
        obj["radiusX"] = e->radiusX();
        obj["radiusY"] = e->radiusY();
        obj["rotation"] = e->rotation();
        obj["fillColor"] = e->fillColor().name();
        obj["strokeColor"] = e->strokeColor().name();
        obj["lineWidth"] = e->lineWidth();
        obj["opacity"] = e->opacity();
        ellipsesArr.append(obj);
    }
    root["ellipses"] = ellipsesArr;

    root["backgroundImage"] = m_backgroundImage;
    root["backgroundOpacity"] = m_backgroundOpacity;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QJsonDocument doc(root);
    file.write(doc.toJson());
    return true;
}

bool Document::load(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) return false;

    clear();

    QJsonObject root = doc.object();

    // Points
    for (const QJsonValue &v : root["points"].toArray()) {
        QJsonObject obj = v.toObject();
        Point *p = addPoint(QPointF(obj["x"].toDouble(), obj["y"].toDouble()));
        p->setName(obj["name"].toString());
        p->setColor(QColor(obj["color"].toString()));
        p->setRadius(obj["radius"].toDouble(2.0));
        p->setVisible(obj["visible"].toBool(true));
    }

    // Lines
    for (const QJsonValue &v : root["lines"].toArray()) {
        QJsonObject obj = v.toObject();
        Line *l = addLine(
            QPointF(obj["startX"].toDouble(), obj["startY"].toDouble()),
            QPointF(obj["endX"].toDouble(), obj["endY"].toDouble())
        );
        l->setColor(QColor(obj["color"].toString()));
        l->setLineWidth(obj["lineWidth"].toDouble(0.8));
        l->setDashed(obj["dashed"].toBool());
        l->setCornerRadius(obj["cornerRadius"].toDouble());
    }

    // Polygons
    for (const QJsonValue &v : root["polygons"].toArray()) {
        QJsonObject obj = v.toObject();
        Polygon *p = addPolygon();
        for (const QJsonValue &vv : obj["vertices"].toArray()) {
            QJsonObject pt = vv.toObject();
            p->addVertex(QPointF(pt["x"].toDouble(), pt["y"].toDouble()),
                        pt["cornerRadius"].toDouble(0.0));
        }
        p->setFillColor(QColor(obj["fillColor"].toString()));
        p->setStrokeColor(QColor(obj["strokeColor"].toString()));
        p->setLineWidth(obj["lineWidth"].toDouble(0.8));
        p->setOpacity(obj["opacity"].toDouble(1.0));
        p->setDefaultCornerRadius(obj["defaultCornerRadius"].toDouble(0.0));
    }

    // Curves
    for (const QJsonValue &v : root["curves"].toArray()) {
        QJsonObject obj = v.toObject();
        Curve *c = addCurve();
        for (const QJsonValue &vv : obj["controlPoints"].toArray()) {
            QJsonObject pt = vv.toObject();
            c->addControlPoint(QPointF(pt["x"].toDouble(), pt["y"].toDouble()));
        }
        c->setCurveType(static_cast<Curve::CurveType>(obj["curveType"].toInt()));
        c->setTension(obj["tension"].toDouble(0.5));
        c->setColor(QColor(obj["color"].toString()));
        c->setLineWidth(obj["lineWidth"].toDouble(0.8));
        c->setDashed(obj["dashed"].toBool());
        c->setClosed(obj["closed"].toBool());
    }

    // Ellipses
    for (const QJsonValue &v : root["ellipses"].toArray()) {
        QJsonObject obj = v.toObject();
        Ellipse *e = addEllipse();
        e->setCenter(QPointF(obj["centerX"].toDouble(), obj["centerY"].toDouble()));
        e->setRadiusX(obj["radiusX"].toDouble(1.0));
        e->setRadiusY(obj["radiusY"].toDouble(1.0));
        e->setRotation(obj["rotation"].toDouble(0.0));
        e->setFillColor(QColor(obj["fillColor"].toString()));
        e->setStrokeColor(QColor(obj["strokeColor"].toString()));
        e->setLineWidth(obj["lineWidth"].toDouble(0.8));
        e->setOpacity(obj["opacity"].toDouble(1.0));
    }

    m_backgroundImage = root["backgroundImage"].toString();
    m_backgroundOpacity = root["backgroundOpacity"].toDouble(0.5);
    if (!m_backgroundImage.isEmpty()) {
        emit backgroundImageChanged();
    }

    return true;
}
