#ifndef LAYERSPANEL_H
#define LAYERSPANEL_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

class Document;

class LayersPanel : public QWidget {
    Q_OBJECT

public:
    explicit LayersPanel(Document *doc, QWidget *parent = nullptr);

public slots:
    void refresh();

private slots:
    void onAddLayer();
    void onRemoveLayer();
    void onLayerSelectionChanged();
    void onVisibilityToggled(QListWidgetItem *item);
    void onLayerDoubleClicked(QListWidgetItem *item);

private:
    Document *m_document;
    QListWidget *m_layerList;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
};

#endif // LAYERSPANEL_H
