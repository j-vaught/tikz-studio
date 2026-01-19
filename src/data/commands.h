#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QPointF>
#include <QColor>
#include <QVector>
#include <QJsonObject>
#include <variant>
#include "common.h"
#include "document.h"

class Document;
class Canvas;
class Point;
class Line;
class Polygon;
class Curve;
class Ellipse;

// ============================================================================
// Create Shape Commands
// ============================================================================

class CreatePointCommand : public QUndoCommand {
public:
    CreatePointCommand(Document *doc, QPointF pos, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    Point *createdPoint() const { return m_point; }

private:
    Document *m_doc;
    QPointF m_pos;
    Point *m_point = nullptr;
    bool m_firstRedo = true;
};

class CreateLineCommand : public QUndoCommand {
public:
    CreateLineCommand(Document *doc, QPointF start, QPointF end, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    Line *createdLine() const { return m_line; }

private:
    Document *m_doc;
    QPointF m_start;
    QPointF m_end;
    Line *m_line = nullptr;
    bool m_firstRedo = true;
};

class CreatePolygonCommand : public QUndoCommand {
public:
    CreatePolygonCommand(Document *doc, const QVector<QPointF> &vertices, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    Polygon *createdPolygon() const { return m_polygon; }

private:
    Document *m_doc;
    QVector<QPointF> m_vertices;
    Polygon *m_polygon = nullptr;
    bool m_firstRedo = true;
};

class CreateCurveCommand : public QUndoCommand {
public:
    CreateCurveCommand(Document *doc, const QVector<QPointF> &controlPoints, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    Curve *createdCurve() const { return m_curve; }

private:
    Document *m_doc;
    QVector<QPointF> m_controlPoints;
    Curve *m_curve = nullptr;
    bool m_firstRedo = true;
};

class CreateEllipseCommand : public QUndoCommand {
public:
    CreateEllipseCommand(Document *doc, QPointF center, float rx, float ry, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    Ellipse *createdEllipse() const { return m_ellipse; }

private:
    Document *m_doc;
    QPointF m_center;
    float m_rx;
    float m_ry;
    Ellipse *m_ellipse = nullptr;
    bool m_firstRedo = true;
};

// ============================================================================
// Delete Shape Commands
// ============================================================================

class DeletePointCommand : public QUndoCommand {
public:
    DeletePointCommand(Document *doc, Point *point, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Document *m_doc;
    Point *m_point;
    QJsonObject m_savedData;
    int m_orderIndex = -1;
};

class DeleteLineCommand : public QUndoCommand {
public:
    DeleteLineCommand(Document *doc, Line *line, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Document *m_doc;
    Line *m_line;
    QJsonObject m_savedData;
    int m_orderIndex = -1;
};

class DeletePolygonCommand : public QUndoCommand {
public:
    DeletePolygonCommand(Document *doc, Polygon *polygon, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Document *m_doc;
    Polygon *m_polygon;
    QJsonObject m_savedData;
    int m_orderIndex = -1;
};

class DeleteCurveCommand : public QUndoCommand {
public:
    DeleteCurveCommand(Document *doc, Curve *curve, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Document *m_doc;
    Curve *m_curve;
    QJsonObject m_savedData;
    int m_orderIndex = -1;
};

class DeleteEllipseCommand : public QUndoCommand {
public:
    DeleteEllipseCommand(Document *doc, Ellipse *ellipse, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Document *m_doc;
    Ellipse *m_ellipse;
    QJsonObject m_savedData;
    int m_orderIndex = -1;
};

// ============================================================================
// Composite Delete Command (for multi-selection)
// ============================================================================

class DeleteSelectionCommand : public QUndoCommand {
public:
    DeleteSelectionCommand(Document *doc,
                          const QVector<Point*> &points,
                          const QVector<Line*> &lines,
                          const QVector<Polygon*> &polygons,
                          const QVector<Curve*> &curves,
                          const QVector<Ellipse*> &ellipses,
                          QUndoCommand *parent = nullptr);
    // Child commands handle undo/redo automatically
};

// ============================================================================
// Move Command
// ============================================================================

class MovePointCommand : public QUndoCommand {
public:
    MovePointCommand(Point *point, QPointF oldPos, QPointF newPos, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    int id() const override { return 1001; }
    bool mergeWith(const QUndoCommand *other) override;

private:
    Point *m_point;
    QPointF m_oldPos;
    QPointF m_newPos;
};

#endif // COMMANDS_H
