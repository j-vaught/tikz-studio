#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>

class Group;

class GroupItem : public QGraphicsItemGroup {
public:
    explicit GroupItem(Group *group, QGraphicsItem *parent = nullptr);
    ~GroupItem();

    Group *group() const { return m_group; }

    void updateFromModel();

    // Show/hide the group bounding box indicator
    void setShowBounds(bool show);
    bool showBounds() const { return m_showBounds; }

    enum { Type = UserType + 6 };
    int type() const override { return Type; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    Group *m_group;
    bool m_showBounds = false;
};

#endif // GROUPITEM_H
