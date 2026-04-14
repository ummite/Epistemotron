# Epistemotron

**An interactive N-body gravitational physics simulation with real-time visualization.**

Epistemotron is a Windows desktop application that simulates gravitational interactions between celestial bodies. Built with Microsoft Foundation Classes (MFC) and featuring GPU-accelerated computation options, it provides an IDE-style interface for exploring orbital mechanics, collision dynamics, and energy conservation in multi-body systems.

![Platform](https://img.shields.io/badge/Platform-Windows-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B-red)
![Framework](https://img.shields.io/badge/Framework-MFC-orange)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17%2F20-green)

## Features

### Physics Simulation
- **N-body gravitational simulation** with accurate Newtonian physics
- **Multiple integration methods**: Symplectic Euler and Velocity Verlet for energy conservation
- **Collision detection and merging** with conservation of mass and momentum
- **Orbit trail visualization** showing historical body positions
- **Energy and momentum tracking** for conservation analysis

### GPU Acceleration
- **CUDA support** for NVIDIA GPUs (100-1000x speedup for large simulations)
- **OpenCL support** for cross-vendor GPU acceleration
- **CPU fallback** with optimized serial implementation
- **Automatic solver selection** via factory pattern

### Visualization
- **3D camera controls** with pan, zoom, and rotation
- **Camera presets** for saving and loading viewpoints
- **Minimap overview** for navigation in large simulations
- **Energy conservation chart** showing KE, PE, and total energy over time
- **Velocity vectors** and center of mass markers
- **Collision flash effects** for visual feedback

### Scenarios
- **Solar System** - Realistic planetary orbits
- **Binary Star** - Two-body orbital dynamics
- **Three-Body Problem** - Chaotic gravitational interactions
- **Galaxy** - Large-scale N-body simulation (1000+ bodies)
- **Custom Scenarios** - Create and save your own configurations

### Simulation Control
- **Variable speed control** (0.1x to 10x real-time)
- **Step-through mode** for frame-by-frame analysis
- **Breakpoints** - Pause at specific iterations
- **Time travel** - Jump to specific simulation states
- **Slow-motion on collision** for detailed observation

### Export & Recording
- **Frame export** - Save individual frames as BMP images
- **Sequence recording** - Record simulation as image sequence
- **State save/load** - Persist and restore simulation states

## Building

### Prerequisites

- **Visual Studio 2019 or later** (2022 recommended)
- **Windows 10 or later**
- **CUDA Toolkit** (optional, for GPU acceleration)
- **CMake 3.16+** (for standalone library build)

### MFC Application (Full GUI)

1. Open `Epistemotron.sln` in Visual Studio
2. Select platform: **x64**
3. Select configuration: **Debug** or **Release**
4. Build > Build Solution (Ctrl+Shift+B)

**Note:** Debug uses v143 toolset (VS 2022), Release uses v142 (VS 2019)

### Standalone Library (CMake)

The simulation engine can be built as a cross-platform C++17 library:

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

**Options:**
```bash
cmake -DBUILD_TESTS=ON ..        # Build tests (default)
cmake -DBUILD_EXAMPLES=ON ..     # Build example programs
cmake -DCMAKE_BUILD_TYPE=Release ..  # Release build
```

**Run tests:**
```bash
ctest --output-on-failure
```

## Usage

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| Space | Start/Pause simulation |
| R | Reset simulation |
| S | Step one frame |
| + | Speed up |
| - | Slow down |
| 1-4 | Load scenario preset |
| C | Cycle scenarios |
| F | Zoom to fit all bodies |
| G | Toggle grid |
| T | Toggle trails |
| L | Toggle labels |
| V | Toggle velocity vectors |
| M | Toggle minimap |
| E | Toggle energy chart |
| H | Show help |
| Esc | Reset camera |

### Mouse Controls

- **Left button drag**: Pan camera
- **Right button drag**: Rotate (pitch/yaw)
- **Middle button drag**: Roll (tilt)
- **Scroll wheel**: Zoom in/out

### Menu Commands

- **Simulation**: Start, Pause, Resume, Stop, Reset, Step, Configure
- **Scenario**: Solar System, Binary Star, Three-Body, Galaxy, Custom
- **View**: Toggle trails, labels, grid, minimap, energy chart
- **Recording**: Start/Stop recording, Export frame/sequence
- **State**: Save/Load simulation state

## Architecture

```
Epistemotron/
├── Epistemotron.cpp/.h      # Application entry point (CEpistemotronApp)
├── EpistemotronDoc.cpp/.h   # Document class (data management)
├── EpistemotronView.cpp/.h  # View class (rendering & interaction)
├── MainFrm.cpp/.h           # Main frame (menus, toolbars, status)
├── ChildFrm.cpp/.h          # MDI child frame
├── framework.h              # Framework includes & type definitions
├── pch.cpp/.h              # Precompiled header
│
├── Science/                 # MFC simulation implementation
│   ├── Mass.cpp/.h         # Celestial body representation
│   ├── Universe.cpp/.h     # Simulation container & physics
│   ├── Simulator.cpp/.h    # Simulation controller
│   └── Environment.cpp/.h  # Environment settings
│
├── Simulation/              # Cross-platform C++17 implementation
│   ├── Mass.cpp/.h
│   ├── Universe.cpp/.h
│   └── Simulator.cpp/.h
│
├── GPU/                     # GPU acceleration backends
│   ├── IGravitySolver.h    # Solver interface
│   ├── CPUGravitySolver.cpp/.h
│   ├── CUDAGravitySolver.cpp/.h
│   ├── OpenCLGravitySolver.cpp/.h
│   └── GravitySolverFactory.cpp/.h
│
├── GPU/                     # Dockable panes
│   ├── FileView.cpp/.h
│   ├── ClassView.cpp/.h
│   ├── OutputWnd.cpp/.h
│   ├── PropertiesWnd.cpp/.h
│   └── ViewTree.cpp/.h
│
├── tests/                   # CMake test suite
│   └── main.cpp
│
└── res/                     # Resources (icons, bitmaps)
```

### Key Classes

| Class | Purpose |
|-------|---------|
| `Mass` | Represents a celestial body (position, velocity, mass) |
| `Universe` | Container for all bodies; manages simulation state |
| `IGravitySolver` | Interface for gravity computation backends |
| `CEpistemotronView` | Main visualization and user interaction |
| `CEpistemotronDoc` | Document data model |

### Physics Units

- **Position**: Kilometers (km)
- **Velocity**: Meters per second (m/s)
- **Mass**: Kilograms (kg)
- **Time step**: Seconds (s)
- **Energy**: Joules (J)

## Physics Implementation

### Gravitational Constant
```
G = 6.67430 × 10⁻¹¹ m³/(kg·s²)
```

### Integration Methods

**Symplectic Euler** (default):
- Semi-implicit method with good energy conservation
- Position updated first, then velocity
- Stable for orbital mechanics

**Velocity Verlet**:
- Higher accuracy than symplectic Euler
- Better for oscillatory systems
- Requires force evaluation at half-steps

### Collision Handling

When two bodies collide:
1. **Mass conservation**: m_merged = m₁ + m₂
2. **Momentum conservation**: v_merged = (m₁v₁ + m₂v₂) / m_merged
3. Position set to center of mass

## Development Notes

- **Mixed language codebase**: Method names are English/French (e.g., `EffectuerPasChangementPosition`)
- **MFC message maps**: Event handling via `DECLARE_MESSAGE_MAP()`
- **Dual C++ standards**: MFC app uses C++20, CMake library uses C++17
- **Precompiled headers**: `pch.h` must be included first

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Ensure compilation succeeds (Debug and Release)
5. Submit a pull request

## License

This project is provided as-is for educational and research purposes.

## References

- **N-body problem**: https://en.wikipedia.org/wiki/N-body_problem
- **Symplectic integrator**: https://en.wikipedia.org/wiki/Symplectic_integrator
- **Velocity Verlet**: https://en.wikipedia.org/wiki/Verlet_integration
