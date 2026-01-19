#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QWidget>
#include <QColor>
#include <QVector>

class QPushButton;
class QGridLayout;

class ColorPalette : public QWidget {
    Q_OBJECT

public:
    explicit ColorPalette(QWidget *parent = nullptr);

    QColor currentColor() const { return m_currentColor; }
    void setCurrentColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);

private:
    void setupColors();
    void addColorButton(QGridLayout *layout, const QColor &color,
                       const QString &name, int row, int col);
    void updateSelection();

    QColor m_currentColor;
    QVector<QPushButton*> m_buttons;
    QVector<QColor> m_colors;
};

#endif // COLORPALETTE_H
