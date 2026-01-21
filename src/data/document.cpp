#include "document.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"
#include "group.h"
#include "common.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Document::Document(QObject *parent)
    : QObject(parent)
{
    // Create default layer
    addLayer("Layer 1");
}

Document::~Document() {
    clear();
}

Point *Document::addPoint(QPointF pos) {
    Point *p = new Point(pos, this);
    p->setName(freshPointName());
    m_points.append(p);
    m_shapeOrder.append({ShapeType::Point, p});
    connect(p, &Point::changed, this, &Document::changed);
    emit changed();
    return p;
}

void Document::removePoint(Point *point) {
    int idx = m_points.indexOf(point);
    if (idx >= 0) {
        m_points.remove(idx);
        // Remove from shape order
        for (int i = 0; i < m_shapeOrder.size(); ++i) {
            if (m_shapeOrder[i].ptr == point) {
                m_shapeOrder.remove(i);
                break;
            }
        }
        point->deleteLater();
        emit changed();
    }
}

Line *Document::addLine(QPointF start, QPointF end) {
    Line *l = new Line(start, end, this);
    m_lines.append(l);
    m_shapeOrder.append({ShapeType::Line, l});
    connect(l, &Line::changed, this, &Document::changed);
    emit changed();
    return l;
}

Line *Document::addLine(Point *start, Point *end) {
    Line *l = new Line(start, end, this);
    m_lines.append(l);
    m_shapeOrder.append({ShapeType::Line, l});
    connect(l, &Line::changed, this, &Document::changed);
    emit changed();
    return l;
}

void Document::removeLine(Line *line) {
    int idx = m_lines.indexOf(line);
    if (idx >= 0) {
        m_lines.remove(idx);
        for (int i = 0; i < m_shapeOrder.size(); ++i) {
            if (m_shapeOrder[i].ptr == line) {
                m_shapeOrder.remove(i);
                break;
            }
        }
        line->deleteLater();
        emit changed();
    }
}

Polygon *Document::addPolygon() {
    Polygon *p = new Polygon(this);
    m_polygons.append(p);
    m_shapeOrder.append({ShapeType::Polygon, p});
    connect(p, &Polygon::changed, this, &Document::changed);
    emit changed();
    return p;
}

void Document::removePolygon(Polygon *polygon) {
    int idx = m_polygons.indexOf(polygon);
    if (idx >= 0) {
        m_polygons.remove(idx);
        for (int i = 0; i < m_shapeOrder.size(); ++i) {
            if (m_shapeOrder[i].ptr == polygon) {
                m_shapeOrder.remove(i);
                break;
            }
        }
        polygon->deleteLater();
        emit changed();
    }
}

Curve *Document::addCurve() {
    Curve *c = new Curve(this);
    m_curves.append(c);
    m_shapeOrder.append({ShapeType::Curve, c});
    connect(c, &Curve::changed, this, &Document::changed);
    emit changed();
    return c;
}

void Document::removeCurve(Curve *curve) {
    int idx = m_curves.indexOf(curve);
    if (idx >= 0) {
        m_curves.remove(idx);
        for (int i = 0; i < m_shapeOrder.size(); ++i) {
            if (m_shapeOrder[i].ptr == curve) {
                m_shapeOrder.remove(i);
                break;
            }
        }
        curve->deleteLater();
        emit changed();
    }
}

Ellipse *Document::addEllipse() {
    Ellipse *e = new Ellipse(this);
    m_ellipses.append(e);
    m_shapeOrder.append({ShapeType::Ellipse, e});
    connect(e, &Ellipse::changed, this, &Document::changed);
    emit changed();
    return e;
}

Ellipse *Document::addCircle(QPointF center, float radius) {
    Ellipse *e = addEllipse();
    e->setCenter(center);
    e->setRadius(radius);
    return e;
}

