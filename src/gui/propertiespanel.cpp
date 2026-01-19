#include "propertiespanel.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"
#include "pointitem.h"
#include "lineitem.h"
#include "polygonitem.h"
#include "curveitem.h"
#include "ellipseitem.h"
#include "common.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QComboBox>
#include <QScrollArea>
#include <QColorDialog>

DrawingDefaults::DrawingDefaults()
    : strokeColor(UofSC::Black())
    , fillColor(UofSC::Garnet())
{
}

PropertiesPanel::PropertiesPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void PropertiesPanel::setupUI() {
    // Main layout for this widget
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    // Create scroll area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    // Create content widget
    m_contentWidget = new QWidget();
    m_mainLayout = new QVBoxLayout(m_contentWidget);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);

    // Selection/Tool info
    m_selectionLabel = new QLabel("No selection");
    m_selectionLabel->setStyleSheet("font-weight: bold; color: #000000; padding: 4px; font-size: 12px;");
    m_mainLayout->addWidget(m_selectionLabel);

    // === STROKE GROUP ===
    m_strokeGroup = new QGroupBox("Stroke");
    m_strokeGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *strokeLayout = new QFormLayout(m_strokeGroup);

    // Line width
    m_lineWidthSpin = new QDoubleSpinBox();
    m_lineWidthSpin->setRange(0.1, 10.0);
    m_lineWidthSpin->setSingleStep(0.1);
    m_lineWidthSpin->setValue(0.8);
    m_lineWidthSpin->setSuffix(" pt");
    strokeLayout->addRow("Width:", m_lineWidthSpin);
    connect(m_lineWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onLineWidthChanged);

    // Stroke color
    m_strokeColorBtn = new QPushButton();
    m_strokeColorBtn->setFixedSize(60, 24);
    m_strokeColorBtn->setToolTip("Stroke color");
    updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
    strokeLayout->addRow("Color:", m_strokeColorBtn);
    connect(m_strokeColorBtn, &QPushButton::clicked, this, &PropertiesPanel::onStrokeColorClicked);

    // Line style
    m_lineStyleCombo = new QComboBox();
    m_lineStyleCombo->addItem("Solid", static_cast<int>(LineStyle::Solid));
    m_lineStyleCombo->addItem("Dashed", static_cast<int>(LineStyle::Dashed));
    m_lineStyleCombo->addItem("Densely Dashed", static_cast<int>(LineStyle::DenselyDashed));
    m_lineStyleCombo->addItem("Loosely Dashed", static_cast<int>(LineStyle::LooselyDashed));
    m_lineStyleCombo->addItem("Dotted", static_cast<int>(LineStyle::Dotted));
    m_lineStyleCombo->addItem("Densely Dotted", static_cast<int>(LineStyle::DenselyDotted));
    m_lineStyleCombo->addItem("Loosely Dotted", static_cast<int>(LineStyle::LooselyDotted));
    m_lineStyleCombo->addItem("Dash-Dot", static_cast<int>(LineStyle::DashDot));
    m_lineStyleCombo->addItem("Densely Dash-Dot", static_cast<int>(LineStyle::DenselyDashDot));
    m_lineStyleCombo->addItem("Loosely Dash-Dot", static_cast<int>(LineStyle::LooselyDashDot));
    m_lineStyleCombo->addItem("Dash-Dot-Dot", static_cast<int>(LineStyle::DashDotDot));
    m_lineStyleCombo->addItem("Densely Dash-Dot-Dot", static_cast<int>(LineStyle::DenselyDashDotDot));
    m_lineStyleCombo->addItem("Loosely Dash-Dot-Dot", static_cast<int>(LineStyle::LooselyDashDotDot));
    strokeLayout->addRow("Style:", m_lineStyleCombo);
    connect(m_lineStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertiesPanel::onLineStyleChanged);

    // Line cap
    m_lineCapCombo = new QComboBox();
    m_lineCapCombo->addItem("Butt", static_cast<int>(LineCap::Butt));
    m_lineCapCombo->addItem("Round", static_cast<int>(LineCap::Round));
    m_lineCapCombo->addItem("Square", static_cast<int>(LineCap::Square));
    strokeLayout->addRow("Cap:", m_lineCapCombo);
    connect(m_lineCapCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertiesPanel::onLineCapChanged);

    // Line join
    m_lineJoinCombo = new QComboBox();
    m_lineJoinCombo->addItem("Miter", static_cast<int>(LineJoin::Miter));
    m_lineJoinCombo->addItem("Round", static_cast<int>(LineJoin::Round));
    m_lineJoinCombo->addItem("Bevel", static_cast<int>(LineJoin::Bevel));
    strokeLayout->addRow("Join:", m_lineJoinCombo);
    connect(m_lineJoinCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertiesPanel::onLineJoinChanged);

    m_mainLayout->addWidget(m_strokeGroup);
    m_strokeGroup->hide();

    // === FILL GROUP ===
    m_fillGroup = new QGroupBox("Fill");
    m_fillGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *fillLayout = new QFormLayout(m_fillGroup);

    // Fill color
    m_fillColorBtn = new QPushButton();
    m_fillColorBtn->setFixedSize(60, 24);
    m_fillColorBtn->setToolTip("Fill color");
    updateColorButton(m_fillColorBtn, m_defaults.fillColor);
    fillLayout->addRow("Color:", m_fillColorBtn);
    connect(m_fillColorBtn, &QPushButton::clicked, this, &PropertiesPanel::onFillColorClicked);

    // Fill pattern
    m_fillPatternCombo = new QComboBox();
    m_fillPatternCombo->addItem("None", static_cast<int>(FillPattern::None));
    m_fillPatternCombo->addItem("Solid", static_cast<int>(FillPattern::Solid));
    m_fillPatternCombo->addItem("Horizontal Lines", static_cast<int>(FillPattern::HorizontalLines));
    m_fillPatternCombo->addItem("Vertical Lines", static_cast<int>(FillPattern::VerticalLines));
    m_fillPatternCombo->addItem("Grid", static_cast<int>(FillPattern::Grid));
    m_fillPatternCombo->addItem("NE Lines", static_cast<int>(FillPattern::NorthEastLines));
    m_fillPatternCombo->addItem("NW Lines", static_cast<int>(FillPattern::NorthWestLines));
    m_fillPatternCombo->addItem("Crosshatch", static_cast<int>(FillPattern::CrossHatch));
    m_fillPatternCombo->addItem("Crosshatch Dots", static_cast<int>(FillPattern::CrossHatchDots));
    m_fillPatternCombo->addItem("Dots", static_cast<int>(FillPattern::Dots));
    m_fillPatternCombo->addItem("5-Pointed Stars", static_cast<int>(FillPattern::FivePointedStars));
    m_fillPatternCombo->addItem("6-Pointed Stars", static_cast<int>(FillPattern::SixPointedStars));
    m_fillPatternCombo->addItem("Bricks", static_cast<int>(FillPattern::Bricks));
    m_fillPatternCombo->setCurrentIndex(1);  // Solid by default
    fillLayout->addRow("Pattern:", m_fillPatternCombo);
    connect(m_fillPatternCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertiesPanel::onFillPatternChanged);

    // Opacity
    m_opacitySpin = new QDoubleSpinBox();
    m_opacitySpin->setRange(0.0, 1.0);
    m_opacitySpin->setSingleStep(0.1);
    m_opacitySpin->setValue(1.0);
    fillLayout->addRow("Opacity:", m_opacitySpin);
    connect(m_opacitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onOpacityChanged);

    m_mainLayout->addWidget(m_fillGroup);
    m_fillGroup->hide();

    // === TRANSFORM GROUP ===
    m_transformGroup = new QGroupBox("Transform");
    m_transformGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *transformLayout = new QFormLayout(m_transformGroup);

    // Rotation
    m_rotationSpin = new QDoubleSpinBox();
    m_rotationSpin->setRange(-180.0, 180.0);
    m_rotationSpin->setSingleStep(5.0);
    m_rotationSpin->setValue(0.0);
    m_rotationSpin->setSuffix(" deg");
    transformLayout->addRow("Rotation:", m_rotationSpin);
    connect(m_rotationSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onRotationChanged);

    // Scale
    m_scaleSpin = new QDoubleSpinBox();
    m_scaleSpin->setRange(0.1, 10.0);
    m_scaleSpin->setSingleStep(0.1);
    m_scaleSpin->setValue(1.0);
    m_scaleSpin->setSuffix("x");
    transformLayout->addRow("Scale:", m_scaleSpin);
    connect(m_scaleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onScaleChanged);

    m_mainLayout->addWidget(m_transformGroup);
    m_transformGroup->hide();

    // === CORNER RADIUS GROUP ===
    m_cornerGroup = new QGroupBox("Corners");
    m_cornerGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *cornerLayout = new QFormLayout(m_cornerGroup);

    m_defaultCornerRadiusSpin = new QDoubleSpinBox();
    m_defaultCornerRadiusSpin->setRange(0.0, 5.0);
    m_defaultCornerRadiusSpin->setSingleStep(0.1);
    m_defaultCornerRadiusSpin->setValue(0.0);
    cornerLayout->addRow("All Corners:", m_defaultCornerRadiusSpin);
    connect(m_defaultCornerRadiusSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onCornerRadiusChanged);

    m_vertexLabel = new QLabel("Per-Vertex:");
    cornerLayout->addRow(m_vertexLabel);

    QHBoxLayout *vertexLayout = new QHBoxLayout();
    m_vertexIndexSpin = new QSpinBox();
    m_vertexIndexSpin->setRange(0, 0);
    m_vertexIndexSpin->setPrefix("V");
    vertexLayout->addWidget(m_vertexIndexSpin);

    m_vertexCornerRadiusSpin = new QDoubleSpinBox();
    m_vertexCornerRadiusSpin->setRange(0.0, 5.0);
    m_vertexCornerRadiusSpin->setSingleStep(0.1);
    m_vertexCornerRadiusSpin->setValue(0.0);
    vertexLayout->addWidget(m_vertexCornerRadiusSpin);
    cornerLayout->addRow(vertexLayout);

    connect(m_vertexIndexSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int) { updateUI(); });
    connect(m_vertexCornerRadiusSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onVertexCornerRadiusChanged);

    m_mainLayout->addWidget(m_cornerGroup);
    m_cornerGroup->hide();

    // === ELLIPSE GROUP ===
    m_ellipseGroup = new QGroupBox("Ellipse");
    m_ellipseGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *ellipseLayout = new QFormLayout(m_ellipseGroup);

    m_radiusXSpin = new QDoubleSpinBox();
    m_radiusXSpin->setRange(0.1, 20.0);
    m_radiusXSpin->setSingleStep(0.25);
    m_radiusXSpin->setValue(1.0);
    ellipseLayout->addRow("Radius X:", m_radiusXSpin);
    connect(m_radiusXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onRadiusXChanged);

    m_radiusYSpin = new QDoubleSpinBox();
    m_radiusYSpin->setRange(0.1, 20.0);
    m_radiusYSpin->setSingleStep(0.25);
    m_radiusYSpin->setValue(1.0);
    ellipseLayout->addRow("Radius Y:", m_radiusYSpin);
    connect(m_radiusYSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onRadiusYChanged);

    m_mainLayout->addWidget(m_ellipseGroup);
    m_ellipseGroup->hide();

    // === CURVE GROUP ===
    m_curveGroup = new QGroupBox("Curve");
    m_curveGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *curveLayout = new QFormLayout(m_curveGroup);

    m_tensionSpin = new QDoubleSpinBox();
    m_tensionSpin->setRange(0.0, 2.0);
    m_tensionSpin->setSingleStep(0.1);
    m_tensionSpin->setValue(0.5);
    curveLayout->addRow("Tension:", m_tensionSpin);
    connect(m_tensionSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onTensionChanged);

    m_mainLayout->addWidget(m_curveGroup);
    m_curveGroup->hide();

    // Add stretch at bottom
    m_mainLayout->addStretch();

    // Set scroll area content
    m_scrollArea->setWidget(m_contentWidget);
    outerLayout->addWidget(m_scrollArea);
}

