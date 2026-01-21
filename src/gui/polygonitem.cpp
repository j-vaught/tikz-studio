#include "polygonitem.h"
#include "polygon.h"
#include "common.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QtMath>

PolygonItem::PolygonItem(Polygon *polygon, QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , m_polygon(polygon)
{
    setFlags(ItemIsSelectable | ItemIsMovable);
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

QRectF PolygonItem::boundingRect() const {
    QRectF base = QGraphicsPathItem::boundingRect();
    if (isSelected()) {
        // Expand to include rotation handle
        QPointF rotPos = rotationHandlePos();
        QRectF rotRect(rotPos.x() - HANDLE_SIZE, rotPos.y() - HANDLE_SIZE,
                       HANDLE_SIZE * 2, HANDLE_SIZE * 2);
        base = base.united(rotRect);

        // Expand for vertex/corner handles
        base.adjust(-HANDLE_SIZE, -HANDLE_SIZE, HANDLE_SIZE, HANDLE_SIZE);
    }
    return base;
}

QPainterPath PolygonItem::shape() const {
    QPainterPath p = QGraphicsPathItem::shape();
    if (isSelected() && m_polygon) {
        // Add rotation handle to shape
        QPointF rotPos = rotationHandlePos();
        p.addEllipse(rotPos, HANDLE_SIZE, HANDLE_SIZE);

        if (m_polygon->allowsVertexEditing()) {
            // Add vertex handles to shape
            QVector<QPointF> vertices = m_polygon->vertexPositions();
            for (const QPointF &v : vertices) {
                QPointF screenPos = toScreen(v);
                p.addRect(screenPos.x() - HANDLE_SIZE, screenPos.y() - HANDLE_SIZE,
                         HANDLE_SIZE * 2, HANDLE_SIZE * 2);
            }
        } else {
            // Add corner handles to shape
            for (int i = TopLeft; i <= BottomLeft; ++i) {
                QPointF pos = cornerHandlePosition(static_cast<CornerHandle>(i));
                p.addRect(pos.x() - HANDLE_SIZE, pos.y() - HANDLE_SIZE,
                         HANDLE_SIZE * 2, HANDLE_SIZE * 2);
            }
        }
    }
    return p;
}

void PolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *widget) {
    // Draw the polygon itself
    QGraphicsPathItem::paint(painter, option, widget);

    // Draw handles when selected
    if (isSelected() && m_polygon) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        QPen handlePen(UofSC::Atlantic());
        handlePen.setWidthF(1.5);
        handlePen.setCosmetic(true);
        painter->setPen(handlePen);
        painter->setBrush(Qt::white);

        if (m_polygon->allowsVertexEditing()) {
            // Draw vertex handles
            QVector<QPointF> vertices = m_polygon->vertexPositions();
            for (const QPointF &v : vertices) {
                QPointF screenPos = toScreen(v);
                QRectF handleRect(screenPos.x() - HANDLE_SIZE/2,
                                 screenPos.y() - HANDLE_SIZE/2,
                                 HANDLE_SIZE, HANDLE_SIZE);
                painter->drawRect(handleRect);
            }
        } else {
            // Draw corner handles for rescaling
            for (int i = TopLeft; i <= BottomLeft; ++i) {
                QPointF pos = cornerHandlePosition(static_cast<CornerHandle>(i));
                QRectF handleRect(pos.x() - HANDLE_SIZE/2,
                                 pos.y() - HANDLE_SIZE/2,
                                 HANDLE_SIZE, HANDLE_SIZE);
                painter->drawRect(handleRect);
            }
        }

        // Draw rotation handle
        QPointF rotHandlePos = rotationHandlePos();
        QRectF bounds = path().boundingRect();
        QPointF center = bounds.center();

        // Draw line from center to rotation handle
        QPen linePen(UofSC::Atlantic());
        linePen.setStyle(Qt::DashLine);
        linePen.setCosmetic(true);
        painter->setPen(linePen);
        painter->drawLine(center, rotHandlePos);

        // Draw rotation handle as a circle
        painter->setPen(handlePen);
        painter->setBrush(UofSC::Garnet());
        painter->drawEllipse(rotHandlePos, HANDLE_SIZE/2, HANDLE_SIZE/2);

        painter->restore();
    }
}

