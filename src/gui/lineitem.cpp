#include "lineitem.h"
#include "line.h"
#include "common.h"

#include <QPen>
#include <QVector>

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

    // Calculate center for transform origin
    QPointF center = (start + end) / 2.0;
    setTransformOriginPoint(center);

    // Apply rotation and scale
    setRotation(m_line->rotation());
    setScale(m_line->scale());

    QPen pen(m_line->color());
    pen.setWidthF(m_line->lineWidth() * 1.33);  // pt to px
    pen.setStyle(lineStyleToQt(m_line->lineStyle()));
    pen.setCapStyle(lineCapToQt(m_line->lineCap()));
    pen.setJoinStyle(lineJoinToQt(m_line->lineJoin()));

    setPen(pen);
}
