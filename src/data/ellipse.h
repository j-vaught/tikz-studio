#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QString>
#include "common.h"

class Ellipse : public QObject {
    Q_OBJECT

public:
    explicit Ellipse(QObject *parent = nullptr);

    // Center position
    QPointF center() const { return m_center; }
    void setCenter(QPointF center);

    // Radii
    float radiusX() const { return m_radiusX; }
    void setRadiusX(float rx);

    float radiusY() const { return m_radiusY; }
    void setRadiusY(float ry);

    // For circles, use this convenience method
    void setRadius(float r) { setRadiusX(r); setRadiusY(r); }
    bool isCircle() const { return qFuzzyCompare(m_radiusX, m_radiusY); }

    // Rotation (degrees)
    float rotation() const { return m_rotation; }
    void setRotation(float degrees);

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

    // TikZ generation
    QString tikz() const;

signals:
    void changed();

private:
    QPointF m_center;
    float m_radiusX = 1.0f;
    float m_radiusY = 1.0f;
    float m_rotation = 0.0f;

    QColor m_fillColor;
    QColor m_strokeColor;
    float m_lineWidth = 0.8f;
    LineStyle m_lineStyle = LineStyle::Solid;
    FillPattern m_fillPattern = FillPattern::Solid;
    LineCap m_lineCap = LineCap::Butt;
    LineJoin m_lineJoin = LineJoin::Miter;
    float m_opacity = 1.0f;
};

#endif // ELLIPSE_H
