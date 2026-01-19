# TikZ Studio - Proposed Features

## Product Vision

TikZ Studio is developed in phases, each potentially a standalone product:

| Phase | Product | Purpose |
|-------|---------|---------|
| **Part 1** | TikZ Studio (General Diagrams) | Draw geometric shapes, export TikZ code |
| **Part 2** | TikZ Studio (Diagram Generator) | Flowcharts, block diagrams, labeled arrows |
| **Part 3** | TikZ Studio (Electrical) | Circuit diagrams with component library |

**This document focuses on Part 1 features only.**

Parts 2 and 3 will be forked from Part 1 when ready, adding specialized tools.

---

## Design Philosophy

- **Progressive disclosure**: Basic tools visible, advanced features discoverable
- **Keyboard-optional**: Everything works with mouse, keyboard shortcuts accelerate
- **Non-modal**: Avoid popups; use inline editing
- **Familiar patterns**: Follow conventions from PowerPoint, Illustrator, Figma
- **Standalone scope**: Part 1 works completely on its own for general drawing

---

## Part 1 Features (General Diagrams)

### Tier 1: Critical (Must Have)

These are blocking issues or expected baseline functionality.

---

#### 1.1 Undo/Redo System

**Priority**: Critical
**Effort**: Medium
**Status**: Currently stubbed

| Requirement | Details |
|-------------|---------|
| Unlimited undo | Stack-based, limited only by memory |
| Keyboard shortcuts | Ctrl+Z (undo), Ctrl+Shift+Z or Ctrl+Y (redo) |
| Per-action granularity | Each shape creation, move, property change is one undo step |
| Undo after save | Preserve undo stack after saving |

**Implementation**: Use Qt's `QUndoStack` and `QUndoCommand` pattern.

**Commands to implement**:
- `CreateShapeCommand` (point, line, polygon, curve, ellipse)
- `DeleteShapeCommand`
- `MoveCommand` (single or multi-shape)
- `PropertyChangeCommand` (color, line width, etc.)
- `TransformCommand` (rotate, scale, flip)
- `GroupCommand` / `UngroupCommand`

---

#### 1.2 Copy, Cut, Paste, Duplicate

**Priority**: Critical
**Effort**: Low

| Feature | Shortcut | Behavior |
|---------|----------|----------|
| Copy | Ctrl+C | Copy selection to internal clipboard |
| Cut | Ctrl+X | Copy + delete selection |
| Paste | Ctrl+V | Paste at mouse position or center of view |
| Duplicate | Ctrl+D | Clone selection with small offset (0.25, -0.25) |
| Paste in Place | Ctrl+Shift+V | Paste at exact original position |

**Bonus**: Also copy TikZ code to system clipboard for pasting into LaTeX editors.

---

#### 1.3 Select All

**Priority**: Critical
**Effort**: Minimal
**Shortcut**: Ctrl+A

---

#### 1.4 Delete Selected

**Priority**: Critical
**Effort**: Minimal
**Shortcut**: Delete or Backspace

Confirm this works reliably. Add to Edit menu.

---

### Tier 2: High Priority (Core Experience)

Features that significantly improve usability.

---

#### 2.1 Grouping

**Priority**: High
**Effort**: Medium
**Inspiration**: PowerPoint, Illustrator

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

**TikZ Output**:
```latex
\begin{scope}[shift={(2,3)}, rotate=45, scale=0.8]
  % child shapes here
\end{scope}
```

---

#### 2.2 Align Tools

**Priority**: High
**Effort**: Low
**Inspiration**: PowerPoint, Figma

Align selected shapes relative to selection bounding box:

| Alignment | Behavior |
|-----------|----------|
| Align Left | Left edges match leftmost shape |
| Align Center H | Horizontal centers aligned |
| Align Right | Right edges match rightmost shape |
| Align Top | Top edges match topmost shape |
| Align Center V | Vertical centers aligned |
| Align Bottom | Bottom edges match bottommost shape |

