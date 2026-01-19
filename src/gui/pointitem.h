#ifndef POINTITEM_H
#define POINTITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

class Point;

class PointItem : public QGraphicsEllipseItem {
public:
    explicit PointItem(Point *point, QGraphicsItem *parent = nullptr);

    Point *point() const { return m_point; }

    void updateFromModel();

    // Type identification for qgraphicsitem_cast
    enum { Type = UserType + 1 };
    int type() const override { return Type; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    Point *m_point;
    bool m_dragging = false;
    QPointF m_dragStartPos;
};

#endif // POINTITEM_H
