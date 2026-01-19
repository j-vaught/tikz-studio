#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QUndoStack>

class Point;
class Line;
class Polygon;
class Curve;
class Ellipse;

class Document : public QObject {
    Q_OBJECT

public:
    explicit Document(QObject *parent = nullptr);
    ~Document();

    // Points
    const QVector<Point*> &points() const { return m_points; }
    Point *addPoint(QPointF pos);
    void removePoint(Point *point);

    // Lines
    const QVector<Line*> &lines() const { return m_lines; }
    Line *addLine(QPointF start, QPointF end);
    Line *addLine(Point *start, Point *end);
    void removeLine(Line *line);

    // Polygons
    const QVector<Polygon*> &polygons() const { return m_polygons; }
    Polygon *addPolygon();
    void removePolygon(Polygon *polygon);

    // Curves
    const QVector<Curve*> &curves() const { return m_curves; }
    Curve *addCurve();
    void removeCurve(Curve *curve);

    // Ellipses (includes circles)
    const QVector<Ellipse*> &ellipses() const { return m_ellipses; }
    Ellipse *addEllipse();
    Ellipse *addCircle(QPointF center, float radius);
    Ellipse *addEllipse(QPointF center, float rx, float ry);
    void removeEllipse(Ellipse *ellipse);

    // Clear all
    void clear();

    // Undo/Redo
    QUndoStack *undoStack() { return &m_undoStack; }

    // Background image
    QString backgroundImage() const { return m_backgroundImage; }
    void setBackgroundImage(const QString &path);
    float backgroundOpacity() const { return m_backgroundOpacity; }
    void setBackgroundOpacity(float opacity);

    // TikZ generation
    QString tikz() const;
    QString tikzPreamble() const;
    QString tikzFull() const;

    // File I/O
    bool save(const QString &path);
    bool load(const QString &path);

    // Generate unique point name
    QString freshPointName();

signals:
    void changed();
    void backgroundImageChanged();

private:
    QVector<Point*> m_points;
    QVector<Line*> m_lines;
    QVector<Polygon*> m_polygons;
    QVector<Curve*> m_curves;
    QVector<Ellipse*> m_ellipses;

    QUndoStack m_undoStack;
    QString m_backgroundImage;
    float m_backgroundOpacity = 0.5f;

    int m_pointCounter = 0;
};

#endif // DOCUMENT_H
