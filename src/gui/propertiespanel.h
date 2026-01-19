#ifndef PROPERTIESPANEL_H
#define PROPERTIESPANEL_H

#include <QWidget>
#include <QGraphicsItem>

class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
class QPushButton;
class QLabel;
class QVBoxLayout;
class QGroupBox;
class ColorPalette;

class Point;
class Line;
class Polygon;
class Curve;
class Ellipse;

class PropertiesPanel : public QWidget {
    Q_OBJECT

public:
    explicit PropertiesPanel(QWidget *parent = nullptr);

    void setSelection(QList<QGraphicsItem*> items);
    void clearSelection();

signals:
    void propertiesChanged();

private slots:
    void onCornerRadiusChanged(double value);
    void onLineWidthChanged(double value);
    void onOpacityChanged(double value);
    void onDashedChanged(int state);
    void onVertexCornerRadiusChanged(double value);

private:
    void setupUI();
    void updateUI();
    void showPointProperties(Point *point);
    void showLineProperties(Line *line);
    void showPolygonProperties(Polygon *polygon);
    void showCurveProperties(Curve *curve);
    void showEllipseProperties(Ellipse *ellipse);
    void hideAllGroups();

    QVBoxLayout *m_mainLayout;

    // Selection info
    QLabel *m_selectionLabel;

    // Common properties
    QGroupBox *m_commonGroup;
    QDoubleSpinBox *m_lineWidthSpin;
    QDoubleSpinBox *m_opacitySpin;
    QCheckBox *m_dashedCheck;

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
    QDoubleSpinBox *m_rotationSpin;

    // Current selection
    QList<QGraphicsItem*> m_selection;
    Point *m_currentPoint = nullptr;
    Line *m_currentLine = nullptr;
    Polygon *m_currentPolygon = nullptr;
    Curve *m_currentCurve = nullptr;
    Ellipse *m_currentEllipse = nullptr;

    bool m_updating = false;
};

#endif // PROPERTIESPANEL_H
