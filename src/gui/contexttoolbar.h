#ifndef CONTEXTTOOLBAR_H
#define CONTEXTTOOLBAR_H

#include <QToolBar>
#include "common.h"

class QDoubleSpinBox;
class QComboBox;
class QPushButton;
class QLabel;
class QGraphicsItem;

class ContextToolbar : public QToolBar {
    Q_OBJECT

public:
    explicit ContextToolbar(QWidget *parent = nullptr);

    void updateForSelection(const QList<QGraphicsItem*> &items);
    void clearSelection();

signals:
    void lineWidthChanged(double width);
    void strokeColorChanged(const QColor &color);
    void fillColorChanged(const QColor &color);
    void lineStyleChanged(LineStyle style);
    void lineCapChanged(LineCap cap);
    void lineJoinChanged(LineJoin join);
    void fillPatternChanged(FillPattern pattern);
    void rotationChanged(double angle);
    void scaleChanged(double scale);

private slots:
    void onStrokeColorClicked();
    void onFillColorClicked();

private:
    void setupWidgets();
    void showColorDialog(bool forFill);
    void updateColorButton(QPushButton *btn, const QColor &color);

    QLabel *m_selectionLabel;

    // Line/stroke controls
    QDoubleSpinBox *m_lineWidthSpin;
    QPushButton *m_strokeColorBtn;
    QComboBox *m_lineStyleCombo;
    QComboBox *m_lineCapCombo;
    QComboBox *m_lineJoinCombo;

    // Fill controls
    QPushButton *m_fillColorBtn;
    QComboBox *m_fillPatternCombo;

    // Transform controls
    QDoubleSpinBox *m_rotationSpin;
    QDoubleSpinBox *m_scaleSpin;

    QColor m_currentStrokeColor = Qt::black;
    QColor m_currentFillColor = QColor(115, 0, 10);  // Garnet

    bool m_updating = false;
};

#endif // CONTEXTTOOLBAR_H