Ellipse *Document::addEllipse(QPointF center, float rx, float ry) {
    Ellipse *e = addEllipse();
    e->setCenter(center);
    e->setRadiusX(rx);
    e->setRadiusY(ry);
    return e;
}

void Document::removeEllipse(Ellipse *ellipse) {
    int idx = m_ellipses.indexOf(ellipse);
    if (idx >= 0) {
        m_ellipses.remove(idx);
        for (int i = 0; i < m_shapeOrder.size(); ++i) {
            if (m_shapeOrder[i].ptr == ellipse) {
                m_shapeOrder.remove(i);
                break;
            }
        }
        ellipse->deleteLater();
        emit changed();
    }
}

Group *Document::addGroup() {
    Group *g = new Group(this);
    m_groups.append(g);
    m_shapeOrder.append({ShapeType::Group, g});
    connect(g, &Group::changed, this, &Document::changed);
    emit changed();
    return g;
}

void Document::removeGroup(Group *group) {
    int idx = m_groups.indexOf(group);
    if (idx >= 0) {
        m_groups.remove(idx);
        for (int i = 0; i < m_shapeOrder.size(); ++i) {
            if (m_shapeOrder[i].ptr == group) {
                m_shapeOrder.remove(i);
                break;
            }
        }
        group->deleteLater();
        emit changed();
    }
}

void Document::clear() {
    for (Point *p : m_points) p->deleteLater();
    for (Line *l : m_lines) l->deleteLater();
    for (Polygon *p : m_polygons) p->deleteLater();
    for (Curve *c : m_curves) c->deleteLater();
    for (Ellipse *e : m_ellipses) e->deleteLater();
    for (Group *g : m_groups) g->deleteLater();

    m_points.clear();
    m_lines.clear();
    m_polygons.clear();
    m_curves.clear();
    m_ellipses.clear();
    m_groups.clear();
    m_shapeOrder.clear();
    m_pointCounter = 0;
    m_backgroundImage.clear();

    emit changed();
}

void Document::setBackgroundImage(const QString &path) {
    if (m_backgroundImage != path) {
        m_backgroundImage = path;
        emit backgroundImageChanged();
    }
}

void Document::setBackgroundOpacity(float opacity) {
    m_backgroundOpacity = qBound(0.0f, opacity, 1.0f);
    emit backgroundImageChanged();
}

QString Document::freshPointName() {
    return QString("p%1").arg(m_pointCounter++);
}

QString Document::tikzPreamble() const {
    return R"(\documentclass[tikz,border=5pt]{standalone}
\usepackage{tikz}
\usetikzlibrary{patterns}

% UofSC Color definitions
\definecolor{garnet}{RGB}{115,0,10}
\definecolor{rose}{RGB}{204,46,64}
\definecolor{atlantic}{RGB}{70,106,159}
\definecolor{congaree}{RGB}{31,65,77}
\definecolor{horseshoe}{RGB}{101,120,11}
\definecolor{grass}{RGB}{206,211,24}
\definecolor{honeycomb}{RGB}{164,145,55}
\definecolor{darkgarnet}{RGB}{87,0,8}
\definecolor{azalea}{RGB}{132,66,71}

\begin{document}
)";
}

QString Document::tikz() const {
    QString result;
    QTextStream out(&result);

    out << "\\begin{tikzpicture}\n";

    // Background image (if any)
    if (!m_backgroundImage.isEmpty()) {
        out << "  % Background image\n";
        out << "  \\node[anchor=south west, inner sep=0] at (0,0) "
            << "{\\includegraphics{" << m_backgroundImage << "}};\n\n";
    }

    // Output shapes in drawing order
    for (const ShapeRef &ref : m_shapeOrder) {
        QString tikzStr;
        switch (ref.type) {
            case ShapeType::Point:
                tikzStr = static_cast<Point*>(ref.ptr)->tikz();
                break;
            case ShapeType::Line:
                tikzStr = static_cast<Line*>(ref.ptr)->tikz();
                break;
            case ShapeType::Polygon:
                tikzStr = static_cast<Polygon*>(ref.ptr)->tikz();
                break;
            case ShapeType::Curve:
                tikzStr = static_cast<Curve*>(ref.ptr)->tikz();
                break;
            case ShapeType::Ellipse:
                tikzStr = static_cast<Ellipse*>(ref.ptr)->tikz();
                break;
            case ShapeType::Group:
                tikzStr = static_cast<Group*>(ref.ptr)->tikz();
                break;
        }
        if (!tikzStr.isEmpty()) {
            out << "  " << tikzStr << "\n";
        }
    }

    out << "\\end{tikzpicture}\n";

    return result;
}

