#include "canvas.h"
#include "document.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"
#include "group.h"
#include "commands.h"
#include "pointitem.h"
#include "lineitem.h"
#include "polygonitem.h"
#include "curveitem.h"
#include "ellipseitem.h"
#include "groupitem.h"
#include "toolpalette.h"
#include "colorpalette.h"
#include "propertiespanel.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QJsonObject>
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

    for (auto it = m_groupItems.begin(); it != m_groupItems.end(); ) {
        if (!m_document->groups().contains(it.key())) {
            removeItem(it.value());
            delete it.value();
            it = m_groupItems.erase(it);
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
    if (selected.isEmpty()) return;

    // Collect all shapes to delete
    QVector<Point*> points;
    QVector<Line*> lines;
    QVector<Polygon*> polygons;
    QVector<Curve*> curves;
    QVector<Ellipse*> ellipses;

    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            points.append(pi->point());
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            lines.append(li->line());
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            polygons.append(pi->polygon());
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            curves.append(ci->curve());
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            ellipses.append(ei->ellipse());
        }
    }

    // Create composite delete command and push to undo stack
    m_document->undoStack()->push(
        new DeleteSelectionCommand(m_document, points, lines, polygons, curves, ellipses)
    );
}

void Canvas::selectAll() {
    for (QGraphicsItem *item : items()) {
        if (item->flags() & QGraphicsItem::ItemIsSelectable) {
            item->setSelected(true);
        }
    }
}

// Helper functions for clipboard serialization
namespace {

QJsonObject serializePoint(Point *p) {
    QJsonObject obj;
    obj["name"] = p->name();
    obj["x"] = p->pos().x();
    obj["y"] = p->pos().y();
    obj["color"] = p->color().name();
    obj["radius"] = p->radius();
    obj["visible"] = p->isVisible();
    return obj;
}

QJsonObject serializeLine(Line *l) {
    QJsonObject obj;
    obj["startX"] = l->startPos().x();
    obj["startY"] = l->startPos().y();
    obj["endX"] = l->endPos().x();
    obj["endY"] = l->endPos().y();
    obj["color"] = l->color().name();
    obj["lineWidth"] = l->lineWidth();
    obj["lineStyle"] = static_cast<int>(l->lineStyle());
    obj["lineCap"] = static_cast<int>(l->lineCap());
    obj["lineJoin"] = static_cast<int>(l->lineJoin());
    obj["cornerRadius"] = l->cornerRadius();
    obj["rotation"] = l->rotation();
    obj["scale"] = l->scale();
    return obj;
}

QJsonObject serializePolygon(Polygon *p) {
    QJsonObject obj;
    QJsonArray verts;
    for (const Vertex &v : p->vertices()) {
        QJsonObject vobj;
        vobj["x"] = v.pos.x();
        vobj["y"] = v.pos.y();
        vobj["cornerRadius"] = v.cornerRadius;
        verts.append(vobj);
    }
    obj["vertices"] = verts;
    obj["fillColor"] = p->fillColor().name();
    obj["strokeColor"] = p->strokeColor().name();
    obj["lineWidth"] = p->lineWidth();
    obj["lineStyle"] = static_cast<int>(p->lineStyle());
    obj["lineCap"] = static_cast<int>(p->lineCap());
    obj["lineJoin"] = static_cast<int>(p->lineJoin());
    obj["fillPattern"] = static_cast<int>(p->fillPattern());
    obj["opacity"] = p->opacity();
    obj["rotation"] = p->rotation();
    obj["scale"] = p->scale();
    obj["defaultCornerRadius"] = p->defaultCornerRadius();
    return obj;
}

QJsonObject serializeCurve(Curve *c) {
    QJsonObject obj;
    QJsonArray pts;
    for (const QPointF &pt : c->controlPoints()) {
        QJsonObject v;
        v["x"] = pt.x();
        v["y"] = pt.y();
        pts.append(v);
    }
    obj["controlPoints"] = pts;
    obj["curveType"] = static_cast<int>(c->curveType());
    obj["tension"] = c->tension();
    obj["color"] = c->color().name();
    obj["lineWidth"] = c->lineWidth();
    obj["lineStyle"] = static_cast<int>(c->lineStyle());
    obj["lineCap"] = static_cast<int>(c->lineCap());
    obj["lineJoin"] = static_cast<int>(c->lineJoin());
    obj["closed"] = c->isClosed();
    obj["rotation"] = c->rotation();
    obj["scale"] = c->scale();
    return obj;
}

QJsonObject serializeEllipse(Ellipse *e) {
    QJsonObject obj;
    obj["centerX"] = e->center().x();
    obj["centerY"] = e->center().y();
    obj["radiusX"] = e->radiusX();
    obj["radiusY"] = e->radiusY();
    obj["rotation"] = e->rotation();
    obj["scale"] = e->scale();
    obj["fillColor"] = e->fillColor().name();
    obj["strokeColor"] = e->strokeColor().name();
    obj["lineWidth"] = e->lineWidth();
    obj["lineStyle"] = static_cast<int>(e->lineStyle());
    obj["lineCap"] = static_cast<int>(e->lineCap());
    obj["lineJoin"] = static_cast<int>(e->lineJoin());
    obj["fillPattern"] = static_cast<int>(e->fillPattern());
    obj["opacity"] = e->opacity();
    return obj;
}

void deserializeToPoint(Point *p, const QJsonObject &obj) {
    p->setName(obj["name"].toString());
    p->setPos(QPointF(obj["x"].toDouble(), obj["y"].toDouble()));
    p->setColor(QColor(obj["color"].toString()));
    p->setRadius(obj["radius"].toDouble(2.0));
    p->setVisible(obj["visible"].toBool(true));
}

void deserializeToLine(Line *l, const QJsonObject &obj) {
    l->setStartPos(QPointF(obj["startX"].toDouble(), obj["startY"].toDouble()));
    l->setEndPos(QPointF(obj["endX"].toDouble(), obj["endY"].toDouble()));
    l->setColor(QColor(obj["color"].toString()));
    l->setLineWidth(obj["lineWidth"].toDouble(0.8));
    l->setLineStyle(static_cast<LineStyle>(obj["lineStyle"].toInt(0)));
    l->setLineCap(static_cast<LineCap>(obj["lineCap"].toInt(0)));
    l->setLineJoin(static_cast<LineJoin>(obj["lineJoin"].toInt(0)));
    l->setCornerRadius(obj["cornerRadius"].toDouble());
    l->setRotation(obj["rotation"].toDouble(0.0));
    l->setScale(obj["scale"].toDouble(1.0));
}

void deserializeToPolygon(Polygon *p, const QJsonObject &obj) {
    p->clear();
    for (const QJsonValue &vv : obj["vertices"].toArray()) {
        QJsonObject pt = vv.toObject();
        p->addVertex(QPointF(pt["x"].toDouble(), pt["y"].toDouble()),
                    pt["cornerRadius"].toDouble(0.0));
    }
    p->setFillColor(QColor(obj["fillColor"].toString()));
    p->setStrokeColor(QColor(obj["strokeColor"].toString()));
    p->setLineWidth(obj["lineWidth"].toDouble(0.8));
    p->setLineStyle(static_cast<LineStyle>(obj["lineStyle"].toInt(0)));
    p->setLineCap(static_cast<LineCap>(obj["lineCap"].toInt(0)));
    p->setLineJoin(static_cast<LineJoin>(obj["lineJoin"].toInt(0)));
    p->setFillPattern(static_cast<FillPattern>(obj["fillPattern"].toInt(1)));
    p->setOpacity(obj["opacity"].toDouble(1.0));
    p->setRotation(obj["rotation"].toDouble(0.0));
    p->setScale(obj["scale"].toDouble(1.0));
    p->setDefaultCornerRadius(obj["defaultCornerRadius"].toDouble(0.0));
}

void deserializeToCurve(Curve *c, const QJsonObject &obj) {
    c->clear();
    for (const QJsonValue &vv : obj["controlPoints"].toArray()) {
        QJsonObject pt = vv.toObject();
        c->addControlPoint(QPointF(pt["x"].toDouble(), pt["y"].toDouble()));
    }
    c->setCurveType(static_cast<Curve::CurveType>(obj["curveType"].toInt()));
    c->setTension(obj["tension"].toDouble(0.5));
    c->setColor(QColor(obj["color"].toString()));
    c->setLineWidth(obj["lineWidth"].toDouble(0.8));
    c->setLineStyle(static_cast<LineStyle>(obj["lineStyle"].toInt(0)));
    c->setLineCap(static_cast<LineCap>(obj["lineCap"].toInt(0)));
    c->setLineJoin(static_cast<LineJoin>(obj["lineJoin"].toInt(0)));
    c->setClosed(obj["closed"].toBool());
    c->setRotation(obj["rotation"].toDouble(0.0));
    c->setScale(obj["scale"].toDouble(1.0));
}

void deserializeToEllipse(Ellipse *e, const QJsonObject &obj) {
    e->setCenter(QPointF(obj["centerX"].toDouble(), obj["centerY"].toDouble()));
    e->setRadiusX(obj["radiusX"].toDouble(1.0));
    e->setRadiusY(obj["radiusY"].toDouble(1.0));
    e->setRotation(obj["rotation"].toDouble(0.0));
    e->setScale(obj["scale"].toDouble(1.0));
    e->setFillColor(QColor(obj["fillColor"].toString()));
    e->setStrokeColor(QColor(obj["strokeColor"].toString()));
    e->setLineWidth(obj["lineWidth"].toDouble(0.8));
    e->setLineStyle(static_cast<LineStyle>(obj["lineStyle"].toInt(0)));
    e->setLineCap(static_cast<LineCap>(obj["lineCap"].toInt(0)));
    e->setLineJoin(static_cast<LineJoin>(obj["lineJoin"].toInt(0)));
    e->setFillPattern(static_cast<FillPattern>(obj["fillPattern"].toInt(1)));
    e->setOpacity(obj["opacity"].toDouble(1.0));
}

} // anonymous namespace

