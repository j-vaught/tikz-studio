#include "curveitem.h"
#include "curve.h"
#include "common.h"

#include <QPen>
#include <QBrush>

static Qt::PenStyle lineStyleToQt(LineStyle style) {
    switch (style) {
        case LineStyle::Dashed:
        case LineStyle::DenselyDashed:
        case LineStyle::LooselyDashed:
            return Qt::DashLine;
        case LineStyle::Dotted:
        case LineStyle::DenselyDotted:
        case LineStyle::LooselyDotted:
            return Qt::DotLine;
        case LineStyle::DashDot:
        case LineStyle::DenselyDashDot:
        case LineStyle::LooselyDashDot:
            return Qt::DashDotLine;
        case LineStyle::DashDotDot:
        case LineStyle::DenselyDashDotDot:
        case LineStyle::LooselyDashDotDot:
            return Qt::DashDotDotLine;
        default:
            return Qt::SolidLine;
    }
}

static Qt::PenCapStyle lineCapToQt(LineCap cap) {
    switch (cap) {
        case LineCap::Round: return Qt::RoundCap;
        case LineCap::Square: return Qt::SquareCap;
        default: return Qt::FlatCap;
    }
}

static Qt::PenJoinStyle lineJoinToQt(LineJoin join) {
    switch (join) {
        case LineJoin::Round: return Qt::RoundJoin;
        case LineJoin::Bevel: return Qt::BevelJoin;
        default: return Qt::MiterJoin;
    }
}

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

    QPainterPath path = m_curve->painterPath();
    setPath(path);

    // Calculate center for transform origin
    QRectF bounds = path.boundingRect();
    QPointF center = bounds.center();
    setTransformOriginPoint(center);

    // Apply rotation and scale
    setRotation(m_curve->rotation());
    setScale(m_curve->scale());

    QPen pen(m_curve->color());
    pen.setWidthF(m_curve->lineWidth() * 1.33);
    pen.setStyle(lineStyleToQt(m_curve->lineStyle()));
    pen.setCapStyle(lineCapToQt(m_curve->lineCap()));
    pen.setJoinStyle(lineJoinToQt(m_curve->lineJoin()));

    setPen(pen);
    setBrush(Qt::NoBrush);
}
