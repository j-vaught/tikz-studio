#include "contexttoolbar.h"
#include "pointitem.h"
#include "lineitem.h"
#include "polygonitem.h"
#include "curveitem.h"
#include "ellipseitem.h"
#include "point.h"
#include "line.h"
#include "polygon.h"
#include "curve.h"
#include "ellipse.h"

#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QHBoxLayout>

ContextToolbar::ContextToolbar(QWidget *parent)
    : QToolBar(parent)
{
    setWindowTitle("Properties");
    setMovable(false);
    setFloatable(false);

    setupWidgets();
}

void ContextToolbar::setupWidgets() {
    // Selection label
    m_selectionLabel = new QLabel("No selection");
    m_selectionLabel->setStyleSheet("font-weight: bold; padding: 0 8px;");
    addWidget(m_selectionLabel);

    addSeparator();

    // Line width
    QLabel *widthLabel = new QLabel(" Width:");
    addWidget(widthLabel);

    m_lineWidthSpin = new QDoubleSpinBox();
    m_lineWidthSpin->setRange(0.1, 10.0);
    m_lineWidthSpin->setSingleStep(0.1);
    m_lineWidthSpin->setValue(0.8);
    m_lineWidthSpin->setSuffix(" pt");
    m_lineWidthSpin->setFixedWidth(75);
    m_lineWidthSpin->setToolTip("Line/stroke width");
    connect(m_lineWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) {
        if (!m_updating) emit lineWidthChanged(value);
    });
    addWidget(m_lineWidthSpin);

    addSeparator();

    // Stroke color
    QLabel *strokeLabel = new QLabel(" Stroke:");
    addWidget(strokeLabel);

    m_strokeColorBtn = new QPushButton();
    m_strokeColorBtn->setFixedSize(28, 28);
    m_strokeColorBtn->setToolTip("Stroke/outline color");
    updateColorButton(m_strokeColorBtn, m_currentStrokeColor);
    connect(m_strokeColorBtn, &QPushButton::clicked, this, &ContextToolbar::onStrokeColorClicked);
    addWidget(m_strokeColorBtn);

    // Line style
    m_lineStyleCombo = new QComboBox();
    m_lineStyleCombo->addItem("Solid", static_cast<int>(LineStyle::Solid));
    m_lineStyleCombo->addItem("Dashed", static_cast<int>(LineStyle::Dashed));
    m_lineStyleCombo->addItem("Dotted", static_cast<int>(LineStyle::Dotted));
    m_lineStyleCombo->addItem("Dash-Dot", static_cast<int>(LineStyle::DashDot));
    m_lineStyleCombo->setFixedWidth(85);
    m_lineStyleCombo->setToolTip("Line/stroke style");
    connect(m_lineStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (!m_updating) {
            emit lineStyleChanged(static_cast<LineStyle>(m_lineStyleCombo->itemData(index).toInt()));
        }
    });
    addWidget(m_lineStyleCombo);

    addSeparator();

    // Fill color
    QLabel *fillLabel = new QLabel(" Fill:");
    addWidget(fillLabel);

    m_fillColorBtn = new QPushButton();
    m_fillColorBtn->setFixedSize(28, 28);
    m_fillColorBtn->setToolTip("Fill color");
    updateColorButton(m_fillColorBtn, m_currentFillColor);
    connect(m_fillColorBtn, &QPushButton::clicked, this, &ContextToolbar::onFillColorClicked);
    addWidget(m_fillColorBtn);

    // Fill pattern
    m_fillPatternCombo = new QComboBox();
    m_fillPatternCombo->addItem("None", static_cast<int>(FillPattern::None));
    m_fillPatternCombo->addItem("Solid", static_cast<int>(FillPattern::Solid));
    m_fillPatternCombo->addItem("Horiz Lines", static_cast<int>(FillPattern::HorizontalLines));
    m_fillPatternCombo->addItem("Vert Lines", static_cast<int>(FillPattern::VerticalLines));
    m_fillPatternCombo->addItem("Cross Hatch", static_cast<int>(FillPattern::CrossHatch));
    m_fillPatternCombo->addItem("Diagonal", static_cast<int>(FillPattern::DiagonalLines));
    m_fillPatternCombo->addItem("Diag Cross", static_cast<int>(FillPattern::DiagonalCrossHatch));
    m_fillPatternCombo->addItem("Dots", static_cast<int>(FillPattern::Dots));
    m_fillPatternCombo->setCurrentIndex(1);  // Solid by default
    m_fillPatternCombo->setFixedWidth(100);
    m_fillPatternCombo->setToolTip("Fill pattern");
    connect(m_fillPatternCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (!m_updating) {
            emit fillPatternChanged(static_cast<FillPattern>(m_fillPatternCombo->itemData(index).toInt()));
        }
    });
    addWidget(m_fillPatternCombo);

    addSeparator();

    // Rotation
    QLabel *rotLabel = new QLabel(" Rotate:");
    addWidget(rotLabel);

    m_rotationSpin = new QDoubleSpinBox();
    m_rotationSpin->setRange(-180.0, 180.0);
    m_rotationSpin->setSingleStep(5.0);
    m_rotationSpin->setValue(0.0);
    m_rotationSpin->setSuffix("°");
    m_rotationSpin->setFixedWidth(70);
    m_rotationSpin->setToolTip("Rotation angle");
    connect(m_rotationSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) {
        if (!m_updating) emit rotationChanged(value);
    });
    addWidget(m_rotationSpin);

    // Scale
    QLabel *scaleLabel = new QLabel(" Scale:");
    addWidget(scaleLabel);

    m_scaleSpin = new QDoubleSpinBox();
    m_scaleSpin->setRange(0.1, 10.0);
    m_scaleSpin->setSingleStep(0.1);
    m_scaleSpin->setValue(1.0);
    m_scaleSpin->setSuffix("x");
    m_scaleSpin->setFixedWidth(65);
    m_scaleSpin->setToolTip("Scale factor");
    connect(m_scaleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) {
        if (!m_updating) emit scaleChanged(value);
    });
    addWidget(m_scaleSpin);
}

