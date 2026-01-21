#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include <QVector>
#include <QRectF>
#include <QPointF>

// Forward declarations
class Point;
class Line;
class Polygon;
class Curve;
class Ellipse;

// Shape reference within a group
enum class GroupedShapeType { Point, Line, Polygon, Curve, Ellipse };

struct GroupedShape {
    GroupedShapeType type;
    void *ptr;

    bool operator==(const GroupedShape &other) const {
        return type == other.type && ptr == other.ptr;
    }
};

class Group : public QObject {
    Q_OBJECT

public:
    explicit Group(QObject *parent = nullptr);
    ~Group();

    // Member shapes
    const QVector<GroupedShape> &members() const { return m_members; }
    void addMember(void *shape, GroupedShapeType type);
    void removeMember(void *shape, GroupedShapeType type);
    bool contains(void *shape, GroupedShapeType type) const;
    int memberCount() const { return m_members.size(); }
    void clear();

    // Bounding box of all members (in TikZ coordinates)
    QRectF boundingBox() const;
    void updateBoundingBox();

    // Transform operations (applied to all members)
    void translate(QPointF delta);
    void rotate(float degrees);  // Around center
    void scale(float factor);    // From center

    // Name for identification
    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    // TikZ generation (wraps members in scope)
    QString tikz() const;

signals:
    void changed();

private:
    QVector<GroupedShape> m_members;
    QString m_name;
    mutable QRectF m_cachedBBox;
    mutable bool m_bboxDirty = true;
};

#endif // GROUP_H
