#include "canvasview.h"
#include "canvas.h"
#include "common.h"

#include <QWheelEvent>
#include <QScrollBar>
#include <QPainter>
#include <QImageReader>

CanvasView::CanvasView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::RubberBandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setBackgroundBrush(Qt::white);

    // Start zoomed in a bit
    scale(2.0, 2.0);
    m_scale = 2.0;
}

void CanvasView::setCanvas(Canvas *canvas) {
    setScene(canvas);
}

void CanvasView::zoomIn() {
    applyZoom(1.25f);
}

void CanvasView::zoomOut() {
    applyZoom(0.8f);
}

void CanvasView::zoomFit() {
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    m_scale = transform().m11();
    emit zoomChanged(m_scale);
}

void CanvasView::zoomReset() {
    resetTransform();
    scale(2.0, 2.0);
    m_scale = 2.0;
    emit zoomChanged(m_scale);
}

void CanvasView::applyZoom(float factor) {
    float newScale = m_scale * factor;

    // Limit zoom range
    if (newScale < 0.1f || newScale > 20.0f) return;

    scale(factor, factor);
    m_scale = newScale;
    emit zoomChanged(m_scale);
}

void CanvasView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else if (event->modifiers() & Qt::ShiftModifier) {
        // Horizontal scroll
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() - event->angleDelta().y());
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void CanvasView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_panStart = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent *event) {
    if (m_panning) {
        QPoint delta = event->pos() - m_panStart;
        m_panStart = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::setBackgroundImage(const QString &path) {
    QImageReader reader(path);
    reader.setAutoTransform(true);

    QImage image = reader.read();
    if (image.isNull()) return;

    m_backgroundImage = QPixmap::fromImage(image);
    m_backgroundImagePath = path;

    // Center image at origin, scale to reasonable size
    float imgWidth = m_backgroundImage.width() / GLOBAL_SCALE;
    float imgHeight = m_backgroundImage.height() / GLOBAL_SCALE;

    m_imageRect = QRectF(0, -imgHeight * GLOBAL_SCALE,
                         imgWidth * GLOBAL_SCALE, imgHeight * GLOBAL_SCALE);

    viewport()->update();
}

void CanvasView::clearBackgroundImage() {
    m_backgroundImage = QPixmap();
    m_backgroundImagePath.clear();
    m_imageRect = QRectF();
    viewport()->update();
}

void CanvasView::drawBackground(QPainter *painter, const QRectF &rect) {
    // Let the scene draw its background (grid)
    QGraphicsView::drawBackground(painter, rect);

    // Draw background image if set
    if (!m_backgroundImage.isNull()) {
        painter->setOpacity(0.5);  // Semi-transparent for tracing
        painter->drawPixmap(m_imageRect.toRect(), m_backgroundImage);
        painter->setOpacity(1.0);
    }
}
