#include "groupitem.h"
#include "group.h"
#include "common.h"

#include <QPainter>
#include <QPen>

GroupItem::GroupItem(Group *group, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent)
    , m_group(group)
{
    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptHoverEvents(true);

    updateFromModel();

    QObject::connect(group, &Group::changed, [this]() {
        updateFromModel();
    });
}

GroupItem::~GroupItem() {
}

void GroupItem::updateFromModel() {
    if (!m_group) return;
    // The group's bounding box is calculated from its member shapes
    // Child items are managed by Canvas when grouping/ungrouping
    update();
}

void GroupItem::setShowBounds(bool show) {
    m_showBounds = show;
    update();
}

void GroupItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget) {
    // Call base implementation to paint children
    QGraphicsItemGroup::paint(painter, option, widget);

    // Draw bounding box when selected or showing bounds
    if (isSelected() || m_showBounds) {
        QPen pen(UofSC::Atlantic());
        pen.setStyle(Qt::DashLine);
        pen.setWidthF(1.0);
        pen.setCosmetic(true);  // Don't scale with zoom
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        QRectF bounds = childrenBoundingRect();
        painter->drawRect(bounds);
    }
}
