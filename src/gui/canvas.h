#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>
#include <QMap>
#include "common.h"

class Document;
class Point;
class Line;
class Polygon;
class Curve;
class Ellipse;
class PointItem;
class LineItem;
class PolygonItem;
class CurveItem;
class EllipseItem;
class ToolPalette;
class ColorPalette;

class Canvas : public QGraphicsScene {
    Q_OBJECT

public:
    explicit Canvas(Document *doc, QObject *parent = nullptr);

    Document *document() const { return m_document; }

    void setToolPalette(ToolPalette *tools) { m_tools = tools; }
    void setColorPalette(ColorPalette *colors) { m_colors = colors; }

    // Number of sides for regular polygon tool
    int regularPolygonSides() const { return m_regPolySides; }
    void setRegularPolygonSides(int sides) { m_regPolySides = qBound(3, sides, 20); }

    // Sync graphics items with document
    void syncWithDocument();

    // Delete selected items
    void deleteSelected();

    // Grid visibility
    bool gridVisible() const { return m_gridVisible; }
    void setGridVisible(bool visible);

signals:
    void selectionChanged();
    void statusMessage(const QString &msg);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    void addPointItem(Point *point);
    void addLineItem(Line *line);
    void addPolygonItem(Polygon *polygon);
    void addCurveItem(Curve *curve);
    void addEllipseItem(Ellipse *ellipse);

    void handleSelectTool(QGraphicsSceneMouseEvent *event);
    void handlePointTool(QGraphicsSceneMouseEvent *event);
    void handleLineTool(QGraphicsSceneMouseEvent *event);
    void handlePolygonTool(QGraphicsSceneMouseEvent *event);
    void handleCurveTool(QGraphicsSceneMouseEvent *event);
    void handleRectangleTool(QGraphicsSceneMouseEvent *event);
    void handleCircleTool(QGraphicsSceneMouseEvent *event);
    void handleEllipseTool(QGraphicsSceneMouseEvent *event);
    void handleTriangleTool(QGraphicsSceneMouseEvent *event);
    void handleRegularPolygonTool(QGraphicsSceneMouseEvent *event);

    void handleShapeDrag(QGraphicsSceneMouseEvent *event);
    void finishShapeDrag(QGraphicsSceneMouseEvent *event);

    void showContextMenu(QGraphicsSceneContextMenuEvent *event);

    Document *m_document;
    ToolPalette *m_tools = nullptr;
    ColorPalette *m_colors = nullptr;

    QMap<Point*, PointItem*> m_pointItems;
    QMap<Line*, LineItem*> m_lineItems;
    QMap<Polygon*, PolygonItem*> m_polygonItems;
    QMap<Curve*, CurveItem*> m_curveItems;
    QMap<Ellipse*, EllipseItem*> m_ellipseItems;

    // Interaction state
    bool m_drawing = false;
    QPointF m_lineStartPos;
    Point *m_lineStartPoint = nullptr;
    QGraphicsLineItem *m_previewLine = nullptr;

    Polygon *m_currentPolygon = nullptr;
    Curve *m_currentCurve = nullptr;

    // Shape dragging (for rect, circle, ellipse, triangle, ngon)
    bool m_shapeDragging = false;
    QPointF m_shapeStartPos;
    QGraphicsItem *m_previewShape = nullptr;

    int m_regPolySides = 6;  // Default for N-gon

    bool m_gridVisible = true;
};

#endif // CANVAS_H
