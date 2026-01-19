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

PropertiesPanel::PropertiesPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void PropertiesPanel::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);

    // Selection info
    m_selectionLabel = new QLabel("No selection");
    m_selectionLabel->setStyleSheet("font-weight: bold; color: #73000a; padding: 4px;");
    m_mainLayout->addWidget(m_selectionLabel);

    // Common properties group
    m_commonGroup = new QGroupBox("Style");
    m_commonGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *commonLayout = new QFormLayout(m_commonGroup);

    m_lineWidthSpin = new QDoubleSpinBox();
    m_lineWidthSpin->setRange(0.1, 10.0);
    m_lineWidthSpin->setSingleStep(0.1);
    m_lineWidthSpin->setValue(0.8);
    m_lineWidthSpin->setSuffix(" pt");
    commonLayout->addRow("Line Width:", m_lineWidthSpin);
    connect(m_lineWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onLineWidthChanged);

    m_opacitySpin = new QDoubleSpinBox();
    m_opacitySpin->setRange(0.0, 1.0);
    m_opacitySpin->setSingleStep(0.1);
    m_opacitySpin->setValue(1.0);
    commonLayout->addRow("Opacity:", m_opacitySpin);
    connect(m_opacitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PropertiesPanel::onOpacityChanged);

    m_dashedCheck = new QCheckBox("Dashed");
    commonLayout->addRow("", m_dashedCheck);
    connect(m_dashedCheck, &QCheckBox::stateChanged,
            this, &PropertiesPanel::onDashedChanged);

    m_mainLayout->addWidget(m_commonGroup);
    m_commonGroup->hide();

    // Corner radius group
    m_cornerGroup = new QGroupBox("Corner Radius");
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
    m_vertexIndexSpin->setPrefix("Vertex ");
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

    // Ellipse properties group
    m_ellipseGroup = new QGroupBox("Ellipse");
    m_ellipseGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout *ellipseLayout = new QFormLayout(m_ellipseGroup);

    m_radiusXSpin = new QDoubleSpinBox();
    m_radiusXSpin->setRange(0.1, 20.0);
    m_radiusXSpin->setSingleStep(0.25);
    m_radiusXSpin->setValue(1.0);
    ellipseLayout->addRow("Radius X:", m_radiusXSpin);

    m_radiusYSpin = new QDoubleSpinBox();
    m_radiusYSpin->setRange(0.1, 20.0);
    m_radiusYSpin->setSingleStep(0.25);
    m_radiusYSpin->setValue(1.0);
    ellipseLayout->addRow("Radius Y:", m_radiusYSpin);

    m_rotationSpin = new QDoubleSpinBox();
    m_rotationSpin->setRange(-180.0, 180.0);
    m_rotationSpin->setSingleStep(5.0);
    m_rotationSpin->setValue(0.0);
    m_rotationSpin->setSuffix("°");
    ellipseLayout->addRow("Rotation:", m_rotationSpin);

    connect(m_radiusXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double v) {
        if (!m_updating && m_currentEllipse) {
            m_currentEllipse->setRadiusX(v);
            emit propertiesChanged();
        }
    });
    connect(m_radiusYSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double v) {
        if (!m_updating && m_currentEllipse) {
            m_currentEllipse->setRadiusY(v);
            emit propertiesChanged();
        }
    });
    connect(m_rotationSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double v) {
        if (!m_updating && m_currentEllipse) {
            m_currentEllipse->setRotation(v);
            emit propertiesChanged();
        }
    });

    m_mainLayout->addWidget(m_ellipseGroup);
    m_ellipseGroup->hide();

    m_mainLayout->addStretch();
}

void PropertiesPanel::setSelection(QList<QGraphicsItem*> items) {
    m_selection = items;
    m_currentPoint = nullptr;
    m_currentLine = nullptr;
    m_currentPolygon = nullptr;
    m_currentCurve = nullptr;
    m_currentEllipse = nullptr;

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
    hideAllGroups();
    m_selectionLabel->setText("No selection");
}