void Canvas::copySelected() {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.isEmpty()) return;

    // Clear previous clipboard
    m_clipboardPoints = QJsonArray();
    m_clipboardLines = QJsonArray();
    m_clipboardPolygons = QJsonArray();
    m_clipboardCurves = QJsonArray();
    m_clipboardEllipses = QJsonArray();

    // Calculate bounding rect center for paste offset
    QRectF bounds;
    bool first = true;

    for (QGraphicsItem *item : selected) {
        QRectF itemBounds = item->boundingRect();
        itemBounds.translate(item->pos());
        if (first) {
            bounds = itemBounds;
            first = false;
        } else {
            bounds = bounds.united(itemBounds);
        }

        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            m_clipboardPoints.append(serializePoint(pi->point()));
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            m_clipboardLines.append(serializeLine(li->line()));
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            m_clipboardPolygons.append(serializePolygon(pi->polygon()));
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            m_clipboardCurves.append(serializeCurve(ci->curve()));
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            m_clipboardEllipses.append(serializeEllipse(ei->ellipse()));
        }
    }

    m_clipboardCenter = bounds.center();
    emit statusMessage(QString("Copied %1 item(s)").arg(selected.size()));
}

void Canvas::cutSelected() {
    copySelected();
    deleteSelected();
}

void Canvas::paste() {
    if (m_clipboardPoints.isEmpty() && m_clipboardLines.isEmpty() &&
        m_clipboardPolygons.isEmpty() && m_clipboardCurves.isEmpty() &&
        m_clipboardEllipses.isEmpty()) {
        return;
    }

    // Clear selection
    clearSelection();

    // Offset for paste (0.5 units down and right)
    QPointF offset(0.5, -0.5);

    int count = 0;

    // Paste points
    for (const QJsonValue &v : m_clipboardPoints) {
        QJsonObject obj = v.toObject();
        Point *p = m_document->addPoint(QPointF(
            obj["x"].toDouble() + offset.x(),
            obj["y"].toDouble() + offset.y()
        ));
        deserializeToPoint(p, obj);
        p->setPos(p->pos() + offset);  // Apply offset after restore
        count++;

        // Select newly pasted item
        if (m_pointItems.contains(p)) {
            m_pointItems[p]->setSelected(true);
        }
    }

    // Paste lines
    for (const QJsonValue &v : m_clipboardLines) {
        QJsonObject obj = v.toObject();
        Line *l = m_document->addLine(
            QPointF(obj["startX"].toDouble() + offset.x(), obj["startY"].toDouble() + offset.y()),
            QPointF(obj["endX"].toDouble() + offset.x(), obj["endY"].toDouble() + offset.y())
        );
        deserializeToLine(l, obj);
        l->setStartPos(l->startPos() + offset);
        l->setEndPos(l->endPos() + offset);
        count++;

        if (m_lineItems.contains(l)) {
            m_lineItems[l]->setSelected(true);
        }
    }

    // Paste polygons
    for (const QJsonValue &v : m_clipboardPolygons) {
        QJsonObject obj = v.toObject();
        Polygon *p = m_document->addPolygon();
        deserializeToPolygon(p, obj);
        // Offset all vertices
        for (int i = 0; i < p->vertexCount(); ++i) {
            p->setVertexPosition(i, p->vertices()[i].pos + offset);
        }
        count++;

        if (m_polygonItems.contains(p)) {
            m_polygonItems[p]->setSelected(true);
        }
    }

    // Paste curves
    for (const QJsonValue &v : m_clipboardCurves) {
        QJsonObject obj = v.toObject();
        Curve *c = m_document->addCurve();
        deserializeToCurve(c, obj);
        // Offset all control points
        for (int i = 0; i < c->controlPointCount(); ++i) {
            c->setControlPoint(i, c->controlPoints()[i] + offset);
        }
        count++;

        if (m_curveItems.contains(c)) {
            m_curveItems[c]->setSelected(true);
        }
    }

    // Paste ellipses
    for (const QJsonValue &v : m_clipboardEllipses) {
        QJsonObject obj = v.toObject();
        Ellipse *e = m_document->addEllipse();
        deserializeToEllipse(e, obj);
        e->setCenter(e->center() + offset);
        count++;

        if (m_ellipseItems.contains(e)) {
            m_ellipseItems[e]->setSelected(true);
        }
    }

    // Update clipboard center for next paste
    m_clipboardCenter += offset;

    emit statusMessage(QString("Pasted %1 item(s)").arg(count));
}

