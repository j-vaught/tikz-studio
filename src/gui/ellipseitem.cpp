#include "ellipseitem.h"
#include "ellipse.h"
#include "common.h"

#include <QPen>
#include <QBrush>

EllipseItem::EllipseItem(Ellipse *ellipse, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , m_ellipse(ellipse)
{
    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptHoverEvents(true);

    updateFromModel();

    QObject::connect(ellipse, &Ellipse::changed, [this]() {
        updateFromModel();
    });
}

void EllipseItem::updateFromModel() {
    if (!m_ellipse) return;

    QPointF center = toScreen(m_ellipse->center());
    float rx = toScreen(m_ellipse->radiusX());
    float ry = toScreen(m_ellipse->radiusY());

    setRect(-rx, -ry, rx * 2, ry * 2);
    setPos(center);
    setRotation(m_ellipse->rotation());

    // Stroke
    QPen pen(m_ellipse->strokeColor());
    pen.setWidthF(m_ellipse->lineWidth() * 1.33);
    setPen(pen);

    // Fill
    QColor fillColor = m_ellipse->fillColor();
    fillColor.setAlphaF(m_ellipse->opacity());
    setBrush(QBrush(fillColor));
}
