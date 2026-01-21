#include "layerspanel.h"
#include "document.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

LayersPanel::LayersPanel(Document *doc, QWidget *parent)
    : QWidget(parent)
    , m_document(doc)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Layer list
    m_layerList = new QListWidget;
    m_layerList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_layerList->setDragDropMode(QAbstractItemView::InternalMove);
    mainLayout->addWidget(m_layerList);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(4);

    m_addButton = new QPushButton("+");
    m_addButton->setFixedSize(28, 28);
    m_addButton->setToolTip("Add Layer");
    buttonLayout->addWidget(m_addButton);

    m_removeButton = new QPushButton("-");
    m_removeButton->setFixedSize(28, 28);
    m_removeButton->setToolTip("Remove Layer");
    buttonLayout->addWidget(m_removeButton);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(m_addButton, &QPushButton::clicked, this, &LayersPanel::onAddLayer);
    connect(m_removeButton, &QPushButton::clicked, this, &LayersPanel::onRemoveLayer);
    connect(m_layerList, &QListWidget::currentRowChanged, this, &LayersPanel::onLayerSelectionChanged);
    connect(m_layerList, &QListWidget::itemChanged, this, &LayersPanel::onVisibilityToggled);
    connect(m_layerList, &QListWidget::itemDoubleClicked, this, &LayersPanel::onLayerDoubleClicked);
    connect(m_document, &Document::layersChanged, this, &LayersPanel::refresh);

    refresh();
}

void LayersPanel::refresh() {
    m_layerList->blockSignals(true);
    m_layerList->clear();

    const auto &layers = m_document->layers();
    for (int i = layers.size() - 1; i >= 0; --i) {  // Show top layer first
        const auto &layer = layers[i];
        QListWidgetItem *item = new QListWidgetItem(m_layerList);
        item->setText(layer.name);
        item->setData(Qt::UserRole, i);  // Store actual index
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(layer.visible ? Qt::Checked : Qt::Unchecked);

        // Highlight locked layers
        if (layer.locked) {
            item->setForeground(Qt::gray);
        }

        // Highlight current layer
        if (i == m_document->currentLayer()) {
            item->setBackground(QColor(70, 106, 159, 50));  // Atlantic with alpha
        }
    }

    // Select current layer (keep signals blocked)
    for (int i = 0; i < m_layerList->count(); ++i) {
        if (m_layerList->item(i)->data(Qt::UserRole).toInt() == m_document->currentLayer()) {
            m_layerList->setCurrentRow(i);
            break;
        }
    }

    m_layerList->blockSignals(false);

    // Update remove button state
    m_removeButton->setEnabled(layers.size() > 1);
}

void LayersPanel::onAddLayer() {
    bool ok;
    QString name = QInputDialog::getText(this, "New Layer", "Layer name:",
                                         QLineEdit::Normal, "", &ok);
    if (ok) {
        m_document->addLayer(name.isEmpty() ? QString() : name);
    }
}

void LayersPanel::onRemoveLayer() {
    QListWidgetItem *item = m_layerList->currentItem();
    if (!item) return;

    int layerIndex = item->data(Qt::UserRole).toInt();

    if (m_document->layers().size() <= 1) {
        QMessageBox::warning(this, "Cannot Remove", "You must have at least one layer.");
        return;
    }

    m_document->removeLayer(layerIndex);
}

void LayersPanel::onLayerSelectionChanged() {
    QListWidgetItem *item = m_layerList->currentItem();
    if (!item) return;

    int layerIndex = item->data(Qt::UserRole).toInt();
    m_document->setCurrentLayer(layerIndex);
}

void LayersPanel::onVisibilityToggled(QListWidgetItem *item) {
    int layerIndex = item->data(Qt::UserRole).toInt();
    bool visible = (item->checkState() == Qt::Checked);
    m_document->setLayerVisible(layerIndex, visible);
}

void LayersPanel::onLayerDoubleClicked(QListWidgetItem *item) {
    int layerIndex = item->data(Qt::UserRole).toInt();
    const auto &layer = m_document->layers()[layerIndex];

    bool ok;
    QString name = QInputDialog::getText(this, "Rename Layer", "Layer name:",
                                         QLineEdit::Normal, layer.name, &ok);
    if (ok && !name.isEmpty()) {
        m_document->setLayerName(layerIndex, name);
    }
}