void PropertiesPanel::hideAllGroups() {
    m_strokeGroup->hide();
    m_fillGroup->hide();
    m_transformGroup->hide();
    m_cornerGroup->hide();
    m_ellipseGroup->hide();
    m_curveGroup->hide();
}

void PropertiesPanel::updateColorButton(QPushButton *btn, const QColor &color) {
    QString style = QString(
        "QPushButton { background-color: %1; border: 2px solid #000000; }"
        "QPushButton:hover { border: 3px solid #000000; }"
    ).arg(color.name());
    btn->setStyleSheet(style);
}

void PropertiesPanel::setCurrentTool(Tool tool) {
    m_currentTool = tool;

    if (tool == Tool::Select) {
        // In select mode, show properties of selection
        if (!m_selection.isEmpty()) {
            setSelection(m_selection);
        } else {
            hideAllGroups();
            m_selectionLabel->setText("No selection");
        }
    } else {
        // In drawing mode, show defaults for that tool
        m_currentPoint = nullptr;
        m_currentLine = nullptr;
        m_currentPolygon = nullptr;
        m_currentCurve = nullptr;
        m_currentEllipse = nullptr;
        showToolDefaults(tool);
    }
}

void PropertiesPanel::showToolDefaults(Tool tool) {
    hideAllGroups();
    m_updating = true;

    QString toolName;
    switch (tool) {
        case Tool::Point:
            toolName = "Point Tool";
            // Points have minimal properties
            break;

        case Tool::Line:
            toolName = "Line Tool";
            m_strokeGroup->show();
            m_transformGroup->show();
            m_lineWidthSpin->setValue(m_defaults.lineWidth);
            updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
            m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->findData(static_cast<int>(m_defaults.lineStyle)));
            m_lineCapCombo->setCurrentIndex(m_lineCapCombo->findData(static_cast<int>(m_defaults.lineCap)));
            m_lineJoinCombo->setCurrentIndex(m_lineJoinCombo->findData(static_cast<int>(m_defaults.lineJoin)));
            m_rotationSpin->setValue(m_defaults.rotation);
            m_scaleSpin->setValue(m_defaults.scale);
            break;

        case Tool::Rectangle:
            toolName = "Rectangle Tool";
            m_strokeGroup->show();
            m_fillGroup->show();
            m_transformGroup->show();
            m_cornerGroup->show();
            m_lineWidthSpin->setValue(m_defaults.lineWidth);
            updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
            updateColorButton(m_fillColorBtn, m_defaults.fillColor);
            m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->findData(static_cast<int>(m_defaults.lineStyle)));
            m_lineCapCombo->setCurrentIndex(m_lineCapCombo->findData(static_cast<int>(m_defaults.lineCap)));
            m_lineJoinCombo->setCurrentIndex(m_lineJoinCombo->findData(static_cast<int>(m_defaults.lineJoin)));
            m_fillPatternCombo->setCurrentIndex(m_fillPatternCombo->findData(static_cast<int>(m_defaults.fillPattern)));
            m_opacitySpin->setValue(m_defaults.opacity);
            m_rotationSpin->setValue(m_defaults.rotation);
            m_scaleSpin->setValue(m_defaults.scale);
            m_defaultCornerRadiusSpin->setValue(m_defaults.cornerRadius);
            m_vertexLabel->hide();
            m_vertexIndexSpin->hide();
            m_vertexCornerRadiusSpin->hide();
            break;

        case Tool::Triangle:
            toolName = "Triangle Tool";
            m_strokeGroup->show();
            m_fillGroup->show();
            m_transformGroup->show();
            m_cornerGroup->show();
            m_lineWidthSpin->setValue(m_defaults.lineWidth);
            updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
            updateColorButton(m_fillColorBtn, m_defaults.fillColor);
            m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->findData(static_cast<int>(m_defaults.lineStyle)));
            m_fillPatternCombo->setCurrentIndex(m_fillPatternCombo->findData(static_cast<int>(m_defaults.fillPattern)));
            m_opacitySpin->setValue(m_defaults.opacity);
            m_rotationSpin->setValue(m_defaults.rotation);
            m_scaleSpin->setValue(m_defaults.scale);
            m_defaultCornerRadiusSpin->setValue(m_defaults.cornerRadius);
            m_vertexLabel->hide();
            m_vertexIndexSpin->hide();
            m_vertexCornerRadiusSpin->hide();
            break;

        case Tool::Polygon:
        case Tool::RegularPolygon:
            toolName = (tool == Tool::RegularPolygon) ? "N-gon Tool" : "Polygon Tool";
            m_strokeGroup->show();
            m_fillGroup->show();
            m_transformGroup->show();
            m_cornerGroup->show();
            m_lineWidthSpin->setValue(m_defaults.lineWidth);
            updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
            updateColorButton(m_fillColorBtn, m_defaults.fillColor);
            m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->findData(static_cast<int>(m_defaults.lineStyle)));
            m_fillPatternCombo->setCurrentIndex(m_fillPatternCombo->findData(static_cast<int>(m_defaults.fillPattern)));
            m_opacitySpin->setValue(m_defaults.opacity);
            m_rotationSpin->setValue(m_defaults.rotation);
            m_scaleSpin->setValue(m_defaults.scale);
            m_defaultCornerRadiusSpin->setValue(m_defaults.cornerRadius);
            m_vertexLabel->hide();
            m_vertexIndexSpin->hide();
            m_vertexCornerRadiusSpin->hide();
            break;

        case Tool::Circle:
            toolName = "Circle Tool";
            m_strokeGroup->show();
            m_fillGroup->show();
            m_transformGroup->show();
            m_lineWidthSpin->setValue(m_defaults.lineWidth);
            updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
            updateColorButton(m_fillColorBtn, m_defaults.fillColor);
            m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->findData(static_cast<int>(m_defaults.lineStyle)));
            m_fillPatternCombo->setCurrentIndex(m_fillPatternCombo->findData(static_cast<int>(m_defaults.fillPattern)));
            m_opacitySpin->setValue(m_defaults.opacity);
            m_rotationSpin->setValue(m_defaults.rotation);
            m_scaleSpin->setValue(m_defaults.scale);
            break;

        case Tool::Ellipse:
            toolName = "Ellipse Tool";
            m_strokeGroup->show();
            m_fillGroup->show();
            m_transformGroup->show();
            m_lineWidthSpin->setValue(m_defaults.lineWidth);
            updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
            updateColorButton(m_fillColorBtn, m_defaults.fillColor);
            m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->findData(static_cast<int>(m_defaults.lineStyle)));
            m_fillPatternCombo->setCurrentIndex(m_fillPatternCombo->findData(static_cast<int>(m_defaults.fillPattern)));
            m_opacitySpin->setValue(m_defaults.opacity);
            m_rotationSpin->setValue(m_defaults.rotation);
            m_scaleSpin->setValue(m_defaults.scale);
            break;

        case Tool::Curve:
            toolName = "Curve Tool";
            m_strokeGroup->show();
            m_transformGroup->show();
            m_curveGroup->show();
            m_lineWidthSpin->setValue(m_defaults.lineWidth);
            updateColorButton(m_strokeColorBtn, m_defaults.strokeColor);
            m_lineStyleCombo->setCurrentIndex(m_lineStyleCombo->findData(static_cast<int>(m_defaults.lineStyle)));
            m_rotationSpin->setValue(m_defaults.rotation);
            m_scaleSpin->setValue(m_defaults.scale);
            m_tensionSpin->setValue(m_defaults.curveTension);
            break;

        default:
            toolName = "Select";
            break;
    }

    m_selectionLabel->setText(toolName + " Defaults");
    m_updating = false;
}

