#include "colorpalette.h"
#include "common.h"

#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

ColorPalette::ColorPalette(QWidget *parent)
    : QWidget(parent)
    , m_currentColor(UofSC::Black())
{
    setupColors();
}

void ColorPalette::setupColors() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Primary colors
    QLabel *primaryLabel = new QLabel("Primary");
    primaryLabel->setStyleSheet("font-weight: bold; font-size: 10px;");
    mainLayout->addWidget(primaryLabel);

    QGridLayout *primaryGrid = new QGridLayout();
    primaryGrid->setSpacing(2);
    addColorButton(primaryGrid, UofSC::Black(), "Black", 0, 0);
    addColorButton(primaryGrid, UofSC::Garnet(), "Garnet", 0, 1);
    addColorButton(primaryGrid, UofSC::White(), "White", 0, 2);
    mainLayout->addLayout(primaryGrid);

    // Accent colors
    QLabel *accentLabel = new QLabel("Accent");
    accentLabel->setStyleSheet("font-weight: bold; font-size: 10px;");
    mainLayout->addWidget(accentLabel);

    QGridLayout *accentGrid = new QGridLayout();
    accentGrid->setSpacing(2);
    addColorButton(accentGrid, UofSC::Rose(), "Rose", 0, 0);
    addColorButton(accentGrid, UofSC::Atlantic(), "Atlantic", 0, 1);
    addColorButton(accentGrid, UofSC::Congaree(), "Congaree", 0, 2);
    addColorButton(accentGrid, UofSC::Horseshoe(), "Horseshoe", 1, 0);
    addColorButton(accentGrid, UofSC::Grass(), "Grass", 1, 1);
    addColorButton(accentGrid, UofSC::Honeycomb(), "Honeycomb", 1, 2);
    mainLayout->addLayout(accentGrid);

    // Neutral colors
    QLabel *neutralLabel = new QLabel("Neutral");
    neutralLabel->setStyleSheet("font-weight: bold; font-size: 10px;");
    mainLayout->addWidget(neutralLabel);

    QGridLayout *neutralGrid = new QGridLayout();
    neutralGrid->setSpacing(2);
    addColorButton(neutralGrid, UofSC::Gray90(), "90% Black", 0, 0);
    addColorButton(neutralGrid, UofSC::Gray70(), "70% Black", 0, 1);
    addColorButton(neutralGrid, UofSC::Gray50(), "50% Black", 0, 2);
    addColorButton(neutralGrid, UofSC::WarmGrey(), "Warm Grey", 1, 0);
    addColorButton(neutralGrid, UofSC::Sandstorm(), "Sandstorm", 1, 1);
    mainLayout->addLayout(neutralGrid);

    // Special colors
    QLabel *specialLabel = new QLabel("Special");
    specialLabel->setStyleSheet("font-weight: bold; font-size: 10px;");
    mainLayout->addWidget(specialLabel);

    QGridLayout *specialGrid = new QGridLayout();
    specialGrid->setSpacing(2);
    addColorButton(specialGrid, UofSC::DarkGarnet(), "Dark Garnet", 0, 0);
    addColorButton(specialGrid, UofSC::Azalea(), "Azalea", 0, 1);
    mainLayout->addLayout(specialGrid);

    mainLayout->addStretch();

    // Select black by default
    updateSelection();
}

void ColorPalette::addColorButton(QGridLayout *layout, const QColor &color,
                                  const QString &name, int row, int col)
{
    QPushButton *btn = new QPushButton();
    btn->setFixedSize(28, 28);
    btn->setToolTip(name);
    btn->setCheckable(true);

    // Style with the color
    QString borderColor = (color == UofSC::White()) ? "#cccccc" : color.name();
    btn->setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 2px solid %2; }"
        "QPushButton:checked { border: 3px solid #000000; }"
        "QPushButton:hover { border: 2px solid #666666; }"
    ).arg(color.name(), borderColor));

    connect(btn, &QPushButton::clicked, this, [this, color, btn]() {
        m_currentColor = color;
        updateSelection();
        emit colorChanged(color);
    });

    layout->addWidget(btn, row, col);
    m_buttons.append(btn);
    m_colors.append(color);
}

void ColorPalette::setCurrentColor(const QColor &color) {
    m_currentColor = color;
    updateSelection();
    emit colorChanged(color);
}

void ColorPalette::updateSelection() {
    for (int i = 0; i < m_buttons.size(); i++) {
        m_buttons[i]->setChecked(m_colors[i] == m_currentColor);
    }
}
