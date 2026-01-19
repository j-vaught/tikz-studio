#include "canvas.h"
#include "document.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"
#include "pointitem.h"
#include "lineitem.h"
#include "polygonitem.h"
#include "curveitem.h"
#include "ellipseitem.h"
#include "toolpalette.h"
#include "colorpalette.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QtMath>

Canvas::Canvas(Document *doc, QObject *parent)
    : QGraphicsScene(parent)
    , m_document(doc)
{
    setSceneRect(-500, -500, 1000, 1000);

    connect(doc, &Document::changed, this, &Canvas::syncWithDocument);

    syncWithDocument();
}

void Canvas::syncWithDocument() {
    // Remove items for deleted objects
    for (auto it = m_pointItems.begin(); it != m_pointItems.end(); ) {
        if (!m_document->points().contains(it.key())) {
            removeItem(it.value());
            delete it.value();
            it = m_pointItems.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = m_lineItems.begin(); it != m_lineItems.end(); ) {
        if (!m_document->lines().contains(it.key())) {
            removeItem(it.value());
            delete it.value();
            it = m_lineItems.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = m_polygonItems.begin(); it != m_polygonItems.end(); ) {
        if (!m_document->polygons().contains(it.key())) {
            removeItem(it.value());
            delete it.value();
            it = m_polygonItems.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = m_curveItems.begin(); it != m_curveItems.end(); ) {
        if (!m_document->curves().contains(it.key())) {
            removeItem(it.value());
            delete it.value();
            it = m_curveItems.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = m_ellipseItems.begin(); it != m_ellipseItems.end(); ) {
        if (!m_document->ellipses().contains(it.key())) {
            removeItem(it.value());
            delete it.value();
            it = m_ellipseItems.erase(it);
        } else {
            ++it;
        }
    }

    // Add items for new objects
    for (Point *p : m_document->points()) {
        if (!m_pointItems.contains(p)) {
            addPointItem(p);
        }
    }

    for (Line *l : m_document->lines()) {
        if (!m_lineItems.contains(l)) {
            addLineItem(l);
        }
    }

    for (Polygon *p : m_document->polygons()) {
        if (!m_polygonItems.contains(p)) {
            addPolygonItem(p);
        }
    }

    for (Curve *c : m_document->curves()) {
        if (!m_curveItems.contains(c)) {
            addCurveItem(c);
        }
    }

    for (Ellipse *e : m_document->ellipses()) {
        if (!m_ellipseItems.contains(e)) {
            addEllipseItem(e);
        }
    }

    update();
}

void Canvas::addPointItem(Point *point) {
    PointItem *item = new PointItem(point);
    addItem(item);
    m_pointItems[point] = item;
}

void Canvas::addLineItem(Line *line) {
    LineItem *item = new LineItem(line);
    addItem(item);
    m_lineItems[line] = item;
}

void Canvas::addPolygonItem(Polygon *polygon) {
    PolygonItem *item = new PolygonItem(polygon);
    addItem(item);
    m_polygonItems[polygon] = item;
}

void Canvas::addCurveItem(Curve *curve) {
    CurveItem *item = new CurveItem(curve);
    addItem(item);
    m_curveItems[curve] = item;
}

void Canvas::addEllipseItem(Ellipse *ellipse) {
    EllipseItem *item = new EllipseItem(ellipse);
    addItem(item);
    m_ellipseItems[ellipse] = item;
}

void Canvas::deleteSelected() {
    QList<QGraphicsItem*> selected = selectedItems();

    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            m_document->removePoint(pi->point());
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            m_document->removeLine(li->line());
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            m_document->removePolygon(pi->polygon());
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            m_document->removeCurve(ci->curve());
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            m_document->removeEllipse(ei->ellipse());
        }
    }
}

void Canvas::setGridVisible(bool visible) {
    if (m_gridVisible != visible) {
        m_gridVisible = visible;
        update();
    }
}

void Canvas::setAxesVisible(bool visible) {
    if (m_axesVisible != visible) {
        m_axesVisible = visible;
        update();
    }
}

void Canvas::setAxisTicksVisible(bool visible) {
    if (m_axisTicksVisible != visible) {
        m_axisTicksVisible = visible;
        update();
    }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (!m_tools) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    Tool tool = m_tools->currentTool();

    // Shape tools use drag
    if (tool == Tool::Rectangle || tool == Tool::Circle ||
        tool == Tool::Ellipse || tool == Tool::Triangle ||
        tool == Tool::RegularPolygon) {
        if (event->button() == Qt::LeftButton) {
            m_shapeDragging = true;
            m_shapeStartPos = fromScreen(event->scenePos());
            m_shapeStartPos = snapToGrid(m_shapeStartPos, GRID_MINOR);
        }
        return;
    }

    switch (tool) {
    case Tool::Select:
        handleSelectTool(event);
        break;
    case Tool::Point:
        handlePointTool(event);
        break;
    case Tool::Line:
        handleLineTool(event);
        break;
    case Tool::Polygon:
        handlePolygonTool(event);
        break;
    case Tool::Curve:
        handleCurveTool(event);
        break;
    default:
        break;
    }
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_previewLine) {
        QPointF snapped = snapToGrid(fromScreen(event->scenePos()), GRID_MINOR);
        m_previewLine->setLine(QLineF(m_previewLine->line().p1(), toScreen(snapped)));
    }

    if (m_shapeDragging) {
        handleShapeDrag(event);
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_shapeDragging && event->button() == Qt::LeftButton) {
        finishShapeDrag(event);
        m_shapeDragging = false;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void Canvas::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // Double-click finishes polygon or curve
    if (m_currentPolygon) {
        if (m_currentPolygon->vertexCount() < 3) {
            m_document->removePolygon(m_currentPolygon);
        }
        m_currentPolygon = nullptr;
        emit statusMessage("Polygon completed");
    }
    if (m_currentCurve) {
        if (m_currentCurve->controlPointCount() < 2) {
            m_document->removeCurve(m_currentCurve);
        }
        m_currentCurve = nullptr;
        emit statusMessage("Curve completed");
    }

    QGraphicsScene::mouseDoubleClickEvent(event);
}

void Canvas::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        deleteSelected();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        // Cancel current drawing operation
        if (m_previewLine) {
            removeItem(m_previewLine);
            delete m_previewLine;
            m_previewLine = nullptr;
            m_drawing = false;
        }
        if (m_previewShape) {
            removeItem(m_previewShape);
            delete m_previewShape;
            m_previewShape = nullptr;
            m_shapeDragging = false;
        }
        if (m_currentPolygon) {
            if (m_currentPolygon->vertexCount() < 3) {
                m_document->removePolygon(m_currentPolygon);
            }
            m_currentPolygon = nullptr;
        }
        if (m_currentCurve) {
            if (m_currentCurve->controlPointCount() < 2) {
                m_document->removeCurve(m_currentCurve);
            }
            m_currentCurve = nullptr;
        }
        clearSelection();
        emit statusMessage("Cancelled");
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Finish polygon or curve
        if (m_currentPolygon) {
            m_currentPolygon = nullptr;
            emit statusMessage("Polygon completed");
        }
        if (m_currentCurve) {
            m_currentCurve = nullptr;
            emit statusMessage("Curve completed");
        }
        event->accept();
        return;
    }

    QGraphicsScene::keyPressEvent(event);
}

