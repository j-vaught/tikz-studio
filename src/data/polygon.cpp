#include "polygon.h"
#include "point.h"
#include "common.h"
#include <QtMath>

Polygon::Polygon(QObject *parent)
    : QObject(parent)
    , m_fillColor(UofSC::Garnet())
    , m_strokeColor(UofSC::Black())
{
}

QVector<QPointF> Polygon::vertexPositions() const {
    QVector<QPointF> result;
    for (int i = 0; i < m_vertices.size(); i++) {
        if (i < m_pointRefs.size() && m_pointRefs[i]) {
            result.append(m_pointRefs[i]->pos());
        } else {
            result.append(m_vertices[i].pos);
        }
    }
    return result;
}

void Polygon::addVertex(QPointF pos, float cornerRadius) {
    m_vertices.append(Vertex(pos, cornerRadius));
    m_pointRefs.append(nullptr);
    emit changed();
}

void Polygon::addVertex(Point *point) {
    m_vertices.append(Vertex(point ? point->pos() : QPointF(), 0.0f));
    m_pointRefs.append(point);
    if (point) {
        connect(point, &Point::changed, this, &Polygon::onPointChanged);
    }
    emit changed();
}

void Polygon::insertVertex(int index, QPointF pos, float cornerRadius) {
    if (index < 0 || index > m_vertices.size()) return;
    m_vertices.insert(index, Vertex(pos, cornerRadius));
    m_pointRefs.insert(index, nullptr);
    emit changed();
}

void Polygon::removeVertex(int index) {
    if (index < 0 || index >= m_vertices.size()) return;
    if (index < m_pointRefs.size() && m_pointRefs[index]) {
        disconnect(m_pointRefs[index], &Point::changed, this, &Polygon::onPointChanged);
    }
    m_vertices.remove(index);
    if (index < m_pointRefs.size()) {
        m_pointRefs.remove(index);
    }
    emit changed();
}

void Polygon::setVertexPosition(int index, QPointF pos) {
    if (index < 0 || index >= m_vertices.size()) return;
    if (index < m_pointRefs.size() && m_pointRefs[index]) {
        m_pointRefs[index]->setPos(pos);
    } else {
        m_vertices[index].pos = pos;
    }
    emit changed();
}

void Polygon::setVertexCornerRadius(int index, float radius) {
    if (index < 0 || index >= m_vertices.size()) return;
    m_vertices[index].cornerRadius = radius;
    emit changed();
}

float Polygon::vertexCornerRadius(int index) const {
    if (index < 0 || index >= m_vertices.size()) return 0.0f;
    float r = m_vertices[index].cornerRadius;
    return (r > 0) ? r : m_defaultCornerRadius;
}

int Polygon::vertexCount() const {
    return m_vertices.size();
}

void Polygon::clear() {
    for (int i = 0; i < m_pointRefs.size(); i++) {
        if (m_pointRefs[i]) {
            disconnect(m_pointRefs[i], &Point::changed, this, &Polygon::onPointChanged);
        }
    }
    m_vertices.clear();
    m_pointRefs.clear();
    emit changed();
}

void Polygon::setFillColor(const QColor &color) {
    if (m_fillColor != color) {
        m_fillColor = color;
        emit changed();
    }
}

void Polygon::setStrokeColor(const QColor &color) {
    if (m_strokeColor != color) {
        m_strokeColor = color;
        emit changed();
    }
}

void Polygon::setLineWidth(float width) {
    if (m_lineWidth != width) {
        m_lineWidth = width;
        emit changed();
    }
}

void Polygon::setLineStyle(LineStyle style) {
    if (m_lineStyle != style) {
        m_lineStyle = style;
        emit changed();
    }
}

void Polygon::setFillPattern(FillPattern pattern) {
    if (m_fillPattern != pattern) {
        m_fillPattern = pattern;
        emit changed();
    }
}

