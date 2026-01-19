# TikZ Studio - Proposed Features

A curated list of features to enhance TikZ Studio while maintaining simplicity for beginners and power for advanced users.

## Design Philosophy

- **Progressive disclosure**: Basic tools visible, advanced features discoverable
- **Keyboard-optional**: Everything works with mouse, keyboard shortcuts accelerate
- **Non-modal**: Avoid popups and dialogs where possible; use inline editing
- **Familiar patterns**: Follow conventions from PowerPoint, Illustrator, Figma

---

## Phase 1: Essential Editing

Core features that users expect from any drawing application.

### 1.1 Undo/Redo System

**Priority**: Critical
**Effort**: Medium
**Inspiration**: Every application ever

| Requirement | Details |
|-------------|---------|
| Unlimited undo | Stack-based, limited only by memory |
| Keyboard shortcuts | Ctrl+Z (undo), Ctrl+Shift+Z or Ctrl+Y (redo) |
| Per-action granularity | Each shape creation, move, property change is one undo step |
| Undo after save | Preserve undo stack after saving |

**Implementation**: Use Qt's `QUndoStack` and `QUndoCommand` pattern. Create command classes for:
- `CreateShapeCommand`
- `DeleteShapeCommand`
- `MoveCommand`
- `PropertyChangeCommand`
- `GroupCommand` / `UngroupCommand`

**UI**: Optional Edit menu entries. No need for visible undo history panel initially.

---

### 1.2 Copy, Cut, Paste, Duplicate

**Priority**: Critical
**Effort**: Low
**Inspiration**: Universal

| Feature | Shortcut | Behavior |
|---------|----------|----------|
| Copy | Ctrl+C | Copy selection to internal clipboard |
| Cut | Ctrl+X | Copy + delete selection |
| Paste | Ctrl+V | Paste at mouse position or center of view |
| Duplicate | Ctrl+D | Clone selection with small offset (0.25, -0.25) |
| Paste in Place | Ctrl+Shift+V | Paste at exact original position |

**Clipboard format**: Internal object references. Optionally also copy TikZ code to system clipboard for pasting into LaTeX editors.

---

### 1.3 Arrow Heads

**Priority**: High
**Effort**: Low
**Inspiration**: TikZiT, Illustrator

Add arrow head/tail options to lines and curves.

| Arrow Style | TikZ Equivalent |
|-------------|-----------------|
| None | (default) |
| Arrow | `->` or `<-` |
| Double Arrow | `<->` |
| Stealth | `-stealth` |
| Latex | `-latex` |
| Circle | `-o` |
| Bar | `-|` |

**UI**: Add dropdown or icon buttons to context toolbar when line/curve selected.

**Data Model**: Add `ArrowHead` and `ArrowTail` enums to `Line` and `Curve` classes.

---

### 1.4 Select All

**Priority**: High
**Effort**: Minimal
**Shortcut**: Ctrl+A

Select all shapes on canvas. Simple but essential.

---

## Phase 2: Organization & Grouping

Features for managing complex diagrams.

### 2.1 Grouping

**Priority**: High
**Effort**: Medium
**Inspiration**: PowerPoint, Illustrator, Figma

| Feature | Shortcut | Behavior |
|---------|----------|----------|
| Group | Ctrl+G | Combine selected shapes into a group |
| Ungroup | Ctrl+Shift+G | Dissolve group back to individual shapes |
| Enter Group | Double-click | Edit children within group context |
| Exit Group | Escape or click outside | Return to parent context |

**Behavior**:
- Groups move, scale, and rotate as one unit
- Groups can be nested (groups within groups)
- Selection shows single bounding box around group
- Properties panel shows "Mixed" for differing values

**TikZ Output**:
```latex
\begin{scope}[shift={(2,3)}, rotate=45]
  % child shapes here
\end{scope}
```

---

### 2.2 Layers Panel

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Photoshop, Illustrator

A simple layers panel in a dock widget:

