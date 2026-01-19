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

private:
    Ellipse *m_ellipse;
};

#endif // ELLIPSEITEM_H