void PropertiesPanel::setSelection(QList<QGraphicsItem*> items) {
    m_selection = items;
    m_currentPoint = nullptr;
    m_currentLine = nullptr;
    m_currentPolygon = nullptr;
    m_currentCurve = nullptr;
    m_currentEllipse = nullptr;

    // If not in select mode, don't show selection properties
    if (m_currentTool != Tool::Select) {
        return;
    }

    hideAllGroups();

    if (items.isEmpty()) {
        m_selectionLabel->setText("No selection");
        return;
    }

    if (items.size() > 1) {
        m_selectionLabel->setText(QString("%1 items selected").arg(items.size()));
        return;
    }

    QGraphicsItem *item = items.first();

    if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
        m_currentPoint = pi->point();
        showPointProperties(m_currentPoint);
    } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
        m_currentLine = li->line();
        showLineProperties(m_currentLine);
    } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
        m_currentPolygon = pi->polygon();
        showPolygonProperties(m_currentPolygon);
    } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
        m_currentCurve = ci->curve();
        showCurveProperties(m_currentCurve);
    } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
        m_currentEllipse = ei->ellipse();
        showEllipseProperties(m_currentEllipse);
    }
}

void PropertiesPanel::clearSelection() {
    m_selection.clear();
    m_currentPoint = nullptr;
    m_currentLine = nullptr;
    m_currentPolygon = nullptr;
    m_currentCurve = nullptr;
    m_currentEllipse = nullptr;

    if (m_currentTool == Tool::Select) {
        hideAllGroups();
        m_selectionLabel->setText("No selection");
    }
}

