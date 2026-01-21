#include "group.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"
#include <algorithm>
#include <cmath>

Group::Group(QObject *parent)
    : QObject(parent)
{
}

Group::~Group() {
}

void Group::addMember(void *shape, GroupedShapeType type) {
    GroupedShape gs{type, shape};
    if (!contains(shape, type)) {
        m_members.append(gs);
        m_bboxDirty = true;
        emit changed();
    }
}

void Group::removeMember(void *shape, GroupedShapeType type) {
    for (int i = 0; i < m_members.size(); ++i) {
        if (m_members[i].ptr == shape && m_members[i].type == type) {
            m_members.remove(i);
            m_bboxDirty = true;
            emit changed();
            return;
        }
    }
}

bool Group::contains(void *shape, GroupedShapeType type) const {
    for (const auto &gs : m_members) {
        if (gs.ptr == shape && gs.type == type) {
            return true;
        }
    }
    return false;
}

void Group::clear() {
    m_members.clear();
    m_bboxDirty = true;
    emit changed();
}

QRectF Group::boundingBox() const {
    if (!m_bboxDirty) {
        return m_cachedBBox;
    }

    if (m_members.isEmpty()) {
        m_cachedBBox = QRectF();
        m_bboxDirty = false;
        return m_cachedBBox;
    }

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();

    auto updateBounds = [&](QPointF pt) {
        minX = std::min(minX, (float)pt.x());
        maxX = std::max(maxX, (float)pt.x());
        minY = std::min(minY, (float)pt.y());
        maxY = std::max(maxY, (float)pt.y());
    };

    for (const auto &gs : m_members) {
        switch (gs.type) {
            case GroupedShapeType::Point: {
                Point *p = static_cast<Point*>(gs.ptr);
                updateBounds(p->pos());
                break;
            }
            case GroupedShapeType::Line: {
                Line *l = static_cast<Line*>(gs.ptr);
                updateBounds(l->startPos());
                updateBounds(l->endPos());
                break;
            }
            case GroupedShapeType::Polygon: {
                Polygon *poly = static_cast<Polygon*>(gs.ptr);
                for (const QPointF &pt : poly->vertexPositions()) {
                    updateBounds(pt);
                }
                break;
            }
            case GroupedShapeType::Curve: {
                Curve *c = static_cast<Curve*>(gs.ptr);
                for (const QPointF &pt : c->controlPoints()) {
                    updateBounds(pt);
                }
                break;
            }
            case GroupedShapeType::Ellipse: {
                Ellipse *e = static_cast<Ellipse*>(gs.ptr);
                QPointF center = e->center();
                float rx = e->radiusX() * e->scale();
                float ry = e->radiusY() * e->scale();
                updateBounds(center + QPointF(-rx, -ry));
                updateBounds(center + QPointF(rx, ry));
                break;
            }
        }
    }

    m_cachedBBox = QRectF(minX, minY, maxX - minX, maxY - minY);
    m_bboxDirty = false;
    return m_cachedBBox;
}

void Group::updateBoundingBox() {
    m_bboxDirty = true;
}

void Group::translate(QPointF delta) {
    for (const auto &gs : m_members) {
        switch (gs.type) {
            case GroupedShapeType::Point: {
                Point *p = static_cast<Point*>(gs.ptr);
                p->setPos(p->pos() + delta);
                break;
            }
            case GroupedShapeType::Line: {
                Line *l = static_cast<Line*>(gs.ptr);
                l->setStartPos(l->startPos() + delta);
                l->setEndPos(l->endPos() + delta);
                break;
            }
            case GroupedShapeType::Polygon: {
                Polygon *poly = static_cast<Polygon*>(gs.ptr);
                for (int i = 0; i < poly->vertexCount(); ++i) {
                    QPointF pt = poly->vertexPositions()[i];
                    poly->setVertexPosition(i, pt + delta);
                }
                break;
            }
            case GroupedShapeType::Curve: {
                Curve *c = static_cast<Curve*>(gs.ptr);
                QVector<QPointF> pts = c->controlPoints();
                for (int i = 0; i < pts.size(); ++i) {
                    c->setControlPoint(i, pts[i] + delta);
                }
                break;
            }
            case GroupedShapeType::Ellipse: {
                Ellipse *e = static_cast<Ellipse*>(gs.ptr);
                e->setCenter(e->center() + delta);
                break;
            }
        }
    }
    m_bboxDirty = true;
    emit changed();
}

