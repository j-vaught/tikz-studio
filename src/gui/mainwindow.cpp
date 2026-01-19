#include "mainwindow.h"
#include "document.h"
#include "canvas.h"
#include "canvasview.h"
#include "toolpalette.h"
#include "colorpalette.h"
#include "propertiespanel.h"
#include "common.h"

#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QStatusBar>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProcess>
#include <QTemporaryDir>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QApplication>
#include <QSpinBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("TikZ Editor");
    resize(1400, 900);

    // Apply global stylesheet - clean black and white theme
    setStyleSheet(R"(
        QMainWindow {
            background-color: #ffffff;
        }
        QMenuBar {
            background-color: #ffffff;
            color: #000000;
            border-bottom: 1px solid #000000;
            padding: 2px;
        }
        QMenuBar::item:selected {
            background-color: #000000;
            color: #ffffff;
        }
        QMenu {
            background-color: #ffffff;
            border: 1px solid #000000;
        }
        QMenu::item:selected {
            background-color: #000000;
            color: #ffffff;
        }
        QToolBar {
            background-color: #ffffff;
            border: 1px solid #000000;
            spacing: 4px;
            padding: 4px;
        }
        QToolBar QToolButton {
            background-color: #ffffff;
            border: 2px solid #000000;
            padding: 6px 10px;
            margin: 2px;
            font-weight: bold;
            color: #000000;
        }
        QToolBar QToolButton:hover {
            background-color: #e0e0e0;
        }
        QToolBar QToolButton:pressed {
            background-color: #000000;
            color: #ffffff;
        }
        QDockWidget {
            titlebar-close-icon: url(close.png);
            font-weight: bold;
        }
        QDockWidget::title {
            background-color: #ffffff;
            color: #000000;
            border: 1px solid #000000;
            padding: 6px;
            text-align: left;
        }
        QStatusBar {
            background-color: #ffffff;
            color: #000000;
            border-top: 1px solid #000000;
        }
        QPushButton {
            background-color: #ffffff;
            border: 2px solid #000000;
            padding: 6px 12px;
            font-weight: bold;
            color: #000000;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
            border-color: #000000;
        }
        QPushButton:pressed {
            background-color: #000000;
            color: #ffffff;
        }
        QSpinBox, QDoubleSpinBox {
            background-color: #ffffff;
            border: 1px solid #000000;
            padding: 4px;
            color: #000000;
        }
        QSpinBox:focus, QDoubleSpinBox:focus {
            border: 2px solid #000000;
        }
        QSlider::groove:horizontal {
            border: 1px solid #000000;
            height: 6px;
            background: #ffffff;
        }
        QSlider::handle:horizontal {
            background: #000000;
            border: 1px solid #000000;
            width: 14px;
            margin: -4px 0;
        }
        QGroupBox {
            border: 1px solid #000000;
            margin-top: 8px;
            padding-top: 8px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px;
        }
        QCheckBox {
            color: #000000;
        }
        QLabel {
            color: #000000;
        }
    )");

    // Create document
    m_document = new Document(this);

    // Create canvas
    m_canvas = new Canvas(m_document, this);

    // Create canvas view
    m_canvasView = new CanvasView(this);
    m_canvasView->setCanvas(m_canvas);
    setCentralWidget(m_canvasView);

    // Create tool palette
    m_toolPalette = new ToolPalette(this);
    m_canvas->setToolPalette(m_toolPalette);

    // Create color palette
    m_colorPalette = new ColorPalette(this);
    m_canvas->setColorPalette(m_colorPalette);

    // Create properties panel
    m_propertiesPanel = new PropertiesPanel(this);

    setupMenus();
    setupToolbars();
    setupDocks();
    setupStatusBar();

    // Connect signals
    connect(m_document, &Document::changed, this, &MainWindow::updateTikZCode);
    connect(m_canvasView, &CanvasView::zoomChanged, this, &MainWindow::updateStatusBar);
    connect(m_canvas, &QGraphicsScene::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_canvas, &Canvas::statusMessage, this, &MainWindow::onStatusMessage);

    // Initial update
    updateTikZCode();
    updateStatusBar();
}

MainWindow::~MainWindow() {
    if (m_compileProcess) {
        m_compileProcess->kill();
        m_compileProcess->waitForFinished();
    }
}

