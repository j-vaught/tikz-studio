# TikZ Studio

A visual editor for creating TikZ diagrams, built with Qt6 and C++. Draw shapes interactively and export clean LaTeX TikZ code.

## Features

- **Drawing Tools**: Points, lines, polygons, curves (smooth splines), ellipses/circles
- **Shape Tools**: Rectangle, circle, ellipse, triangle, regular polygon (N-gon)
- **Background Tracing**: Load an image to trace over with adjustable opacity
- **Per-Vertex Corner Radius**: Round individual corners of polygons
- **Real-Time TikZ Output**: See generated code update as you draw
- **Compile & Preview**: One-click compilation with pdflatex to verify output
- **Context Menus**: Right-click shapes to edit properties, round corners, delete
- **Grid Snapping**: Coordinates snap to 0.25 TikZ unit grid
- **UofSC Color Palette**: Built-in University of South Carolina brand colors

## Requirements

- Qt6 (or Qt5 with minor adjustments)
- CMake 3.16+
- C++17 compiler
- pdflatex (for compile & preview feature)

### macOS

```bash
brew install qt@6 cmake
```

### Ubuntu/Debian

```bash
sudo apt install qt6-base-dev cmake build-essential texlive-latex-base
```

## Building

```bash
cd tikz-studio
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running

```bash
# macOS
open tikz_editor.app