void PropertiesPanel::showPointProperties(Point *point) {
    if (!point) return;
    m_selectionLabel->setText("Point");
    // Points don't have many editable properties
}

void PropertiesPanel::showLineProperties(Line *line) {
    if (!line) return;
    m_selectionLabel->setText("Line");

    m_updating = true;

    m_strokeGroup->show();
    m_transformGroup->show();

    m_lineWidthSpin->setValue(line->lineWidth());
    updateColorButton(m_strokeColorBtn, line->color());

    // Line uses dashed bool, map to line style
    int styleIdx = line->isDashed() ? 1 : 0;
    m_lineStyleCombo->setCurrentIndex(styleIdx);

    m_rotationSpin->setValue(line->rotation());
    m_scaleSpin->setValue(line->scale());

    m_updating = false;
}

void PropertiesPanel::showPolygonProperties(Polygon *polygon) {
    if (!polygon) return;
    m_selectionLabel->setText("Polygon");

    m_updating = true;

    m_strokeGroup->show();
    m_fillGroup->show();
    m_transformGroup->show();
    m_cornerGroup->show();

    m_lineWidthSpin->setValue(polygon->lineWidth());
    updateColorButton(m_strokeColorBtn, polygon->strokeColor());
    updateColorButton(m_fillColorBtn, polygon->fillColor());

    int lineStyleIdx = m_lineStyleCombo->findData(static_cast<int>(polygon->lineStyle()));
    if (lineStyleIdx >= 0) m_lineStyleCombo->setCurrentIndex(lineStyleIdx);

    int lineCapIdx = m_lineCapCombo->findData(static_cast<int>(polygon->lineCap()));
    if (lineCapIdx >= 0) m_lineCapCombo->setCurrentIndex(lineCapIdx);

    int lineJoinIdx = m_lineJoinCombo->findData(static_cast<int>(polygon->lineJoin()));
    if (lineJoinIdx >= 0) m_lineJoinCombo->setCurrentIndex(lineJoinIdx);

    int fillPatternIdx = m_fillPatternCombo->findData(static_cast<int>(polygon->fillPattern()));
    if (fillPatternIdx >= 0) m_fillPatternCombo->setCurrentIndex(fillPatternIdx);

    m_opacitySpin->setValue(polygon->opacity());
    m_rotationSpin->setValue(polygon->rotation());
    m_scaleSpin->setValue(polygon->scale());

    m_defaultCornerRadiusSpin->setValue(polygon->defaultCornerRadius());
    m_vertexLabel->show();
    m_vertexIndexSpin->show();
    m_vertexCornerRadiusSpin->show();
    m_vertexIndexSpin->setRange(0, polygon->vertexCount() - 1);
    if (polygon->vertexCount() > 0) {
        m_vertexCornerRadiusSpin->setValue(polygon->vertices()[0].cornerRadius);
    }

    m_updating = false;
}