void MainWindow::setupMenus() {
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *newAction = fileMenu->addAction("&New", this, &MainWindow::newDocument);
    newAction->setShortcut(QKeySequence::New);

    QAction *openAction = fileMenu->addAction("&Open...", this, &MainWindow::openDocument);
    openAction->setShortcut(QKeySequence::Open);

    QAction *saveAction = fileMenu->addAction("&Save", this, &MainWindow::saveDocument);
    saveAction->setShortcut(QKeySequence::Save);

    fileMenu->addAction("Save &As...", this, &MainWindow::saveDocumentAs);

    fileMenu->addSeparator();

    fileMenu->addAction("&Export TikZ...", this, &MainWindow::exportTikZ);

    fileMenu->addSeparator();

    QAction *quitAction = fileMenu->addAction("&Quit", this, &QWidget::close);
    quitAction->setShortcut(QKeySequence::Quit);

    // Edit menu
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("&Undo", m_document->undoStack(), &QUndoStack::undo, QKeySequence::Undo);
    editMenu->addAction("&Redo", m_document->undoStack(), &QUndoStack::redo, QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("&Delete", m_canvas, &Canvas::deleteSelected, QKeySequence::Delete);

    // View menu
    QMenu *viewMenu = menuBar()->addMenu("&View");

    QAction *gridAction = viewMenu->addAction("Show &Grid", this, &MainWindow::toggleGrid);
    gridAction->setCheckable(true);
    gridAction->setChecked(true);
    gridAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));

    viewMenu->addSeparator();

    viewMenu->addAction("Zoom &In", m_canvasView, &CanvasView::zoomIn, QKeySequence::ZoomIn);
    viewMenu->addAction("Zoom &Out", m_canvasView, &CanvasView::zoomOut, QKeySequence::ZoomOut);
    viewMenu->addAction("&Fit", m_canvasView, &CanvasView::zoomFit);
    viewMenu->addAction("&Reset Zoom", m_canvasView, &CanvasView::zoomReset, QKeySequence(Qt::CTRL | Qt::Key_0));

    // Image menu
    QMenu *imageMenu = menuBar()->addMenu("&Image");

    imageMenu->addAction("&Load Background Image...", this, &MainWindow::loadBackgroundImage);
    imageMenu->addAction("&Clear Background Image", this, &MainWindow::clearBackgroundImage);

    // Compile menu
    QMenu *compileMenu = menuBar()->addMenu("&Compile");

    QAction *compileAction = compileMenu->addAction("&Compile and Preview",
                                                    this, &MainWindow::compileAndPreview);
    compileAction->setShortcut(QKeySequence(Qt::Key_F5));
}

void MainWindow::setupToolbars() {
    // Main toolbar
    QToolBar *mainToolbar = addToolBar("Main");
    mainToolbar->setMovable(false);
    mainToolbar->setIconSize(QSize(24, 24));

    mainToolbar->addAction("New", this, &MainWindow::newDocument);
    mainToolbar->addAction("Open", this, &MainWindow::openDocument);
    mainToolbar->addAction("Save", this, &MainWindow::saveDocument);
    mainToolbar->addSeparator();

    // Compile button with garnet styling
    QAction *compileAction = mainToolbar->addAction("Compile (F5)", this, &MainWindow::compileAndPreview);
    compileAction->setToolTip("Compile TikZ and preview PDF (F5)");

    mainToolbar->addSeparator();

    // N-gon sides spinner
    QLabel *ngonLabel = new QLabel(" N-gon sides: ");
    mainToolbar->addWidget(ngonLabel);

    m_ngonSidesSpin = new QSpinBox();
    m_ngonSidesSpin->setRange(3, 20);
    m_ngonSidesSpin->setValue(6);
    m_ngonSidesSpin->setToolTip("Number of sides for regular polygon tool");
    mainToolbar->addWidget(m_ngonSidesSpin);

    connect(m_ngonSidesSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            m_canvas, &Canvas::setRegularPolygonSides);

    // Tool palette on the left
    addToolBar(Qt::LeftToolBarArea, m_toolPalette);
}