void Canvas::duplicate() {
    copySelected();
    paste();
}

// ============================================================================
// Z-Ordering
// ============================================================================

void Canvas::bringToFront() {
    QList<QGraphicsItem*> selected = selectedItems();
    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            m_document->bringToFront(pi->point(), ShapeType::Point);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            m_document->bringToFront(li->line(), ShapeType::Line);
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            m_document->bringToFront(pi->polygon(), ShapeType::Polygon);
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            m_document->bringToFront(ci->curve(), ShapeType::Curve);
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            m_document->bringToFront(ei->ellipse(), ShapeType::Ellipse);
        }
    }
}

void Canvas::sendToBack() {
    QList<QGraphicsItem*> selected = selectedItems();
    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            m_document->sendToBack(pi->point(), ShapeType::Point);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            m_document->sendToBack(li->line(), ShapeType::Line);
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            m_document->sendToBack(pi->polygon(), ShapeType::Polygon);
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            m_document->sendToBack(ci->curve(), ShapeType::Curve);
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            m_document->sendToBack(ei->ellipse(), ShapeType::Ellipse);
        }
    }
}

void Canvas::bringForward() {
    QList<QGraphicsItem*> selected = selectedItems();
    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            m_document->bringForward(pi->point(), ShapeType::Point);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            m_document->bringForward(li->line(), ShapeType::Line);
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            m_document->bringForward(pi->polygon(), ShapeType::Polygon);
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            m_document->bringForward(ci->curve(), ShapeType::Curve);
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            m_document->bringForward(ei->ellipse(), ShapeType::Ellipse);
        }
    }
}

void Canvas::sendBackward() {
    QList<QGraphicsItem*> selected = selectedItems();
    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            m_document->sendBackward(pi->point(), ShapeType::Point);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            m_document->sendBackward(li->line(), ShapeType::Line);
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            m_document->sendBackward(pi->polygon(), ShapeType::Polygon);
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            m_document->sendBackward(ci->curve(), ShapeType::Curve);
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            m_document->sendBackward(ei->ellipse(), ShapeType::Ellipse);
        }
    }
}

// ============================================================================
// Flip/Mirror Operations
// ============================================================================

void Canvas::flipHorizontal() {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.isEmpty()) return;

    // Calculate bounding box center
    QRectF bounds;
    bool first = true;
    for (QGraphicsItem *item : selected) {
        QRectF itemBounds = item->sceneBoundingRect();
        if (first) {
            bounds = itemBounds;
            first = false;
        } else {
            bounds = bounds.united(itemBounds);
        }
    }
    float centerX = bounds.center().x();

    // Flip each shape around the center
    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            Point *p = pi->point();
            QPointF pos = p->pos();
            pos.setX(2 * centerX - pos.x());
            p->setPos(pos);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            Line *l = li->line();
            QPointF start = l->startPos();
            QPointF end = l->endPos();
            start.setX(2 * centerX - start.x());
            end.setX(2 * centerX - end.x());
            l->setStartPos(start);
            l->setEndPos(end);
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            Polygon *p = pi->polygon();
            for (int i = 0; i < p->vertexCount(); ++i) {
                QPointF pos = p->vertices()[i].pos;
                pos.setX(2 * centerX - pos.x());
                p->setVertexPosition(i, pos);
            }
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            Curve *c = ci->curve();
            for (int i = 0; i < c->controlPointCount(); ++i) {
                QPointF pos = c->controlPoints()[i];
                pos.setX(2 * centerX - pos.x());
                c->setControlPoint(i, pos);
            }
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            Ellipse *e = ei->ellipse();
            QPointF center = e->center();
            center.setX(2 * centerX - center.x());
            e->setCenter(center);
        }
    }

    emit statusMessage("Flipped horizontally");
}

void Canvas::flipVertical() {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.isEmpty()) return;

    // Calculate bounding box center
    QRectF bounds;
    bool first = true;
    for (QGraphicsItem *item : selected) {
        QRectF itemBounds = item->sceneBoundingRect();
        if (first) {
            bounds = itemBounds;
            first = false;
        } else {
            bounds = bounds.united(itemBounds);
        }
    }
    float centerY = bounds.center().y();

    // Flip each shape around the center
    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            Point *p = pi->point();
            QPointF pos = p->pos();
            pos.setY(2 * centerY - pos.y());
            p->setPos(pos);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            Line *l = li->line();
            QPointF start = l->startPos();
            QPointF end = l->endPos();
            start.setY(2 * centerY - start.y());
            end.setY(2 * centerY - end.y());
            l->setStartPos(start);
            l->setEndPos(end);
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            Polygon *p = pi->polygon();
            for (int i = 0; i < p->vertexCount(); ++i) {
                QPointF pos = p->vertices()[i].pos;
                pos.setY(2 * centerY - pos.y());
                p->setVertexPosition(i, pos);
            }
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            Curve *c = ci->curve();
            for (int i = 0; i < c->controlPointCount(); ++i) {
                QPointF pos = c->controlPoints()[i];
                pos.setY(2 * centerY - pos.y());
                c->setControlPoint(i, pos);
            }
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            Ellipse *e = ei->ellipse();
            QPointF center = e->center();
            center.setY(2 * centerY - center.y());
            e->setCenter(center);
        }
    }

    emit statusMessage("Flipped vertically");
}

// ============================================================================
// Distribute Operations (for 3+ identical shapes)
// ============================================================================