void Canvas::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    showContextMenu(event);
}

void Canvas::showContextMenu(QGraphicsSceneContextMenuEvent *event) {
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());

    QMenu menu;

    if (item) {
        // Select the item if not already selected
        if (!item->isSelected()) {
            clearSelection();
            item->setSelected(true);
        }

        menu.addAction("Delete", this, &Canvas::deleteSelected);
        menu.addSeparator();

        if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            Polygon *poly = pi->polygon();

            QAction *roundAllAction = menu.addAction("Round All Corners...");
            connect(roundAllAction, &QAction::triggered, this, [this, poly]() {
                bool ok;
                double radius = QInputDialog::getDouble(nullptr, "Corner Radius",
                    "Radius for all corners:", poly->defaultCornerRadius(),
                    0, 5, 2, &ok);
                if (ok) {
                    poly->setDefaultCornerRadius(radius);
                }
            });

            QAction *sharpAllAction = menu.addAction("Sharp All Corners");
            connect(sharpAllAction, &QAction::triggered, this, [poly]() {
                poly->setDefaultCornerRadius(0);
                for (int i = 0; i < poly->vertexCount(); i++) {
                    poly->setVertexCornerRadius(i, 0);
                }
            });
        }

        if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            Ellipse *ellipse = ei->ellipse();

            if (!ellipse->isCircle()) {
                QAction *makeCircleAction = menu.addAction("Make Circle");
                connect(makeCircleAction, &QAction::triggered, this, [ellipse]() {
                    float avg = (ellipse->radiusX() + ellipse->radiusY()) / 2.0f;
                    ellipse->setRadius(avg);
                });
            }
        }
    } else {
        // No item clicked - general canvas actions
        menu.addAction("Paste", []() { /* TODO */ })->setEnabled(false);
    }

    if (!menu.isEmpty()) {
        menu.exec(event->screenPos());
    }
}