# Linux
./tikz_editor
```

## Project Structure

```
tikz-studio/
├── CMakeLists.txt          # Build configuration
├── src/
│   ├── main.cpp            # Application entry point
│   ├── common.h            # Global constants, coordinate transforms, colors
│   ├── data/               # Data model classes (document state)
│   │   ├── document.h/cpp  # Central document holding all shapes
│   │   ├── point.h/cpp     # Named point with coordinates
│   │   ├── line.h/cpp      # Line between two points
│   │   ├── polygon.h/cpp   # Closed polygon with per-vertex corner radius
│   │   ├── curve.h/cpp     # Smooth curve through control points
│   │   └── ellipse.h/cpp   # Ellipse/circle with center, radii, rotation
│   └── gui/                # Qt graphics and UI classes
│       ├── mainwindow.h/cpp      # Main application window
│       ├── canvas.h/cpp          # QGraphicsScene handling drawing logic
│       ├── canvasview.h/cpp      # QGraphicsView with zoom/pan
│       ├── toolpalette.h/cpp     # Tool selection buttons
│       ├── colorpalette.h/cpp    # Color selection buttons
│       ├── propertiespanel.h/cpp # Properties editor for selected shapes
│       ├── pointitem.h/cpp       # Visual representation of Point
│       ├── lineitem.h/cpp        # Visual representation of Line
│       ├── polygonitem.h/cpp     # Visual representation of Polygon
│       ├── curveitem.h/cpp       # Visual representation of Curve
│       └── ellipseitem.h/cpp     # Visual representation of Ellipse
└── resources/              # (Reserved for icons, etc.)
```

## Architecture

### Model-View Separation

The application follows a strict model-view pattern:

- **Data classes** (`src/data/`): Pure data objects representing geometric primitives. These emit `changed()` signals when modified and know how to serialize themselves to TikZ code.

- **Graphics items** (`src/gui/*item.cpp`): Qt `QGraphicsItem` subclasses that visualize the data. They observe their corresponding data object and update when it changes.

- **Document**: Central container holding all shapes. Manages creation/deletion and generates the complete TikZ output.

### Coordinate System

- **TikZ coordinates**: Standard mathematical (Y-up) coordinate system
- **Screen coordinates**: Qt's Y-down coordinate system
- **Conversion**: `toScreen()` and `fromScreen()` functions in `common.h`
- **Scale**: 40 pixels = 1 TikZ unit (configurable via `GLOBAL_SCALE`)
- **Grid**: Minor grid at 0.25 units, major grid at 1.0 unit

### Tool System

Tools are enumerated in `common.h`:

| Tool | Description |
|------|-------------|
| Select | Click to select, drag to move shapes |
| Point | Click to place named coordinate points |
| Line | Click two points or positions to draw a line |
| Polygon | Click to add vertices, double-click to close |
| Curve | Click to add control points, double-click to finish |
| Rectangle | Drag to draw axis-aligned rectangle |
| Circle | Drag to draw circle (radius from center to cursor) |
| Ellipse | Drag to draw ellipse (separate X/Y radii) |
| Triangle | Drag to draw equilateral triangle |
| RegularPolygon | Drag to draw N-sided regular polygon |

### TikZ Code Generation

Each shape class implements a `tikz()` method returning its TikZ representation:

- **Points**: `\coordinate (name) at (x, y);`
- **Lines**: `\draw (start) -- (end);`
- **Polygons**: `\draw[fill=color] (p1) -- (p2) -- ... -- cycle;`
- **Curves**: `\draw plot[smooth] coordinates {(p1) (p2) ...};`
- **Ellipses**: `\draw (center) ellipse (rx and ry);`

Rounded corners use TikZ's `rounded corners=Xpt` option or per-vertex bezier control points.

## Usage

### Basic Workflow

1. Select a tool from the left toolbar
2. Choose stroke/fill colors from the color palette
3. Draw on the canvas (click, drag, or click-click depending on tool)
4. View generated TikZ in the right panel
5. Click "Compile & Preview" to verify the LaTeX output
6. Export via File → Export TikZ

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| Ctrl+N | New document |
| Ctrl+O | Open document |
| Ctrl+S | Save document |
| Ctrl+Shift+S | Save as |
| Ctrl+E | Export TikZ |
| Ctrl+Z | Undo |
| Ctrl+Shift+Z | Redo |
| Delete | Delete selected |
| Ctrl++ | Zoom in |
| Ctrl+- | Zoom out |
| Ctrl+0 | Reset zoom |

### Right-Click Context Menu

Right-click on any shape to access:

- **Polygons**: Round All Corners, Sharp All Corners, Convert to Circle
- **Ellipses**: Make Circle (equalize radii)
- **All shapes**: Edit Properties, Delete

### Background Images

1. Open the "Background" dock panel
2. Click "Load Image..." to select an image
3. Adjust opacity slider to see grid through image
4. Draw over the image to trace

## Future Improvements

### High Priority

- [ ] **Undo/Redo integration**: Currently stubbed with QUndoStack but commands not implemented
- [ ] **Arrow heads**: Add arrow tip styles for lines and curves
- [ ] **Text labels**: Add text/label tool for annotating diagrams
- [ ] **Snap to points**: When drawing, snap to existing named points
- [ ] **Bezier curve tool**: Direct bezier control point manipulation
- [ ] **Copy/Paste**: Duplicate shapes with Ctrl+C/Ctrl+V

### Medium Priority

- [ ] **Layer system**: Organize shapes into layers with visibility toggles
- [ ] **Style presets**: Save and reuse line width/color/fill combinations
- [ ] **TikZ import**: Parse existing TikZ code to edit visually
- [ ] **Multiple documents**: Tab-based interface for multiple files
- [ ] **Dimension tool**: Draw measurement annotations
- [ ] **Align/distribute**: Align selected shapes horizontally/vertically

### Low Priority / Nice-to-Have

- [ ] **Node shapes**: TikZ node shapes (rectangle, circle, diamond, etc.)
- [ ] **Decorations**: Zigzag, coil, snake line decorations
- [ ] **Patterns**: Fill patterns (hatching, dots, etc.)
- [ ] **Export formats**: SVG, PNG export in addition to TikZ
- [ ] **Theming**: Dark mode support
- [ ] **Plugin system**: Extensible tool/shape system

### Intelligent Drawing Tools

- [ ] **Image-to-TikZ vectorization**: Import a raster image and automatically trace it into editable TikZ shapes using edge detection and contour extraction
- [ ] **Path simplification**: Use Ramer-Douglas-Peucker with a user-controlled epsilon to trade accuracy for readability — fewer bezier control points, shorter decimals
- [ ] **Coordinate snapping on import**: Round all imported coordinates to a configurable grid (e.g. 0.25 units) to eliminate ugly long decimals and produce human-readable output
- [ ] **Primitive recognition**: Detect circles, ellipses, and polylines in traced contours and emit native TikZ primitives (`\draw ... circle`, `ellipse`, `--`) instead of bezier approximations
- [ ] **Color region detection**: Segment color regions from the source image and assign fill colors to closed shapes automatically
- [ ] **Control system diagram import**: Recognize handdrawn block diagram elements (blocks, summing junctions, signal lines, arrowheads) and emit semantic TikZ using the `positioning` and `arrows` libraries with relative node placement rather than raw coordinates

### Code Quality

- [ ] **Unit tests**: Add tests for coordinate transforms, TikZ generation
- [ ] **Fix Qt6 deprecation warnings**: Update `addAction()` calls to new API
- [ ] **Error handling**: Better error messages for compile failures
- [ ] **Settings persistence**: Remember window layout, recent files
- [ ] **Localization**: i18n support for multiple languages

## Design

The UI uses a clean **black and white** theme for clarity:
- All UI elements (menus, toolbars, buttons, panels) are white with black borders
- Selected tools and pressed buttons invert to black with white text
- The color palette retains the full UofSC color set for selecting shape fill/stroke colors

## Shape Color Palette

Built-in UofSC brand colors for shapes (defined in `common.h`):

| Color | Hex |
|-------|-----|
| Garnet | #73000A |
| Black | #000000 |
| White | #FFFFFF |
| Rose | #CC2E40 |
| Atlantic | #466A9F |
| Congaree | #1F414D |
| Horseshoe | #65780B |
| Grass | #CED318 |
| Honeycomb | #A49137 |
| Dark Garnet | #570008 |
| Azalea | #844247 |

## License

MIT License - See LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

Please follow the existing code style and add tests for new functionality.
