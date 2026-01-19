#ifndef CURVE_H
#define CURVE_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QPainterPath>
#include <QString>
#include "common.h"

class Point;

class Curve : public QObject {
    Q_OBJECT

public:
    enum CurveType {
        Smooth,     // Catmull-Rom spline through points
        Bezier,     // Cubic bezier with explicit control points
        Straight    // Piecewise linear (polyline)
    };

    explicit Curve(QObject *parent = nullptr);

    // Control points
    QVector<QPointF> controlPoints() const;
    void addControlPoint(QPointF pos);
    void addControlPoint(Point *point);
    void insertControlPoint(int index, QPointF pos);
    void removeControlPoint(int index);
    void setControlPoint(int index, QPointF pos);
    int controlPointCount() const;
    void clear();

    // Curve type
    CurveType curveType() const { return m_curveType; }
    void setCurveType(CurveType type);

    // Tension for smooth curves (0 = linear, 1 = normal, >1 = tighter)
    float tension() const { return m_tension; }
    void setTension(float tension);

    // Appearance
    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    float lineWidth() const { return m_lineWidth; }
    void setLineWidth(float width);

    LineStyle lineStyle() const { return m_lineStyle; }
    void setLineStyle(LineStyle style);

    LineCap lineCap() const { return m_lineCap; }
    void setLineCap(LineCap cap);

    LineJoin lineJoin() const { return m_lineJoin; }
    void setLineJoin(LineJoin join);

    // For backwards compatibility
    bool isDashed() const { return m_lineStyle != LineStyle::Solid; }
    void setDashed(bool dashed) { setLineStyle(dashed ? LineStyle::Dashed : LineStyle::Solid); }

    bool isClosed() const { return m_closed; }
    void setClosed(bool closed);

    // Transform
    float rotation() const { return m_rotation; }
    void setRotation(float degrees);

    float scale() const { return m_scale; }
    void setScale(float scale);

    // Qt painter path (for rendering in canvas)
    QPainterPath painterPath() const;

    // TikZ generation
    QString tikz() const;

signals:
    void changed();

private:
    void onPointChanged();
    QPointF catmullRomToBezier(int segment, int controlIndex) const;

    QVector<Point*> m_pointRefs;
    QVector<QPointF> m_ownedPoints;
    QVector<bool> m_isRef;

    CurveType m_curveType = Smooth;
    float m_tension = 0.5f;
    QColor m_color;
    float m_lineWidth = 0.8f;
    float m_rotation = 0.0f;
    float m_scale = 1.0f;
    LineStyle m_lineStyle = LineStyle::Solid;
    LineCap m_lineCap = LineCap::Butt;
    LineJoin m_lineJoin = LineJoin::Miter;
    bool m_closed = false;
};

#endif // CURVE_H