QString Document::tikzFull() const {
    return tikzPreamble() + tikz() + "\\end{document}\n";
}

bool Document::save(const QString &path) {
    QJsonObject root;

    // Points
    QJsonArray pointsArr;
    for (const Point *p : m_points) {
        QJsonObject obj;
        obj["name"] = p->name();
        obj["x"] = p->pos().x();
        obj["y"] = p->pos().y();
        obj["color"] = p->color().name();
        obj["radius"] = p->radius();
        obj["visible"] = p->isVisible();
        pointsArr.append(obj);
    }
    root["points"] = pointsArr;

    // Lines
    QJsonArray linesArr;
    for (const Line *l : m_lines) {
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
        linesArr.append(obj);
    }
    root["lines"] = linesArr;

    // Polygons
    QJsonArray polysArr;
    for (const Polygon *p : m_polygons) {
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
        polysArr.append(obj);
    }
    root["polygons"] = polysArr;

    // Curves
    QJsonArray curvesArr;
    for (const Curve *c : m_curves) {
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
        curvesArr.append(obj);
    }
    root["curves"] = curvesArr;

    // Ellipses
    QJsonArray ellipsesArr;
    for (const Ellipse *e : m_ellipses) {
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
        ellipsesArr.append(obj);
    }
    root["ellipses"] = ellipsesArr;

    root["backgroundImage"] = m_backgroundImage;
    root["backgroundOpacity"] = m_backgroundOpacity;

    // Save drawing order
    QJsonArray orderArr;
    for (const ShapeRef &ref : m_shapeOrder) {
        QJsonObject orderObj;
        orderObj["type"] = static_cast<int>(ref.type);
        // Find index within the shape's type-specific vector
        int idx = -1;
        switch (ref.type) {
            case ShapeType::Point:
                idx = m_points.indexOf(static_cast<Point*>(ref.ptr));
                break;
            case ShapeType::Line:
                idx = m_lines.indexOf(static_cast<Line*>(ref.ptr));
                break;
            case ShapeType::Polygon:
                idx = m_polygons.indexOf(static_cast<Polygon*>(ref.ptr));
                break;
            case ShapeType::Curve:
                idx = m_curves.indexOf(static_cast<Curve*>(ref.ptr));
                break;
            case ShapeType::Ellipse:
                idx = m_ellipses.indexOf(static_cast<Ellipse*>(ref.ptr));
                break;
            case ShapeType::Group:
                idx = m_groups.indexOf(static_cast<Group*>(ref.ptr));
                break;
        }
        orderObj["index"] = idx;
        orderArr.append(orderObj);
    }
    root["shapeOrder"] = orderArr;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QJsonDocument doc(root);
    file.write(doc.toJson());
    return true;
}

