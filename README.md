# Software Rasterizer

A 3D software rasterizer implementation from scratch using C++ and SDL3.

## Project Structure

```
Software-Rasterizer/
├── src/                    # Source code
│   ├── Core/              # Main application and entry point
│   ├── Game/              # Game logic and entities
│   ├── Graphics/          # Rendering and graphics
│   ├── Math/              # Mathematical utilities (vectors, matrices)
│   └── Utils/             # Utility classes (Timer, InputManager)
├── tests/                  # Unit tests
│   ├── Math/              # Math library tests
│   └── TestFramework.h    # Testing framework
├── Makefile               # Build configuration
└── README.md              # This file
```

## Features

- **3D Math Library**: Comprehensive vector (Vec2, Vec3, Vec4) and matrix (Mat4) classes
- **Software Rendering**: Triangle rasterization without GPU acceleration
- **Entity System**: Basic game entity management with hierarchical transforms
- **Cross-platform**: Works on Linux with SDL3
- **3D Perspective Projection**: Transforms 3D coordinates to 2D screen space
- **Primitive Rendering**: Support for points, lines, and triangles
- **Frame Rate Control**: Runs at a smooth 60 FPS

## Building

### Prerequisites

- C++17 compatible compiler (g++ or clang++)
- SDL3 development libraries
- pkg-config

### Build Commands

```bash
# Build the project
make

# Run the application
make run

# Clean build files
make clean

# Run tests
make test

# Run tests with verbose output
make test-verbose
```

## Testing

The project includes a comprehensive test suite using a custom testing framework. Tests are located in the `tests/` directory and cover:

- Vec2, Vec3, Vec4 operations
- Mat4 transformations
- Mathematical utilities

See [tests/README.md](tests/README.md) for more information about the testing framework.

## Architecture

### Math Library
- **Vec2, Vec3, Vec4**: Vector classes with full operator overloading
- **Mat4**: 4x4 transformation matrix with support for:
  - Translation, rotation, scaling
  - Perspective and orthographic projections
  - Matrix inverse and decomposition
  - Euler angle conversions

### Rendering Pipeline
- Software-based triangle rasterization
- Vertex transformation pipeline
- Basic shading support

### Entity System
- Transform hierarchy
- Parent-child relationships
- World/local space transformations

## License

This project is for educational purposes.