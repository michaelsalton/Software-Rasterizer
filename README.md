# Software Rasterizer

A 3D software rasterizer built from scratch in C++ using SDL3 for window management and 2D operations. This project implements fundamental 3D graphics concepts including perspective projection, primitive rendering, and custom triangle rasterization algorithms.

## Features

- **3D Perspective Projection**: Transforms 3D coordinates to 2D screen space with configurable FOV, aspect ratio, and near/far planes
- **Custom Triangle Rasterization**: Implements scanline-based filled triangle rendering algorithm
- **Primitive Rendering**: Support for points, lines, and triangles (both wireframe and filled)
- **Scene Graph**: Entity system with hierarchical parent-child relationships
- **Frame Rate Control**: Runs at a smooth 60 FPS with proper timing utilities

## Project Structure

```
SoftwareRasterizer/
├── Core/           # Main entry point
│   └── Main.cpp
├── Graphics/       # Rendering systems
│   ├── Graphics.cpp/h    # SDL window/context management
│   └── Renderer.cpp/h    # 3D projection and rasterization
├── Math/           # Mathematics utilities
│   ├── MathHelper.cpp/h  # Math utilities and rotations
│   └── Vector3.h         # 3D vector implementation
├── Game/           # Game framework
│   ├── Entity.cpp/h      # Base entity class
│   └── GameManager.cpp/h # Main game loop
├── Utils/          # Utility classes
│   ├── InputManager.cpp/h # Input handling
│   └── Timer.cpp/h        # Frame timing
└── Makefile        # Build configuration
```

## Building

### Prerequisites

- C++17 compatible compiler (g++ or clang++)
- SDL3 development libraries
- pkg-config

### Linux/Unix

```bash
cd SoftwareRasterizer
make
```

### Running

```bash
./SoftwareRasterizer
```

Press the window close button to exit.

## Architecture

The rasterizer uses a hybrid approach:
- **Custom 3D Mathematics**: All 3D transformations and projections are implemented from scratch
- **SDL3 Integration**: Used for window creation, event handling, and 2D pixel operations
- **Software Rendering**: Triangle rasterization is performed entirely in software using a scanline algorithm

## Current Demo

The current implementation renders:
- A filled green triangle demonstrating the custom rasterization algorithm
- A red line showing basic line drawing capabilities

## Future Enhancements

The architecture is designed to support:
- Texture mapping
- Depth buffering/Z-buffering
- Shading models (flat, Gouraud, Phong)
- 3D model loading and rendering
- Camera systems
- Lighting calculations

## Development

This project serves as an educational implementation for understanding low-level 3D graphics concepts without relying on GPU acceleration. It provides a foundation for learning how modern graphics APIs work under the hood.

## License

This project is open source. Feel free to use it for educational purposes or as a starting point for your own graphics experiments.