**UI**: Toolbar dropdown or Arrange menu. Only enabled when 2+ shapes selected.

---

#### 2.3 Distribute Tools

**Priority**: High
**Effort**: Low
**Inspiration**: PowerPoint, Figma

Evenly space 3+ selected shapes:

| Distribution | Behavior |
|--------------|----------|
| Distribute Horizontally | Equal horizontal gaps between shapes |
| Distribute Vertically | Equal vertical gaps between shapes |

Only enabled when 3+ shapes selected.

---

#### 2.4 Flip Operations

**Priority**: High
**Effort**: Minimal

| Action | Shortcut | Behavior |
|--------|----------|----------|
| Flip Horizontal | H (with selection) | Mirror across vertical axis |
| Flip Vertical | V (with selection) | Mirror across horizontal axis |

**Implementation**: Negate x or y coordinates relative to selection center.

---

#### 2.5 Z-Ordering

**Priority**: High
**Effort**: Low
**Inspiration**: PowerPoint

| Action | Shortcut | Menu |
|--------|----------|------|
| Bring to Front | Ctrl+Shift+] | Arrange → Bring to Front |
| Bring Forward | Ctrl+] | Arrange → Bring Forward |
| Send Backward | Ctrl+[ | Arrange → Send Backward |
| Send to Back | Ctrl+Shift+[ | Arrange → Send to Back |

Affects draw order and TikZ output order.

---

### Tier 3: Medium Priority (Polish)

Features that improve workflow but aren't blocking.

---

#### 3.1 Eyedropper Tool

**Priority**: Medium
**Effort**: Low
**Inspiration**: Photoshop

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

#### 3.2 Format Painter

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
- Corner radius

---

#### 3.3 Style Presets

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
│ ■ Thick Red              │
└──────────────────────────┘
```

- Click to apply style to selection
- Right-click → Update Style from Selection
- Styles saved with document

---

#### 3.4 Transform Handles

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

#### 3.5 Rotate 90°

**Priority**: Medium
**Effort**: Minimal

| Action | Shortcut |
|--------|----------|
| Rotate 90° CW | Ctrl+R |
| Rotate 90° CCW | Ctrl+Shift+R |

---

#### 3.6 Direct Selection Tool

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Illustrator

Secondary selection mode (A key) that selects individual vertices:
- Click vertex to select it
- Drag vertex to move it
- Works on polygons, curves, lines
- Shift+click to add to selection

**Visual**: Vertices as small squares. Selected = filled, unselected = hollow.

---

#### 3.7 Smart Guides

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Figma, Keynote

While dragging shapes, show guides when:
- Edges align with other shapes
- Centers align with other shapes
- Equal spacing achieved

**Visual**: Thin magenta/cyan lines. Optional snap.

---

#### 3.8 Layers Panel

**Priority**: Medium
**Effort**: Medium
**Inspiration**: Photoshop

Simple layers panel:

```
┌─ Layers ─────────────────┐
│ 👁 🔒 Layer 3             │
│ 👁    Layer 2          ◄──│
│ 👁 🔒 Layer 1             │
└──────────────────────────┘
```

- Visibility toggle (eye icon)
- Lock toggle (prevent editing)
- Drag to reorder
- Double-click to rename

---

### Tier 4: Lower Priority (Nice to Have)

Features that add value but can wait.

---

#### 4.1 SVG Export

**Priority**: Lower
**Effort**: Medium

Export diagram as SVG for web, presentations.

**Implementation**: Qt's `QSvgGenerator`.

---

#### 4.2 PNG Export

**Priority**: Lower
**Effort**: Low

Render canvas to PNG at configurable resolution.

**Implementation**: Render `QGraphicsScene` to `QImage`.

---

#### 4.3 Grid Customization

**Priority**: Lower
**Effort**: Low

Currently: Fixed 0.25 minor / 1.0 major grid.

Add preferences:
- Custom grid sizes
- Toggle minor grid independently
- Grid color options

---

#### 4.4 Canvas/Bounds Resize

**Priority**: Lower
**Effort**: Low

Adjust the TikZ picture boundaries:
- Fit to content
- Add margin
- Set explicit dimensions

---

#### 4.5 Recent Files

**Priority**: Lower
**Effort**: Low

Track recently opened files in File menu.

---

#### 4.6 Preferences Dialog

**Priority**: Lower
**Effort**: Medium

Centralized settings:
- Default colors
- Grid settings
- Auto-save interval
- LaTeX compiler path

---

#### 4.7 Keyboard Shortcut Reference

**Priority**: Lower
**Effort**: Minimal

Help → Keyboard Shortcuts shows all shortcuts.

---

#### 4.8 Manual Guide Lines

**Priority**: Lower
**Effort**: Medium

Drag from rulers to create alignment guides.

---

#### 4.9 Bezier Handle Editing

**Priority**: Lower
**Effort**: High

Direct manipulation of Bezier control points on curves.

Complex to implement. Current smooth curves may be sufficient for Part 1.

---

#### 4.10 Boolean Operations

**Priority**: Lower
**Effort**: High

Union, subtract, intersect shapes.

Requires computational geometry. Defer to later version.

---

## Features Deferred to Part 2 (Diagram Generator)

These features are **out of scope** for Part 1. They will be added when Part 2 is forked.

| Feature | Reason for Deferral |
|---------|---------------------|
| Arrow heads/tails | Diagram-specific (flowcharts, etc.) |
| Node anchors (north/south/east/west) | Connection logic for diagrams |
| Smart connectors | Auto-routing arrows between shapes |
| Text tool / Labels | Text in boxes is diagram-specific |
| Label on line/arrow | Diagram annotation feature |
| Shape-to-shape snapping | Connection behavior |
| Flowchart shapes | Specialized shape library |
| Auto-layout | Automatic arrangement algorithms |
| TikZ `\node` generation | Part 1 uses `\draw`, Part 2 uses `\node` |

---

## Features Deferred to Part 3 (Electrical)

| Feature | Description |
|---------|-------------|
| Component library | Resistors, capacitors, inductors, etc. |
| Circuit symbols | Standard electrical symbols |
| Wire tool | Orthogonal routing for circuits |
| Net labels | Naming wires/connections |
| SPICE export | Simulation integration |
| Component values | Resistance, capacitance labels |

---

## Part 1 Feature Summary

### Must Ship (Blockers)

| Feature | Effort | Notes |
|---------|--------|-------|
| Undo/Redo | Medium | Users will rage-quit without this |
| Copy/Cut/Paste | Low | Basic expectation |
| Duplicate (Ctrl+D) | Minimal | Very useful for patterns |
| Select All | Minimal | |
| Delete | Minimal | Confirm working |

### Should Ship (Core Polish)

| Feature | Effort | Notes |
|---------|--------|-------|
| Grouping | Medium | Essential for complex diagrams |
| Align tools | Low | Quick win, high value |
| Distribute tools | Low | Pairs with align |
| Flip H/V | Minimal | Very easy to add |
| Z-ordering | Low | Front/back control |

### Nice to Ship (Quality)

| Feature | Effort | Notes |
|---------|--------|-------|
| Eyedropper | Low | Quick color sampling |
| Format painter | Low | Style copying |
| Style presets | Medium | Reusable styles |
| Transform handles | Medium | Visual scale/rotate |
| Rotate 90° | Minimal | Precise rotation |
| Direct selection | Medium | Vertex editing |
| Smart guides | Medium | Alignment hints |
| Layers | Medium | Organization |

### Can Wait (Future 1.x)

| Feature | Effort |
|---------|--------|
| SVG export | Medium |
| PNG export | Low |
| Grid customization | Low |
| Canvas resize | Low |
| Recent files | Low |
| Preferences dialog | Medium |
| Guide lines | Medium |
| Bezier handles | High |
| Boolean ops | High |

---

## Suggested Release Plan

### Version 1.1 - "It Actually Works"
- [x] Current feature set
- [ ] Undo/Redo system
- [ ] Copy/Cut/Paste/Duplicate
- [ ] Select All
- [ ] Flip H/V

### Version 1.2 - "Organization"
- [ ] Grouping / Ungrouping
- [ ] Z-ordering (front/back)
- [ ] Align tools
- [ ] Distribute tools

### Version 1.3 - "Polish"
- [ ] Eyedropper
- [ ] Format painter
- [ ] Style presets
- [ ] Rotate 90° CW/CCW

### Version 1.4 - "Precision"
- [ ] Transform handles (scale/rotate)
- [ ] Direct selection tool
- [ ] Smart guides

### Version 1.5 - "Pro"
- [ ] Layers panel
- [ ] SVG/PNG export
- [ ] Preferences dialog
- [ ] Recent files

### Version 2.0 - Fork for Part 2
- [ ] Arrow heads/tails
- [ ] Text tool
- [ ] Node anchors
- [ ] Smart connectors
- [ ] Flowchart shapes

---

## UI Layout for Part 1

### Main Toolbar
```
[New][Open][Save] | [Undo][Redo] | [Cut][Copy][Paste][Duplicate] | [Group][Ungroup] | [Align ▼][Distribute ▼] | [Flip H][Flip V] | [↑Front][↓Back]
```

### Tool Palette (Left Side)
```
[Select]        V
[Direct Select] A    (Tier 3)
───────────────────
[Point]         P
[Line]          L
[Polygon]       G
[Curve]         C
───────────────────
[Rectangle]     R
[Circle]        O
[Ellipse]       E
[Triangle]      T
[N-gon]         N
───────────────────
[Eyedropper]    I    (Tier 3)
```

### Context Toolbar (Top, Dynamic)
Shows relevant controls for current selection:
- **No selection**: Grid toggle, snap toggle, zoom
- **Shape selected**: Fill, stroke, line width, style, opacity, corner radius
- **Multiple selected**: Align, distribute, group

### Right Dock
- Properties Panel (always)
- Styles Panel (collapsible, Tier 3)
- Layers Panel (collapsible, Tier 3)
- TikZ Code (movable)

---

## Keyboard Shortcuts (Part 1)

### File
| Action | Shortcut |
|--------|----------|
| New | Ctrl+N |
| Open | Ctrl+O |
| Save | Ctrl+S |
| Save As | Ctrl+Shift+S |
| Export TikZ | Ctrl+E |

### Edit
| Action | Shortcut |
|--------|----------|
| Undo | Ctrl+Z |
| Redo | Ctrl+Shift+Z |
| Cut | Ctrl+X |
| Copy | Ctrl+C |
| Paste | Ctrl+V |
| Paste in Place | Ctrl+Shift+V |
| Duplicate | Ctrl+D |
| Select All | Ctrl+A |
| Delete | Delete |

### Arrange
| Action | Shortcut |
|--------|----------|
| Group | Ctrl+G |
| Ungroup | Ctrl+Shift+G |
| Bring to Front | Ctrl+Shift+] |
| Bring Forward | Ctrl+] |
| Send Backward | Ctrl+[ |
| Send to Back | Ctrl+Shift+[ |
| Flip Horizontal | H |
| Flip Vertical | V |
| Rotate 90° CW | Ctrl+R |
| Rotate 90° CCW | Ctrl+Shift+R |

### View
| Action | Shortcut |
|--------|----------|
| Zoom In | Ctrl++ |
| Zoom Out | Ctrl+- |
| Zoom to Fit | Ctrl+0 |
| Toggle Grid | G |

### Tools
| Tool | Shortcut |
|------|----------|
| Select | V |
| Direct Select | A |
| Point | P |
| Line | L |
| Polygon | G |
| Curve | C |
| Rectangle | R |
| Circle | O |
| Ellipse | E |
| Eyedropper | I |

---

*Document reflects Part 1 scope only. Last updated: January 2026*
