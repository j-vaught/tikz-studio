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

    // Apply line style
    switch (m_ellipse->lineStyle()) {
        case LineStyle::Dashed:
            pen.setStyle(Qt::DashLine);
            break;
        case LineStyle::Dotted:
            pen.setStyle(Qt::DotLine);
            break;
        case LineStyle::DashDot:
            pen.setStyle(Qt::DashDotLine);
            break;
        case LineStyle::DashDotDot:
            pen.setStyle(Qt::DashDotDotLine);
            break;
        default:
            pen.setStyle(Qt::SolidLine);
            break;
    }
    setPen(pen);

    // Fill
    QColor fillColor = m_ellipse->fillColor();
    fillColor.setAlphaF(m_ellipse->opacity());

    // Apply fill pattern
    QBrush brush;
    switch (m_ellipse->fillPattern()) {
        case FillPattern::None:
            brush = Qt::NoBrush;
            break;
        case FillPattern::HorizontalLines:
            brush = QBrush(fillColor, Qt::HorPattern);
            break;
        case FillPattern::VerticalLines:
            brush = QBrush(fillColor, Qt::VerPattern);
            break;
        case FillPattern::CrossHatch:
            brush = QBrush(fillColor, Qt::CrossPattern);
            break;
        case FillPattern::DiagonalLines:
            brush = QBrush(fillColor, Qt::BDiagPattern);
            break;
        case FillPattern::DiagonalCrossHatch:
            brush = QBrush(fillColor, Qt::DiagCrossPattern);
            break;
        case FillPattern::Dots:
            brush = QBrush(fillColor, Qt::Dense6Pattern);
            break;
        default:  // Solid
            brush = QBrush(fillColor);
            break;
    }
    setBrush(brush);
}
