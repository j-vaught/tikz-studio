#include "curveitem.h"
#include "curve.h"
#include "common.h"

#include <QPen>
#include <QBrush>

CurveItem::CurveItem(Curve *curve, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , m_curve(curve)
{
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);

    updateFromModel();

    QObject::connect(curve, &Curve::changed, [this]() {
        updateFromModel();
    });
}

void CurveItem::updateFromModel() {
    if (!m_curve) return;

    setPath(m_curve->painterPath());

    QPen pen(m_curve->color());
    pen.setWidthF(m_curve->lineWidth() * 1.33);

    if (m_curve->isDashed()) {
        pen.setStyle(Qt::DashLine);
    } else {
        pen.setStyle(Qt::SolidLine);
    }

    setPen(pen);
    setBrush(Qt::NoBrush);
}
