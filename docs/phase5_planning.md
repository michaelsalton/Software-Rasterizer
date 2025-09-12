# Phase 5 Planning - 3D Model Loading

## Current State (End of Phase 4)

### Completed Features
- ✅ Complete rendering pipeline (vertex processing, clipping, rasterization)
- ✅ Texture mapping system with mipmaps and filtering
- ✅ Fragment shader system with hot-reload
- ✅ Comprehensive lighting system:
  - Directional lights (sun)
  - Point lights with attenuation
  - Spot lights with cone falloff
  - Multiple shading models (Phong, Blinn-Phong, Lambert)
- ✅ Material system with texture support
- ✅ Camera controller (FPS and Orbit modes)
- ✅ GUI integration for all parameters

### Deferred from Phase 4
- Shadow mapping (requires render-to-texture)
- Normal mapping (requires tangent space)
These will be addressed in a future phase after model loading is complete.

## Phase 5 Goals

### Primary Objectives
1. **Load 3D models from files** instead of hardcoded geometry
2. **Support industry-standard formats** (OBJ first, glTF later)
3. **Handle materials and textures** from model files
4. **Implement scene management** for multiple models
5. **Enable real-world asset rendering**

### Why Phase 5 Now?
- Core renderer is feature-complete for basic rendering
- Loading real models makes the renderer immediately useful
- Shadow/normal mapping can be added after we can load models
- Testing advanced features is easier with complex geometry

## Implementation Approach

### Stage 1: Basic OBJ Loading (Week 1)
**Goal**: Load and render a simple OBJ file

Tasks:
1. Create OBJ parser
   - Parse v/vt/vn/f lines
   - Handle multiple index formats
   - Triangulate faces
2. Implement Mesh class
   - Store vertices and indices
   - Support submeshes
3. Update GameManager
   - Replace hardcoded cube with loaded model
   - Test with simple OBJ files

**Deliverable**: Render stanford_bunny.obj

### Stage 2: Material Support (Week 2)
**Goal**: Load materials and textures from MTL files

Tasks:
1. Create MTL parser
   - Parse material properties
   - Handle texture paths
2. Enhance Material class
   - Map MTL properties to our system
   - Load textures automatically
3. Multi-material rendering
   - Assign materials to submeshes
   - Batch by material

**Deliverable**: Render textured models with multiple materials

### Stage 3: Scene Graph (Week 3)
**Goal**: Support model hierarchies and instancing

Tasks:
1. Implement SceneNode system
   - Transform hierarchy
   - Visibility culling
2. Create Scene class
   - Manage all nodes
   - Integrate lighting
3. Model instancing
   - Share mesh data
   - Per-instance transforms

**Deliverable**: Forest scene with instanced trees

### Stage 4: Resource Management (Week 4)
**Goal**: Efficient loading and memory management

Tasks:
1. Model caching system
   - Avoid duplicate loads
   - Reference counting
2. Async loading
   - Background loading
   - Progress feedback
3. Memory optimization
   - Vertex buffer sharing
   - Texture atlasing

**Deliverable**: Load Sponza scene efficiently

## File Format Details

### OBJ Format (Priority 1)
```
# Vertices
v x y z [w]

# Texture coordinates  
vt u v [w]

# Normals
vn x y z

# Faces (1-indexed)
f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 [v4/vt4/vn4]

# Material reference
usemtl material_name

# Group/Object names
g group_name
o object_name
```

### MTL Format
```
# Material name
newmtl material_name

# Colors
Ka r g b  # Ambient
Kd r g b  # Diffuse  
Ks r g b  # Specular
Ke r g b  # Emissive

# Properties
Ns value  # Shininess (0-1000)
Ni value  # Index of refraction
d value   # Opacity (1=opaque, 0=transparent)
Tr value  # Transparency (inverse of d)
illum n   # Illumination model

# Texture maps
map_Kd filename  # Diffuse texture
map_Ks filename  # Specular texture
map_bump filename # Normal map
```

## Testing Models

### Development Models
1. **cube.obj** - Simple test (12 triangles)
2. **sphere.obj** - Smooth shading test (960 triangles)  
3. **teapot.obj** - Classic test model (6,320 triangles)

### Validation Models
1. **bunny.obj** - Stanford Bunny (69,451 triangles)
2. **dragon.obj** - Stanford Dragon (871,414 triangles)
3. **sponza.obj** - Sponza Palace (262,267 triangles)

### Performance Targets
- Bunny: 60+ FPS
- Dragon: 30+ FPS  
- Sponza: 20+ FPS

## Integration Points

### Renderer Changes
```cpp
// Old API
renderer->DrawVertexMesh(vertices, indices, transform, wireframe);

// New API  
renderer->DrawMesh(mesh, transform, material);
renderer->DrawModel(model, transform);
```

### GameManager Updates
```cpp
// Replace hardcoded cube with:
mScene = std::make_unique<Scene>();

// Load a model
auto bunny = mScene->loadModel("assets/models/bunny.obj");
bunny->setLocalTransform(Mat4::translate(0, -1, 0));

// In render loop
mScene->draw(mRenderer);
```

## Success Metrics

### Week 1
- [x] Parse basic OBJ files
- [x] Render untextured models
- [x] Maintain existing performance

### Week 2  
- [ ] Load MTL materials
- [ ] Render textured models
- [ ] Support multiple materials

### Week 3
- [ ] Scene graph working
- [ ] Model instancing
- [ ] Frustum culling

### Week 4
- [ ] Resource caching
- [ ] Large scene support
- [ ] Memory efficiency

## Risks and Mitigations

### Risk 1: Performance degradation
**Mitigation**: Profile early, optimize critical paths, add LOD support

### Risk 2: Memory usage explosion
**Mitigation**: Implement proper caching, share resources, stream large models

### Risk 3: Format compatibility issues  
**Mitigation**: Start with simple OBJ, test with standard models, add format features incrementally

## Next Steps

1. Create basic Mesh and Model classes
2. Implement simple OBJ parser (vertices and faces only)
3. Update Renderer to accept Mesh objects
4. Replace cube with loaded model in GameManager
5. Test with cube.obj to verify pipeline

The goal is to have a working model loader by end of Week 1, with materials and scene management following in subsequent weeks.