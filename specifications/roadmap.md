# Software Rasterizer - Development Roadmap

## Goal: Render 3D Models with Full Feature Set

This roadmap outlines the development path from the current state to a fully-featured 3D renderer capable of loading and rendering complex 3D models with textures, lighting, and materials.

---

## Phase 1: Core Pipeline Completion ✓ (Mostly Complete)

### Overview
The core rendering pipeline handles vertex transformation, primitive assembly, clipping, rasterization, and depth testing. This phase focuses on completing and optimizing the geometric pipeline before moving to fragment processing and texturing.

### ✓ Completed Components

#### 1. Vertex Transformation Pipeline
**Current Implementation:**
- Full MVP (Model-View-Projection) matrix transformation chain
- Transforms vertices from object space → world space → view space → clip space → NDC → screen space
- Perspective divide with w-component handling
- Viewport transformation with Y-axis flip for screen coordinates

**Technical Details:**
```
Pipeline: Object Space → [Model Matrix] → World Space → [View Matrix] → 
          View Space → [Projection Matrix] → Clip Space → [Perspective Divide] → 
          NDC (-1 to 1) → [Viewport Transform] → Screen Space (pixels)
```

**Known Limitations:**
- Normal matrix uses model matrix directly (incorrect for non-uniform scaling)
- No support for multiple viewports
- Fixed-function pipeline only (no programmable stages beyond basic vertex shader)

#### 2. Vertex Attribute System
**Supported Attributes:**
- Position (Vec3) - 3D coordinates
- Normal (Vec3) - Surface normal for lighting
- Texture Coordinates (Vec2) - UV mapping
- Color (RGBA 8-bit) - Vertex coloring

**TransformedVertex Structure:**
- `clipPosition` (Vec4) - Homogeneous clip coordinates
- `worldPosition` (Vec3) - For lighting calculations
- `worldNormal` (Vec3) - Transformed normal
- `screenPosition` (Vec3) - x,y in pixels, z in [0,1]
- Interpolated attributes preserved through pipeline

#### 3. Primitive Assembly
**Implemented Topology Types:**
- `TRIANGLES` - Every 3 vertices form a triangle
- `TRIANGLE_STRIP` - Shared vertices with alternating winding
- `TRIANGLE_FAN` - First vertex shared by all triangles

**Features:**
- Indexed and non-indexed rendering
- Proper winding order handling for strips
- Degenerate triangle rejection
- Face normal calculation in screen space

#### 4. Culling System
**Back-Face Culling:**
- Configurable winding order (CCW/CW)
- Screen-space normal calculation using 2D cross product
- Cull modes: NONE, BACK, FRONT, FRONT_AND_BACK
- Proper handling of Y-down screen coordinates

**Implementation:**
```cpp
// Screen space winding test
float signedArea = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);
bool isFrontFacing = (windingOrder == CCW) ? (signedArea < 0) : (signedArea > 0);
```

#### 5. View Frustum Clipping
**Sutherland-Hodgman Algorithm:**
- Full 6-plane clipping (near, far, left, right, top, bottom)
- Homogeneous clipping in 4D clip space
- Proper attribute interpolation during clip vertex generation
- Supports up to 9-vertex output polygons

**Clipping Planes:**
```
Left:   x + w >= 0
Right:  -x + w >= 0  
Bottom: y + w >= 0
Top:    -y + w >= 0
Near:   z + w >= 0
Far:    -z + w >= 0
```

#### 6. Rasterization Algorithms
**Three Selectable Algorithms:**

1. **Scanline Rasterization:**
   - Traditional top-to-bottom scanline conversion
   - Vertex sorting by Y-coordinate
   - Edge interpolation for each scanline
   - Good for long, thin triangles

2. **Edge Equation (Default):**
   - Barycentric coordinate calculation
   - Half-space testing with edge functions
   - Sub-pixel precision (pixel centers at +0.5)
   - Better for arbitrary triangles

3. **Hierarchical/Tiled:**
   - 8x8 pixel tile processing
   - Tile-level culling
   - Better cache coherence
   - Configurable tile size

