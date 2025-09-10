# Software Rasterizer - Technical Specifications

## Overview

This is a software rasterizer implemented in C++ that renders 3D graphics without using hardware acceleration (GPU). It implements the full graphics pipeline from vertex transformation to pixel rendering using only CPU computation.

## Architecture

### Core Components

1. **Renderer** (`src/Graphics/Renderer.cpp`)
   - Main rendering engine that orchestrates the graphics pipeline
   - Manages framebuffer, camera, and vertex/fragment processing
   - Implements drawing primitives (points, lines, triangles, meshes)
   - Handles depth testing and primitive assembly

2. **Primitive Assembler** (`src/Graphics/PrimitiveAssembler.cpp`)
   - Assembles vertices into triangles based on primitive type
   - Implements back-face culling with configurable winding order
   - Supports indexed and non-indexed rendering
   - Primitive types: TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN

3. **Vertex Shader** (`src/Graphics/VertexShader.cpp`)
   - Transforms vertices from model space to clip space
   - Applies Model-View-Projection (MVP) matrix transformations
   - Passes through vertex attributes (color, normals, UVs)

4. **Rasterizer** (`src/Graphics/Rasterizer.cpp`)
   - Converts triangles from clip space to screen pixels
   - Implements multiple rasterization algorithms:
     - Edge equation method
     - Scanline algorithm
   - Handles perspective-correct interpolation
   - Performs per-pixel depth testing

5. **Clipper** (`src/Graphics/Clipper.cpp`)
   - Clips triangles against the view frustum
   - Prevents rendering of geometry outside the viewing volume
   - Generates new vertices at clip boundaries

6. **Framebuffer** (`src/Graphics/Framebuffer.cpp`)
   - Manages pixel color and depth buffers
   - Implements per-pixel operations (depth test, color write)
   - Provides primitive drawing functions (points, lines)

### Math Library

- **Vec3/Vec4**: 3D and 4D vector mathematics
- **Mat4**: 4x4 matrix for transformations
- **Math utilities**: Trigonometric functions, interpolation

### Game Framework

- **GameManager**: Main game loop and update logic
- **Entity/Transform**: Scene graph and object transformations
- **Camera**: View and projection matrix generation
- **Timer**: Frame timing and delta time calculation

## Rendering Pipeline

### 1. Vertex Processing
```
Input Vertices → Vertex Shader → Transformed Vertices
```
- Apply model transformation (position, rotation, scale)
- Apply view transformation (camera position/orientation)
- Apply projection transformation (perspective/orthographic)

### 2. Primitive Assembly
```
Transformed Vertices → Primitive Assembler → Triangles
```
- Group vertices into triangles based on topology
- Calculate face normals for culling
- Perform back-face culling (configurable)

### 3. Clipping
```
Triangles → Clipper → Clipped Triangles
```
- Clip against near/far planes
- Clip against viewport boundaries
- Generate new vertices at clip intersections

### 4. Rasterization
```
Clipped Triangles → Rasterizer → Fragments/Pixels
```
- Convert from normalized device coordinates to screen coordinates
- Determine which pixels are covered by each triangle
- Interpolate vertex attributes across triangle surface

### 5. Fragment Processing
```
Fragments → Depth Test → Framebuffer
```
- Test fragment depth against depth buffer
- Write color to framebuffer if depth test passes
- Update depth buffer with fragment depth

## Features

### Implemented
- ✓ 3D vertex transformation pipeline
- ✓ Perspective projection
- ✓ Back-face culling with configurable winding order
- ✓ View frustum clipping
- ✓ Depth buffering (Z-buffer)
- ✓ Multiple primitive types (triangles, strips, fans)
- ✓ Wireframe and filled rendering modes
- ✓ Per-vertex colors with interpolation
- ✓ Face and vertex normal visualization
- ✓ Edge equation and scanline rasterization

### Culling System
- **Winding Order**: Counter-clockwise (CCW) for front faces
- **Cull Modes**:
  - NONE: Render all faces
  - BACK: Cull back-facing triangles (default)
  - FRONT: Cull front-facing triangles
  - FRONT_AND_BACK: Cull all faces (wireframe only)
- **Screen Space**: Y-axis points down, requiring adjusted winding calculations

### Coordinate Systems
- **Model Space**: Local object coordinates
- **World Space**: Global scene coordinates
- **View Space**: Camera-relative coordinates
- **Clip Space**: Post-projection coordinates (-w to +w)
- **NDC**: Normalized device coordinates (-1 to +1)
- **Screen Space**: Pixel coordinates (0 to width/height)

## Performance Considerations

- Pure CPU implementation (no GPU acceleration)
- Single-threaded rendering pipeline
- Optimized inner loops for rasterization
- Early rejection through culling and clipping
- Integer arithmetic for screen coordinates

## Usage Example

```cpp
// Create renderer
Renderer* renderer = new Renderer(sdlRenderer, width, height);

// Setup camera
Camera* camera = new Camera();
camera->setPerspective(60.0f, aspectRatio, 0.1f, 100.0f);
camera->setPosition(0, 0, 5);
renderer->SetCamera(camera);

// Configure culling
renderer->SetCullMode(CullMode::BACK);
renderer->SetWindingOrder(WindingOrder::CCW);

// Draw mesh
renderer->DrawVertexMesh(vertices, indices, modelMatrix, filled);
```

## Build Requirements

- C++11 or later
- SDL3 for window management and input
- No external graphics dependencies (pure software rendering)

## File Structure

```
Software-Rasterizer/
├── include/           # Header files
│   ├── Core/         # Core systems (Camera, Entity, Transform)
│   ├── Graphics/     # Rendering components
│   └── Math/         # Mathematical utilities
├── src/              # Implementation files
│   ├── Core/
│   ├── Game/
│   ├── Graphics/
│   └── Math/
├── specifications/   # Technical documentation
└── README.md        # Project overview
```

## Future Enhancements

- Texture mapping and sampling
- Fragment shaders
- Multi-threaded rasterization
- SIMD optimizations
- Shadow mapping
- Anti-aliasing
- More complex lighting models