void MainWindow::setupDocks() {
    // Color palette dock
    QDockWidget *colorDock = new QDockWidget("Colors", this);
    colorDock->setWidget(m_colorPalette);
    colorDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, colorDock);

    // Properties panel dock
    m_propertiesDock = new QDockWidget("Properties", this);
    m_propertiesDock->setWidget(m_propertiesPanel);
    m_propertiesDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);

    // Display options dock (grid, axes, background)
    QDockWidget *displayDock = new QDockWidget("Display", this);
    QWidget *displayWidget = new QWidget();
    QVBoxLayout *displayLayout = new QVBoxLayout(displayWidget);
    displayLayout->setContentsMargins(8, 8, 8, 8);

    // Grid/Axis controls
    QLabel *gridLabel = new QLabel("Grid & Axes:");
    gridLabel->setStyleSheet("font-weight: bold;");
    displayLayout->addWidget(gridLabel);

    QCheckBox *gridCheck = new QCheckBox("Show Grid");
    gridCheck->setChecked(true);
    connect(gridCheck, &QCheckBox::toggled, this, &MainWindow::toggleGrid);
    displayLayout->addWidget(gridCheck);

    QCheckBox *axesCheck = new QCheckBox("Show Axes");
    axesCheck->setChecked(true);
    connect(axesCheck, &QCheckBox::toggled, this, &MainWindow::toggleAxes);
    displayLayout->addWidget(axesCheck);

    QCheckBox *ticksCheck = new QCheckBox("Show Axis Ticks");
    ticksCheck->setChecked(true);
    connect(ticksCheck, &QCheckBox::toggled, this, &MainWindow::toggleAxisTicks);
    displayLayout->addWidget(ticksCheck);

    displayLayout->addSpacing(12);

    // Background image controls
    QLabel *bgLabel = new QLabel("Background Image:");
    bgLabel->setStyleSheet("font-weight: bold;");
    displayLayout->addWidget(bgLabel);

    QPushButton *loadBgBtn = new QPushButton("Load Image...");
    connect(loadBgBtn, &QPushButton::clicked, this, &MainWindow::loadBackgroundImage);
    displayLayout->addWidget(loadBgBtn);

    QPushButton *clearBgBtn = new QPushButton("Clear Image");
    connect(clearBgBtn, &QPushButton::clicked, this, &MainWindow::clearBackgroundImage);
    displayLayout->addWidget(clearBgBtn);

    QLabel *opacityLabel = new QLabel("Image Opacity:");
    displayLayout->addWidget(opacityLabel);

    m_bgOpacitySlider = new QSlider(Qt::Horizontal);
    m_bgOpacitySlider->setRange(10, 100);
    m_bgOpacitySlider->setValue(50);
    m_bgOpacitySlider->setToolTip("Background image opacity for tracing");
    connect(m_bgOpacitySlider, &QSlider::valueChanged, this, &MainWindow::setBackgroundOpacity);
    displayLayout->addWidget(m_bgOpacitySlider);

    displayLayout->addStretch();
    displayDock->setWidget(displayWidget);
    displayDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, displayDock);

    // TikZ code dock
    m_codeDock = new QDockWidget("TikZ Code", this);
    m_tikzCodeView = new QTextEdit();
    m_tikzCodeView->setReadOnly(true);
    m_tikzCodeView->setFontFamily("Menlo, Monaco, Consolas, monospace");
    m_tikzCodeView->setFontPointSize(10);
    m_tikzCodeView->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #d4d4d4; }");
    m_codeDock->setWidget(m_tikzCodeView);
    m_codeDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, m_codeDock);

    // Stack the right docks
    tabifyDockWidget(colorDock, m_propertiesDock);
    tabifyDockWidget(m_propertiesDock, displayDock);
    colorDock->raise();
}

void MainWindow::setupStatusBar() {
    m_statusLabel = new QLabel("Ready - Select a tool to begin");
    m_statusLabel->setStyleSheet("padding: 4px;");
    statusBar()->addWidget(m_statusLabel, 1);

    m_zoomLabel = new QLabel();
    m_zoomLabel->setStyleSheet("padding: 4px;");
    statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    event->accept();
}

void MainWindow::newDocument() {
    m_document->clear();
    m_canvasView->clearBackgroundImage();
    m_currentFile.clear();
    setWindowTitle("TikZ Editor - New");
    m_statusLabel->setText("New document created");
}

void MainWindow::openDocument() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Document",
        QString(), "TikZ Editor Files (*.tikzedit);;All Files (*)");

    if (fileName.isEmpty()) return;

    if (m_document->load(fileName)) {
        m_currentFile = fileName;
        setWindowTitle(QString("TikZ Editor - %1").arg(fileName));
        m_statusLabel->setText(QString("Opened: %1").arg(fileName));
    } else {
        QMessageBox::warning(this, "Error", "Failed to open file.");
    }
}

void MainWindow::saveDocument() {
    if (m_currentFile.isEmpty()) {
        saveDocumentAs();
        return;
    }

    if (m_document->save(m_currentFile)) {
        m_statusLabel->setText(QString("Saved: %1").arg(m_currentFile));
    } else {
        QMessageBox::warning(this, "Error", "Failed to save file.");
    }
}