void Canvas::drawBackground(QPainter *painter, const QRectF &rect) {
    // White background
    painter->fillRect(rect, Qt::white);

    float minorStep = GRID_MINOR * GLOBAL_SCALE;
    float majorStep = GRID_MAJOR * GLOBAL_SCALE;

    // Draw grid if visible
    if (m_gridVisible) {
        // Minor grid (light gray)
        QPen minorPen(QColor(220, 220, 220), 0.5);
        painter->setPen(minorPen);

        float left = std::floor(rect.left() / minorStep) * minorStep;
        float top = std::floor(rect.top() / minorStep) * minorStep;

        for (float x = left; x <= rect.right(); x += minorStep) {
            painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
        }
        for (float y = top; y <= rect.bottom(); y += minorStep) {
            painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
        }

        // Major grid (darker gray)
        QPen majorPen(QColor(180, 180, 180), 1.0);
        painter->setPen(majorPen);

        left = std::floor(rect.left() / majorStep) * majorStep;
        top = std::floor(rect.top() / majorStep) * majorStep;

        for (float x = left; x <= rect.right(); x += majorStep) {
            painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
        }
        for (float y = top; y <= rect.bottom(); y += majorStep) {
            painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
        }
    }

    // Draw axes if visible
    if (m_axesVisible) {
        QPen axisPen(Qt::black, 2.0);
        painter->setPen(axisPen);
        painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom()));
        painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0));

        // Draw axis ticks if visible
        if (m_axisTicksVisible) {
            QPen tickPen(Qt::black, 1.5);
            painter->setPen(tickPen);

            float tickSize = 5.0f;
            QFont tickFont("Arial", 9);
            painter->setFont(tickFont);

            // X-axis ticks (along y=0)
            float left = std::floor(rect.left() / majorStep) * majorStep;
            for (float x = left; x <= rect.right(); x += majorStep) {
                if (std::abs(x) < 0.001f) continue;  // Skip origin

                // Draw tick mark
                painter->drawLine(QPointF(x, -tickSize), QPointF(x, tickSize));

                // Draw label (TikZ coordinate)
                float tikzX = x / GLOBAL_SCALE;
                QString label = QString::number(tikzX, 'g', 3);
                QRectF textRect(x - 20, tickSize + 2, 40, 15);
                painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, label);
            }

            // Y-axis ticks (along x=0)
            float top = std::floor(rect.top() / majorStep) * majorStep;
            for (float y = top; y <= rect.bottom(); y += majorStep) {
                if (std::abs(y) < 0.001f) continue;  // Skip origin

                // Draw tick mark
                painter->drawLine(QPointF(-tickSize, y), QPointF(tickSize, y));

                // Draw label (TikZ coordinate - note Y is inverted)
                float tikzY = -y / GLOBAL_SCALE;
                QString label = QString::number(tikzY, 'g', 3);
                QRectF textRect(-35, y - 8, 28, 16);
                painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
            }

            // Origin label
            QRectF originRect(4, 4, 20, 15);
            painter->drawText(originRect, Qt::AlignLeft | Qt::AlignTop, "0");
        }
    }
}

void Canvas::handleSelectTool(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mousePressEvent(event);
}

void Canvas::handlePointTool(QGraphicsSceneMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    QPointF tikzPos = fromScreen(event->scenePos());
    tikzPos = snapToGrid(tikzPos, GRID_MINOR);

    Point *point = m_document->addPoint(tikzPos);

    if (m_colors) {
        point->setColor(m_colors->currentColor());
    }

    emit statusMessage(QString("Point added at (%1, %2)")
        .arg(tikzPos.x(), 0, 'f', 2).arg(tikzPos.y(), 0, 'f', 2));
}

