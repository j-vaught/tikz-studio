#ifndef CURVEITEM_H
#define CURVEITEM_H

#include <QGraphicsPathItem>

class Curve;

class CurveItem : public QGraphicsPathItem {
public:
    explicit CurveItem(Curve *curve, QGraphicsItem *parent = nullptr);

    Curve *curve() const { return m_curve; }

    void updateFromModel();

    enum { Type = UserType + 4 };
    int type() const override { return Type; }

private:
    Curve *m_curve;
};

#endif // CURVEITEM_H
