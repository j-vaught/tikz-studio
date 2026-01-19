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

void Line::setDashed(bool dashed) {
    if (m_dashed != dashed) {
        m_dashed = dashed;
        emit changed();
    }
}

void Line::setCornerRadius(float radius) {
    if (m_cornerRadius != radius) {
        m_cornerRadius = radius;
        emit changed();
    }
}

QString Line::tikz() const {
    QStringList opts;

    // Color
    if (m_color == UofSC::Black()) {
        // Default, no need to specify
    } else if (m_color == UofSC::Garnet()) {
        opts << "garnet";
    } else {
        opts << QString("color={rgb,255:red,%1;green,%2;blue,%3}")
            .arg(m_color.red()).arg(m_color.green()).arg(m_color.blue());
    }

    // Line width
    if (m_lineWidth != 0.8f) {
        opts << QString("line width=%1pt").arg(m_lineWidth, 0, 'f', 1);
    }

    // Dashed
    if (m_dashed) {
        opts << "dashed";
    }

    // Rounded corners
    if (m_cornerRadius > 0) {
        opts << QString("rounded corners=%1").arg(m_cornerRadius, 0, 'f', 2);
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
