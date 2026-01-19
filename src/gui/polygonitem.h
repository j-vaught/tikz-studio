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

private:
    Polygon *m_polygon;
};

#endif // POLYGONITEM_H