void PropertiesPanel::showCurveProperties(Curve *curve) {
    if (!curve) return;
    m_selectionLabel->setText("Curve");

    m_updating = true;

    m_strokeGroup->show();
    m_transformGroup->show();
    m_curveGroup->show();

    m_lineWidthSpin->setValue(curve->lineWidth());
    updateColorButton(m_strokeColorBtn, curve->color());

    // Curve uses dashed bool
    int styleIdx = curve->isDashed() ? 1 : 0;
    m_lineStyleCombo->setCurrentIndex(styleIdx);

    m_rotationSpin->setValue(curve->rotation());
    m_scaleSpin->setValue(curve->scale());
    m_tensionSpin->setValue(curve->tension());

    m_updating = false;
}

void PropertiesPanel::showEllipseProperties(Ellipse *ellipse) {
    if (!ellipse) return;
    m_selectionLabel->setText(ellipse->isCircle() ? "Circle" : "Ellipse");

    m_updating = true;

    m_strokeGroup->show();
    m_fillGroup->show();
    m_transformGroup->show();
    m_ellipseGroup->show();

    m_lineWidthSpin->setValue(ellipse->lineWidth());
    updateColorButton(m_strokeColorBtn, ellipse->strokeColor());
    updateColorButton(m_fillColorBtn, ellipse->fillColor());

    int lineStyleIdx = m_lineStyleCombo->findData(static_cast<int>(ellipse->lineStyle()));
    if (lineStyleIdx >= 0) m_lineStyleCombo->setCurrentIndex(lineStyleIdx);

    int lineCapIdx = m_lineCapCombo->findData(static_cast<int>(ellipse->lineCap()));
    if (lineCapIdx >= 0) m_lineCapCombo->setCurrentIndex(lineCapIdx);

    int lineJoinIdx = m_lineJoinCombo->findData(static_cast<int>(ellipse->lineJoin()));
    if (lineJoinIdx >= 0) m_lineJoinCombo->setCurrentIndex(lineJoinIdx);

    int fillPatternIdx = m_fillPatternCombo->findData(static_cast<int>(ellipse->fillPattern()));
    if (fillPatternIdx >= 0) m_fillPatternCombo->setCurrentIndex(fillPatternIdx);

    m_opacitySpin->setValue(ellipse->opacity());
    m_rotationSpin->setValue(ellipse->rotation());
    m_scaleSpin->setValue(ellipse->scale());

    m_radiusXSpin->setValue(ellipse->radiusX());
    m_radiusYSpin->setValue(ellipse->radiusY());

    m_updating = false;
}