namespace {

// Check if two polygons are "identical" (same vertex count, same relative positions, same scale)
bool arePolygonsIdentical(Polygon *a, Polygon *b) {
    if (a->vertexCount() != b->vertexCount()) return false;
    if (a->vertexCount() < 2) return true;
    if (!qFuzzyCompare(a->scale(), b->scale())) return false;
    if (!qFuzzyCompare(a->rotation(), b->rotation())) return false;

    // Compare relative vertex positions (shape should be the same)
    QVector<Vertex> va = a->vertices();
    QVector<Vertex> vb = b->vertices();

    // Calculate bounding boxes
    float minAx = va[0].pos.x(), maxAx = va[0].pos.x();
    float minAy = va[0].pos.y(), maxAy = va[0].pos.y();
    float minBx = vb[0].pos.x(), maxBx = vb[0].pos.x();
    float minBy = vb[0].pos.y(), maxBy = vb[0].pos.y();

    for (const Vertex &v : va) {
        minAx = qMin(minAx, (float)v.pos.x());
        maxAx = qMax(maxAx, (float)v.pos.x());
        minAy = qMin(minAy, (float)v.pos.y());
        maxAy = qMax(maxAy, (float)v.pos.y());
    }
    for (const Vertex &v : vb) {
        minBx = qMin(minBx, (float)v.pos.x());
        maxBx = qMax(maxBx, (float)v.pos.x());
        minBy = qMin(minBy, (float)v.pos.y());
        maxBy = qMax(maxBy, (float)v.pos.y());
    }

    float widthA = maxAx - minAx;
    float heightA = maxAy - minAy;
    float widthB = maxBx - minBx;
    float heightB = maxBy - minBy;

    // Check if dimensions match
    if (!qFuzzyCompare(widthA + 1, widthB + 1) || !qFuzzyCompare(heightA + 1, heightB + 1))
        return false;

    return true;
}

bool areEllipsesIdentical(Ellipse *a, Ellipse *b) {
    if (!qFuzzyCompare(a->radiusX(), b->radiusX())) return false;
    if (!qFuzzyCompare(a->radiusY(), b->radiusY())) return false;
    if (!qFuzzyCompare(a->rotation(), b->rotation())) return false;
    if (!qFuzzyCompare(a->scale(), b->scale())) return false;
    return true;
}

bool areLinesIdentical(Line *a, Line *b) {
    // Lines are identical if they have the same length and rotation
    QPointF da = a->endPos() - a->startPos();
    QPointF db = b->endPos() - b->startPos();
    float lenA = std::sqrt(da.x()*da.x() + da.y()*da.y());
    float lenB = std::sqrt(db.x()*db.x() + db.y()*db.y());
    if (!qFuzzyCompare(lenA + 1, lenB + 1)) return false;

    // Check angle
    float angleA = std::atan2(da.y(), da.x());
    float angleB = std::atan2(db.y(), db.x());
    if (!qFuzzyCompare(angleA + 10, angleB + 10)) return false;

    return true;
}

} // anonymous namespace

void Canvas::distributeHorizontally() {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.size() < 3) {
        emit statusMessage("Select 3 or more identical shapes to distribute");
        return;
    }

    // Check if all shapes are the same type and identical
    bool allPolygons = true, allEllipses = true, allLines = true, allPoints = true;

    for (QGraphicsItem *item : selected) {
        if (!qgraphicsitem_cast<PolygonItem*>(item)) allPolygons = false;
        if (!qgraphicsitem_cast<EllipseItem*>(item)) allEllipses = false;
        if (!qgraphicsitem_cast<LineItem*>(item)) allLines = false;
        if (!qgraphicsitem_cast<PointItem*>(item)) allPoints = false;
    }

    if (!allPolygons && !allEllipses && !allLines && !allPoints) {
        emit statusMessage("All shapes must be the same type to distribute");
        return;
    }

    // Collect centers and check identity
    QVector<QPair<QGraphicsItem*, QPointF>> itemCenters;

    if (allPolygons) {
        Polygon *first = qgraphicsitem_cast<PolygonItem*>(selected[0])->polygon();
        for (QGraphicsItem *item : selected) {
            Polygon *p = qgraphicsitem_cast<PolygonItem*>(item)->polygon();
            if (!arePolygonsIdentical(first, p)) {
                emit statusMessage("Shapes must be identical (same size/orientation) to distribute");
                return;
            }
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    } else if (allEllipses) {
        Ellipse *first = qgraphicsitem_cast<EllipseItem*>(selected[0])->ellipse();
        for (QGraphicsItem *item : selected) {
            Ellipse *e = qgraphicsitem_cast<EllipseItem*>(item)->ellipse();
            if (!areEllipsesIdentical(first, e)) {
                emit statusMessage("Shapes must be identical (same size/orientation) to distribute");
                return;
            }
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    } else if (allLines) {
        Line *first = qgraphicsitem_cast<LineItem*>(selected[0])->line();
        for (QGraphicsItem *item : selected) {
            Line *l = qgraphicsitem_cast<LineItem*>(item)->line();
            if (!areLinesIdentical(first, l)) {
                emit statusMessage("Shapes must be identical (same length/orientation) to distribute");
                return;
            }
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    } else if (allPoints) {
        for (QGraphicsItem *item : selected) {
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    }

    // Sort by X position
    std::sort(itemCenters.begin(), itemCenters.end(),
              [](const QPair<QGraphicsItem*, QPointF> &a, const QPair<QGraphicsItem*, QPointF> &b) {
                  return a.second.x() < b.second.x();
              });

    // Calculate even spacing
    float minX = itemCenters.first().second.x();
    float maxX = itemCenters.last().second.x();
    float spacing = (maxX - minX) / (itemCenters.size() - 1);

    // Apply new positions
    for (int i = 1; i < itemCenters.size() - 1; ++i) {
        QGraphicsItem *item = itemCenters[i].first;
        QPointF oldCenter = itemCenters[i].second;
        float newX = minX + i * spacing;
        float dx = newX - oldCenter.x();

        if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            Polygon *p = pi->polygon();
            for (int j = 0; j < p->vertexCount(); ++j) {
                QPointF pos = p->vertices()[j].pos;
                pos.setX(pos.x() + dx);
                p->setVertexPosition(j, pos);
            }
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            Ellipse *e = ei->ellipse();
            QPointF center = e->center();
            center.setX(center.x() + dx);
            e->setCenter(center);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            Line *l = li->line();
            l->setStartPos(l->startPos() + QPointF(dx, 0));
            l->setEndPos(l->endPos() + QPointF(dx, 0));
        } else if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            Point *p = pi->point();
            QPointF pos = p->pos();
            pos.setX(pos.x() + dx);
            p->setPos(pos);
        }
    }

    emit statusMessage(QString("Distributed %1 shapes horizontally").arg(selected.size()));
}

void Canvas::distributeVertically() {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.size() < 3) {
        emit statusMessage("Select 3 or more identical shapes to distribute");
        return;
    }

    // Check if all shapes are the same type
    bool allPolygons = true, allEllipses = true, allLines = true, allPoints = true;

    for (QGraphicsItem *item : selected) {
        if (!qgraphicsitem_cast<PolygonItem*>(item)) allPolygons = false;
        if (!qgraphicsitem_cast<EllipseItem*>(item)) allEllipses = false;
        if (!qgraphicsitem_cast<LineItem*>(item)) allLines = false;
        if (!qgraphicsitem_cast<PointItem*>(item)) allPoints = false;
    }

    if (!allPolygons && !allEllipses && !allLines && !allPoints) {
        emit statusMessage("All shapes must be the same type to distribute");
        return;
    }

    // Collect centers and check identity
    QVector<QPair<QGraphicsItem*, QPointF>> itemCenters;

    if (allPolygons) {
        Polygon *first = qgraphicsitem_cast<PolygonItem*>(selected[0])->polygon();
        for (QGraphicsItem *item : selected) {
            Polygon *p = qgraphicsitem_cast<PolygonItem*>(item)->polygon();
            if (!arePolygonsIdentical(first, p)) {
                emit statusMessage("Shapes must be identical (same size/orientation) to distribute");
                return;
            }
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    } else if (allEllipses) {
        Ellipse *first = qgraphicsitem_cast<EllipseItem*>(selected[0])->ellipse();
        for (QGraphicsItem *item : selected) {
            Ellipse *e = qgraphicsitem_cast<EllipseItem*>(item)->ellipse();
            if (!areEllipsesIdentical(first, e)) {
                emit statusMessage("Shapes must be identical (same size/orientation) to distribute");
                return;
            }
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    } else if (allLines) {
        Line *first = qgraphicsitem_cast<LineItem*>(selected[0])->line();
        for (QGraphicsItem *item : selected) {
            Line *l = qgraphicsitem_cast<LineItem*>(item)->line();
            if (!areLinesIdentical(first, l)) {
                emit statusMessage("Shapes must be identical (same length/orientation) to distribute");
                return;
            }
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    } else if (allPoints) {
        for (QGraphicsItem *item : selected) {
            itemCenters.append({item, item->sceneBoundingRect().center()});
        }
    }

    // Sort by Y position
    std::sort(itemCenters.begin(), itemCenters.end(),
              [](const QPair<QGraphicsItem*, QPointF> &a, const QPair<QGraphicsItem*, QPointF> &b) {
                  return a.second.y() < b.second.y();
              });

    // Calculate even spacing
    float minY = itemCenters.first().second.y();
    float maxY = itemCenters.last().second.y();
    float spacing = (maxY - minY) / (itemCenters.size() - 1);

    // Apply new positions
    for (int i = 1; i < itemCenters.size() - 1; ++i) {
        QGraphicsItem *item = itemCenters[i].first;
        QPointF oldCenter = itemCenters[i].second;
        float newY = minY + i * spacing;
        float dy = newY - oldCenter.y();

        if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            Polygon *p = pi->polygon();
            for (int j = 0; j < p->vertexCount(); ++j) {
                QPointF pos = p->vertices()[j].pos;
                pos.setY(pos.y() + dy);
                p->setVertexPosition(j, pos);
            }
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            Ellipse *e = ei->ellipse();
            QPointF center = e->center();
            center.setY(center.y() + dy);
            e->setCenter(center);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            Line *l = li->line();
            l->setStartPos(l->startPos() + QPointF(0, dy));
            l->setEndPos(l->endPos() + QPointF(0, dy));
        } else if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            Point *p = pi->point();
            QPointF pos = p->pos();
            pos.setY(pos.y() + dy);
            p->setPos(pos);
        }
    }

    emit statusMessage(QString("Distributed %1 shapes vertically").arg(selected.size()));
}

void Canvas::groupSelected() {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.size() < 2) {
        emit statusMessage("Select 2 or more shapes to group");
        return;
    }

    // Create a new group in the document
    Group *group = m_document->addGroup();

    // Add all selected shapes to the group
    for (QGraphicsItem *item : selected) {
        if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
            group->addMember(pi->point(), GroupedShapeType::Point);
        } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
            group->addMember(li->line(), GroupedShapeType::Line);
        } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
            group->addMember(pi->polygon(), GroupedShapeType::Polygon);
        } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
            group->addMember(ci->curve(), GroupedShapeType::Curve);
        } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
            group->addMember(ei->ellipse(), GroupedShapeType::Ellipse);
        }
    }

    // Create group item and add children
    GroupItem *groupItem = new GroupItem(group);
    for (QGraphicsItem *item : selected) {
        item->setSelected(false);
        groupItem->addToGroup(item);
    }
    addItem(groupItem);
    m_groupItems[group] = groupItem;

    groupItem->setSelected(true);
    emit statusMessage(QString("Grouped %1 shapes").arg(selected.size()));
}

