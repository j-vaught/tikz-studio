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
    switch (m_ellipse->lineCap()) {
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
    switch (m_ellipse->lineJoin()) {
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
