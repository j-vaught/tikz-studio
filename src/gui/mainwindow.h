#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "common.h"

class Document;
class Canvas;
class CanvasView;
class ToolPalette;
class ColorPalette;
class PropertiesPanel;
class ContextToolbar;
class LayersPanel;
class QTextEdit;
class QDockWidget;
class QLabel;
class QProcess;
class QSpinBox;
class QSlider;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void exportTikZ();

    void loadBackgroundImage();
    void clearBackgroundImage();
    void setBackgroundOpacity(int value);
    void toggleGrid(bool visible);
    void toggleAxes(bool visible);
    void toggleAxisTicks(bool visible);

    void compileAndPreview();
    void onCompileFinished(int exitCode);

    void updateTikZCode();
    void updateStatusBar();
    void onSelectionChanged();
    void onStatusMessage(const QString &msg);

    // Context toolbar slots
    void onLineWidthChanged(double width);
    void onStrokeColorChanged(const QColor &color);
    void onFillColorChanged(const QColor &color);
    void onLineStyleChanged(int style);
    void onLineCapChanged(int cap);
    void onLineJoinChanged(int join);
    void onFillPatternChanged(int pattern);
    void onRotationChanged(double angle);
    void onScaleChanged(double scale);
    void onToolChanged(Tool tool);

private:
    void setupMenus();
    void setupToolbars();
    void setupDocks();
    void setupStatusBar();

    Document *m_document;
    Canvas *m_canvas;
    CanvasView *m_canvasView;
    ToolPalette *m_toolPalette;
    ColorPalette *m_colorPalette;
    PropertiesPanel *m_propertiesPanel;
    ContextToolbar *m_contextToolbar;

    QTextEdit *m_tikzCodeView;
    QDockWidget *m_codeDock;
    QDockWidget *m_propertiesDock;
    QDockWidget *m_layersDock;
    LayersPanel *m_layersPanel;

    QLabel *m_statusLabel;
    QLabel *m_zoomLabel;
    QSpinBox *m_ngonSidesSpin;
    QSpinBox *m_parallelogramSkewSpin;
    QSlider *m_bgOpacitySlider;

    QString m_currentFile;
    QProcess *m_compileProcess = nullptr;
    QString m_tempDir;
};

#endif // MAINWINDOW_H