void PropertiesPanel::hideAllGroups() {
    m_commonGroup->hide();
    m_cornerGroup->hide();
    m_ellipseGroup->hide();
}

void PropertiesPanel::showPointProperties(Point *point) {
    if (!point) return;
    m_selectionLabel->setText("Point");
    // Points don't have many editable properties in this simplified version
}

void PropertiesPanel::showLineProperties(Line *line) {
    if (!line) return;
    m_selectionLabel->setText("Line");

    m_updating = true;
    m_commonGroup->show();
    m_lineWidthSpin->setValue(line->lineWidth());
    m_opacitySpin->setValue(1.0);
    m_opacitySpin->setEnabled(false);
    m_dashedCheck->setChecked(line->isDashed());
    m_updating = false;
}

void PropertiesPanel::showPolygonProperties(Polygon *polygon) {
    if (!polygon) return;
    m_selectionLabel->setText("Polygon");

    m_updating = true;

    m_commonGroup->show();
    m_lineWidthSpin->setValue(polygon->lineWidth());
    m_opacitySpin->setValue(polygon->opacity());
    m_opacitySpin->setEnabled(true);
    m_dashedCheck->setChecked(false);
    m_dashedCheck->setEnabled(false);

    m_cornerGroup->show();
    m_defaultCornerRadiusSpin->setValue(polygon->defaultCornerRadius());
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
    m_commonGroup->show();
    m_lineWidthSpin->setValue(curve->lineWidth());
    m_opacitySpin->setValue(1.0);
    m_opacitySpin->setEnabled(false);
    m_dashedCheck->setChecked(curve->isDashed());
    m_updating = false;
}

void PropertiesPanel::showEllipseProperties(Ellipse *ellipse) {
    if (!ellipse) return;
    m_selectionLabel->setText(ellipse->isCircle() ? "Circle" : "Ellipse");

    m_updating = true;

    m_commonGroup->show();
    m_lineWidthSpin->setValue(ellipse->lineWidth());
    m_opacitySpin->setValue(ellipse->opacity());
    m_opacitySpin->setEnabled(true);
    m_dashedCheck->setChecked(false);
    m_dashedCheck->setEnabled(false);

    m_ellipseGroup->show();
    m_radiusXSpin->setValue(ellipse->radiusX());
    m_radiusYSpin->setValue(ellipse->radiusY());
    m_rotationSpin->setValue(ellipse->rotation());

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

void PropertiesPanel::onCornerRadiusChanged(double value) {
    if (m_updating) return;
    if (m_currentPolygon) {
        m_currentPolygon->setDefaultCornerRadius(value);
        emit propertiesChanged();
    }
}

void PropertiesPanel::onLineWidthChanged(double value) {
    if (m_updating) return;
    if (m_currentLine) {
        m_currentLine->setLineWidth(value);
    } else if (m_currentPolygon) {
        m_currentPolygon->setLineWidth(value);
    } else if (m_currentCurve) {
        m_currentCurve->setLineWidth(value);
    } else if (m_currentEllipse) {
        m_currentEllipse->setLineWidth(value);
    }
    emit propertiesChanged();
}

void PropertiesPanel::onOpacityChanged(double value) {
    if (m_updating) return;
    if (m_currentPolygon) {
        m_currentPolygon->setOpacity(value);
    } else if (m_currentEllipse) {
        m_currentEllipse->setOpacity(value);
    }
    emit propertiesChanged();
}

void PropertiesPanel::onDashedChanged(int state) {
    if (m_updating) return;
    bool dashed = (state == Qt::Checked);
    if (m_currentLine) {
        m_currentLine->setDashed(dashed);
    } else if (m_currentCurve) {
        m_currentCurve->setDashed(dashed);
    }
    emit propertiesChanged();
}

void PropertiesPanel::onVertexCornerRadiusChanged(double value) {
    if (m_updating) return;
    if (m_currentPolygon) {
        int idx = m_vertexIndexSpin->value();
        m_currentPolygon->setVertexCornerRadius(idx, value);
        emit propertiesChanged();
    }
}