void PropertiesPanel::updateUI() {
    if (m_currentPolygon && !m_updating) {
        m_updating = true;
        int idx = m_vertexIndexSpin->value();
        if (idx >= 0 && idx < m_currentPolygon->vertexCount()) {
            m_vertexCornerRadiusSpin->setValue(m_currentPolygon->vertices()[idx].cornerRadius);
        }
        m_updating = false;
    }
}

// === PROPERTY CHANGE HANDLERS ===

void PropertiesPanel::onLineWidthChanged(double value) {
    if (m_updating) return;

    // Update defaults
    m_defaults.lineWidth = value;

    // Update selection if any
    if (m_currentLine) {
        m_currentLine->setLineWidth(value);
    } else if (m_currentPolygon) {
        m_currentPolygon->setLineWidth(value);
    } else if (m_currentCurve) {
        m_currentCurve->setLineWidth(value);
    } else if (m_currentEllipse) {
        m_currentEllipse->setLineWidth(value);
    }

    emit lineWidthChanged(value);
    emit propertiesChanged();
}

void PropertiesPanel::onStrokeColorClicked() {
    QColor initial = m_defaults.strokeColor;
    if (m_currentLine) initial = m_currentLine->color();
    else if (m_currentPolygon) initial = m_currentPolygon->strokeColor();
    else if (m_currentCurve) initial = m_currentCurve->color();
    else if (m_currentEllipse) initial = m_currentEllipse->strokeColor();

    QColor color = QColorDialog::getColor(initial, this, "Select Stroke Color");
    if (!color.isValid()) return;

    m_defaults.strokeColor = color;
    updateColorButton(m_strokeColorBtn, color);

    if (m_currentLine) {
        m_currentLine->setColor(color);
    } else if (m_currentPolygon) {
        m_currentPolygon->setStrokeColor(color);
    } else if (m_currentCurve) {
        m_currentCurve->setColor(color);
    } else if (m_currentEllipse) {
        m_currentEllipse->setStrokeColor(color);
    }

    emit strokeColorChanged(color);
    emit propertiesChanged();
}

