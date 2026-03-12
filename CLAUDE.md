# CLAUDE.md

This file provides guidance to Claude Code when working with code in this repository.

## Project Overview

Epistemotron is a C++ desktop application built with Microsoft Foundation Classes (MFC) for Visual Studio 2017 (v142 toolset). It implements a physics simulation of gravitational N-body interactions, with a multi-pane IDE-style UI for visualization and control.

## Build & Run

### MFC Application (Visual Studio)

**Building the GUI application:**
- Open `Epistemotron.sln` in Visual Studio 2019 or later
- Select configuration (Debug/Release) and platform (x64)
- Build via Build > Build Solution or press Ctrl+Shift+B

**Platform:** Windows desktop application (SubSystem: Windows)

**Toolset Notes:**
- Debug configuration uses v143 (Visual Studio 2022)
- Release configuration uses v142 (Visual Studio 2019)
- MFC usage: Dynamic linking (`UseOfMfc>Dynamic`)

### Simulation Library (CMake)

The `Science/` module can be built independently as a cross-platform C++17 library:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

**Build options:**
- `BUILD_TESTS=ON` (default) - Builds `test_simulation` executable
- `BUILD_EXAMPLES=ON` - Builds example programs

**Running tests:**
```bash
ctest --output-on-failure
```

**Recommended IDE:** GitHub Desktop for version control, Visual Studio for MFC development

## Important Path Notes

The project maintains two versions of the simulation code:

- **`Science/`** - MFC-specific version using `CString` and MFC conventions, linked into the GUI application
- **`Simulation/`** - Cross-platform C++17 version using `std::string` and modern C++, built via CMake

These are **separate implementations** of the same physics simulation, not symbolic links or aliases.

## Code Architecture

### Main Application Structure

**Entry Point & App Lifecycle:**
- `Epistemotron.cpp/.h` - Application entry point, `CEpistemotronApp` class (derived from `CWinAppEx`)
- `framework.h` - Framework includes, `targetver.h` for Windows SDK
- `pch.h/.cpp` - Precompiled header for faster builds

**Document-View Architecture:**
- `EpistemotronDoc.cpp/.h` - Document class (`CEpistemotronDoc`), data management
- `EpistemotronView.cpp/.h` - View class (`CEpistemotronView`), drawing and user interaction
- `MainFrm.cpp/.h` - Main frame window (`CMainFrame`), hosts menu, toolbars, status bar

**Control Bar Docking Framework:**
The main frame includes several dockable panes using MFC's `CMFC` classes:
- `ChildFrm.cpp/.h` - MDI child frame for multiple document windows
- `FileView.cpp/.h` - File tree explorer pane
- `ClassView.cpp/.h` - Class navigator pane
- `OutputWnd.cpp/.h` - Output/debug window
- `PropertiesWnd.cpp/.h` - Properties inspector pane
- `ViewTree.cpp/.h` - View tree management

### Simulation Engine (Science Module)

Located in the `Science/` subdirectory, implementing gravitational N-body physics:

**`Mass.h/.cpp`** - Physical body representation
- Properties: position (X, Y, Z in km), velocity (m/s), mass (kg)
- Methods: `Distance()`, `EffectuerPasChangementPosition()`, `EffectuerPasChangementVitesse()`
- French-named methods reflect physics timestep calculations

**`Universe.h/.cpp`** - Simulation container
- Holds `CArray<Mass, Mass>` of all bodies
- Manages simulation iteration state
- Methods: `Randomize()`, `GenerateSimulationStep()`, `SimulateFrom()`, `ExportPPM()`
- Supports bitmap export for visualization

**`Simulator.h/.cpp`** - Simulation controller
- Orchestrates simulation steps between view and universe
- `Test()` method for running demonstrations

**`Environment.h/.cpp`** - (Structure exists, check implementation for details)

### UI Framework Details

The application uses Modern UI chrome with `CMFC` classes:
- `CMFCMenuBar` for menus
- `CMFCToolBar` for toolbars with bitmap images
- `CMFCStatusBar` for status display
- `CMFCDockingPane` for dockable control bars
- High-contrast theme support via `_hc` resource variants

**Note:** The UI has a dockable pane layout similar to Visual Studio, with File View, Class View, Output Window, and Properties Window.

Resource files in `res/` directory contain:
- Application icons (`.ico`)
- Toolbar bitmaps (`.bmp`)
- Resource scripts (`Epistemotron.rc`, `Resource.rc`)

## Key Development Notes

1. **Mixed Language Codebase**: Comments and method names are mixed English/French (e.g., `EffectuerPasChangementVitesse`, `m_MasseKG`)

2. **MFC Message Map Architecture**: All event handling uses `DECLARE_MESSAGE_MAP()` and `afx_msg` handlers - not modern event/delegate patterns

3. **Precompiled Headers**: `pch.h` must be included first in all source files (enforced by project settings)

4. **Dual C++ Standards**:
   - MFC application uses C++20 (`stdcpp20`)
   - CMake simulation library uses C++17 (`cxx_std_17`)

5. **No Unit Tests for MFC**: The GUI application has no test infrastructure - development is GUI-driven with manual testing. The CMake build includes `tests/main.cpp` for the simulation library.

6. **Resource-Heavy UI**: Application relies on embedded bitmap resources for toolbar icons and UI chrome. High-contrast variants use `_hc` suffix (e.g., `Toolbar.bmp` and `menuimages_hc.bmp`).

7. **Physics Units**:
   - Position: kilometers (X, Y, Z)
   - Velocity: meters per second
   - Mass: kilograms
   - Time step: seconds

## File Locations Summary

| Component | Files |
|-----------|-------|
| Application Core | `Epistemotron.{cpp,h}`, `framework.h`, `pch.{cpp,h}` |
| Document/View | `EpistemotronDoc.{cpp,h}`, `EpistemotronView.{cpp,h}`, `Picture.{cpp,h}` |
| Main Frame | `MainFrm.{cpp,h}`, `ChildFrm.{cpp,h}` |
| Dockable Panes | `FileView.{cpp,h}`, `ClassView.{cpp,h}`, `OutputWnd.{cpp,h}`, `PropertiesWnd.{cpp,h}`, `ViewTree.{cpp,h}` |
| Simulation | `Science/Mass.{cpp,h}`, `Science/Universe.{cpp,h}`, `Science/Simulator.{cpp,h}`, `Science/Environment.{cpp,h}` |
| Resources | `res/*`, `Epistemotron.rc`, `Resource.rc`, `Resource.h`, `resource1.h` |