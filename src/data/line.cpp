#include "line.h"
#include "point.h"
#include "common.h"

Line::Line(QObject *parent)
    : QObject(parent)
    , m_color(UofSC::Black())
{
}

Line::Line(Point *start, Point *end, QObject *parent)
    : QObject(parent)
    , m_startPoint(start)
    , m_endPoint(end)
    , m_color(UofSC::Black())
{
    if (start) connect(start, &Point::changed, this, &Line::changed);
    if (end) connect(end, &Point::changed, this, &Line::changed);
}

Line::Line(QPointF start, QPointF end, QObject *parent)
    : QObject(parent)
    , m_startPos(start)
    , m_endPos(end)
    , m_color(UofSC::Black())
{
}

void Line::setStartPoint(Point *p) {
    if (m_startPoint != p) {
        if (m_startPoint) disconnect(m_startPoint, &Point::changed, this, &Line::changed);
        m_startPoint = p;
        if (p) connect(p, &Point::changed, this, &Line::changed);
        emit changed();
    }
}

void Line::setEndPoint(Point *p) {
    if (m_endPoint != p) {
        if (m_endPoint) disconnect(m_endPoint, &Point::changed, this, &Line::changed);
        m_endPoint = p;
        if (p) connect(p, &Point::changed, this, &Line::changed);
        emit changed();
    }
}

QPointF Line::startPos() const {
    return m_startPoint ? m_startPoint->pos() : m_startPos;
}

QPointF Line::endPos() const {
    return m_endPoint ? m_endPoint->pos() : m_endPos;
}

void Line::setStartPos(QPointF pos) {
    if (!m_startPoint && m_startPos != pos) {
        m_startPos = pos;
        emit changed();
    }
}

void Line::setEndPos(QPointF pos) {
    if (!m_endPoint && m_endPos != pos) {
        m_endPos = pos;
        emit changed();
    }
}

void Line::setColor(const QColor &color) {
    if (m_color != color) {
        m_color = color;
        emit changed();
    }
}

void Line::setLineWidth(float width) {
    if (m_lineWidth != width) {
        m_lineWidth = width;
        emit changed();
    }
}

void Line::setLineStyle(LineStyle style) {
    if (m_lineStyle != style) {
        m_lineStyle = style;
        emit changed();
    }
}

void Line::setLineCap(LineCap cap) {
    if (m_lineCap != cap) {
        m_lineCap = cap;
        emit changed();
    }
}

void Line::setLineJoin(LineJoin join) {
    if (m_lineJoin != join) {
        m_lineJoin = join;
        emit changed();
    }
}

void Line::setCornerRadius(float radius) {
    if (m_cornerRadius != radius) {
        m_cornerRadius = radius;
        emit changed();
    }
}

void Line::setRotation(float degrees) {
    if (m_rotation != degrees) {
        m_rotation = degrees;
        emit changed();
    }
}

void Line::setScale(float scale) {
    if (m_scale != scale) {
        m_scale = qBound(0.1f, scale, 10.0f);
        emit changed();
    }
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

static QString lineStyleToTikz(LineStyle style) {
    switch (style) {
        case LineStyle::Dashed: return "dashed";
        case LineStyle::DenselyDashed: return "densely dashed";
        case LineStyle::LooselyDashed: return "loosely dashed";
        case LineStyle::Dotted: return "dotted";
        case LineStyle::DenselyDotted: return "densely dotted";
        case LineStyle::LooselyDotted: return "loosely dotted";
        case LineStyle::DashDot: return "dashdotted";
        case LineStyle::DenselyDashDot: return "densely dashdotted";
        case LineStyle::LooselyDashDot: return "loosely dashdotted";
        case LineStyle::DashDotDot: return "dashdotdotted";
        case LineStyle::DenselyDashDotDot: return "densely dashdotdotted";
        case LineStyle::LooselyDashDotDot: return "loosely dashdotdotted";
        default: return QString();
    }
}

static QString lineCapToTikz(LineCap cap) {
    switch (cap) {
        case LineCap::Round: return "line cap=round";
        case LineCap::Square: return "line cap=rect";
        default: return QString();
    }
}

static QString lineJoinToTikz(LineJoin join) {
    switch (join) {
        case LineJoin::Round: return "line join=round";
        case LineJoin::Bevel: return "line join=bevel";
        default: return QString();
    }
}

QString Line::tikz() const {
    QStringList opts;

    // Color
    if (m_color != UofSC::Black()) {
        opts << colorToTikz(m_color);
    }

    // Line width
    if (m_lineWidth != 0.8f) {
        opts << QString("line width=%1pt").arg(m_lineWidth, 0, 'f', 1);
    }

    // Line style
    QString lineStyleStr = lineStyleToTikz(m_lineStyle);
    if (!lineStyleStr.isEmpty()) {
        opts << lineStyleStr;
    }

    // Line cap
    QString lineCapStr = lineCapToTikz(m_lineCap);
    if (!lineCapStr.isEmpty()) {
        opts << lineCapStr;
    }

    // Line join
    QString lineJoinStr = lineJoinToTikz(m_lineJoin);
    if (!lineJoinStr.isEmpty()) {
        opts << lineJoinStr;
    }

    // Rounded corners
    if (m_cornerRadius > 0) {
        opts << QString("rounded corners=%1").arg(m_cornerRadius, 0, 'f', 2);
    }

    // Rotation
    if (m_rotation != 0.0f) {
        opts << QString("rotate=%1").arg(m_rotation, 0, 'f', 1);
    }

    // Scale
    if (m_scale != 1.0f) {
        opts << QString("scale=%1").arg(m_scale, 0, 'f', 2);
    }

    QString optStr = opts.isEmpty() ? "" : QString("[%1]").arg(opts.join(", "));

    QPointF s = startPos();
    QPointF e = endPos();

    return QString("\\draw%1 (%2,%3) -- (%4,%5);")
        .arg(optStr)
        .arg(s.x(), 0, 'f', 3)
        .arg(s.y(), 0, 'f', 3)
        .arg(e.x(), 0, 'f', 3)
        .arg(e.y(), 0, 'f', 3);
}