void Polygon::setLineCap(LineCap cap) {
    if (m_lineCap != cap) {
        m_lineCap = cap;
        emit changed();
    }
}

void Polygon::setLineJoin(LineJoin join) {
    if (m_lineJoin != join) {
        m_lineJoin = join;
        emit changed();
    }
}

void Polygon::setOpacity(float opacity) {
    if (m_opacity != opacity) {
        m_opacity = qBound(0.0f, opacity, 1.0f);
        emit changed();
    }
}

void Polygon::setRotation(float degrees) {
    if (m_rotation != degrees) {
        m_rotation = degrees;
        emit changed();
    }
}

void Polygon::setScale(float scale) {
    if (m_scale != scale) {
        m_scale = qBound(0.1f, scale, 10.0f);
        emit changed();
    }
}

void Polygon::setDefaultCornerRadius(float radius) {
    if (m_defaultCornerRadius != radius) {
        m_defaultCornerRadius = radius;
        emit changed();
    }
}

void Polygon::onPointChanged() {
    emit changed();
}

QPainterPath Polygon::painterPath() const {
    QPainterPath path;
    QVector<QPointF> pts = vertexPositions();
    int n = pts.size();

    if (n < 2) return path;
    if (n == 2) {
        path.moveTo(toScreen(pts[0]));
        path.lineTo(toScreen(pts[1]));
        return path;
    }

    // Check if any vertex has rounding
    bool hasRounding = false;
    for (int i = 0; i < n; i++) {
        if (vertexCornerRadius(i) > 0) {
            hasRounding = true;
            break;
        }
    }

    if (!hasRounding) {
        // Simple polygon
        path.moveTo(toScreen(pts[0]));
        for (int i = 1; i < n; i++) {
            path.lineTo(toScreen(pts[i]));
        }
        path.closeSubpath();
        return path;
    }

    // Polygon with rounded corners
    for (int i = 0; i < n; i++) {
        int prevIdx = (i - 1 + n) % n;
        int nextIdx = (i + 1) % n;

        QPointF prev = pts[prevIdx];
        QPointF curr = pts[i];
        QPointF next = pts[nextIdx];

        float radius = vertexCornerRadius(i);

        if (radius <= 0) {
            // Sharp corner
            if (i == 0) {
                path.moveTo(toScreen(curr));
            } else {
                path.lineTo(toScreen(curr));
            }
        } else {
            // Rounded corner
            QPointF toPrev = prev - curr;
            QPointF toNext = next - curr;

            float lenPrev = std::sqrt(toPrev.x()*toPrev.x() + toPrev.y()*toPrev.y());
            float lenNext = std::sqrt(toNext.x()*toNext.x() + toNext.y()*toNext.y());

            if (lenPrev < 0.001f || lenNext < 0.001f) {
                if (i == 0) path.moveTo(toScreen(curr));
                else path.lineTo(toScreen(curr));
                continue;
            }

            // Normalize directions
            toPrev /= lenPrev;
            toNext /= lenNext;

            // Limit radius to half the shorter edge
            float maxRadius = qMin(lenPrev, lenNext) / 2.0f;
            radius = qMin(radius, maxRadius);

            // Calculate start and end points of the arc
            QPointF arcStart = curr + toPrev * radius;
            QPointF arcEnd = curr + toNext * radius;

            if (i == 0) {
                path.moveTo(toScreen(arcStart));
            } else {
                path.lineTo(toScreen(arcStart));
            }

            // Use quadratic bezier for smooth corner
            path.quadTo(toScreen(curr), toScreen(arcEnd));
        }
    }
    path.closeSubpath();

    return path;
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

QString Polygon::tikz() const {
    if (vertexCount() < 2) return QString();

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

    QVector<QPointF> pts = vertexPositions();
    int n = pts.size();

    // Check if we have per-vertex rounding
    bool hasPerVertexRounding = false;
    for (int i = 0; i < n; i++) {
        if (m_vertices[i].cornerRadius > 0) {
            hasPerVertexRounding = true;
            break;
        }
    }

    if (!hasPerVertexRounding && m_defaultCornerRadius > 0) {
        // Use global rounded corners
        opts << QString("rounded corners=%1").arg(m_defaultCornerRadius, 0, 'f', 2);
        optStr = QString("[%1]").arg(opts.join(", "));

        QString pathStr;
        for (int i = 0; i < n; i++) {
            if (i > 0) pathStr += " -- ";
            pathStr += QString("(%1,%2)")
                .arg(pts[i].x(), 0, 'f', 3)
                .arg(pts[i].y(), 0, 'f', 3);
        }
        pathStr += " -- cycle";

        return QString("\\filldraw%1 %2;").arg(optStr, pathStr);
    }

    if (!hasPerVertexRounding) {
        // Simple polygon, no rounding
        QString pathStr;
        for (int i = 0; i < n; i++) {
            if (i > 0) pathStr += " -- ";
            pathStr += QString("(%1,%2)")
                .arg(pts[i].x(), 0, 'f', 3)
                .arg(pts[i].y(), 0, 'f', 3);
        }
        pathStr += " -- cycle";

        return QString("\\filldraw%1 %2;").arg(optStr, pathStr);
    }

    // Per-vertex rounding - need to use manual arc construction
    // This generates a path with rounded corners where specified
    QString pathStr;

    for (int i = 0; i < n; i++) {
        int prevIdx = (i - 1 + n) % n;
        int nextIdx = (i + 1) % n;

        QPointF prev = pts[prevIdx];
        QPointF curr = pts[i];
        QPointF next = pts[nextIdx];

        float radius = vertexCornerRadius(i);

        if (radius <= 0) {
            // Sharp corner
            if (i == 0) {
                pathStr = QString("(%1,%2)").arg(curr.x(), 0, 'f', 3).arg(curr.y(), 0, 'f', 3);
            } else {
                pathStr += QString(" -- (%1,%2)").arg(curr.x(), 0, 'f', 3).arg(curr.y(), 0, 'f', 3);
            }
        } else {
            // Rounded corner
            QPointF toPrev = prev - curr;
            QPointF toNext = next - curr;

            float lenPrev = std::sqrt(toPrev.x()*toPrev.x() + toPrev.y()*toPrev.y());
            float lenNext = std::sqrt(toNext.x()*toNext.x() + toNext.y()*toNext.y());

            if (lenPrev < 0.001f || lenNext < 0.001f) {
                if (i == 0) {
                    pathStr = QString("(%1,%2)").arg(curr.x(), 0, 'f', 3).arg(curr.y(), 0, 'f', 3);
                } else {
                    pathStr += QString(" -- (%1,%2)").arg(curr.x(), 0, 'f', 3).arg(curr.y(), 0, 'f', 3);
                }
                continue;
            }

            toPrev /= lenPrev;
            toNext /= lenNext;

            float maxRadius = qMin(lenPrev, lenNext) / 2.0f;
            radius = qMin(radius, maxRadius);

            QPointF arcStart = curr + toPrev * radius;
            QPointF arcEnd = curr + toNext * radius;

            if (i == 0) {
                pathStr = QString("(%1,%2)").arg(arcStart.x(), 0, 'f', 3).arg(arcStart.y(), 0, 'f', 3);
            } else {
                pathStr += QString(" -- (%1,%2)").arg(arcStart.x(), 0, 'f', 3).arg(arcStart.y(), 0, 'f', 3);
            }

            // Bezier curve for the corner
            pathStr += QString(" .. controls (%1,%2) .. (%3,%4)")
                .arg(curr.x(), 0, 'f', 3).arg(curr.y(), 0, 'f', 3)
                .arg(arcEnd.x(), 0, 'f', 3).arg(arcEnd.y(), 0, 'f', 3);
        }
    }
    pathStr += " -- cycle";

    return QString("\\filldraw%1 %2;").arg(optStr, pathStr);
}