bool Document::load(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) return false;

    clear();

    QJsonObject root = doc.object();

    // Points
    for (const QJsonValue &v : root["points"].toArray()) {
        QJsonObject obj = v.toObject();
        Point *p = addPoint(QPointF(obj["x"].toDouble(), obj["y"].toDouble()));
        p->setName(obj["name"].toString());
        p->setColor(QColor(obj["color"].toString()));
        p->setRadius(obj["radius"].toDouble(2.0));
        p->setVisible(obj["visible"].toBool(true));
    }

    // Lines
    for (const QJsonValue &v : root["lines"].toArray()) {
        QJsonObject obj = v.toObject();
        Line *l = addLine(
            QPointF(obj["startX"].toDouble(), obj["startY"].toDouble()),
            QPointF(obj["endX"].toDouble(), obj["endY"].toDouble())
        );
        l->setColor(QColor(obj["color"].toString()));
        l->setLineWidth(obj["lineWidth"].toDouble(0.8));
        l->setLineStyle(static_cast<LineStyle>(obj["lineStyle"].toInt(0)));
        l->setLineCap(static_cast<LineCap>(obj["lineCap"].toInt(0)));
        l->setLineJoin(static_cast<LineJoin>(obj["lineJoin"].toInt(0)));
        l->setCornerRadius(obj["cornerRadius"].toDouble());
        l->setRotation(obj["rotation"].toDouble(0.0));
        l->setScale(obj["scale"].toDouble(1.0));
    }

    // Polygons
    for (const QJsonValue &v : root["polygons"].toArray()) {
        QJsonObject obj = v.toObject();
        Polygon *p = addPolygon();
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
        p->setFillPattern(static_cast<FillPattern>(obj["fillPattern"].toInt(1)));  // Default Solid
        p->setOpacity(obj["opacity"].toDouble(1.0));
        p->setRotation(obj["rotation"].toDouble(0.0));
        p->setScale(obj["scale"].toDouble(1.0));
        p->setDefaultCornerRadius(obj["defaultCornerRadius"].toDouble(0.0));
    }

    // Curves
    for (const QJsonValue &v : root["curves"].toArray()) {
        QJsonObject obj = v.toObject();
        Curve *c = addCurve();
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

    // Ellipses
    for (const QJsonValue &v : root["ellipses"].toArray()) {
        QJsonObject obj = v.toObject();
        Ellipse *e = addEllipse();
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
        e->setFillPattern(static_cast<FillPattern>(obj["fillPattern"].toInt(1)));  // Default Solid
        e->setOpacity(obj["opacity"].toDouble(1.0));
    }

    m_backgroundImage = root["backgroundImage"].toString();
    m_backgroundOpacity = root["backgroundOpacity"].toDouble(0.5);
    if (!m_backgroundImage.isEmpty()) {
        emit backgroundImageChanged();
    }

    // Restore drawing order
    QJsonArray orderArr = root["shapeOrder"].toArray();
    if (!orderArr.isEmpty()) {
        // Clear the order that was auto-populated during adds
        m_shapeOrder.clear();
        // Reconstruct order from saved data
        for (const QJsonValue &v : orderArr) {
            QJsonObject orderObj = v.toObject();
            ShapeType type = static_cast<ShapeType>(orderObj["type"].toInt());
            int idx = orderObj["index"].toInt();
            void *ptr = nullptr;
            switch (type) {
                case ShapeType::Point:
                    if (idx >= 0 && idx < m_points.size())
                        ptr = m_points[idx];
                    break;
                case ShapeType::Line:
                    if (idx >= 0 && idx < m_lines.size())
                        ptr = m_lines[idx];
                    break;
                case ShapeType::Polygon:
                    if (idx >= 0 && idx < m_polygons.size())
                        ptr = m_polygons[idx];
                    break;
                case ShapeType::Curve:
                    if (idx >= 0 && idx < m_curves.size())
                        ptr = m_curves[idx];
                    break;
                case ShapeType::Ellipse:
                    if (idx >= 0 && idx < m_ellipses.size())
                        ptr = m_ellipses[idx];
                    break;
                case ShapeType::Group:
                    if (idx >= 0 && idx < m_groups.size())
                        ptr = m_groups[idx];
                    break;
            }
            if (ptr) {
                m_shapeOrder.append({type, ptr});
            }
        }
    }

    return true;
}