void Canvas::ungroupSelected() {
    QList<QGraphicsItem*> selected = selectedItems();

    int ungroupedCount = 0;
    for (QGraphicsItem *item : selected) {
        if (GroupItem *gi = qgraphicsitem_cast<GroupItem*>(item)) {
            Group *group = gi->group();

            // Remove all children from the group item and restore them to scene
            QList<QGraphicsItem*> children = gi->childItems();
            for (QGraphicsItem *child : children) {
                gi->removeFromGroup(child);
                child->setSelected(true);
            }

            // Remove group from document
            m_groupItems.remove(group);
            removeItem(gi);
            delete gi;
            m_document->removeGroup(group);

            ungroupedCount++;
        }
    }

    if (ungroupedCount > 0) {
        emit statusMessage(QString("Ungrouped %1 group(s)").arg(ungroupedCount));
    } else {
        emit statusMessage("No groups selected to ungroup");
    }
}

void Canvas::addGroupItem(Group *group) {
    if (m_groupItems.contains(group)) return;

    GroupItem *item = new GroupItem(group);
    addItem(item);
    m_groupItems[group] = item;
}

void Canvas::setGridVisible(bool visible) {
    if (m_gridVisible != visible) {
        m_gridVisible = visible;
        invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
    }
}

void Canvas::setAxesVisible(bool visible) {
    if (m_axesVisible != visible) {
        m_axesVisible = visible;
        invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
    }
}

void Canvas::setAxisTicksVisible(bool visible) {
    if (m_axisTicksVisible != visible) {
        m_axisTicksVisible = visible;
        invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
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
        tool == Tool::RegularPolygon || tool == Tool::Arc ||
        tool == Tool::Star || tool == Tool::Diamond ||
        tool == Tool::Arrow || tool == Tool::Trapezoid ||
        tool == Tool::Parallelogram) {
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

        // Apply defaults from properties panel
        DrawingDefaults defaults;
        if (m_propertiesPanel) {
            defaults = m_propertiesPanel->drawingDefaults();
        }
        line->setColor(defaults.strokeColor);
        line->setLineWidth(defaults.lineWidth);
        line->setLineStyle(defaults.lineStyle);
        line->setLineCap(defaults.lineCap);
        line->setLineJoin(defaults.lineJoin);
        line->setRotation(defaults.rotation);
        line->setScale(defaults.scale);

        emit statusMessage("Line created");
    }
}

