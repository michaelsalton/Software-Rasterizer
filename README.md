# Software Rasterizer

A fully-featured 3D software rasterizer from scratch using C++. This project implements a complete rendering pipeline in software, demonstrating how modern GPUs work internally without any hardware acceleration.

## Features

### Core Rendering Pipeline

#### Multiple Rasterization Algorithms
- **Edge Equation Method** (default): Efficient half-space testing using edge equations
- **Scanline Algorithm**: Traditional scanline-based triangle rasterization
- **Hierarchical Rasterization**: Tile-based approach for improved cache coherence

#### Complete Graphics Pipeline Stages
1. **Vertex Processing**
   - Programmable vertex shaders with base class system
   - Built-in shaders: Default, Wave, Twist, Explode, and Spherize effects
   - Full MVP (Model-View-Projection) transformation support
   - Normal transformation to world space

2. **Primitive Assembly**
   - Support for multiple primitive types:
     - Triangles, Triangle Strips, Triangle Fans
     - Lines, Line Strips, Line Loops
     - Points
   - Efficient index buffer support for vertex reuse

3. **Clipping**
   - Full 3D frustum clipping using Sutherland-Hodgman algorithm
   - Clips against all 6 frustum planes
   - Proper homogeneous clipping in 4D space
   - Interpolates all vertex attributes during clipping

4. **Culling**
   - Configurable back-face culling
   - Support for both CCW and CW winding orders
   - Front face, back face, or no culling modes

5. **Rasterization**
   - Sub-pixel precision (pixel centers at x+0.5, y+0.5)
   - Perspective-correct attribute interpolation
   - Barycentric coordinate calculation for smooth interpolation

6. **Fragment Processing**
   - Z-buffer depth testing
   - Alpha blending support
   - Per-pixel color output

### Advanced Features

#### Programmable Vertex Shaders
- **Uniform System**: Pass custom parameters to shaders
- **Time-based Effects**: Real-time vertex animation support
- **Extensible Architecture**: Easy to add custom shaders

#### Transform System
- **Hierarchical Transforms**: Parent-child relationships for scene graphs
- **Transform Caching**: Optimized with dirty flag system
- **Camera System**: Perspective and orthographic projections with look-at functionality

#### Rendering Capabilities
- **Wireframe and Filled Modes**: Toggle between rendering modes
- **Debug Visualization**: Coordinate axes, grids, and local coordinate systems
- **Alpha Transparency**: Full alpha blending support

### Performance Optimizations
- **SIMD-friendly Math**: Vec4 operations aligned for potential vectorization
- **Early Rejection**: Bounding box and frustum culling
- **Cache-efficient Design**: Separate color and depth buffers
- **Optimized Matrix Operations**: Fast 4x4 matrix multiplication
- **Inline Math Functions**: Extensive inlining for small operations

### Technical Features
- **3D Math Library**: Comprehensive vector (Vec2, Vec3, Vec4) and matrix (Mat4) classes
- **Cross-platform**: Works on Linux, macOS, and Windows with SDL3
- **Modern C++17**: Smart pointers, STL containers, and clean architecture
- **Frame Rate Control**: Smooth 60 FPS with frame timing
- **Testing Framework**: Custom unit testing with comprehensive math library coverage

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

### Rendering Pipeline Implementation

The renderer implements a complete graphics pipeline with the following stages:

1. **Vertex Shader Stage**: Transforms vertices from model space to clip space
2. **Primitive Assembly**: Groups vertices into geometric primitives
3. **Clipping**: Clips primitives against the view frustum
4. **Perspective Division**: Converts from homogeneous to normalized device coordinates
5. **Viewport Transform**: Maps to screen coordinates
6. **Rasterization**: Converts primitives to fragments
7. **Fragment Processing**: Determines final pixel colors with depth testing

Key algorithms implemented:
- **Sutherland-Hodgman** for polygon clipping
- **Edge equations** for efficient triangle rasterization
- **Barycentric coordinates** for attribute interpolation
- **Z-buffer algorithm** for hidden surface removal

### Entity System
- Transform hierarchy
- Parent-child relationships
- World/local space transformations

## License

This project is for educational purposes.