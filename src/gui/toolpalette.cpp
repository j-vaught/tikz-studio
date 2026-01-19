#include "toolpalette.h"
#include <QActionGroup>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolButton>

ToolPalette::ToolPalette(QWidget *parent)
    : QToolBar(parent)
{
    setWindowTitle("Tools");
    setOrientation(Qt::Vertical);
    setMovable(false);
    setIconSize(QSize(32, 32));

    // Style the toolbar - clean black and white theme
    setStyleSheet(R"(
        QToolBar {
            background-color: #ffffff;
            border: 1px solid #000000;
            spacing: 4px;
            padding: 4px;
        }
        QToolButton {
            background-color: #ffffff;
            border: 2px solid #000000;
            padding: 8px;
            margin: 2px;
            font-weight: bold;
            font-size: 11px;
            min-width: 70px;
            min-height: 40px;
            color: #000000;
        }
        QToolButton:hover {
            background-color: #e0e0e0;
            border-color: #000000;
        }
        QToolButton:checked {
            background-color: #000000;
            color: #ffffff;
            border-color: #000000;
        }
        QToolButton:pressed {
            background-color: #000000;
            color: #ffffff;
        }
    )");

    setupActions();
}

void ToolPalette::setupActions() {
    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);

    // Add section label
    QLabel *editLabel = new QLabel("  Edit");
    editLabel->setStyleSheet("font-weight: bold; color: #000000; padding: 4px;");
    addWidget(editLabel);

    // Select tool
    m_selectAction = new QAction("Select", this);
    m_selectAction->setCheckable(true);
    m_selectAction->setChecked(true);
    m_selectAction->setShortcut(QKeySequence(Qt::Key_S));
    m_selectAction->setToolTip("Select and move objects (S)\nRight-click for options");
    m_actionGroup->addAction(m_selectAction);
    addAction(m_selectAction);

    addSeparator();

    // Drawing section
    QLabel *drawLabel = new QLabel("  Draw");
    drawLabel->setStyleSheet("font-weight: bold; color: #000000; padding: 4px;");
    addWidget(drawLabel);

    // Point tool
    m_pointAction = new QAction("Point", this);
    m_pointAction->setCheckable(true);
    m_pointAction->setShortcut(QKeySequence(Qt::Key_P));
    m_pointAction->setToolTip("Add points/vertices (P)\nClick to place");
    m_actionGroup->addAction(m_pointAction);
    addAction(m_pointAction);

    // Line tool
    m_lineAction = new QAction("Line", this);
    m_lineAction->setCheckable(true);
    m_lineAction->setShortcut(QKeySequence(Qt::Key_L));
    m_lineAction->setToolTip("Draw lines (L)\nClick start, click end");
    m_actionGroup->addAction(m_lineAction);
    addAction(m_lineAction);

    // Polygon tool
    m_polygonAction = new QAction("Polygon", this);
    m_polygonAction->setCheckable(true);
    m_polygonAction->setShortcut(QKeySequence(Qt::Key_G));
    m_polygonAction->setToolTip("Draw filled polygon (G)\nLeft-click: add vertex\nRight-click: finish");
    m_actionGroup->addAction(m_polygonAction);
    addAction(m_polygonAction);

    // Curve tool
    m_curveAction = new QAction("Curve", this);
    m_curveAction->setCheckable(true);
    m_curveAction->setShortcut(QKeySequence(Qt::Key_C));
    m_curveAction->setToolTip("Draw smooth curve (C)\nLeft-click: add control point\nRight-click: finish");
    m_actionGroup->addAction(m_curveAction);
    addAction(m_curveAction);

    addSeparator();

    // Shapes section
    QLabel *shapeLabel = new QLabel("  Shapes");
    shapeLabel->setStyleSheet("font-weight: bold; color: #000000; padding: 4px;");
    addWidget(shapeLabel);

    // Rectangle tool
    m_rectAction = new QAction("Rectangle", this);
    m_rectAction->setCheckable(true);
    m_rectAction->setShortcut(QKeySequence(Qt::Key_R));
    m_rectAction->setToolTip("Draw rectangle (R)\nClick and drag");
    m_actionGroup->addAction(m_rectAction);
    addAction(m_rectAction);

    // Circle tool
    m_circleAction = new QAction("Circle", this);
    m_circleAction->setCheckable(true);
    m_circleAction->setShortcut(QKeySequence(Qt::Key_O));
    m_circleAction->setToolTip("Draw circle (O)\nClick center, drag radius");
    m_actionGroup->addAction(m_circleAction);
    addAction(m_circleAction);

    // Ellipse tool
    m_ellipseAction = new QAction("Ellipse", this);
    m_ellipseAction->setCheckable(true);
    m_ellipseAction->setShortcut(QKeySequence(Qt::Key_E));
    m_ellipseAction->setToolTip("Draw ellipse (E)\nClick and drag");
    m_actionGroup->addAction(m_ellipseAction);
    addAction(m_ellipseAction);

    // Triangle tool
    m_triangleAction = new QAction("Triangle", this);
    m_triangleAction->setCheckable(true);
    m_triangleAction->setShortcut(QKeySequence(Qt::Key_T));
    m_triangleAction->setToolTip("Draw triangle (T)\nClick and drag");
    m_actionGroup->addAction(m_triangleAction);
    addAction(m_triangleAction);

    // Regular polygon tool
    m_regPolyAction = new QAction("N-gon", this);
    m_regPolyAction->setCheckable(true);
    m_regPolyAction->setShortcut(QKeySequence(Qt::Key_N));
    m_regPolyAction->setToolTip("Draw regular polygon (N)\nClick center, drag size\nUse spinbox for sides");
    m_actionGroup->addAction(m_regPolyAction);
    addAction(m_regPolyAction);

    // Connect signals
    connect(m_actionGroup, &QActionGroup::triggered, this, [this](QAction*) {
        emit toolChanged(currentTool());
    });
}

Tool ToolPalette::currentTool() const {
    if (m_selectAction->isChecked()) return Tool::Select;
    if (m_pointAction->isChecked()) return Tool::Point;
    if (m_lineAction->isChecked()) return Tool::Line;
    if (m_polygonAction->isChecked()) return Tool::Polygon;
    if (m_curveAction->isChecked()) return Tool::Curve;
    if (m_rectAction->isChecked()) return Tool::Rectangle;
    if (m_circleAction->isChecked()) return Tool::Circle;
    if (m_ellipseAction->isChecked()) return Tool::Ellipse;
    if (m_triangleAction->isChecked()) return Tool::Triangle;
    if (m_regPolyAction->isChecked()) return Tool::RegularPolygon;
    return Tool::Select;
}

void ToolPalette::setCurrentTool(Tool tool) {
    switch (tool) {
    case Tool::Select: m_selectAction->setChecked(true); break;
    case Tool::Point: m_pointAction->setChecked(true); break;
    case Tool::Line: m_lineAction->setChecked(true); break;
    case Tool::Polygon: m_polygonAction->setChecked(true); break;
    case Tool::Curve: m_curveAction->setChecked(true); break;
    case Tool::Rectangle: m_rectAction->setChecked(true); break;
    case Tool::Circle: m_circleAction->setChecked(true); break;
    case Tool::Ellipse: m_ellipseAction->setChecked(true); break;
    case Tool::Triangle: m_triangleAction->setChecked(true); break;
    case Tool::RegularPolygon: m_regPolyAction->setChecked(true); break;
    }
    emit toolChanged(tool);
}
