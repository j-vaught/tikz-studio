#ifndef ELLIPSEITEM_H
#define ELLIPSEITEM_H

#include <QGraphicsEllipseItem>

class Ellipse;

class EllipseItem : public QGraphicsEllipseItem {
public:
    explicit EllipseItem(Ellipse *ellipse, QGraphicsItem *parent = nullptr);

    Ellipse *ellipse() const { return m_ellipse; }

    void updateFromModel();

    enum { Type = UserType + 5 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    static constexpr float HANDLE_SIZE = 8.0f;
    static constexpr float ROTATION_HANDLE_DISTANCE = 30.0f;

    // Corner handles: 0=TopLeft, 1=TopRight, 2=BottomRight, 3=BottomLeft, 4=Rotation
    enum HandleType { None = -1, TopLeft = 0, TopRight = 1, BottomRight = 2, BottomLeft = 3, Rotation = 4 };

    HandleType handleAtPos(const QPointF &pos) const;
    QPointF handlePosition(HandleType handle) const;

    Ellipse *m_ellipse;
    HandleType m_dragHandle = None;
    QPointF m_dragStartPos;
    float m_dragStartRx = 0;
    float m_dragStartRy = 0;
    float m_dragStartRotation = 0;
};

#endif // ELLIPSEITEM_H
