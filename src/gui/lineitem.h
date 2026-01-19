#ifndef LINEITEM_H
#define LINEITEM_H

#include <QGraphicsLineItem>

class Line;

class LineItem : public QGraphicsLineItem {
public:
    explicit LineItem(Line *line, QGraphicsItem *parent = nullptr);

    Line *line() const { return m_line; }

    void updateFromModel();

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

private:
    Line *m_line;
};

#endif // LINEITEM_H