void Canvas::handlePolygonTool(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF tikzPos = fromScreen(event->scenePos());
        tikzPos = snapToGrid(tikzPos, GRID_MINOR);

        if (!m_currentPolygon) {
            m_currentPolygon = m_document->addPolygon();
            m_currentPolygon->setPolygonType(PolygonType::Freeform);

            // Apply defaults from properties panel
            DrawingDefaults defaults;
            if (m_propertiesPanel) {
                defaults = m_propertiesPanel->drawingDefaults();
            }
            m_currentPolygon->setFillColor(defaults.fillColor);
            m_currentPolygon->setStrokeColor(defaults.strokeColor);
            m_currentPolygon->setLineWidth(defaults.lineWidth);
            m_currentPolygon->setLineStyle(defaults.lineStyle);
            m_currentPolygon->setLineCap(defaults.lineCap);
            m_currentPolygon->setLineJoin(defaults.lineJoin);
            m_currentPolygon->setFillPattern(defaults.fillPattern);
            m_currentPolygon->setOpacity(defaults.opacity);
            m_currentPolygon->setRotation(defaults.rotation);
            m_currentPolygon->setScale(defaults.scale);
            m_currentPolygon->setDefaultCornerRadius(defaults.cornerRadius);

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

            // Apply defaults from properties panel
            DrawingDefaults defaults;
            if (m_propertiesPanel) {
                defaults = m_propertiesPanel->drawingDefaults();
            }
            m_currentCurve->setColor(defaults.strokeColor);
            m_currentCurve->setLineWidth(defaults.lineWidth);
            m_currentCurve->setLineStyle(defaults.lineStyle);
            m_currentCurve->setLineCap(defaults.lineCap);
            m_currentCurve->setLineJoin(defaults.lineJoin);
            m_currentCurve->setTension(defaults.curveTension);
            m_currentCurve->setRotation(defaults.rotation);
            m_currentCurve->setScale(defaults.scale);

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
    else if (tool == Tool::Star) {
        int points = 5;  // 5-pointed star
        QPointF delta = currentPos - m_shapeStartPos;
        float outerRadius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        float innerRadius = outerRadius * 0.4f;  // Inner points at 40% of outer
        float startAngle = std::atan2(delta.y(), delta.x());

        QPolygonF poly;
        for (int i = 0; i < points * 2; i++) {
            float angle = startAngle + M_PI * i / points;
            float r = (i % 2 == 0) ? outerRadius : innerRadius;
            QPointF pt = m_shapeStartPos + QPointF(r * std::cos(angle), r * std::sin(angle));
            poly << toScreen(pt);
        }

        QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(poly);
        polyItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        polyItem->setBrush(QBrush(previewColor));
        m_previewShape = polyItem;
    }
    else if (tool == Tool::Diamond) {
        float dx = std::abs(currentPos.x() - m_shapeStartPos.x());
        float dy = std::abs(currentPos.y() - m_shapeStartPos.y());

        QPolygonF poly;
        poly << toScreen(QPointF(m_shapeStartPos.x(), m_shapeStartPos.y() + dy));  // Top
        poly << toScreen(QPointF(m_shapeStartPos.x() + dx, m_shapeStartPos.y()));  // Right
        poly << toScreen(QPointF(m_shapeStartPos.x(), m_shapeStartPos.y() - dy));  // Bottom
        poly << toScreen(QPointF(m_shapeStartPos.x() - dx, m_shapeStartPos.y()));  // Left

        QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(poly);
        polyItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        polyItem->setBrush(QBrush(previewColor));
        m_previewShape = polyItem;
    }
    else if (tool == Tool::Arrow) {
        float dx = currentPos.x() - m_shapeStartPos.x();
        float dy = currentPos.y() - m_shapeStartPos.y();
        float length = std::sqrt(dx*dx + dy*dy);
        if (length < 0.1f) length = 0.1f;

        // Arrow pointing in drag direction
        float headWidth = length * 0.4f;
        float headLength = length * 0.35f;
        float shaftWidth = length * 0.15f;

        // Normalize direction
        float nx = dx / length;
        float ny = dy / length;
        // Perpendicular
        float px = -ny;
        float py = nx;

        QPointF tip = currentPos;
        QPointF headBase = m_shapeStartPos + QPointF(nx * (length - headLength), ny * (length - headLength));
        QPointF shaftEnd = m_shapeStartPos;

        QPolygonF poly;
        poly << toScreen(tip);  // Arrow tip
        poly << toScreen(headBase + QPointF(px * headWidth/2, py * headWidth/2));  // Head right
        poly << toScreen(headBase + QPointF(px * shaftWidth/2, py * shaftWidth/2));  // Shaft right top
        poly << toScreen(shaftEnd + QPointF(px * shaftWidth/2, py * shaftWidth/2));  // Shaft right bottom
        poly << toScreen(shaftEnd - QPointF(px * shaftWidth/2, py * shaftWidth/2));  // Shaft left bottom
        poly << toScreen(headBase - QPointF(px * shaftWidth/2, py * shaftWidth/2));  // Shaft left top
        poly << toScreen(headBase - QPointF(px * headWidth/2, py * headWidth/2));  // Head left

        QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(poly);
        polyItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        polyItem->setBrush(QBrush(previewColor));
        m_previewShape = polyItem;
    }
    else if (tool == Tool::Trapezoid) {
        float x1 = qMin(m_shapeStartPos.x(), currentPos.x());
        float y1 = qMin(m_shapeStartPos.y(), currentPos.y());
        float x2 = qMax(m_shapeStartPos.x(), currentPos.x());
        float y2 = qMax(m_shapeStartPos.y(), currentPos.y());
        float width = x2 - x1;
        float inset = width * 0.2f;  // Top is narrower by 20% on each side

        QPolygonF poly;
        poly << toScreen(QPointF(x1 + inset, y2));  // Top left
        poly << toScreen(QPointF(x2 - inset, y2));  // Top right
        poly << toScreen(QPointF(x2, y1));          // Bottom right
        poly << toScreen(QPointF(x1, y1));          // Bottom left

        QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(poly);
        polyItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        polyItem->setBrush(QBrush(previewColor));
        m_previewShape = polyItem;
    }
    else if (tool == Tool::Parallelogram) {
        float x1 = qMin(m_shapeStartPos.x(), currentPos.x());
        float y1 = qMin(m_shapeStartPos.y(), currentPos.y());
        float x2 = qMax(m_shapeStartPos.x(), currentPos.x());
        float y2 = qMax(m_shapeStartPos.y(), currentPos.y());
        float height = y2 - y1;
        // Calculate skew from angle: skew = height * tan(angle)
        float skew = height * std::tan(m_parallelogramSkew * M_PI / 180.0f);

        QPolygonF poly;
        poly << toScreen(QPointF(x1 + skew, y2));   // Top left
        poly << toScreen(QPointF(x2 + skew, y2));   // Top right
        poly << toScreen(QPointF(x2, y1));          // Bottom right
        poly << toScreen(QPointF(x1, y1));          // Bottom left

        QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(poly);
        polyItem->setPen(QPen(Qt::gray, 1, Qt::DashLine));
        polyItem->setBrush(QBrush(previewColor));
        m_previewShape = polyItem;
    }
    else if (tool == Tool::Arc) {
        // Arc preview - draw as partial ellipse
        QPointF delta = currentPos - m_shapeStartPos;
        float radius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        float angle = std::atan2(delta.y(), delta.x()) * 180.0 / M_PI;

        // Arc from 0 to drag angle
        QPainterPath path;
        QRectF rect(toScreen(m_shapeStartPos).x() - toScreen(radius),
                   toScreen(m_shapeStartPos).y() - toScreen(radius),
                   toScreen(radius) * 2, toScreen(radius) * 2);
        path.arcMoveTo(rect, 0);
        path.arcTo(rect, 0, -angle);  // Negative because Qt Y is inverted

        QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
        pathItem->setPen(QPen(Qt::gray, 2, Qt::DashLine));
        m_previewShape = pathItem;
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

    // Get defaults from properties panel, or use fallbacks
    DrawingDefaults defaults;
    if (m_propertiesPanel) {
        defaults = m_propertiesPanel->drawingDefaults();
    }

    if (tool == Tool::Rectangle) {
        Polygon *poly = m_document->addPolygon();
        poly->setPolygonType(PolygonType::Rectangle);

        // Apply all defaults
        poly->setFillColor(defaults.fillColor);
        poly->setStrokeColor(defaults.strokeColor);
        poly->setLineWidth(defaults.lineWidth);
        poly->setLineStyle(defaults.lineStyle);
        poly->setLineCap(defaults.lineCap);
        poly->setLineJoin(defaults.lineJoin);
        poly->setFillPattern(defaults.fillPattern);
        poly->setOpacity(defaults.opacity);
        poly->setRotation(defaults.rotation);
        poly->setScale(defaults.scale);
        poly->setDefaultCornerRadius(defaults.cornerRadius);

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

        // Apply all defaults
        ellipse->setFillColor(defaults.fillColor);
        ellipse->setStrokeColor(defaults.strokeColor);
        ellipse->setLineWidth(defaults.lineWidth);
        ellipse->setLineStyle(defaults.lineStyle);
        ellipse->setLineCap(defaults.lineCap);
        ellipse->setLineJoin(defaults.lineJoin);
        ellipse->setFillPattern(defaults.fillPattern);
        ellipse->setOpacity(defaults.opacity);
        ellipse->setRotation(defaults.rotation);
        ellipse->setScale(defaults.scale);

        emit statusMessage("Circle created");
    }
    else if (tool == Tool::Ellipse) {
        float rx = std::abs(endPos.x() - m_shapeStartPos.x());
        float ry = std::abs(endPos.y() - m_shapeStartPos.y());

        Ellipse *ellipse = m_document->addEllipse(m_shapeStartPos, rx, ry);

        // Apply all defaults
        ellipse->setFillColor(defaults.fillColor);
        ellipse->setStrokeColor(defaults.strokeColor);
        ellipse->setLineWidth(defaults.lineWidth);
        ellipse->setLineStyle(defaults.lineStyle);
        ellipse->setLineCap(defaults.lineCap);
        ellipse->setLineJoin(defaults.lineJoin);
        ellipse->setFillPattern(defaults.fillPattern);
        ellipse->setOpacity(defaults.opacity);
        ellipse->setRotation(defaults.rotation);
        ellipse->setScale(defaults.scale);

        emit statusMessage("Ellipse created");
    }
    else if (tool == Tool::Triangle || tool == Tool::RegularPolygon) {
        int sides = (tool == Tool::Triangle) ? 3 : m_regPolySides;
        QPointF delta = endPos - m_shapeStartPos;
        float radius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        float startAngle = std::atan2(delta.y(), delta.x());

        Polygon *poly = m_document->addPolygon();
        poly->setPolygonType(tool == Tool::Triangle ? PolygonType::Triangle : PolygonType::RegularPolygon);

        // Apply all defaults
        poly->setFillColor(defaults.fillColor);
        poly->setStrokeColor(defaults.strokeColor);
        poly->setLineWidth(defaults.lineWidth);
        poly->setLineStyle(defaults.lineStyle);
        poly->setLineCap(defaults.lineCap);
        poly->setLineJoin(defaults.lineJoin);
        poly->setFillPattern(defaults.fillPattern);
        poly->setOpacity(defaults.opacity);
        poly->setRotation(defaults.rotation);
        poly->setScale(defaults.scale);
        poly->setDefaultCornerRadius(defaults.cornerRadius);

        for (int i = 0; i < sides; i++) {
            float angle = startAngle + 2.0 * M_PI * i / sides;
            QPointF pt = m_shapeStartPos + QPointF(radius * std::cos(angle), radius * std::sin(angle));
            poly->addVertex(pt);
        }

        emit statusMessage(QString("%1 created").arg(tool == Tool::Triangle ? "Triangle" : "Regular polygon"));
    }
    else if (tool == Tool::Star) {
        int points = 5;  // 5-pointed star
        QPointF delta = endPos - m_shapeStartPos;
        float outerRadius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        float innerRadius = outerRadius * 0.4f;
        float startAngle = std::atan2(delta.y(), delta.x());

        Polygon *poly = m_document->addPolygon();
        poly->setPolygonType(PolygonType::Star);

        poly->setFillColor(defaults.fillColor);
        poly->setStrokeColor(defaults.strokeColor);
        poly->setLineWidth(defaults.lineWidth);
        poly->setLineStyle(defaults.lineStyle);
        poly->setLineCap(defaults.lineCap);
        poly->setLineJoin(defaults.lineJoin);
        poly->setFillPattern(defaults.fillPattern);
        poly->setOpacity(defaults.opacity);
        poly->setRotation(defaults.rotation);
        poly->setScale(defaults.scale);

        for (int i = 0; i < points * 2; i++) {
            float angle = startAngle + M_PI * i / points;
            float r = (i % 2 == 0) ? outerRadius : innerRadius;
            QPointF pt = m_shapeStartPos + QPointF(r * std::cos(angle), r * std::sin(angle));
            poly->addVertex(pt);
        }

        emit statusMessage("Star created");
    }
    else if (tool == Tool::Diamond) {
        float dx = std::abs(endPos.x() - m_shapeStartPos.x());
        float dy = std::abs(endPos.y() - m_shapeStartPos.y());

        Polygon *poly = m_document->addPolygon();
        poly->setPolygonType(PolygonType::Diamond);

        poly->setFillColor(defaults.fillColor);
        poly->setStrokeColor(defaults.strokeColor);
        poly->setLineWidth(defaults.lineWidth);
        poly->setLineStyle(defaults.lineStyle);
        poly->setLineCap(defaults.lineCap);
        poly->setLineJoin(defaults.lineJoin);
        poly->setFillPattern(defaults.fillPattern);
        poly->setOpacity(defaults.opacity);
        poly->setRotation(defaults.rotation);
        poly->setScale(defaults.scale);
        poly->setDefaultCornerRadius(defaults.cornerRadius);

        poly->addVertex(QPointF(m_shapeStartPos.x(), m_shapeStartPos.y() + dy));  // Top
        poly->addVertex(QPointF(m_shapeStartPos.x() + dx, m_shapeStartPos.y()));  // Right
        poly->addVertex(QPointF(m_shapeStartPos.x(), m_shapeStartPos.y() - dy));  // Bottom
        poly->addVertex(QPointF(m_shapeStartPos.x() - dx, m_shapeStartPos.y()));  // Left

        emit statusMessage("Diamond created");
    }
    else if (tool == Tool::Arrow) {
        float dx = endPos.x() - m_shapeStartPos.x();
        float dy = endPos.y() - m_shapeStartPos.y();
        float length = std::sqrt(dx*dx + dy*dy);
        if (length < 0.1f) return;  // Too small

        float headWidth = length * 0.4f;
        float headLength = length * 0.35f;
        float shaftWidth = length * 0.15f;

        float nx = dx / length;
        float ny = dy / length;
        float px = -ny;
        float py = nx;

        QPointF tip = endPos;
        QPointF headBase = m_shapeStartPos + QPointF(nx * (length - headLength), ny * (length - headLength));
        QPointF shaftEnd = m_shapeStartPos;

        Polygon *poly = m_document->addPolygon();
        poly->setPolygonType(PolygonType::Arrow);

        poly->setFillColor(defaults.fillColor);
        poly->setStrokeColor(defaults.strokeColor);
        poly->setLineWidth(defaults.lineWidth);
        poly->setLineStyle(defaults.lineStyle);
        poly->setLineCap(defaults.lineCap);
        poly->setLineJoin(defaults.lineJoin);
        poly->setFillPattern(defaults.fillPattern);
        poly->setOpacity(defaults.opacity);
        poly->setRotation(defaults.rotation);
        poly->setScale(defaults.scale);

        poly->addVertex(tip);
        poly->addVertex(headBase + QPointF(px * headWidth/2, py * headWidth/2));
        poly->addVertex(headBase + QPointF(px * shaftWidth/2, py * shaftWidth/2));
        poly->addVertex(shaftEnd + QPointF(px * shaftWidth/2, py * shaftWidth/2));
        poly->addVertex(shaftEnd - QPointF(px * shaftWidth/2, py * shaftWidth/2));
        poly->addVertex(headBase - QPointF(px * shaftWidth/2, py * shaftWidth/2));
        poly->addVertex(headBase - QPointF(px * headWidth/2, py * headWidth/2));

        emit statusMessage("Arrow created");
    }
    else if (tool == Tool::Trapezoid) {
        float x1 = qMin(m_shapeStartPos.x(), endPos.x());
        float y1 = qMin(m_shapeStartPos.y(), endPos.y());
        float x2 = qMax(m_shapeStartPos.x(), endPos.x());
        float y2 = qMax(m_shapeStartPos.y(), endPos.y());
        float width = x2 - x1;
        float inset = width * 0.2f;

        Polygon *poly = m_document->addPolygon();
        poly->setPolygonType(PolygonType::Trapezoid);

        poly->setFillColor(defaults.fillColor);
        poly->setStrokeColor(defaults.strokeColor);
        poly->setLineWidth(defaults.lineWidth);
        poly->setLineStyle(defaults.lineStyle);
        poly->setLineCap(defaults.lineCap);
        poly->setLineJoin(defaults.lineJoin);
        poly->setFillPattern(defaults.fillPattern);
        poly->setOpacity(defaults.opacity);
        poly->setRotation(defaults.rotation);
        poly->setScale(defaults.scale);
        poly->setDefaultCornerRadius(defaults.cornerRadius);

        poly->addVertex(QPointF(x1 + inset, y2));
        poly->addVertex(QPointF(x2 - inset, y2));
        poly->addVertex(QPointF(x2, y1));
        poly->addVertex(QPointF(x1, y1));

        emit statusMessage("Trapezoid created");
    }
    else if (tool == Tool::Parallelogram) {
        float x1 = qMin(m_shapeStartPos.x(), endPos.x());
        float y1 = qMin(m_shapeStartPos.y(), endPos.y());
        float x2 = qMax(m_shapeStartPos.x(), endPos.x());
        float y2 = qMax(m_shapeStartPos.y(), endPos.y());
        float height = y2 - y1;
        // Calculate skew from angle: skew = height * tan(angle)
        float skew = height * std::tan(m_parallelogramSkew * M_PI / 180.0f);

        Polygon *poly = m_document->addPolygon();
        poly->setPolygonType(PolygonType::Parallelogram);

        poly->setFillColor(defaults.fillColor);
        poly->setStrokeColor(defaults.strokeColor);
        poly->setLineWidth(defaults.lineWidth);
        poly->setLineStyle(defaults.lineStyle);
        poly->setLineCap(defaults.lineCap);
        poly->setLineJoin(defaults.lineJoin);
        poly->setFillPattern(defaults.fillPattern);
        poly->setOpacity(defaults.opacity);
        poly->setRotation(defaults.rotation);
        poly->setScale(defaults.scale);
        poly->setDefaultCornerRadius(defaults.cornerRadius);

        poly->addVertex(QPointF(x1 + skew, y2));
        poly->addVertex(QPointF(x2 + skew, y2));
        poly->addVertex(QPointF(x2, y1));
        poly->addVertex(QPointF(x1, y1));

        emit statusMessage(QString("Parallelogram created (skew: %1°)").arg(m_parallelogramSkew));
    }
    else if (tool == Tool::Arc) {
        // For now, create arc as a curve with multiple points
        QPointF delta = endPos - m_shapeStartPos;
        float radius = std::sqrt(delta.x()*delta.x() + delta.y()*delta.y());
        float endAngle = std::atan2(delta.y(), delta.x());

        if (radius < 0.1f) return;

        Curve *curve = m_document->addCurve();

        curve->setColor(defaults.strokeColor);
        curve->setLineWidth(defaults.lineWidth);
        curve->setLineStyle(defaults.lineStyle);
        curve->setLineCap(defaults.lineCap);
        curve->setLineJoin(defaults.lineJoin);
        curve->setRotation(defaults.rotation);
        curve->setScale(defaults.scale);
        curve->setCurveType(Curve::Smooth);

        // Create arc from 0 to endAngle with multiple points
        int segments = qMax(8, (int)(std::abs(endAngle) * 4));
        for (int i = 0; i <= segments; i++) {
            float angle = endAngle * i / segments;
            QPointF pt = m_shapeStartPos + QPointF(radius * std::cos(angle), radius * std::sin(angle));
            curve->addControlPoint(pt);
        }

        emit statusMessage("Arc created");
    }
}