void Canvas::handleLineTool(QGraphicsSceneMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;

    QPointF tikzPos = fromScreen(event->scenePos());
    tikzPos = snapToGrid(tikzPos, GRID_MINOR);

    if (!m_drawing) {
        // Start line
        m_drawing = true;
        m_lineStartPos = tikzPos;

        // Check if clicking on existing point
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            m_lineStartPoint = pi->point();
            m_lineStartPos = m_lineStartPoint->pos();
        } else {
            m_lineStartPoint = nullptr;
        }

        // Create preview line
        m_previewLine = new QGraphicsLineItem();
        m_previewLine->setLine(QLineF(toScreen(m_lineStartPos), toScreen(m_lineStartPos)));
        m_previewLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        addItem(m_previewLine);

        emit statusMessage("Line: click endpoint");
    } else {
        // End line
        m_drawing = false;

        if (m_previewLine) {
            removeItem(m_previewLine);
            delete m_previewLine;
            m_previewLine = nullptr;
        }

        Line *line = m_document->addLine(m_lineStartPos, tikzPos);

        if (m_colors) {
            line->setColor(m_colors->currentColor());
        }

        emit statusMessage("Line created");
    }
}

void Canvas::handlePolygonTool(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF tikzPos = fromScreen(event->scenePos());
        tikzPos = snapToGrid(tikzPos, GRID_MINOR);

        if (!m_currentPolygon) {
            m_currentPolygon = m_document->addPolygon();
            if (m_colors) {
                m_currentPolygon->setFillColor(m_colors->currentColor());
            }
            emit statusMessage("Polygon: adding vertices (right-click or Enter to finish)");
        }

        m_currentPolygon->addVertex(tikzPos);
        emit statusMessage(QString("Polygon: %1 vertices").arg(m_currentPolygon->vertexCount()));
    }
    else if (event->button() == Qt::RightButton) {
        // Finish polygon
        if (m_currentPolygon) {
            if (m_currentPolygon->vertexCount() < 3) {
                m_document->removePolygon(m_currentPolygon);
                emit statusMessage("Polygon cancelled (need at least 3 vertices)");
            } else {
                emit statusMessage("Polygon completed");
            }
            m_currentPolygon = nullptr;
        }
    }
}

void Canvas::handleCurveTool(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF tikzPos = fromScreen(event->scenePos());
        tikzPos = snapToGrid(tikzPos, GRID_MINOR);

        if (!m_currentCurve) {
            m_currentCurve = m_document->addCurve();
            if (m_colors) {
                m_currentCurve->setColor(m_colors->currentColor());
            }
            emit statusMessage("Curve: adding control points (right-click or Enter to finish)");
        }

        m_currentCurve->addControlPoint(tikzPos);
        emit statusMessage(QString("Curve: %1 control points").arg(m_currentCurve->controlPointCount()));
    }
    else if (event->button() == Qt::RightButton) {
        // Finish curve
        if (m_currentCurve) {
            if (m_currentCurve->controlPointCount() < 2) {
                m_document->removeCurve(m_currentCurve);
                emit statusMessage("Curve cancelled (need at least 2 points)");
            } else {
                emit statusMessage("Curve completed");
            }
            m_currentCurve = nullptr;
        }
    }
}

