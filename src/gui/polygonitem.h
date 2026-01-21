#ifndef POLYGONITEM_H
#define POLYGONITEM_H

#include <QGraphicsPathItem>

class Polygon;

class PolygonItem : public QGraphicsPathItem {
public:
    explicit PolygonItem(Polygon *polygon, QGraphicsItem *parent = nullptr);

    Polygon *polygon() const { return m_polygon; }

    void updateFromModel();

    enum { Type = UserType + 3 };
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

    // Corner handles for rescaling mode
    enum CornerHandle { NoCorner = -1, TopLeft = 0, TopRight = 1, BottomRight = 2, BottomLeft = 3, RotationHandle = 4 };

    // Vertex editing mode (Freeform, Triangle)
    int vertexHandleAtPos(const QPointF &pos) const;

    // Corner rescaling mode (all other shapes)
    CornerHandle cornerHandleAtPos(const QPointF &pos) const;
    QPointF cornerHandlePosition(CornerHandle handle) const;

    // Common
    bool rotationHandleAtPos(const QPointF &pos) const;
    QPointF rotationHandlePos() const;

    Polygon *m_polygon;

    // Vertex editing state
    int m_dragVertexHandle = -1;

    // Corner rescaling state
    CornerHandle m_dragCornerHandle = NoCorner;
    QRectF m_dragStartBounds;           // Original bounding box in TikZ coords
    QRectF m_dragStartScreenBounds;     // Original bounding box in screen coords
    QVector<QPointF> m_dragStartVertices;  // Original vertex positions

    // Common drag state
    bool m_draggingRotation = false;
    bool m_handlingDrag = false;  // True when we're handling a drag (skip base class)
    QPointF m_dragStartPos;
    float m_dragStartRotation = 0;
};

#endif // POLYGONITEM_H
