#include "point.h"
#include "common.h"

Point::Point(QPointF pos, QObject *parent)
    : QObject(parent)
    , m_pos(pos)
    , m_color(UofSC::Black())
{
}

void Point::setPos(QPointF pos) {
    if (m_pos != pos) {
        m_pos = pos;
        emit changed();
    }
}

void Point::setColor(const QColor &color) {
    if (m_color != color) {
        m_color = color;
        emit changed();
    }
}

void Point::setRadius(float radius) {
    if (m_radius != radius) {
        m_radius = radius;
        emit changed();
    }
}

void Point::setVisible(bool visible) {
    if (m_visible != visible) {
        m_visible = visible;
        emit changed();
    }
}

QString Point::tikz() const {
    if (!m_visible) return QString();

    QString colorStr;
    if (m_color == UofSC::Black()) {
        colorStr = "black";
    } else if (m_color == UofSC::Garnet()) {
        colorStr = "garnet";
    } else {
        colorStr = QString("{rgb,255:red,%1;green,%2;blue,%3}")
            .arg(m_color.red()).arg(m_color.green()).arg(m_color.blue());
    }

    return QString("\\fill[%1] (%2,%3) circle (%4pt);")
        .arg(colorStr)
        .arg(m_pos.x(), 0, 'f', 3)
        .arg(m_pos.y(), 0, 'f', 3)
        .arg(m_radius, 0, 'f', 1);
}
