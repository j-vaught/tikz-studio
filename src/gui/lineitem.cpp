#include "lineitem.h"
#include "line.h"
#include "common.h"

#include <QPen>

LineItem::LineItem(Line *line, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_line(line)
{
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);

    updateFromModel();

    QObject::connect(line, &Line::changed, [this]() {
        updateFromModel();
    });
}

void LineItem::updateFromModel() {
    if (!m_line) return;

    QPointF start = toScreen(m_line->startPos());
    QPointF end = toScreen(m_line->endPos());
    setLine(QLineF(start, end));

    QPen pen(m_line->color());
    pen.setWidthF(m_line->lineWidth() * 1.33);  // pt to px

    if (m_line->isDashed()) {
        pen.setStyle(Qt::DashLine);
    } else {
        pen.setStyle(Qt::SolidLine);
    }

    setPen(pen);
}
