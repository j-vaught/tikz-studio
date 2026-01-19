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

void Ellipse::setScale(float scale) {
    if (m_scale != scale) {
        m_scale = qBound(0.1f, scale, 10.0f);
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

void Ellipse::setLineStyle(LineStyle style) {
    if (m_lineStyle != style) {
        m_lineStyle = style;
        emit changed();
    }
}

void Ellipse::setFillPattern(FillPattern pattern) {
    if (m_fillPattern != pattern) {
        m_fillPattern = pattern;
        emit changed();
    }
}

void Ellipse::setLineCap(LineCap cap) {
    if (m_lineCap != cap) {
        m_lineCap = cap;
        emit changed();
    }
}

void Ellipse::setLineJoin(LineJoin join) {
    if (m_lineJoin != join) {
        m_lineJoin = join;
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

static QString lineStyleToTikz(LineStyle style) {
    switch (style) {
        case LineStyle::Dashed: return "dashed";
        case LineStyle::DenselyDashed: return "densely dashed";
        case LineStyle::LooselyDashed: return "loosely dashed";
        case LineStyle::Dotted: return "dotted";
        case LineStyle::DenselyDotted: return "densely dotted";
        case LineStyle::LooselyDotted: return "loosely dotted";
        case LineStyle::DashDot: return "dashdotted";
        case LineStyle::DenselyDashDot: return "densely dashdotted";
        case LineStyle::LooselyDashDot: return "loosely dashdotted";
        case LineStyle::DashDotDot: return "dashdotdotted";
        case LineStyle::DenselyDashDotDot: return "densely dashdotdotted";
        case LineStyle::LooselyDashDotDot: return "loosely dashdotdotted";
        default: return QString();
    }
}

static QString lineCapToTikz(LineCap cap) {
    switch (cap) {
        case LineCap::Round: return "line cap=round";
        case LineCap::Square: return "line cap=rect";
        default: return QString();  // Butt is default
    }
}

static QString lineJoinToTikz(LineJoin join) {
    switch (join) {
        case LineJoin::Round: return "line join=round";
        case LineJoin::Bevel: return "line join=bevel";
        default: return QString();  // Miter is default
    }
}

static QString fillPatternToTikz(FillPattern pattern, const QColor &color) {
    QString colorName = colorToTikz(color);
    switch (pattern) {
        case FillPattern::HorizontalLines:
            return QString("pattern=horizontal lines, pattern color=%1").arg(colorName);
        case FillPattern::VerticalLines:
            return QString("pattern=vertical lines, pattern color=%1").arg(colorName);
        case FillPattern::Grid:
            return QString("pattern=grid, pattern color=%1").arg(colorName);
        case FillPattern::NorthEastLines:
            return QString("pattern=north east lines, pattern color=%1").arg(colorName);
        case FillPattern::NorthWestLines:
            return QString("pattern=north west lines, pattern color=%1").arg(colorName);
        case FillPattern::CrossHatch:
            return QString("pattern=crosshatch, pattern color=%1").arg(colorName);
        case FillPattern::CrossHatchDots:
            return QString("pattern=crosshatch dots, pattern color=%1").arg(colorName);
        case FillPattern::Dots:
            return QString("pattern=dots, pattern color=%1").arg(colorName);
        case FillPattern::FivePointedStars:
            return QString("pattern=fivepointed stars, pattern color=%1").arg(colorName);
        case FillPattern::SixPointedStars:
            return QString("pattern=sixpointed stars, pattern color=%1").arg(colorName);
        case FillPattern::Bricks:
            return QString("pattern=bricks, pattern color=%1").arg(colorName);
        default:
            return QString();
    }
}

QString Ellipse::tikz() const {
    QStringList opts;

    // Fill - either solid color or pattern
    if (m_fillPattern != FillPattern::None && m_fillPattern != FillPattern::Solid) {
        QString patternStr = fillPatternToTikz(m_fillPattern, m_fillColor);
        if (!patternStr.isEmpty()) {
            opts << patternStr;
        }
    } else if (m_fillPattern == FillPattern::Solid && m_fillColor.isValid() && m_fillColor.alpha() > 0) {
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

    // Line style
    QString lineStyleStr = lineStyleToTikz(m_lineStyle);
    if (!lineStyleStr.isEmpty()) {
        opts << lineStyleStr;
    }

    // Line cap
    QString lineCapStr = lineCapToTikz(m_lineCap);
    if (!lineCapStr.isEmpty()) {
        opts << lineCapStr;
    }

    // Line join
    QString lineJoinStr = lineJoinToTikz(m_lineJoin);
    if (!lineJoinStr.isEmpty()) {
        opts << lineJoinStr;
    }

    // Opacity
    if (m_opacity < 1.0f) {
        opts << QString("opacity=%1").arg(m_opacity, 0, 'f', 2);
    }

    // Rotation
    if (m_rotation != 0.0f) {
        opts << QString("rotate=%1").arg(m_rotation, 0, 'f', 1);
    }

    // Scale
    if (m_scale != 1.0f) {
        opts << QString("scale=%1").arg(m_scale, 0, 'f', 2);
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