void PropertiesPanel::onFillColorClicked() {
    QColor initial = m_defaults.fillColor;
    if (m_currentPolygon) initial = m_currentPolygon->fillColor();
    else if (m_currentEllipse) initial = m_currentEllipse->fillColor();

    QColor color = QColorDialog::getColor(initial, this, "Select Fill Color");
    if (!color.isValid()) return;

    m_defaults.fillColor = color;
    updateColorButton(m_fillColorBtn, color);

    if (m_currentPolygon) {
        m_currentPolygon->setFillColor(color);
    } else if (m_currentEllipse) {
        m_currentEllipse->setFillColor(color);
    }

    emit fillColorChanged(color);
    emit propertiesChanged();
}

void PropertiesPanel::onLineStyleChanged(int index) {
    if (m_updating) return;

    LineStyle style = static_cast<LineStyle>(m_lineStyleCombo->itemData(index).toInt());
    m_defaults.lineStyle = style;
    bool dashed = (style != LineStyle::Solid);

    if (m_currentLine) {
        m_currentLine->setDashed(dashed);
    } else if (m_currentPolygon) {
        m_currentPolygon->setLineStyle(style);
    } else if (m_currentCurve) {
        m_currentCurve->setDashed(dashed);
    } else if (m_currentEllipse) {
        m_currentEllipse->setLineStyle(style);
    }

    emit lineStyleChanged(style);
    emit propertiesChanged();
}

void PropertiesPanel::onLineCapChanged(int index) {
    if (m_updating) return;

    LineCap cap = static_cast<LineCap>(m_lineCapCombo->itemData(index).toInt());
    m_defaults.lineCap = cap;

    if (m_currentPolygon) {
        m_currentPolygon->setLineCap(cap);
    } else if (m_currentEllipse) {
        m_currentEllipse->setLineCap(cap);
    }

    emit lineCapChanged(cap);
    emit propertiesChanged();
}

void PropertiesPanel::onLineJoinChanged(int index) {
    if (m_updating) return;

    LineJoin join = static_cast<LineJoin>(m_lineJoinCombo->itemData(index).toInt());
    m_defaults.lineJoin = join;

    if (m_currentPolygon) {
        m_currentPolygon->setLineJoin(join);
    } else if (m_currentEllipse) {
        m_currentEllipse->setLineJoin(join);
    }

    emit lineJoinChanged(join);
    emit propertiesChanged();
}

void PropertiesPanel::onFillPatternChanged(int index) {
    if (m_updating) return;

    FillPattern pattern = static_cast<FillPattern>(m_fillPatternCombo->itemData(index).toInt());
    m_defaults.fillPattern = pattern;

    if (m_currentPolygon) {
        m_currentPolygon->setFillPattern(pattern);
    } else if (m_currentEllipse) {
        m_currentEllipse->setFillPattern(pattern);
    }

    emit fillPatternChanged(pattern);
    emit propertiesChanged();
}

void PropertiesPanel::onOpacityChanged(double value) {
    if (m_updating) return;

    m_defaults.opacity = value;

    if (m_currentPolygon) {
        m_currentPolygon->setOpacity(value);
    } else if (m_currentEllipse) {
        m_currentEllipse->setOpacity(value);
    }

    emit opacityChanged(value);
    emit propertiesChanged();
}

