# Software Rasterizer File Structure

## Overview
The project follows a modular architecture with clear separation of concerns. Each directory has a specific purpose, making the codebase scalable and maintainable.

## Directory Structure

```
Software-Rasterizer/
├── include/              # Header files
│   ├── core/            # Core engine systems
│   ├── math/            # Mathematics library
│   ├── pipeline/        # Graphics pipeline stages
│   ├── rendering/       # High-level rendering
│   ├── graphics/        # Low-level graphics
│   ├── lighting/        # Lighting system
│   ├── scene/          # Scene management
│   ├── ui/             # User interface
│   ├── game/           # Game-specific code
│   ├── utils/          # Utilities
│   └── resources/      # Resource management
│
├── src/                # Source files (mirrors include)
├── assets/             # Game assets
├── docs/               # Documentation
├── tools/              # Build tools and scripts
├── tests/              # Unit tests
└── third_party/        # External dependencies
```

## Module Descriptions

### Core (`core/`)
Core engine systems including camera management and transformations.
- `camera.h/cpp` - Camera view and projection
- `camera_controller.h/cpp` - FPS/orbit camera controls
- `transform.h/cpp` - 3D transformations
- `main.cpp` - Application entry point

### Math (`math/`)
Mathematical primitives and operations.
- `vec2/3/4.h` - Vector classes
- `mat4.h` - 4x4 matrix operations
- `math.h` - Common math utilities

### Pipeline (`pipeline/`)
Graphics pipeline stages for vertex and fragment processing.
- `vertex_processor.h/cpp` - Vertex data structures
- `vertex_shader.h/cpp` - Programmable vertex shaders
- `primitive_assembler.h/cpp` - Triangle assembly
- `clipper.h/cpp` - View frustum clipping
- `rasterizer.h/cpp` - Triangle rasterization
- `fragment_shader.h/cpp` - Programmable fragment shaders
- `pipeline_state.h` - Pipeline configuration

### Rendering (`rendering/`)
High-level rendering concepts and resource management.
- `renderer.h/cpp` - Main rendering interface
- `texture.h/cpp` - Texture loading and sampling
- `material.h` - Material properties

### Graphics (`graphics/`)
Low-level graphics utilities and contexts.
- `graphics_context.h/cpp` - SDL/window management
- `frame_buffer.h/cpp` - Pixel buffer management
- `bitmap_font.h/cpp` - Text rendering
- `texture_generator.h/cpp` - Procedural textures

### Lighting (`lighting/`)
Lighting system components.
- `light.h/cpp` - Light types (directional, point, spot)
- `lighting_calculations.h` - Lighting algorithms

### Scene (`scene/`)
Scene graph and entity management.
- `entity.h/cpp` - Base entity class

### UI (`ui/`)
User interface and debug tools.
- `ui_manager.h/cpp` - GUI management
- ImGui integration files

### Game (`game/`)
Game-specific logic and management.
- `game_manager.h/cpp` - Main game loop

### Utils (`utils/`)
Utility classes and helpers.
- `timer.h/cpp` - Timing utilities
- `input_manager.h/cpp` - Input handling

## Naming Conventions

### Files
- Use snake_case: `vertex_processor.h`
- Header files: `.h`
- Source files: `.cpp`

### Directories
- Use lowercase: `rendering/`, `pipeline/`

### Classes
- Use PascalCase: `VertexProcessor`, `GameManager`
- Renamed classes:
  - `Graphics` → `GraphicsContext`
  - `GUIManager` → `UIManager`
  - `Vertex::` → `VertexProcessor::`

## Build System
The project uses Make with automatic dependency tracking. Object files are built in the `build/` directory, maintaining the source structure.

## Future Expansion
The structure is designed to accommodate:
- `resources/` - Asset loading system
- Scene graph in `scene/`
- Additional shader types in `pipeline/`
- Post-processing effects
- Physics system