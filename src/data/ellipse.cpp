#include "ellipse.h"
#include "common.h"

Ellipse::Ellipse(QObject *parent)
    : QObject(parent)
    , m_fillColor(UofSC::Garnet())
    , m_strokeColor(UofSC::Black())
{
}

void Ellipse::setCenter(QPointF center) {
    if (m_center != center) {
        m_center = center;
        emit changed();
    }
}

void Ellipse::setRadiusX(float rx) {
    if (m_radiusX != rx) {
        m_radiusX = rx;
        emit changed();
    }
}

void Ellipse::setRadiusY(float ry) {
    if (m_radiusY != ry) {
        m_radiusY = ry;
        emit changed();
    }
}

void Ellipse::setRotation(float degrees) {
    if (m_rotation != degrees) {
        m_rotation = degrees;
        emit changed();
    }
}

void Ellipse::setFillColor(const QColor &color) {
    if (m_fillColor != color) {
        m_fillColor = color;
        emit changed();
    }
}

void Ellipse::setStrokeColor(const QColor &color) {
    if (m_strokeColor != color) {
        m_strokeColor = color;
        emit changed();
    }
}

void Ellipse::setLineWidth(float width) {
    if (m_lineWidth != width) {
        m_lineWidth = width;
        emit changed();
    }
}

void Ellipse::setOpacity(float opacity) {
    if (m_opacity != opacity) {
        m_opacity = qBound(0.0f, opacity, 1.0f);
        emit changed();
    }
}

static QString colorToTikz(const QColor &color) {
    if (color == UofSC::Black()) return "black";
    if (color == UofSC::White()) return "white";
    if (color == UofSC::Garnet()) return "garnet";
    if (color == UofSC::Rose()) return "rose";
    if (color == UofSC::Atlantic()) return "atlantic";
    if (color == UofSC::Congaree()) return "congaree";
    if (color == UofSC::Horseshoe()) return "horseshoe";
    if (color == UofSC::Grass()) return "grass";
    if (color == UofSC::Honeycomb()) return "honeycomb";

    return QString("{rgb,255:red,%1;green,%2;blue,%3}")
        .arg(color.red()).arg(color.green()).arg(color.blue());
}

QString Ellipse::tikz() const {
    QStringList opts;

    // Fill
    if (m_fillColor.isValid() && m_fillColor.alpha() > 0) {
        opts << QString("fill=%1").arg(colorToTikz(m_fillColor));
    }

    // Stroke
    if (m_strokeColor.isValid() && m_strokeColor.alpha() > 0) {
        opts << QString("draw=%1").arg(colorToTikz(m_strokeColor));
    }

    // Line width
    if (m_lineWidth != 0.8f) {
        opts << QString("line width=%1pt").arg(m_lineWidth, 0, 'f', 1);
    }

    // Opacity
    if (m_opacity < 1.0f) {
        opts << QString("opacity=%1").arg(m_opacity, 0, 'f', 2);
    }

    // Rotation
    if (m_rotation != 0.0f) {
        opts << QString("rotate=%1").arg(m_rotation, 0, 'f', 1);
    }

    QString optStr = opts.isEmpty() ? "" : QString("[%1]").arg(opts.join(", "));

    if (isCircle()) {
        return QString("\\filldraw%1 (%2,%3) circle (%4);")
            .arg(optStr)
            .arg(m_center.x(), 0, 'f', 3)
            .arg(m_center.y(), 0, 'f', 3)
            .arg(m_radiusX, 0, 'f', 3);
    } else {
        return QString("\\filldraw%1 (%2,%3) ellipse (%4 and %5);")
            .arg(optStr)
            .arg(m_center.x(), 0, 'f', 3)
            .arg(m_center.y(), 0, 'f', 3)
            .arg(m_radiusX, 0, 'f', 3)
            .arg(m_radiusY, 0, 'f', 3);
    }
}
