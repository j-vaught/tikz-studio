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
    QPainterPath path = m_polygon->painterPath();
    setPath(path);

    // Calculate centroid for transform origin
    QRectF bounds = path.boundingRect();
    QPointF center = bounds.center();
    setTransformOriginPoint(center);

    // Apply rotation and scale
    setRotation(m_polygon->rotation());
    setScale(m_polygon->scale());

    // Stroke
    QPen pen(m_polygon->strokeColor());
    pen.setWidthF(m_polygon->lineWidth() * 1.33);

    // Apply line style
    switch (m_polygon->lineStyle()) {
        case LineStyle::Dashed:
            pen.setStyle(Qt::DashLine);
            break;
        case LineStyle::DenselyDashed:
            pen.setStyle(Qt::DashLine);
            pen.setDashPattern({4, 2});
            break;
        case LineStyle::LooselyDashed:
            pen.setStyle(Qt::DashLine);
            pen.setDashPattern({4, 8});
            break;
        case LineStyle::Dotted:
            pen.setStyle(Qt::DotLine);
            break;
        case LineStyle::DenselyDotted:
            pen.setStyle(Qt::DotLine);
            pen.setDashPattern({1, 1});
            break;
        case LineStyle::LooselyDotted:
            pen.setStyle(Qt::DotLine);
            pen.setDashPattern({1, 4});
            break;
        case LineStyle::DashDot:
            pen.setStyle(Qt::DashDotLine);
            break;
        case LineStyle::DenselyDashDot:
            pen.setStyle(Qt::DashDotLine);
            pen.setDashPattern({4, 2, 1, 2});
            break;
        case LineStyle::LooselyDashDot:
            pen.setStyle(Qt::DashDotLine);
            pen.setDashPattern({4, 8, 1, 8});
            break;
        case LineStyle::DashDotDot:
            pen.setStyle(Qt::DashDotDotLine);
            break;
        case LineStyle::DenselyDashDotDot:
            pen.setStyle(Qt::DashDotDotLine);
            pen.setDashPattern({4, 2, 1, 2, 1, 2});
            break;
        case LineStyle::LooselyDashDotDot:
            pen.setStyle(Qt::DashDotDotLine);
            pen.setDashPattern({4, 8, 1, 8, 1, 8});
            break;
        default:
            pen.setStyle(Qt::SolidLine);
            break;
    }

    // Apply line cap
    switch (m_polygon->lineCap()) {
        case LineCap::Round:
            pen.setCapStyle(Qt::RoundCap);
            break;
        case LineCap::Square:
            pen.setCapStyle(Qt::SquareCap);
            break;
        default:
            pen.setCapStyle(Qt::FlatCap);
            break;
    }

    // Apply line join
    switch (m_polygon->lineJoin()) {
        case LineJoin::Round:
            pen.setJoinStyle(Qt::RoundJoin);
            break;
        case LineJoin::Bevel:
            pen.setJoinStyle(Qt::BevelJoin);
            break;
        default:
            pen.setJoinStyle(Qt::MiterJoin);
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
        case FillPattern::Grid:
            brush = QBrush(fillColor, Qt::CrossPattern);
            break;
        case FillPattern::NorthEastLines:
            brush = QBrush(fillColor, Qt::BDiagPattern);
            break;
        case FillPattern::NorthWestLines:
            brush = QBrush(fillColor, Qt::FDiagPattern);
            break;
        case FillPattern::CrossHatch:
            brush = QBrush(fillColor, Qt::DiagCrossPattern);
            break;
        case FillPattern::CrossHatchDots:
            brush = QBrush(fillColor, Qt::Dense5Pattern);
            break;
        case FillPattern::Dots:
            brush = QBrush(fillColor, Qt::Dense6Pattern);
            break;
        case FillPattern::FivePointedStars:
        case FillPattern::SixPointedStars:
            brush = QBrush(fillColor, Qt::Dense4Pattern);  // Approximation
            break;
        case FillPattern::Bricks:
            brush = QBrush(fillColor, Qt::Dense3Pattern);  // Approximation
            break;
        default:  // Solid
            brush = QBrush(fillColor);
            break;
    }
    setBrush(brush);
}