**Interpolation:**
- Perspective-correct attribute interpolation
- Optimized barycentric calculations
- Proper depth interpolation

#### 7. Depth Buffer
**Implementation:**
- 32-bit floating-point Z-buffer
- Per-pixel depth testing (less-than comparison)
- Clear to 1.0 (far plane)
- Optional depth testing (can be disabled)
- Bounds-checked depth writes

#### 8. Vertex Shader System
**Architecture:**
- Pluggable vertex shader interface
- Comprehensive uniform support (matrices, vectors, floats, custom data)
- Built-in shaders: Default, Wave, Twist, Explode, Spherize

**Shader Uniforms Structure:**
```cpp
- Mat4: model, view, projection, mvp, normal matrices
- Vec3: camera position, light positions/directions
- Custom uniforms via void* storage
```

### 🔧 Needs Refinement

#### 1. Clipping Improvements
- [ ] **Robust Near-Plane Handling**
  - Special case for near plane to avoid precision issues
  - Better handling of triangles straddling near plane
  - Proper Z-value clamping

- [ ] **Clipping Optimization**
  - Early rejection using outcodes
  - SIMD-optimized plane testing
  - Guard band clipping to reduce work

- [ ] **Polygon Triangulation**
  - Current fan triangulation can create slivers
  - Implement ear-clipping algorithm for better quality
  - Optimize for common cases (4-5 vertices)

#### 2. Normal Transformation
- [ ] **Proper Normal Matrix**
  - Calculate inverse-transpose of model matrix
  - Cache normal matrix to avoid recalculation
  - Handle non-uniform scaling correctly

#### 3. Precision and Robustness
- [ ] **Floating-Point Precision**
  - Use fixed-point for screen coordinates
  - Implement proper epsilon testing
  - Handle edge cases in perspective divide

- [ ] **Degenerate Triangle Handling**
  - Better detection of zero-area triangles
  - Consistent handling across pipeline stages
  - Proper attribute handling for degenerate cases

### 🆕 Missing Core Features to Implement

#### 1. Pipeline State Management
- [ ] **Rasterizer State Object**
  ```cpp
  struct RasterizerState {
      FillMode fillMode;        // SOLID, WIREFRAME, POINT
      CullMode cullMode;        // NONE, BACK, FRONT
      bool scissorEnable;
      bool depthClamp;
      float depthBias;
      float slopeScaledDepthBias;
  };
  ```

- [ ] **Depth Stencil State**
  ```cpp
  struct DepthStencilState {
      bool depthEnable;
      bool depthWrite;
      ComparisonFunc depthFunc;  // LESS, LEQUAL, GREATER, etc.
      bool stencilEnable;
      uint8_t stencilRef;
      uint8_t stencilMask;
  };
  ```

#### 2. Advanced Culling
- [ ] **Scissor Test**
  - Rectangle-based pixel rejection
  - Hierarchical scissor for tiles
  - Multiple scissor rectangles

- [ ] **Occlusion Culling**
  - Early Z-pass for depth prepass
  - Hierarchical Z-buffer (Hi-Z)
  - Software occlusion queries

#### 3. Primitive Features
- [ ] **Line and Point Rendering**
  - Configurable line width
  - Line anti-aliasing
  - Point sprites with size
  - Smooth points

- [ ] **Primitive Restart**
  - Special index value to restart strips
  - Reduces draw call overhead
  - Better batching support

#### 4. Performance Optimizations
- [ ] **Vertex Cache Optimization**
  - Post-transform vertex cache simulation
  - Index buffer optimization
  - FIFO cache replacement

- [ ] **Triangle Binning**
  - Sort triangles into screen-space bins
  - Process bins independently
  - Better for parallelization

- [ ] **SIMD Optimizations**
  - SSE/AVX matrix multiplication
  - Vectorized transformation
  - SIMD frustum testing

### 📊 Performance Metrics & Targets

