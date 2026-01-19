#ifndef POINT_H
#define POINT_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QString>

class Point : public QObject {
    Q_OBJECT

public:
    explicit Point(QPointF pos = QPointF(), QObject *parent = nullptr);

    QPointF pos() const { return m_pos; }
    void setPos(QPointF pos);

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    float radius() const { return m_radius; }
    void setRadius(float radius);

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible);

    QString tikz() const;

signals:
    void changed();

private:
    QPointF m_pos;
    QString m_name;
    QColor m_color;
    float m_radius = 2.0f;  // TikZ units (pt)
    bool m_visible = true;
};

#endif // POINT_H