```
┌─ Layers ─────────────────┐
│ [+] [-] [↑] [↓]          │
├──────────────────────────┤
│ 👁 🔒 Background          │
│ 👁 🔒 Grid Lines          │
│ 👁    Annotations         │
│ 👁    Main Diagram     ◄──│ (selected)
└──────────────────────────┘
```

| Icon | Meaning |
|------|---------|
| 👁 | Visibility toggle |
| 🔒 | Lock toggle (prevent editing) |

**Features**:
- Drag to reorder layers
- Double-click to rename
- New shapes added to selected layer
- Layers map to TikZ `scope` or `pgfonlayer`

**Keep it simple**: No opacity per layer initially. No blend modes.

---

### 2.3 Z-Ordering

**Priority**: Medium
**Effort**: Low
**Inspiration**: PowerPoint

| Action | Shortcut | Menu |
|--------|----------|------|
| Bring to Front | Ctrl+Shift+] | Arrange → Bring to Front |
| Bring Forward | Ctrl+] | Arrange → Bring Forward |
| Send Backward | Ctrl+[ | Arrange → Send Backward |
| Send to Back | Ctrl+Shift+[ | Arrange → Send to Back |

Changes draw order within layer. Affects TikZ output order.

---

## Phase 3: Alignment & Distribution

Precision layout tools.

### 3.1 Align Tools

**Priority**: High
**Effort**: Low
**Inspiration**: PowerPoint, Figma

Align selected shapes relative to selection bounding box:

| Alignment | Icon | Behavior |
|-----------|------|----------|
| Align Left | `[│· · ]` | Left edges aligned |
| Align Center H | `[ · │ · ]` | Horizontal centers aligned |
| Align Right | `[ · ·│]` | Right edges aligned |
| Align Top | Top edge aligned |
| Align Center V | Vertical centers aligned |
| Align Bottom | Bottom edges aligned |

**UI**: Toolbar buttons or Arrange menu. Only enabled when 2+ shapes selected.

**Option**: Align to selection vs. align to canvas. Default to selection.

---

### 3.2 Distribute Tools

**Priority**: Medium
**Effort**: Low
**Inspiration**: PowerPoint, Figma

Evenly space 3+ selected shapes:

| Distribution | Behavior |
|--------------|----------|
| Distribute Horizontally | Equal horizontal spacing between shapes |
| Distribute Vertically | Equal vertical spacing between shapes |

**UI**: Same location as align tools. Only enabled when 3+ shapes selected.

---

### 3.3 Smart Guides

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Figma, Keynote

While dragging shapes, show guides when:
- Edges align with other shapes
- Centers align with other shapes
- Equal spacing is achieved

**Visual**: Thin magenta lines (standard guide color)

**Behavior**: Optional snap to guides. Toggle with View menu or Alt key to temporarily disable.

---

### 3.4 Manual Guides

**Priority**: Low
**Effort**: Medium
**Inspiration**: Photoshop, Illustrator

Drag from rulers to create guide lines:
- Horizontal guides from top ruler
- Vertical guides from left ruler
- Drag guide off canvas to delete
- Guides are document-specific, not exported to TikZ

---

## Phase 4: Transform Tools

### 4.1 Transform Handles

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Illustrator, PowerPoint

When shape selected, show:
- **Corner handles**: Drag to scale proportionally
- **Edge handles**: Drag to scale in one dimension
- **Rotation handle**: Circular handle above shape for rotation

**Modifier keys**:
- Shift: Constrain proportions
- Alt: Scale from center
- Ctrl: Rotate in 15° increments

---

### 4.2 Flip Operations

**Priority**: High
**Effort**: Minimal
**Inspiration**: Every drawing app

| Action | Shortcut | Behavior |
|--------|----------|----------|
| Flip Horizontal | H (when selected) | Mirror across vertical axis |
| Flip Vertical | V (when selected) | Mirror across horizontal axis |

**Implementation**: Negate x or y coordinates relative to selection center.

---

### 4.3 Rotate 90°

**Priority**: Medium
**Effort**: Minimal

| Action | Shortcut |
|--------|----------|
| Rotate 90° CW | Ctrl+R |
| Rotate 90° CCW | Ctrl+Shift+R |

Complements free rotation with precise increments.

---

### 4.4 Direct Selection Tool

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Illustrator

A secondary selection mode (A key) that selects individual vertices:
- Click vertex to select it
- Drag vertex to move it
- Works on polygons, curves, lines
- Shift+click to add to selection

**Visual**: Show vertices as small squares. Selected vertices filled, others hollow.

---

## Phase 5: Color & Style Tools

### 5.1 Eyedropper Tool

**Priority**: Medium
**Effort**: Low
**Inspiration**: Photoshop, Illustrator

| Shortcut | Behavior |
|----------|----------|
| I | Activate eyedropper tool |
| Alt+Click (any tool) | Sample color under cursor |

**Behavior**:
- Click on shape: Sample its fill color
- Shift+Click: Sample its stroke color
- Set as active color in palette
- Return to previous tool after sampling

---

### 5.2 Format Painter

**Priority**: Medium
**Effort**: Low
**Inspiration**: PowerPoint, Word

| Shortcut | Behavior |
|----------|----------|
| Ctrl+Shift+C | Copy formatting of selected shape |
| Ctrl+Shift+V | Apply formatting to selected shapes |

**Copied properties**:
- Fill color and pattern
- Stroke color, width, style
- Line cap and join
- Opacity

---

### 5.3 Style Presets

**Priority**: Medium
**Effort**: Medium
**Inspiration**: TikZiT, Figma

Save and reuse style combinations:

```
┌─ Styles ─────────────────┐
│ [+] [−]                  │
├──────────────────────────┤
│ ■ Default                │
│ ■ Highlighted            │
│ ■ Dashed Outline         │
│ ■ Filled Blue            │
│ ■ Arrow Line             │
└──────────────────────────┘
```

**Features**:
- Click to apply style to selection
- Right-click → Update Style from Selection
- Styles saved with document
- Optional: Global style library

**Keep simple**: No live linking (changing style doesn't update existing shapes). Just one-time application.

---

### 5.4 Gradient Fills

**Priority**: Low
**Effort**: High
**Inspiration**: Illustrator

Support TikZ shading:
- Linear gradient (two-color, angle)
- Radial gradient (center, edge colors)

**UI**: Gradient editor in properties panel when fill selected.

**TikZ Output**: Use `\shade` command with `left color`, `right color`, etc.

**Note**: TikZ gradient support is limited. May not be worth the complexity.

---

## Phase 6: Advanced Drawing

### 6.1 Bezier Curve Tool

**Priority**: Medium
**Effort**: High
**Inspiration**: Illustrator, Inkscape

A true pen tool for Bezier curves:
- Click to add corner point
- Click+drag to add smooth point with handles
- Hold Alt to break handle symmetry

**Current state**: TikZ Studio has smooth curves through points. This would add direct control point manipulation.

**Complexity warning**: Bezier tools have a learning curve. Consider making this an "advanced" tool that's hidden by default.

---

### 6.2 Boolean Operations

**Priority**: Low
**Effort**: High
**Inspiration**: Illustrator, Figma

Combine shapes:

| Operation | Result |
|-----------|--------|
| Union | Combined outline of all shapes |
| Subtract | First shape minus overlapping areas |
| Intersect | Only overlapping areas |
| Exclude | Non-overlapping areas only |

**Note**: Complex to implement correctly. May require computational geometry library. Lower priority.

---

### 6.3 Path Offset

**Priority**: Low
**Effort**: Medium
**Inspiration**: Illustrator

Create a parallel path at specified distance:
- Offset outside (expand)
- Offset inside (contract)

Useful for creating outlines and borders.

---

## Phase 7: Interoperability

### 7.1 TikZ Import

**Priority**: High
**Effort**: High
**Inspiration**: TikZiT

Parse existing TikZ code and convert to editable shapes:
- Load `.tikz` files
- Paste TikZ code to import
- Round-trip editing (import → edit → export)

**Scope**: Start with basic commands:
- `\draw` paths
- `\fill` and `\filldraw`
- `\node` (as labeled points)
- `\coordinate`
- Basic options (color, line width)

**Parser approach**: Could use regex for simple cases, or Bison/Flex for full TikZ grammar (complex).

---

### 7.2 SVG Export

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Inkscape

Export diagram as SVG for use in web, presentations, etc.

**Implementation**: Qt has SVG export via `QSvgGenerator`. Should be straightforward.

---

### 7.3 PNG/PDF Export

**Priority**: Medium
**Effort**: Low (for PNG), Medium (for direct PDF)

| Format | Method |
|--------|--------|
| PNG | Render QGraphicsScene to QImage |
| PDF (via LaTeX) | Already implemented via compile |
| PDF (direct) | QPrinter or QPdfWriter |

---

### 7.4 Clipboard TikZ

**Priority**: Medium
**Effort**: Minimal

When copying shapes, also place TikZ code in system clipboard. Allows pasting directly into LaTeX editor.

---

## Phase 8: Quality of Life

### 8.1 Measurement/Dimension Tool

**Priority**: Low
**Effort**: Medium
**Inspiration**: CAD software, Figma

Show distances between objects:
- Hover between shapes to see spacing
- Click to place permanent dimension annotation
- Dimension lines export to TikZ using `\draw` with arrows and labels

---

### 8.2 Grid Customization

**Priority**: Low
**Effort**: Low

Currently: Fixed 0.25 minor / 1.0 major grid.

Add preferences:
- Custom grid sizes
- Toggle minor grid independently
- Grid color options

---

### 8.3 Canvas Resize

**Priority**: Low
**Effort**: Low
**Inspiration**: Photoshop

Adjust the TikZ picture boundaries:
- Fit to content
- Add margin
- Set explicit size
- Affects `\begin{tikzpicture}[scale=...]` output

---

### 8.4 Recent Files

**Priority**: Low
**Effort**: Low

Track recently opened files in File menu. Standard application feature.

---

### 8.5 Preferences Dialog

**Priority**: Low
**Effort**: Medium

Centralized settings:
- Default colors
- Grid settings
- Keyboard shortcut customization
- Auto-save interval
- LaTeX compiler path

---

### 8.6 Keyboard Shortcut Cheat Sheet

**Priority**: Low
**Effort**: Minimal

Help → Keyboard Shortcuts opens overlay or dialog showing all shortcuts.

---

## Phase 9: Labels & Text

### 9.1 Text Tool

**Priority**: High
**Effort**: Medium
**Inspiration**: TikZiT, Illustrator

Add text labels to diagrams:
- Click to place text
- Inline editing (type directly on canvas)
- Basic formatting (bold, italic via markdown or LaTeX)

**TikZ Output**:
```latex
\node at (2,3) {Label text};
\node[above] at (1,1) {$x^2$};  % LaTeX math supported
```

**Keep simple**: No rich text editor. Just single-line or simple multi-line text.

---

### 9.2 Label Positions

**Priority**: Medium
**Effort**: Low

For point labels, support positioning:
- Above, below, left, right
- Above left, above right, below left, below right

Maps to TikZ node anchors.

---

### 9.3 Shape Labels

**Priority**: Medium
**Effort**: Low

Add centered label inside shapes (polygons, ellipses, rectangles):
- Optional label field in properties
- Rendered at shape centroid

---

## Feature Comparison Matrix

| Feature | Beginner Friendly | Power User Value | Effort | Priority |
|---------|-------------------|------------------|--------|----------|
| Undo/Redo | ★★★ | ★★★ | Medium | Critical |
| Copy/Paste | ★★★ | ★★★ | Low | Critical |
| Arrow Heads | ★★★ | ★★★ | Low | High |
| Grouping | ★★☆ | ★★★ | Medium | High |
| Align/Distribute | ★★☆ | ★★★ | Low | High |
| Text Tool | ★★★ | ★★★ | Medium | High |
| Flip H/V | ★★★ | ★★☆ | Minimal | High |
| Z-Ordering | ★★☆ | ★★★ | Low | Medium |
| Eyedropper | ★★☆ | ★★★ | Low | Medium |
| Style Presets | ★★☆ | ★★★ | Medium | Medium |
| Layers | ★☆☆ | ★★★ | Medium | Medium |
| Direct Selection | ★☆☆ | ★★★ | Medium | Medium |
| TikZ Import | ★☆☆ | ★★★ | High | Medium |
| Smart Guides | ★★★ | ★★☆ | Medium | Medium |
| Bezier Tool | ★☆☆ | ★★★ | High | Low |
| Boolean Ops | ★☆☆ | ★★☆ | High | Low |
| Gradients | ★☆☆ | ★★☆ | High | Low |

---

## Implementation Roadmap

### Version 1.1 - Essential Polish
- [ ] Undo/Redo system
- [ ] Copy/Cut/Paste/Duplicate
- [ ] Arrow heads on lines and curves
- [ ] Select All
- [ ] Flip Horizontal/Vertical

### Version 1.2 - Organization
- [ ] Grouping and ungrouping
- [ ] Z-ordering (front/back)
- [ ] Basic align tools (left, center, right, top, middle, bottom)

### Version 1.3 - Text & Style
- [ ] Text/label tool
- [ ] Eyedropper
- [ ] Format painter
- [ ] Style presets panel

### Version 1.4 - Precision
- [ ] Distribute tools
- [ ] Smart guides
- [ ] Transform handles (scale/rotate)
- [ ] Direct selection tool

### Version 1.5 - Interoperability
- [ ] TikZ import (basic)
- [ ] SVG export
- [ ] PNG export
- [ ] Clipboard TikZ code

### Version 2.0 - Advanced
- [ ] Layers panel
- [ ] Bezier curve tool
- [ ] Boolean operations
- [ ] Gradient fills

---

## UI Placement Suggestions

### Toolbar (Top)
```
[New][Open][Save] | [Undo][Redo] | [Cut][Copy][Paste] | [Group][Ungroup] | [Align ▼][Distribute ▼] | [Flip H][Flip V]
```

### Tool Palette (Left)
```
[Select]        - V
[Direct Select] - A
[Point]         - P
[Line]          - L
[Polygon]       - G
[Curve]         - C
[Rectangle]     - R
[Ellipse]       - E
[Text]          - T
[Eyedropper]    - I
```

### Context Toolbar (Dynamic)
Changes based on selection:
- **Line selected**: Width, color, style, arrow head, arrow tail
- **Shape selected**: Fill, stroke, opacity, corner radius
- **Text selected**: Font size, alignment
- **Multiple selected**: Align, distribute, group

### Dock Panels (Right)
- Properties (always visible)
- Styles (collapsible)
- Layers (collapsible)
- TikZ Code (movable)

---

## Notes for Implementation

### Keep It Simple
- Prefer toolbar buttons over nested menus
- Use icons with tooltips, not just text
- Show keyboard shortcuts in tooltips
- Avoid modal dialogs; use inline editing

### Discoverability
- Right-click context menus for common actions
- Keyboard shortcuts listed in menus
- Help → Keyboard Shortcuts reference

### Performance
- Lazy loading for panels
- Efficient undo stack (don't store full document copies)
- Background compilation for preview

### Accessibility
- Keyboard navigation for all tools
- High contrast icons
- Screen reader labels where possible

---

## References

- [TikZiT](https://tikzit.github.io/) - Node-graph TikZ editor
- [Ipe](http://ipe.otfried.org/) - LaTeX figure editor
- [Figma](https://figma.com/) - Modern vector design
- [PowerPoint](https://www.microsoft.com/en-us/microsoft-365/powerpoint) - Presentation software
- [Illustrator](https://www.adobe.com/products/illustrator.html) - Professional vector graphics

---

*Last updated: January 2026*
