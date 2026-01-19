#include "commands.h"
#include "document.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"

#include <QJsonArray>

// Helper functions to serialize/deserialize shapes
namespace {

QJsonObject savePoint(Point *p) {
    QJsonObject obj;
    obj["name"] = p->name();
    obj["x"] = p->pos().x();
    obj["y"] = p->pos().y();
    obj["color"] = p->color().name();
    obj["radius"] = p->radius();
    obj["visible"] = p->isVisible();
    return obj;
}

void restorePoint(Point *p, const QJsonObject &obj) {
    p->setName(obj["name"].toString());
    p->setPos(QPointF(obj["x"].toDouble(), obj["y"].toDouble()));
    p->setColor(QColor(obj["color"].toString()));
    p->setRadius(obj["radius"].toDouble(2.0));
    p->setVisible(obj["visible"].toBool(true));
}

QJsonObject saveLine(Line *l) {
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

void restoreLine(Line *l, const QJsonObject &obj) {
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

QJsonObject savePolygon(Polygon *p) {
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

void restorePolygon(Polygon *p, const QJsonObject &obj) {
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

QJsonObject saveCurve(Curve *c) {
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

void restoreCurve(Curve *c, const QJsonObject &obj) {
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

QJsonObject saveEllipse(Ellipse *e) {
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

void restoreEllipse(Ellipse *e, const QJsonObject &obj) {
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

// ============================================================================
// Create Commands Implementation
// ============================================================================

CreatePointCommand::CreatePointCommand(Document *doc, QPointF pos, QUndoCommand *parent)
    : QUndoCommand("Create Point", parent)
    , m_doc(doc)
    , m_pos(pos)
{
}

void CreatePointCommand::undo() {
    if (m_point) {
        m_doc->removePoint(m_point);
    }
}

void CreatePointCommand::redo() {
    if (m_firstRedo) {
        m_point = m_doc->addPoint(m_pos);
        m_firstRedo = false;
    } else {
        m_point = m_doc->addPoint(m_pos);
    }
}

// ---

CreateLineCommand::CreateLineCommand(Document *doc, QPointF start, QPointF end, QUndoCommand *parent)
    : QUndoCommand("Create Line", parent)
    , m_doc(doc)
    , m_start(start)
    , m_end(end)
{
}

void CreateLineCommand::undo() {
    if (m_line) {
        m_doc->removeLine(m_line);
    }
}

void CreateLineCommand::redo() {
    if (m_firstRedo) {
        m_line = m_doc->addLine(m_start, m_end);
        m_firstRedo = false;
    } else {
        m_line = m_doc->addLine(m_start, m_end);
    }
}

// ---

CreatePolygonCommand::CreatePolygonCommand(Document *doc, const QVector<QPointF> &vertices, QUndoCommand *parent)
    : QUndoCommand("Create Polygon", parent)
    , m_doc(doc)
    , m_vertices(vertices)
{
}

void CreatePolygonCommand::undo() {
    if (m_polygon) {
        m_doc->removePolygon(m_polygon);
    }
}

void CreatePolygonCommand::redo() {
    m_polygon = m_doc->addPolygon();
    for (const QPointF &v : m_vertices) {
        m_polygon->addVertex(v);
    }
}

// ---

CreateCurveCommand::CreateCurveCommand(Document *doc, const QVector<QPointF> &controlPoints, QUndoCommand *parent)
    : QUndoCommand("Create Curve", parent)
    , m_doc(doc)
    , m_controlPoints(controlPoints)
{
}

void CreateCurveCommand::undo() {
    if (m_curve) {
        m_doc->removeCurve(m_curve);
    }
}

void CreateCurveCommand::redo() {
    m_curve = m_doc->addCurve();
    for (const QPointF &pt : m_controlPoints) {
        m_curve->addControlPoint(pt);
    }
}

// ---

CreateEllipseCommand::CreateEllipseCommand(Document *doc, QPointF center, float rx, float ry, QUndoCommand *parent)
    : QUndoCommand("Create Ellipse", parent)
    , m_doc(doc)
    , m_center(center)
    , m_rx(rx)
    , m_ry(ry)
{
}

void CreateEllipseCommand::undo() {
    if (m_ellipse) {
        m_doc->removeEllipse(m_ellipse);
    }
}

void CreateEllipseCommand::redo() {
    m_ellipse = m_doc->addEllipse(m_center, m_rx, m_ry);
}

// ============================================================================
// Delete Commands Implementation
// ============================================================================

DeletePointCommand::DeletePointCommand(Document *doc, Point *point, QUndoCommand *parent)
    : QUndoCommand("Delete Point", parent)
    , m_doc(doc)
    , m_point(point)
{
    // Save point data before deletion
    m_savedData = savePoint(point);

    // Find position in shape order
    const auto &order = m_doc->shapeOrder();
    for (int i = 0; i < order.size(); ++i) {
        if (order[i].type == ShapeType::Point && order[i].ptr == point) {
            m_orderIndex = i;
            break;
        }
    }
}

void DeletePointCommand::undo() {
    // Recreate the point
    m_point = m_doc->addPoint(QPointF(m_savedData["x"].toDouble(), m_savedData["y"].toDouble()));
    restorePoint(m_point, m_savedData);
}

void DeletePointCommand::redo() {
    m_doc->removePoint(m_point);
}

// ---

DeleteLineCommand::DeleteLineCommand(Document *doc, Line *line, QUndoCommand *parent)
    : QUndoCommand("Delete Line", parent)
    , m_doc(doc)
    , m_line(line)
{
    m_savedData = saveLine(line);

    const auto &order = m_doc->shapeOrder();
    for (int i = 0; i < order.size(); ++i) {
        if (order[i].type == ShapeType::Line && order[i].ptr == line) {
            m_orderIndex = i;
            break;
        }
    }
}

void DeleteLineCommand::undo() {
    m_line = m_doc->addLine(
        QPointF(m_savedData["startX"].toDouble(), m_savedData["startY"].toDouble()),
        QPointF(m_savedData["endX"].toDouble(), m_savedData["endY"].toDouble())
    );
    restoreLine(m_line, m_savedData);
}

void DeleteLineCommand::redo() {
    m_doc->removeLine(m_line);
}

// ---

DeletePolygonCommand::DeletePolygonCommand(Document *doc, Polygon *polygon, QUndoCommand *parent)
    : QUndoCommand("Delete Polygon", parent)
    , m_doc(doc)
    , m_polygon(polygon)
{
    m_savedData = savePolygon(polygon);

    const auto &order = m_doc->shapeOrder();
    for (int i = 0; i < order.size(); ++i) {
        if (order[i].type == ShapeType::Polygon && order[i].ptr == polygon) {
            m_orderIndex = i;
            break;
        }
    }
}

void DeletePolygonCommand::undo() {
    m_polygon = m_doc->addPolygon();
    restorePolygon(m_polygon, m_savedData);
}

void DeletePolygonCommand::redo() {
    m_doc->removePolygon(m_polygon);
}

// ---

DeleteCurveCommand::DeleteCurveCommand(Document *doc, Curve *curve, QUndoCommand *parent)
    : QUndoCommand("Delete Curve", parent)
    , m_doc(doc)
    , m_curve(curve)
{
    m_savedData = saveCurve(curve);

    const auto &order = m_doc->shapeOrder();
    for (int i = 0; i < order.size(); ++i) {
        if (order[i].type == ShapeType::Curve && order[i].ptr == curve) {
            m_orderIndex = i;
            break;
        }
    }
}

void DeleteCurveCommand::undo() {
    m_curve = m_doc->addCurve();
    restoreCurve(m_curve, m_savedData);
}

void DeleteCurveCommand::redo() {
    m_doc->removeCurve(m_curve);
}

// ---

DeleteEllipseCommand::DeleteEllipseCommand(Document *doc, Ellipse *ellipse, QUndoCommand *parent)
    : QUndoCommand("Delete Ellipse", parent)
    , m_doc(doc)
    , m_ellipse(ellipse)
{
    m_savedData = saveEllipse(ellipse);

    const auto &order = m_doc->shapeOrder();
    for (int i = 0; i < order.size(); ++i) {
        if (order[i].type == ShapeType::Ellipse && order[i].ptr == ellipse) {
            m_orderIndex = i;
            break;
        }
    }
}

void DeleteEllipseCommand::undo() {
    m_ellipse = m_doc->addEllipse();
    restoreEllipse(m_ellipse, m_savedData);
}

void DeleteEllipseCommand::redo() {
    m_doc->removeEllipse(m_ellipse);
}

// ============================================================================
// Composite Delete Command
// ============================================================================

DeleteSelectionCommand::DeleteSelectionCommand(Document *doc,
                                               const QVector<Point*> &points,
                                               const QVector<Line*> &lines,
                                               const QVector<Polygon*> &polygons,
                                               const QVector<Curve*> &curves,
                                               const QVector<Ellipse*> &ellipses,
                                               QUndoCommand *parent)
    : QUndoCommand("Delete Selection", parent)
{
    // Create child commands for each shape
    for (Point *p : points) {
        new DeletePointCommand(doc, p, this);
    }
    for (Line *l : lines) {
        new DeleteLineCommand(doc, l, this);
    }
    for (Polygon *p : polygons) {
        new DeletePolygonCommand(doc, p, this);
    }
    for (Curve *c : curves) {
        new DeleteCurveCommand(doc, c, this);
    }
    for (Ellipse *e : ellipses) {
        new DeleteEllipseCommand(doc, e, this);
    }
}

// ============================================================================
// Move Command
// ============================================================================

MovePointCommand::MovePointCommand(Point *point, QPointF oldPos, QPointF newPos, QUndoCommand *parent)
    : QUndoCommand("Move Point", parent)
    , m_point(point)
    , m_oldPos(oldPos)
    , m_newPos(newPos)
{
}

void MovePointCommand::undo() {
    m_point->setPos(m_oldPos);
}

void MovePointCommand::redo() {
    m_point->setPos(m_newPos);
}

bool MovePointCommand::mergeWith(const QUndoCommand *other) {
    if (other->id() != id())
        return false;

    const MovePointCommand *cmd = static_cast<const MovePointCommand*>(other);
    if (cmd->m_point != m_point)
        return false;

    m_newPos = cmd->m_newPos;
    return true;
}
