#ifndef LINE_H
#define LINE_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QString>

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

    bool isDashed() const { return m_dashed; }
    void setDashed(bool dashed);

    // Corner rounding (in TikZ units)
    float cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(float radius);

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
    bool m_dashed = false;
};

#endif // LINE_H
