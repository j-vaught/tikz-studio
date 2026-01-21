#include "ellipseitem.h"
#include "ellipse.h"
#include "common.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QtMath>

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
    setScale(m_ellipse->scale());

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

QRectF EllipseItem::boundingRect() const {
    QRectF base = QGraphicsEllipseItem::boundingRect();
    if (isSelected()) {
        // Expand to include rotation handle
        QPointF rotPos = handlePosition(Rotation);
        QRectF rotRect(rotPos.x() - HANDLE_SIZE, rotPos.y() - HANDLE_SIZE,
                       HANDLE_SIZE * 2, HANDLE_SIZE * 2);
        base = base.united(rotRect);

        // Expand for corner handles
        base.adjust(-HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE, HANDLE_SIZE);
    }
    return base;
}

QPainterPath EllipseItem::shape() const {
    QPainterPath p = QGraphicsEllipseItem::shape();
    if (isSelected()) {
        // Add handles to shape
        for (int i = TopLeft; i <= Rotation; ++i) {
            QPointF pos = handlePosition(static_cast<HandleType>(i));
            if (i == Rotation) {
                p.addEllipse(pos, HANDLE_SIZE, HANDLE_SIZE);
            } else {
                p.addRect(pos.x() - HANDLE_SIZE, pos.y() - HANDLE_SIZE,
                         HANDLE_SIZE * 2, HANDLE_SIZE * 2);
            }
        }
    }
    return p;
}

void EllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *widget) {
    // Draw the ellipse itself
    QGraphicsEllipseItem::paint(painter, option, widget);

    // Draw handles when selected
    if (isSelected() && m_ellipse) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        // Draw corner resize handles
        QPen handlePen(UofSC::Atlantic());
        handlePen.setWidthF(1.5);
        handlePen.setCosmetic(true);
        painter->setPen(handlePen);
        painter->setBrush(Qt::white);

        for (int i = TopLeft; i <= BottomLeft; ++i) {
            QPointF pos = handlePosition(static_cast<HandleType>(i));
            QRectF handleRect(pos.x() - HANDLE_SIZE/2,
                             pos.y() - HANDLE_SIZE/2,
                             HANDLE_SIZE, HANDLE_SIZE);
            painter->drawRect(handleRect);
        }

        // Draw rotation handle
        QPointF rotPos = handlePosition(Rotation);
        QPointF topCenter(0, rect().top());

        // Draw line from top center to rotation handle
        QPen linePen(UofSC::Atlantic());
        linePen.setStyle(Qt::DashLine);
        linePen.setCosmetic(true);
        painter->setPen(linePen);
        painter->drawLine(topCenter, rotPos);

        // Draw rotation handle as a circle
        painter->setPen(handlePen);
        painter->setBrush(UofSC::Garnet());
        painter->drawEllipse(rotPos, HANDLE_SIZE/2, HANDLE_SIZE/2);

        painter->restore();
    }
}

QPointF EllipseItem::handlePosition(HandleType handle) const {
    QRectF r = rect();
    switch (handle) {
        case TopLeft:     return r.topLeft();
        case TopRight:    return r.topRight();
        case BottomRight: return r.bottomRight();
        case BottomLeft:  return r.bottomLeft();
        case Rotation:    return QPointF(0, r.top() - ROTATION_HANDLE_DISTANCE);
        default:          return QPointF(0, 0);
    }
}

EllipseItem::HandleType EllipseItem::handleAtPos(const QPointF &pos) const {
    for (int i = TopLeft; i <= Rotation; ++i) {
        HandleType h = static_cast<HandleType>(i);
        QPointF handlePos = handlePosition(h);
        QRectF handleRect(handlePos.x() - HANDLE_SIZE,
                         handlePos.y() - HANDLE_SIZE,
                         HANDLE_SIZE * 2, HANDLE_SIZE * 2);
        if (handleRect.contains(pos)) {
            return h;
        }
    }
    return None;
}

void EllipseItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isSelected()) {
        QPointF pos = event->pos();
        HandleType handle = handleAtPos(pos);

        if (handle != None) {
            m_dragHandle = handle;
            m_dragStartPos = event->pos();  // In item local coordinates
            m_dragStartRx = m_ellipse->radiusX();
            m_dragStartRy = m_ellipse->radiusY();
            m_dragStartRotation = m_ellipse->rotation();
            event->accept();
            return;
        }
    }

    QGraphicsEllipseItem::mousePressEvent(event);
}

void EllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_dragHandle != None && m_ellipse) {
        QPointF currentPos = event->pos();  // In item local coordinates

        if (m_dragHandle == Rotation) {
            // Calculate rotation based on mouse position relative to center
            QPointF center = this->scenePos();
            QPointF startVec = mapToScene(m_dragStartPos) - center;
            QPointF currentVec = event->scenePos() - center;

            float startAngle = std::atan2(startVec.y(), startVec.x());
            float currentAngle = std::atan2(currentVec.y(), currentVec.x());
            float deltaAngle = (currentAngle - startAngle) * 180.0f / M_PI;

            m_ellipse->setRotation(m_dragStartRotation + deltaAngle);
        } else {
            // Corner resize in object's local reference frame
            // currentPos is already in local coordinates (item coords)
            // The item's rect is centered at (0,0), so the handle positions
            // directly give us the radii

            // Get the signed distance from center in local coords
            float localX = std::abs(currentPos.x());
            float localY = std::abs(currentPos.y());

            // Convert from screen to TikZ coordinates
            float newRx = fromScreen(localX);
            float newRy = fromScreen(localY);

            // Clamp to minimum size
            newRx = qMax(0.1f, newRx);
            newRy = qMax(0.1f, newRy);

            m_ellipse->setRadiusX(newRx);
            m_ellipse->setRadiusY(newRy);
        }

        event->accept();
        return;
    }

    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void EllipseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_dragHandle = None;
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void EllipseItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if (isSelected()) {
        HandleType handle = handleAtPos(event->pos());

        if (handle == Rotation) {
            setCursor(Qt::CrossCursor);
        } else if (handle != None) {
            // Corner handles - diagonal resize cursor
            if (handle == TopLeft || handle == BottomRight) {
                setCursor(Qt::SizeFDiagCursor);
            } else {
                setCursor(Qt::SizeBDiagCursor);
            }
        } else {
            setCursor(Qt::ArrowCursor);
        }
    } else {
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsEllipseItem::hoverMoveEvent(event);
}