**Current Performance:**
- ~1M triangles/second (single-threaded, no texturing)
- 60 FPS for ~16K triangle scenes
- Memory bandwidth limited for large triangles

**Target Performance:**
- 5M triangles/second with SIMD
- 10M+ with multi-threading
- Maintain 60 FPS for 80K triangle scenes

### 🔍 Debugging & Profiling Tools Needed

- [ ] **Pipeline Statistics**
  - Triangles processed/culled/clipped
  - Pixels rendered/rejected
  - Cache hit rates

- [ ] **Visualization Modes**
  - Wireframe overlay
  - Depth buffer visualization
  - Overdraw visualization
  - Mipmap level display

- [ ] **Performance Counters**
  - Per-stage timing
  - Memory bandwidth usage
  - Cache misses

### 📝 Implementation Priority

1. **Fix Normal Matrix** (Critical for lighting)
2. **Improve Clipping Robustness** (Quality)
3. **Add Pipeline State Objects** (Architecture)
4. **Implement Scissor Test** (Feature)
5. **Add SIMD Optimizations** (Performance)
6. **Implement Early-Z** (Performance)

### 🎯 Phase 1 Completion Criteria

Before moving to Phase 2 (Texture Mapping), ensure:
- ✅ All vertex attributes properly transformed
- ✅ Clipping handles all edge cases robustly
- ✅ Normal transformation works with all scales
- ✅ Pipeline state properly managed
- ✅ Basic performance optimizations in place
- ✅ Can render 50K+ triangle meshes at 60 FPS

---

## Phase 2: Texture Mapping System 🎯 (Critical)

### 2.1 Texture Infrastructure
- [ ] **Texture class**
  - Image loading (PNG, JPEG support via stb_image)
  - Mipmap generation
  - Texture coordinate storage

### 2.2 UV Coordinate Pipeline
- [ ] **Vertex UV handling**
  - Pass UV coordinates through vertex shader
  - Perspective-correct UV interpolation in rasterizer
  - Barycentric coordinate calculation for texture sampling

### 2.3 Texture Sampling
- [ ] **Basic sampling**
  - Point sampling (nearest neighbor)
  - Bilinear filtering
  - Texture wrapping modes (repeat, clamp, mirror)
- [ ] **Advanced sampling**
  - Trilinear filtering with mipmaps
  - Anisotropic filtering (basic implementation)

---

## Phase 3: Fragment Shader System 🎨

### 3.1 Fragment Shader Architecture
- [ ] **Fragment shader base class**
  - Input: interpolated vertex attributes
  - Output: final pixel color
  - Uniform variable support

### 3.2 Shader Types
- [ ] **Basic shaders**
  - Textured shader
  - Vertex color shader
  - Solid color shader
- [ ] **Lighting shaders**
  - Phong lighting shader
  - Blinn-Phong shader
  - Normal mapping shader

---

## Phase 4: Lighting System 💡

### 4.1 Light Types
- [ ] **Directional lights**
  - Sun-like lighting
  - Shadow mapping support
- [ ] **Point lights**
  - Attenuation
  - Range limiting
- [ ] **Spot lights**
  - Cone angle
  - Soft edges

### 4.2 Lighting Models
- [ ] **Basic lighting**
  - Ambient component
  - Diffuse (Lambertian) shading
  - Specular highlights (Phong/Blinn-Phong)
- [ ] **Advanced lighting**
  - Normal mapping
  - Multiple light sources
  - HDR tone mapping

---

## Phase 5: 3D Model Loading 📦

### 5.1 File Format Support
- [ ] **OBJ loader** (Priority 1)
  - Vertex positions
  - UV coordinates
  - Normals
  - Face indices
  - Material references
- [ ] **glTF 2.0 loader** (Priority 2)
  - Scene graph
  - Animations
  - PBR materials
  - Binary data

### 5.2 Material System
- [ ] **Material class**
  - Diffuse texture/color
  - Specular properties
  - Normal maps
  - Emission maps
- [ ] **Material loading**
  - MTL file support (for OBJ)
  - glTF material support

---

