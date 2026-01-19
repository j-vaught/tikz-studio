#include "polygonitem.h"
#include "polygon.h"
#include "common.h"

#include <QPen>
#include <QBrush>

PolygonItem::PolygonItem(Polygon *polygon, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , m_polygon(polygon)
{
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);

    updateFromModel();

    QObject::connect(polygon, &Polygon::changed, [this]() {
        updateFromModel();
    });
}

void PolygonItem::updateFromModel() {
    if (!m_polygon) return;

    // Use the polygon's painterPath which handles rounded corners
    setPath(m_polygon->painterPath());

    // Stroke
    QPen pen(m_polygon->strokeColor());
    pen.setWidthF(m_polygon->lineWidth() * 1.33);
    setPen(pen);

    // Fill
    QColor fillColor = m_polygon->fillColor();
    fillColor.setAlphaF(m_polygon->opacity());
    setBrush(QBrush(fillColor));
}