int PolygonItem::vertexHandleAtPos(const QPointF &pos) const {
    if (!m_polygon) return -1;

    QVector<QPointF> vertices = m_polygon->vertexPositions();
    for (int i = 0; i < vertices.size(); ++i) {
        QPointF screenPos = toScreen(vertices[i]);
        QRectF handleRect(screenPos.x() - HANDLE_SIZE,
                         screenPos.y() - HANDLE_SIZE,
                         HANDLE_SIZE * 2, HANDLE_SIZE * 2);
        if (handleRect.contains(pos)) {
            return i;
        }
    }
    return -1;
}

PolygonItem::CornerHandle PolygonItem::cornerHandleAtPos(const QPointF &pos) const {
    for (int i = TopLeft; i <= BottomLeft; ++i) {
        CornerHandle h = static_cast<CornerHandle>(i);
        QPointF handlePos = cornerHandlePosition(h);
        QRectF handleRect(handlePos.x() - HANDLE_SIZE,
                         handlePos.y() - HANDLE_SIZE,
                         HANDLE_SIZE * 2, HANDLE_SIZE * 2);
        if (handleRect.contains(pos)) {
            return h;
        }
    }
    return NoCorner;
}

QPointF PolygonItem::cornerHandlePosition(CornerHandle handle) const {
    QRectF bounds = path().boundingRect();
    switch (handle) {
        case TopLeft:     return bounds.topLeft();
        case TopRight:    return bounds.topRight();
        case BottomRight: return bounds.bottomRight();
        case BottomLeft:  return bounds.bottomLeft();
        default:          return QPointF(0, 0);
    }
}

bool PolygonItem::rotationHandleAtPos(const QPointF &pos) const {
    QPointF rotPos = rotationHandlePos();
    QRectF handleRect(rotPos.x() - HANDLE_SIZE,
                     rotPos.y() - HANDLE_SIZE,
                     HANDLE_SIZE * 2, HANDLE_SIZE * 2);
    return handleRect.contains(pos);
}

QPointF PolygonItem::rotationHandlePos() const {
    QRectF bounds = path().boundingRect();
    QPointF center = bounds.center();
    // Position above the top center
    return QPointF(center.x(), bounds.top() - ROTATION_HANDLE_DISTANCE);
}

void PolygonItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isSelected() && m_polygon) {
        QPointF pos = event->pos();

        // Check rotation handle first
        if (rotationHandleAtPos(pos)) {
            m_draggingRotation = true;
            m_dragStartPos = event->scenePos();
            m_dragStartRotation = m_polygon->rotation();
            event->accept();
            return;
        }

        if (m_polygon->allowsVertexEditing()) {
            // Check vertex handles
            int handle = vertexHandleAtPos(pos);
            if (handle >= 0) {
                m_dragVertexHandle = handle;
                m_dragStartPos = event->scenePos();
                event->accept();
                return;
            }
        } else {
            // Check corner handles for rescaling
            CornerHandle corner = cornerHandleAtPos(pos);
            if (corner != NoCorner) {
                m_dragCornerHandle = corner;
                m_dragStartPos = event->pos();  // In item local coordinates

                // Store original vertex positions (in TikZ coords)
                m_dragStartVertices = m_polygon->vertexPositions();

                // Store the current screen bounds (fixed reference)
                m_dragStartScreenBounds = path().boundingRect();

                // Calculate original bounding box in TikZ coords
                if (!m_dragStartVertices.isEmpty()) {
                    float minX = m_dragStartVertices[0].x();
                    float maxX = minX;
                    float minY = m_dragStartVertices[0].y();
                    float maxY = minY;
                    for (const QPointF &v : m_dragStartVertices) {
                        minX = qMin(minX, (float)v.x());
                        maxX = qMax(maxX, (float)v.x());
                        minY = qMin(minY, (float)v.y());
                        maxY = qMax(maxY, (float)v.y());
                    }
                    m_dragStartBounds = QRectF(minX, minY, maxX - minX, maxY - minY);
                }

                event->accept();
                return;
            }
        }
    }

    QGraphicsPathItem::mousePressEvent(event);
}

void PolygonItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_draggingRotation && m_polygon) {
        // Calculate rotation based on mouse position relative to center
        QRectF bounds = path().boundingRect();
        QPointF center = mapToScene(bounds.center());

        QPointF startVec = m_dragStartPos - center;
        QPointF currentVec = event->scenePos() - center;

        float startAngle = std::atan2(startVec.y(), startVec.x());
        float currentAngle = std::atan2(currentVec.y(), currentVec.x());
        float deltaAngle = (currentAngle - startAngle) * 180.0f / M_PI;

        m_polygon->setRotation(m_dragStartRotation + deltaAngle);
        event->accept();
        return;
    }

    if (m_dragVertexHandle >= 0 && m_polygon) {
        // Move the vertex (vertex editing mode)
        QPointF scenePos = event->scenePos();
        QPointF tikzPos = fromScreen(scenePos);

        // Snap to grid
        tikzPos = snapToGrid(tikzPos, GRID_MINOR);

        m_polygon->setVertexPosition(m_dragVertexHandle, tikzPos);
        event->accept();
        return;
    }

    if (m_dragCornerHandle != NoCorner && m_polygon && !m_dragStartVertices.isEmpty()) {
        // Corner rescaling mode - scale in object's local reference frame
        QPointF currentPos = event->pos();  // In item local coordinates

        // Use the FIXED screen bounds from drag start (not current bounds)
        QPointF screenCenter = m_dragStartScreenBounds.center();

        // Calculate scale factors based on distance from center
        // Original corner position (from center) - use fixed start bounds
        QPointF origCornerFromCenter;
        switch (m_dragCornerHandle) {
            case TopLeft:     origCornerFromCenter = m_dragStartScreenBounds.topLeft() - screenCenter; break;
            case TopRight:    origCornerFromCenter = m_dragStartScreenBounds.topRight() - screenCenter; break;
            case BottomRight: origCornerFromCenter = m_dragStartScreenBounds.bottomRight() - screenCenter; break;
            case BottomLeft:  origCornerFromCenter = m_dragStartScreenBounds.bottomLeft() - screenCenter; break;
            default: break;
        }

        // Current position from center
        QPointF currentFromCenter = currentPos - screenCenter;

        // Calculate scale factors
        float scaleX = 1.0f, scaleY = 1.0f;
        if (std::abs(origCornerFromCenter.x()) > 0.01f) {
            scaleX = std::abs(currentFromCenter.x() / origCornerFromCenter.x());
        }
        if (std::abs(origCornerFromCenter.y()) > 0.01f) {
            scaleY = std::abs(currentFromCenter.y() / origCornerFromCenter.y());
        }

        // Clamp scale factors to prevent inverting
        scaleX = qMax(0.1f, scaleX);
        scaleY = qMax(0.1f, scaleY);

        // For complex shapes (Arrow, Star, RegularPolygon), use uniform scaling
        // to preserve proportions since non-uniform scaling distorts them
        PolygonType ptype = m_polygon->polygonType();
        if (ptype == PolygonType::Arrow || ptype == PolygonType::Star ||
            ptype == PolygonType::RegularPolygon || ptype == PolygonType::Diamond) {
            float uniformScale = (scaleX + scaleY) / 2.0f;
            scaleX = scaleY = uniformScale;
        }

        // Calculate TikZ center
        QPointF tikzCenter(
            m_dragStartBounds.x() + m_dragStartBounds.width() / 2,
            m_dragStartBounds.y() + m_dragStartBounds.height() / 2
        );

        // Scale all vertices relative to center
        for (int i = 0; i < m_dragStartVertices.size(); ++i) {
            QPointF orig = m_dragStartVertices[i];
            QPointF fromCenter = orig - tikzCenter;
            QPointF scaled = tikzCenter + QPointF(fromCenter.x() * scaleX, fromCenter.y() * scaleY);
            m_polygon->setVertexPosition(i, scaled);
        }

        event->accept();
        return;
    }

    QGraphicsPathItem::mouseMoveEvent(event);
}

void PolygonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_dragVertexHandle = -1;
    m_dragCornerHandle = NoCorner;
    m_draggingRotation = false;
    m_dragStartVertices.clear();
    QGraphicsPathItem::mouseReleaseEvent(event);
}

void PolygonItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if (isSelected() && m_polygon) {
        QPointF pos = event->pos();

        if (rotationHandleAtPos(pos)) {
            setCursor(Qt::CrossCursor);  // Rotation cursor
        } else if (m_polygon->allowsVertexEditing()) {
            if (vertexHandleAtPos(pos) >= 0) {
                setCursor(Qt::SizeAllCursor);  // Move vertex cursor
            } else {
                setCursor(Qt::ArrowCursor);
            }
        } else {
            CornerHandle corner = cornerHandleAtPos(pos);
            if (corner == TopLeft || corner == BottomRight) {
                setCursor(Qt::SizeFDiagCursor);
            } else if (corner == TopRight || corner == BottomLeft) {
                setCursor(Qt::SizeBDiagCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
    } else {
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsPathItem::hoverMoveEvent(event);
}
