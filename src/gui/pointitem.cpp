#include "pointitem.h"
#include "point.h"
#include "common.h"

#include <QCursor>
#include <QPen>
#include <QBrush>

PointItem::PointItem(Point *point, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , m_point(point)
{
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setCursor(QCursor(Qt::CrossCursor));

    updateFromModel();

    QObject::connect(point, &Point::changed, [this]() {
        updateFromModel();
    });
}

void PointItem::updateFromModel() {
    if (!m_point) return;

    // Position in screen coordinates
    QPointF screenPos = toScreen(m_point->pos());
    setPos(screenPos);

    // Size based on radius (convert pt to pixels, roughly 1.33 px/pt)
    float radiusPx = m_point->radius() * 1.33f * 2;  // Double for visibility
    setRect(-radiusPx, -radiusPx, radiusPx * 2, radiusPx * 2);

    // Appearance
    QColor color = m_point->color();
    setPen(QPen(color, 1.5));
    setBrush(QBrush(color));

    setVisible(m_point->isVisible());
}

void PointItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = m_point->pos();
    }
    QGraphicsEllipseItem::mousePressEvent(event);
}

void PointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void PointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_dragging = false;
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

QVariant PointItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && m_point) {
        // Snap to grid and update model
        QPointF newScreenPos = value.toPointF();
        QPointF tikzPos = fromScreen(newScreenPos);
        tikzPos = snapToGrid(tikzPos, GRID_MINOR);

        // Block signals to avoid feedback loop
        m_point->blockSignals(true);
        m_point->setPos(tikzPos);
        m_point->blockSignals(false);

        // Return snapped position
        return toScreen(tikzPos);
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}
