#ifndef POLYGON_H
#define POLYGON_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QString>
#include <QPainterPath>
#include "common.h"

class Point;

// Vertex with position and optional corner radius
struct Vertex {
    QPointF pos;
    float cornerRadius = 0.0f;  // 0 = sharp corner, >0 = rounded

    Vertex() = default;
    Vertex(QPointF p, float r = 0.0f) : pos(p), cornerRadius(r) {}
};

class Polygon : public QObject {
    Q_OBJECT

public:
    explicit Polygon(QObject *parent = nullptr);

    // Vertices
    QVector<Vertex> vertices() const { return m_vertices; }
    QVector<QPointF> vertexPositions() const;
    void addVertex(QPointF pos, float cornerRadius = 0.0f);
    void addVertex(Point *point);
    void insertVertex(int index, QPointF pos, float cornerRadius = 0.0f);
    void removeVertex(int index);
    void setVertexPosition(int index, QPointF pos);
    void setVertexCornerRadius(int index, float radius);
    float vertexCornerRadius(int index) const;
    int vertexCount() const;
    void clear();

    // Appearance
    QColor fillColor() const { return m_fillColor; }
    void setFillColor(const QColor &color);

    QColor strokeColor() const { return m_strokeColor; }
    void setStrokeColor(const QColor &color);

    float lineWidth() const { return m_lineWidth; }
    void setLineWidth(float width);

    LineStyle lineStyle() const { return m_lineStyle; }
    void setLineStyle(LineStyle style);

    FillPattern fillPattern() const { return m_fillPattern; }
    void setFillPattern(FillPattern pattern);

    LineCap lineCap() const { return m_lineCap; }
    void setLineCap(LineCap cap);

    LineJoin lineJoin() const { return m_lineJoin; }
    void setLineJoin(LineJoin join);

    float opacity() const { return m_opacity; }
    void setOpacity(float opacity);

    // Global corner radius (applies to vertices with radius=0)
    float defaultCornerRadius() const { return m_defaultCornerRadius; }
    void setDefaultCornerRadius(float radius);

    // Generate painter path with rounded corners
    QPainterPath painterPath() const;

    // TikZ generation
    QString tikz() const;

signals:
    void changed();

private:
    void onPointChanged();

    QVector<Vertex> m_vertices;
    QVector<Point*> m_pointRefs;  // Optional references to Point objects

    QColor m_fillColor;
    QColor m_strokeColor;
    float m_lineWidth = 0.8f;
    LineStyle m_lineStyle = LineStyle::Solid;
    FillPattern m_fillPattern = FillPattern::Solid;
    LineCap m_lineCap = LineCap::Butt;
    LineJoin m_lineJoin = LineJoin::Miter;
    float m_opacity = 1.0f;
    float m_defaultCornerRadius = 0.0f;
};

#endif // POLYGON_H
