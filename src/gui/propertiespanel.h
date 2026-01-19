#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <QWidget>
#include <QGraphicsItem>
#include "common.h"

class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
class QPushButton;
class QLabel;
class QVBoxLayout;
class QGroupBox;
class QComboBox;
class QScrollArea;
class ColorPalette;

class Point;
class Line;
class Polygon;
class Curve;
class Ellipse;

// Drawing defaults that are applied to new shapes
struct DrawingDefaults {
    // Stroke
    QColor strokeColor;
    float lineWidth = 0.8f;
    LineStyle lineStyle = LineStyle::Solid;
    LineCap lineCap = LineCap::Butt;
    LineJoin lineJoin = LineJoin::Miter;

    // Fill
    QColor fillColor;
    FillPattern fillPattern = FillPattern::Solid;
    float opacity = 1.0f;

    // Transform
    float rotation = 0.0f;
    float scale = 1.0f;

    // Shape-specific
    float cornerRadius = 0.0f;
    float curveTension = 0.5f;

    DrawingDefaults();
};

class PropertiesPanel : public QWidget {
    Q_OBJECT

public:
    explicit PropertiesPanel(QWidget *parent = nullptr);

    void setSelection(QList<QGraphicsItem*> items);
    void clearSelection();
    void setCurrentTool(Tool tool);

    // Get drawing defaults for creating new shapes
    const DrawingDefaults& drawingDefaults() const { return m_defaults; }

signals:
    void propertiesChanged();

    // Signals for syncing with context toolbar
    void lineWidthChanged(double width);
    void strokeColorChanged(const QColor &color);
    void fillColorChanged(const QColor &color);
    void lineStyleChanged(LineStyle style);
    void lineCapChanged(LineCap cap);
    void lineJoinChanged(LineJoin join);
    void fillPatternChanged(FillPattern pattern);
    void opacityChanged(double opacity);
    void rotationChanged(double angle);
    void scaleChanged(double scale);

public slots:
    // Slots for receiving updates from context toolbar
    void setLineWidth(double width);
    void setStrokeColor(const QColor &color);
    void setFillColor(const QColor &color);
    void setLineStyle(LineStyle style);
    void setLineCap(LineCap cap);
    void setLineJoin(LineJoin join);
    void setFillPattern(FillPattern pattern);
    void setOpacity(double opacity);
    void setRotation(double angle);
    void setScale(double scale);

private slots:
    void onLineWidthChanged(double value);
    void onOpacityChanged(double value);
    void onCornerRadiusChanged(double value);
    void onVertexCornerRadiusChanged(double value);
    void onStrokeColorClicked();
    void onFillColorClicked();
    void onLineStyleChanged(int index);
    void onLineCapChanged(int index);
    void onLineJoinChanged(int index);
    void onFillPatternChanged(int index);
    void onRotationChanged(double value);
    void onScaleChanged(double value);
    void onRadiusXChanged(double value);
    void onRadiusYChanged(double value);
    void onTensionChanged(double value);

private:
    void setupUI();
    void updateUI();
    void showPointProperties(Point *point);
    void showLineProperties(Line *line);
    void showPolygonProperties(Polygon *polygon);
    void showCurveProperties(Curve *curve);
    void showEllipseProperties(Ellipse *ellipse);
    void showToolDefaults(Tool tool);
    void hideAllGroups();
    void updateColorButton(QPushButton *btn, const QColor &color);

    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    QVBoxLayout *m_mainLayout;

    // Selection info
    QLabel *m_selectionLabel;

    // Stroke properties group
    QGroupBox *m_strokeGroup;
    QDoubleSpinBox *m_lineWidthSpin;
    QPushButton *m_strokeColorBtn;
    QComboBox *m_lineStyleCombo;
    QComboBox *m_lineCapCombo;
    QComboBox *m_lineJoinCombo;

    // Fill properties group
    QGroupBox *m_fillGroup;
    QPushButton *m_fillColorBtn;
    QComboBox *m_fillPatternCombo;
    QDoubleSpinBox *m_opacitySpin;

    // Transform properties group
    QGroupBox *m_transformGroup;
    QDoubleSpinBox *m_rotationSpin;
    QDoubleSpinBox *m_scaleSpin;

    // Corner radius (for polygons)
    QGroupBox *m_cornerGroup;
    QDoubleSpinBox *m_defaultCornerRadiusSpin;
    QLabel *m_vertexLabel;
    QSpinBox *m_vertexIndexSpin;
    QDoubleSpinBox *m_vertexCornerRadiusSpin;

    // Ellipse properties
    QGroupBox *m_ellipseGroup;
    QDoubleSpinBox *m_radiusXSpin;
    QDoubleSpinBox *m_radiusYSpin;

    // Curve properties
    QGroupBox *m_curveGroup;
    QDoubleSpinBox *m_tensionSpin;

    // Current state
    Tool m_currentTool = Tool::Select;
    QList<QGraphicsItem*> m_selection;
    Point *m_currentPoint = nullptr;
    Line *m_currentLine = nullptr;
    Polygon *m_currentPolygon = nullptr;
    Curve *m_currentCurve = nullptr;
    Ellipse *m_currentEllipse = nullptr;

    DrawingDefaults m_defaults;
    bool m_updating = false;
};

#endif // PROPERTIESPANEL_H
