#ifndef COMMON_H
#define COMMON_H

#include <QPointF>
#include <QColor>
#include <cmath>

// Global scale: pixels per TikZ unit
constexpr float GLOBAL_SCALE = 40.0f;
constexpr float GRID_MINOR = 0.25f;  // TikZ units
constexpr float GRID_MAJOR = 1.0f;   // TikZ units

// Coordinate transformations
inline QPointF toScreen(QPointF tikz) {
    return QPointF(tikz.x() * GLOBAL_SCALE, -tikz.y() * GLOBAL_SCALE);
}

inline QPointF fromScreen(QPointF screen) {
    return QPointF(screen.x() / GLOBAL_SCALE, -screen.y() / GLOBAL_SCALE);
}

inline float toScreen(float tikz) { return tikz * GLOBAL_SCALE; }
inline float fromScreen(float screen) { return screen / GLOBAL_SCALE; }

// Snap to grid
inline float snapToGrid(float value, float gridSize) {
    return std::round(value / gridSize) * gridSize;
}

inline QPointF snapToGrid(QPointF pt, float gridSize) {
    return QPointF(snapToGrid(pt.x(), gridSize), snapToGrid(pt.y(), gridSize));
}

// UofSC Color Palette
namespace UofSC {
    // Primary
    inline QColor Garnet()    { return QColor(115, 0, 10); }
    inline QColor Black()     { return QColor(0, 0, 0); }
    inline QColor White()     { return QColor(255, 255, 255); }

    // Accent
    inline QColor Rose()      { return QColor(204, 46, 64); }
    inline QColor Atlantic()  { return QColor(70, 106, 159); }
    inline QColor Congaree()  { return QColor(31, 65, 77); }
    inline QColor Horseshoe() { return QColor(101, 120, 11); }
    inline QColor Grass()     { return QColor(206, 211, 24); }
    inline QColor Honeycomb() { return QColor(164, 145, 55); }

    // Neutral
    inline QColor Gray90()    { return QColor(54, 54, 54); }
    inline QColor Gray70()    { return QColor(92, 92, 92); }
    inline QColor Gray50()    { return QColor(162, 162, 162); }
    inline QColor WarmGrey()  { return QColor(103, 97, 86); }
    inline QColor Sandstorm() { return QColor(255, 242, 227); }

    // Special
    inline QColor DarkGarnet(){ return QColor(87, 0, 8); }
    inline QColor Azalea()    { return QColor(132, 66, 71); }
}

// Tool types
enum class Tool {
    Select,
    Point,
    Line,
    Polygon,
    Curve,
    Rectangle,
    Circle,
    Ellipse,
    Triangle,
    RegularPolygon
};

#endif // COMMON_H