void MainWindow::saveDocumentAs() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Document",
        QString(), "TikZ Editor Files (*.tikzedit);;All Files (*)");

    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".tikzedit")) {
        fileName += ".tikzedit";
    }

    if (m_document->save(fileName)) {
        m_currentFile = fileName;
        setWindowTitle(QString("TikZ Editor - %1").arg(fileName));
        m_statusLabel->setText(QString("Saved: %1").arg(fileName));
    } else {
        QMessageBox::warning(this, "Error", "Failed to save file.");
    }
}

void MainWindow::exportTikZ() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export TikZ",
        QString(), "TeX Files (*.tex);;All Files (*)");

    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".tex")) {
        fileName += ".tex";
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << m_document->tikzFull();
        file.close();
        m_statusLabel->setText(QString("Exported to %1").arg(fileName));
    } else {
        QMessageBox::warning(this, "Error", "Failed to export file.");
    }
}

void MainWindow::loadBackgroundImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Background Image",
        QString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif);;All Files (*)");

    if (fileName.isEmpty()) return;

    m_canvasView->setBackgroundImage(fileName);
    m_document->setBackgroundImage(fileName);
    m_statusLabel->setText(QString("Background image loaded: %1").arg(fileName));
}

void MainWindow::clearBackgroundImage() {
    m_canvasView->clearBackgroundImage();
    m_document->setBackgroundImage(QString());
    m_statusLabel->setText("Background image cleared");
}

void MainWindow::setBackgroundOpacity(int value) {
    float opacity = value / 100.0f;
    m_document->setBackgroundOpacity(opacity);
    m_canvasView->setBackgroundOpacity(opacity);
}

void MainWindow::toggleGrid(bool visible) {
    m_canvas->setGridVisible(visible);
}

void MainWindow::toggleAxes(bool visible) {
    m_canvas->setAxesVisible(visible);
}

void MainWindow::toggleAxisTicks(bool visible) {
    m_canvas->setAxisTicksVisible(visible);
}

void MainWindow::compileAndPreview() {
    if (m_compileProcess) {
        m_statusLabel->setText("Compile already in progress...");
        return;
    }

    m_statusLabel->setText("Compiling...");
    QApplication::processEvents();

    // Create temp directory
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        m_statusLabel->setText("Failed to create temp directory");
        return;
    }
    tempDir.setAutoRemove(false);
    m_tempDir = tempDir.path();

    // Write TeX file
    QString texPath = m_tempDir + "/preview.tex";
    QFile texFile(texPath);
    if (!texFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_statusLabel->setText("Failed to write TeX file");
        return;
    }

    QTextStream out(&texFile);
    out << m_document->tikzFull();
    texFile.close();

    // Run pdflatex
    m_compileProcess = new QProcess(this);
    m_compileProcess->setWorkingDirectory(m_tempDir);

    connect(m_compileProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onCompileFinished);

    m_compileProcess->start("pdflatex", QStringList()
        << "-interaction=nonstopmode"
        << "-halt-on-error"
        << "preview.tex");

    if (!m_compileProcess->waitForStarted(5000)) {
        m_statusLabel->setText("Failed to start pdflatex. Is LaTeX installed?");
        delete m_compileProcess;
        m_compileProcess = nullptr;
    }
}

void MainWindow::onCompileFinished(int exitCode) {
    if (!m_compileProcess) return;

    QString output = m_compileProcess->readAllStandardOutput();
    QString errors = m_compileProcess->readAllStandardError();

    delete m_compileProcess;
    m_compileProcess = nullptr;

    if (exitCode == 0) {
        QString pdfPath = m_tempDir + "/preview.pdf";
        QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath));
        m_statusLabel->setText("Compile successful - PDF opened");
    } else {
        m_statusLabel->setText("Compile failed - check TikZ code");

        QString errorMsg = "Compilation failed.\n\n";
        if (!errors.isEmpty()) {
            errorMsg += "Errors:\n" + errors.left(1000);
        } else if (output.contains("!")) {
            int errorStart = output.indexOf("!");
            int errorEnd = output.indexOf("\n\n", errorStart);
            if (errorEnd < 0) errorEnd = errorStart + 500;
            errorMsg += output.mid(errorStart, errorEnd - errorStart);
        }
        QMessageBox::warning(this, "Compile Error", errorMsg);
    }
}

void MainWindow::updateTikZCode() {
    m_tikzCodeView->setPlainText(m_document->tikz());
}

void MainWindow::updateStatusBar() {
    m_zoomLabel->setText(QString("Zoom: %1%").arg(
        static_cast<int>(m_canvasView->zoomLevel() * 100)));
}

void MainWindow::onSelectionChanged() {
    m_propertiesPanel->setSelection(m_canvas->selectedItems());
}

void MainWindow::onStatusMessage(const QString &msg) {
    m_statusLabel->setText(msg);
}
