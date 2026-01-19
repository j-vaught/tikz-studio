#ifndef LINE_H
#define LINE_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QString>
#include "common.h"

class Point;

class Line : public QObject {
    Q_OBJECT

public:
    explicit Line(QObject *parent = nullptr);
    Line(Point *start, Point *end, QObject *parent = nullptr);
    Line(QPointF start, QPointF end, QObject *parent = nullptr);

    // Point-based line (references external points)
    Point *startPoint() const { return m_startPoint; }
    Point *endPoint() const { return m_endPoint; }
    void setStartPoint(Point *p);
    void setEndPoint(Point *p);

    // Coordinate-based line (standalone)
    QPointF startPos() const;
    QPointF endPos() const;
    void setStartPos(QPointF pos);
    void setEndPos(QPointF pos);

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

    // Corner rounding (in TikZ units)
    float cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(float radius);

    // Transform
    float rotation() const { return m_rotation; }
    void setRotation(float degrees);

    float scale() const { return m_scale; }
    void setScale(float scale);

    QString tikz() const;

signals:
    void changed();

private:
    Point *m_startPoint = nullptr;
    Point *m_endPoint = nullptr;
    QPointF m_startPos;
    QPointF m_endPos;
    QColor m_color;
    float m_lineWidth = 0.8f;  // pt
    float m_cornerRadius = 0.0f;
    float m_rotation = 0.0f;
    float m_scale = 1.0f;
    LineStyle m_lineStyle = LineStyle::Solid;
    LineCap m_lineCap = LineCap::Butt;
    LineJoin m_lineJoin = LineJoin::Miter;
};

#endif // LINE_H