void PropertiesPanel::onRotationChanged(double value) {
    if (m_updating) return;

    m_defaults.rotation = value;

    if (m_currentLine) {
        m_currentLine->setRotation(value);
    } else if (m_currentPolygon) {
        m_currentPolygon->setRotation(value);
    } else if (m_currentCurve) {
        m_currentCurve->setRotation(value);
    } else if (m_currentEllipse) {
        m_currentEllipse->setRotation(value);
    }

    emit rotationChanged(value);
    emit propertiesChanged();
}

void PropertiesPanel::onScaleChanged(double value) {
    if (m_updating) return;

    m_defaults.scale = value;

    if (m_currentLine) {
        m_currentLine->setScale(value);
    } else if (m_currentPolygon) {
        m_currentPolygon->setScale(value);
    } else if (m_currentCurve) {
        m_currentCurve->setScale(value);
    } else if (m_currentEllipse) {
        m_currentEllipse->setScale(value);
    }

    emit scaleChanged(value);
    emit propertiesChanged();
}

void PropertiesPanel::onCornerRadiusChanged(double value) {
    if (m_updating) return;

    m_defaults.cornerRadius = value;

    if (m_currentPolygon) {
        m_currentPolygon->setDefaultCornerRadius(value);
    }

    emit propertiesChanged();
}

void PropertiesPanel::onVertexCornerRadiusChanged(double value) {
    if (m_updating) return;

    if (m_currentPolygon) {
        int idx = m_vertexIndexSpin->value();
        m_currentPolygon->setVertexCornerRadius(idx, value);
    }

    emit propertiesChanged();
}

void PropertiesPanel::onRadiusXChanged(double value) {
    if (m_updating) return;

    if (m_currentEllipse) {
        m_currentEllipse->setRadiusX(value);
    }

    emit propertiesChanged();
}

void PropertiesPanel::onRadiusYChanged(double value) {
    if (m_updating) return;

    if (m_currentEllipse) {
        m_currentEllipse->setRadiusY(value);
    }

    emit propertiesChanged();
}

void PropertiesPanel::onTensionChanged(double value) {
    if (m_updating) return;

    m_defaults.curveTension = value;

    if (m_currentCurve) {
        m_currentCurve->setTension(value);
    }

    emit propertiesChanged();
}

// === SYNC SLOTS (from context toolbar) ===

void PropertiesPanel::setLineWidth(double width) {
    m_updating = true;
    m_defaults.lineWidth = width;
    m_lineWidthSpin->setValue(width);
    m_updating = false;
}

void PropertiesPanel::setStrokeColor(const QColor &color) {
    m_defaults.strokeColor = color;
    updateColorButton(m_strokeColorBtn, color);
}

void PropertiesPanel::setFillColor(const QColor &color) {
    m_defaults.fillColor = color;
    updateColorButton(m_fillColorBtn, color);
}

void PropertiesPanel::setLineStyle(LineStyle style) {
    m_updating = true;
    m_defaults.lineStyle = style;
    int idx = m_lineStyleCombo->findData(static_cast<int>(style));
    if (idx >= 0) m_lineStyleCombo->setCurrentIndex(idx);
    m_updating = false;
}

void PropertiesPanel::setLineCap(LineCap cap) {
    m_updating = true;
    m_defaults.lineCap = cap;
    int idx = m_lineCapCombo->findData(static_cast<int>(cap));
    if (idx >= 0) m_lineCapCombo->setCurrentIndex(idx);
    m_updating = false;
}

void PropertiesPanel::setLineJoin(LineJoin join) {
    m_updating = true;
    m_defaults.lineJoin = join;
    int idx = m_lineJoinCombo->findData(static_cast<int>(join));
    if (idx >= 0) m_lineJoinCombo->setCurrentIndex(idx);
    m_updating = false;
}

void PropertiesPanel::setFillPattern(FillPattern pattern) {
    m_updating = true;
    m_defaults.fillPattern = pattern;
    int idx = m_fillPatternCombo->findData(static_cast<int>(pattern));
    if (idx >= 0) m_fillPatternCombo->setCurrentIndex(idx);
    m_updating = false;
}

void PropertiesPanel::setOpacity(double opacity) {
    m_updating = true;
    m_defaults.opacity = opacity;
    m_opacitySpin->setValue(opacity);
    m_updating = false;
}

void PropertiesPanel::setRotation(double angle) {
    m_updating = true;
    m_defaults.rotation = angle;
    m_rotationSpin->setValue(angle);
    m_updating = false;
}

void PropertiesPanel::setScale(double scale) {
    m_updating = true;
    m_defaults.scale = scale;
    m_scaleSpin->setValue(scale);
    m_updating = false;
}
