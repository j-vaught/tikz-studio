#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <QPixmap>

class Canvas;

class CanvasView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CanvasView(QWidget *parent = nullptr);

    void setCanvas(Canvas *canvas);

    // Zoom
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomReset();
    float zoomLevel() const { return m_scale; }

    // Background image
    void setBackgroundImage(const QString &path);
    void clearBackgroundImage();
    bool hasBackgroundImage() const { return !m_backgroundImage.isNull(); }
    QString backgroundImagePath() const { return m_backgroundImagePath; }

signals:
    void zoomChanged(float level);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void applyZoom(float factor);

    float m_scale = 1.0f;
    QPixmap m_backgroundImage;
    QString m_backgroundImagePath;
    QRectF m_imageRect;

    bool m_panning = false;
    QPoint m_panStart;
};

#endif // CANVASVIEW_H