void Group::rotate(float degrees) {
    QRectF bbox = boundingBox();
    QPointF center = bbox.center();
    float rad = degrees * M_PI / 180.0f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    auto rotatePoint = [&](QPointF pt) -> QPointF {
        QPointF rel = pt - center;
        return center + QPointF(rel.x() * cosA - rel.y() * sinA,
                                 rel.x() * sinA + rel.y() * cosA);
    };

    for (const auto &gs : m_members) {
        switch (gs.type) {
            case GroupedShapeType::Point: {
                Point *p = static_cast<Point*>(gs.ptr);
                p->setPos(rotatePoint(p->pos()));
                break;
            }
            case GroupedShapeType::Line: {
                Line *l = static_cast<Line*>(gs.ptr);
                l->setStartPos(rotatePoint(l->startPos()));
                l->setEndPos(rotatePoint(l->endPos()));
                break;
            }
            case GroupedShapeType::Polygon: {
                Polygon *poly = static_cast<Polygon*>(gs.ptr);
                for (int i = 0; i < poly->vertexCount(); ++i) {
                    QPointF pt = poly->vertexPositions()[i];
                    poly->setVertexPosition(i, rotatePoint(pt));
                }
                break;
            }
            case GroupedShapeType::Curve: {
                Curve *c = static_cast<Curve*>(gs.ptr);
                QVector<QPointF> pts = c->controlPoints();
                for (int i = 0; i < pts.size(); ++i) {
                    c->setControlPoint(i, rotatePoint(pts[i]));
                }
                break;
            }
            case GroupedShapeType::Ellipse: {
                Ellipse *e = static_cast<Ellipse*>(gs.ptr);
                e->setCenter(rotatePoint(e->center()));
                e->setRotation(e->rotation() + degrees);
                break;
            }
        }
    }
    m_bboxDirty = true;
    emit changed();
}

void Group::scale(float factor) {
    QRectF bbox = boundingBox();
    QPointF center = bbox.center();

    auto scalePoint = [&](QPointF pt) -> QPointF {
        QPointF rel = pt - center;
        return center + rel * factor;
    };

    for (const auto &gs : m_members) {
        switch (gs.type) {
            case GroupedShapeType::Point: {
                Point *p = static_cast<Point*>(gs.ptr);
                p->setPos(scalePoint(p->pos()));
                break;
            }
            case GroupedShapeType::Line: {
                Line *l = static_cast<Line*>(gs.ptr);
                l->setStartPos(scalePoint(l->startPos()));
                l->setEndPos(scalePoint(l->endPos()));
                break;
            }
            case GroupedShapeType::Polygon: {
                Polygon *poly = static_cast<Polygon*>(gs.ptr);
                for (int i = 0; i < poly->vertexCount(); ++i) {
                    QPointF pt = poly->vertexPositions()[i];
                    poly->setVertexPosition(i, scalePoint(pt));
                }
                poly->setScale(poly->scale() * factor);
                break;
            }
            case GroupedShapeType::Curve: {
                Curve *c = static_cast<Curve*>(gs.ptr);
                QVector<QPointF> pts = c->controlPoints();
                for (int i = 0; i < pts.size(); ++i) {
                    c->setControlPoint(i, scalePoint(pts[i]));
                }
                c->setScale(c->scale() * factor);
                break;
            }
            case GroupedShapeType::Ellipse: {
                Ellipse *e = static_cast<Ellipse*>(gs.ptr);
                e->setCenter(scalePoint(e->center()));
                e->setScale(e->scale() * factor);
                break;
            }
        }
    }
    m_bboxDirty = true;
    emit changed();
}

QString Group::tikz() const {
    QString result;
    result += "\\begin{scope}";
    if (!m_name.isEmpty()) {
        result += " % " + m_name;
    }
    result += "\n";

    for (const auto &gs : m_members) {
        switch (gs.type) {
            case GroupedShapeType::Point: {
                Point *p = static_cast<Point*>(gs.ptr);
                result += "  " + p->tikz() + "\n";
                break;
            }
            case GroupedShapeType::Line: {
                Line *l = static_cast<Line*>(gs.ptr);
                result += "  " + l->tikz() + "\n";
                break;
            }
            case GroupedShapeType::Polygon: {
                Polygon *poly = static_cast<Polygon*>(gs.ptr);
                result += "  " + poly->tikz() + "\n";
                break;
            }
            case GroupedShapeType::Curve: {
                Curve *c = static_cast<Curve*>(gs.ptr);
                result += "  " + c->tikz() + "\n";
                break;
            }
            case GroupedShapeType::Ellipse: {
                Ellipse *e = static_cast<Ellipse*>(gs.ptr);
                result += "  " + e->tikz() + "\n";
                break;
            }
        }
    }

    result += "\\end{scope}";
    return result;
}