// ============================================================================
// Z-Ordering
// ============================================================================

void Document::bringToFront(void *shape, ShapeType type) {
    for (int i = 0; i < m_shapeOrder.size(); ++i) {
        if (m_shapeOrder[i].ptr == shape && m_shapeOrder[i].type == type) {
            ShapeRef ref = m_shapeOrder.takeAt(i);
            m_shapeOrder.append(ref);
            emit changed();
            return;
        }
    }
}

void Document::sendToBack(void *shape, ShapeType type) {
    for (int i = 0; i < m_shapeOrder.size(); ++i) {
        if (m_shapeOrder[i].ptr == shape && m_shapeOrder[i].type == type) {
            ShapeRef ref = m_shapeOrder.takeAt(i);
            m_shapeOrder.prepend(ref);
            emit changed();
            return;
        }
    }
}

void Document::bringForward(void *shape, ShapeType type) {
    for (int i = 0; i < m_shapeOrder.size() - 1; ++i) {
        if (m_shapeOrder[i].ptr == shape && m_shapeOrder[i].type == type) {
            m_shapeOrder.swapItemsAt(i, i + 1);
            emit changed();
            return;
        }
    }
}

void Document::sendBackward(void *shape, ShapeType type) {
    for (int i = 1; i < m_shapeOrder.size(); ++i) {
        if (m_shapeOrder[i].ptr == shape && m_shapeOrder[i].type == type) {
            m_shapeOrder.swapItemsAt(i, i - 1);
            emit changed();
            return;
        }
    }
}

// ============================================================================
// Layer Management
// ============================================================================

int Document::addLayer(const QString &name) {
    Layer layer;
    layer.name = name.isEmpty() ? QString("Layer %1").arg(++m_layerCounter) : name;
    layer.visible = true;
    layer.locked = false;
    m_layers.append(layer);
    emit layersChanged();
    return m_layers.size() - 1;
}

void Document::removeLayer(int index) {
    if (index < 0 || index >= m_layers.size()) return;
    if (m_layers.size() <= 1) return;  // Keep at least one layer

    // Move shapes from deleted layer to layer 0
    for (const ShapeRef &ref : m_layers[index].shapes) {
        m_layers[0].shapes.append(ref);
    }

    m_layers.remove(index);

    if (m_currentLayer >= m_layers.size()) {
        m_currentLayer = m_layers.size() - 1;
    }

    emit layersChanged();
    emit changed();
}

void Document::setLayerVisible(int index, bool visible) {
    if (index < 0 || index >= m_layers.size()) return;
    m_layers[index].visible = visible;
    emit layersChanged();
    emit changed();
}

void Document::setLayerLocked(int index, bool locked) {
    if (index < 0 || index >= m_layers.size()) return;
    m_layers[index].locked = locked;
    emit layersChanged();
}

void Document::setLayerName(int index, const QString &name) {
    if (index < 0 || index >= m_layers.size()) return;
    m_layers[index].name = name;
    emit layersChanged();
}

void Document::moveShapeToLayer(void *shape, ShapeType type, int layerIndex) {
    if (layerIndex < 0 || layerIndex >= m_layers.size()) return;

    // Remove from current layer
    for (int i = 0; i < m_layers.size(); ++i) {
        for (int j = 0; j < m_layers[i].shapes.size(); ++j) {
            if (m_layers[i].shapes[j].ptr == shape && m_layers[i].shapes[j].type == type) {
                m_layers[i].shapes.remove(j);
                break;
            }
        }
    }

    // Add to new layer
    m_layers[layerIndex].shapes.append({type, shape});
    emit layersChanged();
    emit changed();
}

void Document::setCurrentLayer(int index) {
    if (index < 0 || index >= m_layers.size()) return;
    if (m_currentLayer == index) return;  // No change
    m_currentLayer = index;
    emit layersChanged();
}
