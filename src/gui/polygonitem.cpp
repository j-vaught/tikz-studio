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

    // Apply line style
    switch (m_polygon->lineStyle()) {
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
    QColor fillColor = m_polygon->fillColor();
    fillColor.setAlphaF(m_polygon->opacity());

    // Apply fill pattern
    QBrush brush;
    switch (m_polygon->fillPattern()) {
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