void ContextToolbar::updateForSelection(const QList<QGraphicsItem*> &items) {
    m_updating = true;

    if (items.isEmpty()) {
        m_selectionLabel->setText("No selection");
        m_updating = false;
        return;
    }

    if (items.size() > 1) {
        m_selectionLabel->setText(QString("%1 items").arg(items.size()));
        m_updating = false;
        return;
    }

    QGraphicsItem *item = items.first();

    if (PointItem *pi = qgraphicsitem_cast<PointItem*>(item)) {
        m_selectionLabel->setText("Point");
        // Points have limited properties
    } else if (LineItem *li = qgraphicsitem_cast<LineItem*>(item)) {
        m_selectionLabel->setText("Line");
        Line *line = li->line();
        m_lineWidthSpin->setValue(line->lineWidth());
        m_currentStrokeColor = line->color();
        updateColorButton(m_strokeColorBtn, m_currentStrokeColor);
        m_lineStyleCombo->setCurrentIndex(line->isDashed() ? 1 : 0);
    } else if (PolygonItem *pi = qgraphicsitem_cast<PolygonItem*>(item)) {
        m_selectionLabel->setText("Polygon");
        Polygon *poly = pi->polygon();
        m_lineWidthSpin->setValue(poly->lineWidth());
        m_currentStrokeColor = poly->strokeColor();
        m_currentFillColor = poly->fillColor();
        updateColorButton(m_strokeColorBtn, m_currentStrokeColor);
        updateColorButton(m_fillColorBtn, m_currentFillColor);
    } else if (CurveItem *ci = qgraphicsitem_cast<CurveItem*>(item)) {
        m_selectionLabel->setText("Curve");
        Curve *curve = ci->curve();
        m_lineWidthSpin->setValue(curve->lineWidth());
        m_currentStrokeColor = curve->color();
        updateColorButton(m_strokeColorBtn, m_currentStrokeColor);
        m_lineStyleCombo->setCurrentIndex(curve->isDashed() ? 1 : 0);
    } else if (EllipseItem *ei = qgraphicsitem_cast<EllipseItem*>(item)) {
        Ellipse *ellipse = ei->ellipse();
        m_selectionLabel->setText(ellipse->isCircle() ? "Circle" : "Ellipse");
        m_lineWidthSpin->setValue(ellipse->lineWidth());
        m_currentStrokeColor = ellipse->strokeColor();
        m_currentFillColor = ellipse->fillColor();
        updateColorButton(m_strokeColorBtn, m_currentStrokeColor);
        updateColorButton(m_fillColorBtn, m_currentFillColor);
        m_rotationSpin->setValue(ellipse->rotation());
    }

    m_updating = false;
}

void ContextToolbar::clearSelection() {
    m_updating = true;
    m_selectionLabel->setText("No selection");
    m_updating = false;
}

void ContextToolbar::onStrokeColorClicked() {
    showColorDialog(false);
}

void ContextToolbar::onFillColorClicked() {
    showColorDialog(true);
}

void ContextToolbar::showColorDialog(bool forFill) {
    QColor initial = forFill ? m_currentFillColor : m_currentStrokeColor;
    QColor color = QColorDialog::getColor(initial, this,
        forFill ? "Select Fill Color" : "Select Stroke Color");

    if (color.isValid()) {
        if (forFill) {
            m_currentFillColor = color;
            updateColorButton(m_fillColorBtn, color);
            emit fillColorChanged(color);
        } else {
            m_currentStrokeColor = color;
            updateColorButton(m_strokeColorBtn, color);
            emit strokeColorChanged(color);
        }
    }
}

void ContextToolbar::updateColorButton(QPushButton *btn, const QColor &color) {
    QString style = QString(
        "QPushButton { background-color: %1; border: 2px solid #000000; }"
        "QPushButton:hover { border: 3px solid #000000; }"
    ).arg(color.name());
    btn->setStyleSheet(style);
}
