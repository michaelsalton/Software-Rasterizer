# Proposed File Structure Reorganization

## Current Issues
1. Graphics folder is becoming too large with mixed responsibilities
2. No clear separation between low-level and high-level rendering
3. Missing common folders like Scene, Resources, etc.
4. Pipeline stages mixed with utility classes

## Proposed Structure

```
Software-Rasterizer/
├── assets/                      # All game assets
│   ├── textures/
│   ├── models/                  # Future: 3D models
│   ├── shaders/                 # Future: Shader files
│   └── fonts/                   # Future: Font files
│
├── include/
│   ├── core/                    # Core engine systems
│   │   ├── camera.h
│   │   ├── camera_controller.h
│   │   ├── transform.h
│   │   └── application.h        # Future: App framework
│   │
│   ├── math/                    # Math library (unchanged)
│   │   ├── vec2.h
│   │   ├── vec3.h
│   │   ├── vec4.h
│   │   ├── mat4.h
│   │   ├── math.h
│   │   └── quaternion.h         # Future
│   │
│   ├── rendering/               # High-level rendering
│   │   ├── renderer.h          
│   │   ├── render_state.h       # RenderSettings, etc.
│   │   ├── material.h           # Move from Lighting/
│   │   ├── mesh.h               # Future: Mesh class
│   │   └── texture.h            # Move from Graphics/
│   │
│   ├── pipeline/                # Graphics pipeline stages
│   │   ├── vertex_processor.h   # Rename from vertex.h
│   │   ├── vertex_shader.h
│   │   ├── primitive_assembler.h
│   │   ├── clipper.h
│   │   ├── rasterizer.h
│   │   ├── fragment_shader.h
│   │   └── pipeline_state.h
│   │
│   ├── lighting/                # Lighting system
│   │   ├── light.h
│   │   ├── lighting_calculations.h
│   │   ├── shadow_map.h         # Future
│   │   └── light_manager.h      # Future
│   │
│   ├── scene/                   # Scene management (new)
│   │   ├── scene_node.h         # Scene graph node
│   │   ├── scene_manager.h      # Scene management
│   │   ├── entity.h             # Move from Game/
│   │   └── component.h          # Future: ECS
│   │
│   ├── resources/               # Resource management (new)
│   │   ├── resource_manager.h   # Generic resource manager
│   │   ├── texture_loader.h     # Texture loading
│   │   ├── model_loader.h       # Future: Model loading
│   │   └── shader_loader.h      # Future
│   │
│   ├── graphics/                # Low-level graphics
│   │   ├── frame_buffer.h
│   │   ├── graphics_context.h   # Rename from graphics.h
│   │   ├── bitmap_font.h
│   │   └── texture_generator.h
│   │
│   ├── ui/                      # UI system (rename from GUI)
│   │   ├── ui_manager.h         # Rename from gui_manager.h
│   │   ├── imgui_integration.h  # Future: Separate ImGui
│   │   └── debug_overlay.h      # Future: Debug UI
│   │
│   ├── utils/                   # Utilities
│   │   ├── timer.h
│   │   ├── input_manager.h
│   │   ├── logger.h             # Future: Logging
│   │   └── profiler.h           # Future: Profiling
│   │
│   └── game/                    # Game-specific code
│       ├── game_manager.h
│       └── demo_scenes.h        # Future: Demo scene setups
│
├── src/                         # Mirror include structure
│   └── [same structure as include]
│
├── tests/                       # Unit tests
│   ├── math/
│   ├── rendering/
│   ├── pipeline/
│   └── resources/
│
├── docs/                        # Documentation (new)
│   ├── api/                     # API documentation
│   ├── design/                  # Design documents
│   └── tutorials/               # Usage tutorials
│
├── tools/                       # Build tools & scripts (new)
│   ├── scripts/
│   └── cmake/                   # Future: CMake modules
│
├── specifications/              # Project specifications
└── third_party/                 # External dependencies
```

## Benefits of This Structure

### 1. **Clear Separation of Concerns**
- `pipeline/` - Graphics pipeline stages
- `rendering/` - High-level rendering concepts
- `graphics/` - Low-level graphics utilities
- `lighting/` - All lighting-related code
- `scene/` - Scene graph and entity management

### 2. **Scalability**
- Each folder has a clear purpose
- Easy to add new systems without cluttering
- Follows standard game engine architecture

### 3. **Industry Standards**
- Similar to Unreal Engine, Unity, and open-source engines like Godot
- Familiar to developers joining the project
- Clear boundaries between systems

### 4. **Future-Ready**
- `resources/` ready for asset loading system
- `scene/` ready for scene graph implementation
- `docs/` and `tools/` for better project management

## Migration Plan

### Phase 1: Core Reorganization (High Priority)
1. Create new folder structure
2. Move files to new locations:
   - `Graphics/vertex.h` → `pipeline/vertex_processor.h`
   - `Lighting/material.h` → `rendering/material.h`
   - `Graphics/texture.h` → `rendering/texture.h`
   - `Game/entity.h` → `scene/entity.h`
   - `GUI/*` → `ui/*`
3. Update all #include statements
4. Update Makefile

### Phase 2: Renaming (Medium Priority)
1. Rename classes to match new structure:
   - `Graphics` → `GraphicsContext`
   - `GUIManager` → `UIManager`
   - `Vertex` → `VertexProcessor`
2. Update all references

### Phase 3: New Systems (Low Priority)
1. Add `ResourceManager` for texture/model loading
2. Add `SceneManager` for scene graph
3. Add logging and profiling systems

## Alternative: Minimal Reorganization

If a full reorganization is too disruptive, here's a minimal approach:

```
include/
├── Core/          # Engine core (unchanged)
├── Math/          # Math library (unchanged)
├── Rendering/     # High-level rendering (new)
│   ├── Pipeline/  # Pipeline stages (moved from Graphics/)
│   ├── Resources/ # Textures, materials, etc.
│   └── Scene/     # Scene management
├── Graphics/      # Low-level graphics only
├── Lighting/      # Lighting system (unchanged)
├── UI/            # Renamed from GUI/
└── Game/          # Game logic (unchanged)
```

## Naming Conventions

### File Names
- Use snake_case for all files: `vertex_processor.h`
- Header guards: `SOFTWARERASTERIZER_CATEGORY_FILENAME_H`

### Class Names
- Use PascalCase: `VertexProcessor`, `SceneManager`
- Interfaces prefix with 'I': `IRenderable`, `ILight`

### Folder Names
- Use lowercase: `rendering`, `pipeline`, `scene`
- Avoid abbreviations: `user_interface` not `ui` (except well-known)

## Next Steps

1. **Decide on approach**: Full reorganization or minimal
2. **Create migration script**: Automate file moves and #include updates
3. **Update build system**: Modify Makefile for new structure
4. **Update documentation**: README and other docs
5. **Test thoroughly**: Ensure nothing breaks

This structure will make the project more maintainable and professional as it grows!