## Phase 6: Mesh Management System 🗄️

### 6.1 Mesh Architecture
- [ ] **Mesh class**
  - Vertex buffer management
  - Index buffer management
  - Multiple submeshes with materials
  - Bounding box calculation

### 6.2 Scene Management
- [ ] **Scene graph**
  - Hierarchical transformations
  - Parent-child relationships
  - Efficient culling with bounding volumes
- [ ] **Resource management**
  - Texture caching
  - Mesh instance sharing
  - Material instance management

---

## Phase 7: Advanced Rendering Features 🚀

### 7.1 Shadow Mapping
- [ ] **Basic shadows**
  - Shadow map generation
  - PCF filtering
  - Cascade shadow maps for large scenes

### 7.2 Post-Processing
- [ ] **Screen-space effects**
  - FXAA anti-aliasing
  - Bloom
  - Tone mapping
  - Color grading

### 7.3 Transparency
- [ ] **Alpha blending**
  - Sorting transparent objects
  - Alpha testing
  - Order-independent transparency (basic)

---

## Phase 8: Performance Optimization ⚡

### 8.1 CPU Optimization
- [ ] **Multi-threading**
  - Parallel triangle rasterization
  - Thread pool for work distribution
  - Lock-free framebuffer writes
- [ ] **SIMD optimization**
  - SSE/AVX for matrix operations
  - Vectorized rasterization
  - Batch vertex processing

### 8.2 Culling Optimization
- [ ] **Hierarchical culling**
  - Octree/BVH for scene
  - Early Z-rejection
  - Tile-based rendering

---

## Implementation Priority Order

1. **Texture Mapping** (Phase 2) - Essential for rendering any real 3D model
2. **OBJ Model Loading** (Phase 5.1) - Allows loading actual 3D content
3. **Basic Lighting** (Phase 4.2) - Makes models look 3D
4. **Fragment Shaders** (Phase 3) - Enables proper material rendering
5. **Material System** (Phase 5.2) - Complete model appearance
6. **Performance Optimization** (Phase 8) - Make it practical
7. **Advanced Features** (Phase 7) - Polish and quality

---

## Milestone Targets

### Milestone 1: Textured Cube
- Complete texture mapping system
- Render a textured cube with proper UV mapping
- Basic fragment shader support

### Milestone 2: Stanford Bunny
- OBJ file loading
- Proper normal interpolation
- Basic Phong lighting

### Milestone 3: Sponza Scene
- Multiple meshes and materials
- Texture atlas support
- Basic shadow mapping
- 30+ FPS performance target

### Milestone 4: Character Model
- Skinned mesh support
- Normal mapping
- Multiple texture channels
- Full material properties

---

## Technical Requirements

### Minimum Features for "Complete" 3D Model Rendering
1. ✅ Vertex transformation and projection
2. ✅ Triangle rasterization with depth testing
3. ❌ **Texture mapping with UV coordinates**
4. ❌ **Per-pixel lighting calculations**
5. ❌ **Model file loading (OBJ minimum)**
6. ❌ **Material support (diffuse, specular)**
7. ❌ **Normal interpolation for smooth shading**

### Current Status
- **Pipeline**: 70% complete (missing texture and fragment processing)
- **Model Loading**: 0% complete
- **Lighting**: 0% complete
- **Materials**: 0% complete
- **Performance**: Adequate for simple scenes

---

## Next Immediate Steps

1. **Implement Texture class**
   - Create Texture.h/cpp
   - Add stb_image for loading
   - Basic sampling functions

2. **Update Vertex structure**
   - Add UV coordinates
   - Update all pipeline stages to handle UVs

3. **Modify Rasterizer**
   - Add UV interpolation
   - Implement perspective-correct interpolation
   - Add texture sampling in pixel loop

4. **Create Fragment Shader system**
   - Design shader interface
   - Implement basic textured shader
   - Hook into rasterization pipeline

This roadmap provides a clear path from the current wireframe/solid-color renderer to a fully-featured 3D model renderer with modern graphics capabilities.