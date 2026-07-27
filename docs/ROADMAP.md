# Roadmap

## Project Goal

Toolpath Studio is a desktop CAD/CAM prototype for small CNC milling machines.
The application should import or create simple geometry, generate 2.5D milling
toolpaths, preview them in a graphical viewport, and export machine-ready G-code.

The project is intentionally split into independent modules so it can grow from
a small portfolio demo into a realistic CAD/CAM software architecture.

## Technology Stack

### Language and Build

- C++20 for the full application and computational core.
- CMake as the build system.
- Ninja as the recommended generator on macOS.
- GoogleTest for unit tests.

### Desktop Application

- Qt6 Widgets for the native desktop UI.
- Qt6 OpenGLWidgets for embedding an OpenGL viewport.
- OpenGL for rendering geometry, stock, toolpaths, and later simulation layers.

### CAD and Geometry

- OpenCascade for STEP import, B-Rep topology, solids, faces, edges, and future CAD operations.
- Eigen for vectors, matrices, transforms, and numerical helpers.

### CAM and CNC

- Custom C++ CAM core for profile, pocketing, drilling, and toolpath generation.
- GRBL-compatible G-code output for small desktop CNC machines.
- Future postprocessor layer for GRBL, LinuxCNC, Mach3, and custom dialects.

## High-Level Architecture

```text
ToolpathStudio/
├── include/toolpath/
│   ├── core/              Public CAM core headers
│   └── cad/               Public CAD integration headers
├── src/
│   ├── core/              Geometry, CAM operations, toolpaths, G-code
│   ├── cad/               OpenCascade STEP/B-Rep integration
│   ├── cli/               Command line generator for fast testing
│   └── app/               Qt6 desktop application and OpenGL viewport
├── tests/                 GoogleTest test suite
├── samples/               Example generated G-code and future STEP files
└── docs/                  Architecture, roadmap, machining notes
```

## Module Responsibilities

### `toolpath_core`

Core computational library. It must not depend on Qt or UI code.

- 2D/3D geometry primitives.
- Tool and cutting parameter models.
- CAM operations such as profile, pocket, drilling, facing, and adaptive roughing later.
- Toolpath move model: rapid, plunge, cut, arc, retract.
- G-code generation.
- Validation logic for unsafe or invalid machining parameters.

### `toolpath_cad`

OpenCascade-based CAD integration layer.

- STEP import.
- Shape traversal.
- Face and edge extraction.
- Planar contour extraction.
- Future conversion from CAD topology into CAM-ready 2D profiles.

### `toolpath_app`

Qt6 desktop user interface.

- Main window, menus, toolbars, and operation panels.
- Parameter forms for tools, feeds, speeds, depths, and stock.
- OpenGL viewport for model and toolpath preview.
- G-code preview and export workflow.
- Future selection tools for faces, edges, operations, and coordinate systems.

### `toolpath_cli`

Small command line executable for development and regression testing.

- Generates sample toolpaths without opening the UI.
- Writes example `.nc` files into `samples/`.
- Useful for CI, debugging, and demonstrating the CAM core independently.

### `toolpath_tests`

GoogleTest-based test target.

- CAM operation behavior.
- Geometry edge cases.
- G-code formatting.
- Tool compensation correctness.
- Future OpenCascade contour extraction tests.

## Runtime Data Flow

```text
CAD geometry / manual parameters
        ↓
Geometry extraction or primitive creation
        ↓
CAM operation input
        ↓
Toolpath generation
        ↓
Viewport preview
        ↓
G-code writer
        ↓
NC simulator / CNC controller
```

## Initial CMake Targets

- `toolpath_core` - static library with CAM logic.
- `toolpath_cad` - static library with OpenCascade integration.
- `toolpath_cli` - executable for generating sample G-code.
- `toolpath_app` - Qt6 desktop executable.
- `toolpath_tests` - GoogleTest executable.

## Design Principles

- Keep CAM algorithms independent from the UI.
- Keep OpenCascade isolated behind a CAD module.
- Make every CAM operation testable without launching the desktop app.
- Prefer explicit units in names, for example `diameterMm`, `feedRateMmMin`, `safeZMm`.
- Build small, validated CAM operations before adding complex UI.
- Treat generated G-code as a safety-critical output and test it carefully.

## Milestone 1: CAM Core

- Basic 2D geometry types.
- Rectangle profile operation.
- Tool radius compensation.
- Multi-pass Z cutting.
- GRBL-compatible G-code output.
- GoogleTest coverage for operation validation.

## Milestone 2: Qt6 Desktop UI

- Parameter panel for width, height, tool diameter, depth, stepdown, and feed.
- OpenGL viewport for rapid, plunge, and cutting moves.
- G-code preview.
- G-code export.

## Milestone 3: OpenCascade CAD Layer

- STEP import.
- Shape viewer.
- Face and edge extraction.
- 2D contour extraction from selected planar faces.

## Milestone 4: Real CNC Workflow

- NC viewer validation.
- Air cut.
- MDF or foam machining.
- Portfolio screenshots, videos, and notes.

## Milestone 5: CAD-Driven CAM

- Import a STEP file.
- Display the B-Rep model in the viewport.
- Select a planar face.
- Extract outer and inner wire contours.
- Convert selected contours into profile or pocket operations.

## Milestone 6: Advanced Toolpaths

- 2D pocketing with stepover.
- Drilling cycles.
- Lead-in and lead-out moves.
- Holding tabs.
- Rest machining experiments.
- Stock boundary handling.

## Milestone 7: Simulation and Safety

- Backplot toolpath simulation.
- Bounding box checks.
- Rapid move collision warnings.
- Basic material removal preview.
- G-code statistics: machining time, cutting length, rapid length, max depth.

## Milestone 8: Portfolio Polish

- High-quality README with screenshots.
- Architecture diagram.
- Example STEP models.
- Example G-code files.
- NC viewer validation screenshots.
- CNC machining photos or video.
- Short case study explaining algorithms, tradeoffs, and limitations.