void Canvas::handleShapeDrag(QGraphicsSceneMouseEvent *event) {
    if (!m_tools) return;

    QPointF currentPos = fromScreen(event->scenePos());
    currentPos = snapToGrid(currentPos, GRID_MINOR);

    // Remove old preview
    if (m_previewShape) {
        removeItem(m_previewShape);
        delete m_previewShape;
        m_previewShape = nullptr;
    }

    Tool tool = m_tools->currentTool();
    QColor previewColor(115, 0, 10, 100);  // Semi-transparent garnet

    if (tool == Tool::Rectangle) {
        QRectF rect(toScreen(m_shapeStartPos), toScreen(currentPos));
        QGraphicsRectItem *rectItem = new QGraphicsRectItem(rect.normalized());
        rectItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        rectItem->setBrush(QBrush(previewColor));
        m_previewShape = rectItem;
    }
    else if (tool == Tool::Circle) {
        QPointF delta = currentPos - m_shapeStartPos;
        float radius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        QGraphicsEllipseItem *circleItem = new QGraphicsEllipseItem(
            toScreen(m_shapeStartPos).x() - toScreen(radius),
            toScreen(m_shapeStartPos).y() - toScreen(radius),
            toScreen(radius) * 2, toScreen(radius) * 2);
        circleItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        circleItem->setBrush(QBrush(previewColor));
        m_previewShape = circleItem;
    }
    else if (tool == Tool::Ellipse) {
        float rx = std::abs(currentPos.x() - m_shapeStartPos.x());
        float ry = std::abs(currentPos.y() - m_shapeStartPos.y());
        QGraphicsEllipseItem *ellipseItem = new QGraphicsEllipseItem(
            toScreen(m_shapeStartPos).x() - toScreen(rx),
            toScreen(m_shapeStartPos).y() - toScreen(ry),
            toScreen(rx) * 2, toScreen(ry) * 2);
        ellipseItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        ellipseItem->setBrush(QBrush(previewColor));
        m_previewShape = ellipseItem;
    }
    else if (tool == Tool::Triangle || tool == Tool::RegularPolygon) {
        int sides = (tool == Tool::Triangle) ? 3 : m_regPolySides;
        QPointF delta = currentPos - m_shapeStartPos;
        float radius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        float startAngle = std::atan2(delta.y(), delta.x());

        QPolygonF poly;
        for (int i = 0; i < sides; i++) {
            float angle = startAngle + 2.0 * M_PI * i / sides;
            QPointF pt = m_shapeStartPos + QPointF(radius * std::cos(angle), radius * std::sin(angle));
            poly << toScreen(pt);
        }

        QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(poly);
        polyItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        polyItem->setBrush(QBrush(previewColor));
        m_previewShape = polyItem;
    }

    if (m_previewShape) {
        addItem(m_previewShape);
    }
}

void Canvas::finishShapeDrag(QGraphicsSceneMouseEvent *event) {
    if (!m_tools) return;

    // Remove preview
    if (m_previewShape) {
        removeItem(m_previewShape);
        delete m_previewShape;
        m_previewShape = nullptr;
    }

    QPointF endPos = fromScreen(event->scenePos());
    endPos = snapToGrid(endPos, GRID_MINOR);

    Tool tool = m_tools->currentTool();
    QColor fillColor = m_colors ? m_colors->currentColor() : UofSC::Garnet();

    if (tool == Tool::Rectangle) {
        Polygon *poly = m_document->addPolygon();
        poly->setFillColor(fillColor);

        float x1 = qMin(m_shapeStartPos.x(), endPos.x());
        float y1 = qMin(m_shapeStartPos.y(), endPos.y());
        float x2 = qMax(m_shapeStartPos.x(), endPos.x());
        float y2 = qMax(m_shapeStartPos.y(), endPos.y());

        poly->addVertex(QPointF(x1, y1));
        poly->addVertex(QPointF(x2, y1));
        poly->addVertex(QPointF(x2, y2));
        poly->addVertex(QPointF(x1, y2));

        emit statusMessage("Rectangle created");
    }
    else if (tool == Tool::Circle) {
        QPointF delta = endPos - m_shapeStartPos;
        float radius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());

        Ellipse *ellipse = m_document->addCircle(m_shapeStartPos, radius);
        ellipse->setFillColor(fillColor);

        emit statusMessage("Circle created");
    }
    else if (tool == Tool::Ellipse) {
        float rx = std::abs(endPos.x() - m_shapeStartPos.x());
        float ry = std::abs(endPos.y() - m_shapeStartPos.y());

        Ellipse *ellipse = m_document->addEllipse(m_shapeStartPos, rx, ry);
        ellipse->setFillColor(fillColor);

        emit statusMessage("Ellipse created");
    }
    else if (tool == Tool::Triangle || tool == Tool::RegularPolygon) {
        int sides = (tool == Tool::Triangle) ? 3 : m_regPolySides;
        QPointF delta = endPos - m_shapeStartPos;
        float radius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        float startAngle = std::atan2(delta.y(), delta.x());

        Polygon *poly = m_document->addPolygon();
        poly->setFillColor(fillColor);

        for (int i = 0; i < sides; i++) {
            float angle = startAngle + 2.0 * M_PI * i / sides;
            QPointF pt = m_shapeStartPos + QPointF(radius * std::cos(angle), radius * std::sin(angle));
            poly->addVertex(pt);
        }

        emit statusMessage(QString("%1 created").arg(tool == Tool::Triangle ? "Triangle" : "Regular polygon"));
    }
}
