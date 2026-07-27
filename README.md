# Toolpath Studio

Desktop CAD/CAM prototype written in C++20.

The project is designed as a portfolio-grade learning path toward CAD/CAM software development. The first version generates a 2.5D rectangular profile toolpath and exports GRBL-compatible G-code.

## Stack

- C++20
- CMake
- Qt6
- OpenGL
- OpenCascade
- Eigen
- GoogleTest

## macOS Requirements

Install Xcode Command Line Tools:

```bash
xcode-select --install
```

Install dependencies with Homebrew:

```bash
brew install cmake ninja eigen qt opencascade googletest
```

Expose Qt to CMake:

```bash
export CMAKE_PREFIX_PATH="$(brew --prefix qt):$CMAKE_PREFIX_PATH"
```

## Configure

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

If you want to build only the CLI and core while setting up GUI dependencies:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DTOOLPATH_BUILD_APP=OFF \
  -DTOOLPATH_BUILD_CAD=OFF \
  -DTOOLPATH_BUILD_TESTS=OFF
```

## Build

```bash
cmake --build build
```

## Run CLI

```bash
./build/src/cli/toolpath_cli
```

This writes:

```text
samples/rectangle_profile.nc
```

## Run Desktop App

```bash
./build/src/app/toolpath_app.app/Contents/MacOS/toolpath_app
```

## First UI Workflow

1. Keep the `Rectangle` sketch tool selected.
2. Click and drag in the viewport to draw a closed rectangular profile.
3. Adjust tool diameter, depth, stepdown, and feed in the left panel.
4. Click `Generate`.
5. Inspect the G-code preview.
6. Click `Export G-code` to save an `.nc`, `.gcode`, or `.tap` file.

The viewport uses millimeters and snaps rectangle corners to a 1 mm grid.
White lines are sketch geometry, green lines are cutting moves, blue lines are rapid moves, and yellow lines are plunge moves.

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

## Roadmap

1. Rectangle profile G-code generation.
2. Qt6/OpenGL toolpath preview.
3. STEP import with OpenCascade.
4. Face/edge selection for CAM operations.
5. Pocketing, drilling, tabs, lead-in/lead-out.
6. CNC validation with simulator, air cut, then soft material.
