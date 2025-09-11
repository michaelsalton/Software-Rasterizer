# Phase 4: Advanced Lighting System Specification

## Overview
Phase 4 extends the basic lighting system implemented in Phase 3 by adding multiple light types, advanced shading features, and shadow mapping. This phase will transform the renderer from a simple single-light system to a comprehensive lighting engine capable of realistic scene illumination.

## 1. Architecture Overview

### 1.1 Light Type Hierarchy
```
Light (base class)
├── DirectionalLight (existing)
├── PointLight (new)
├── SpotLight (new)
└── AreaLight (future)
```

### 1.2 Lighting Manager
```cpp
class LightingManager {
    std::vector<std::shared_ptr<Light>> lights;
    ShadowMapManager shadowManager;
    
    // Light limits
    static constexpr int MAX_DIRECTIONAL_LIGHTS = 4;
    static constexpr int MAX_POINT_LIGHTS = 32;
    static constexpr int MAX_SPOT_LIGHTS = 16;
};
```

## 2. New Light Types

### 2.1 Point Light
```cpp
class PointLight : public Light {
    Vec3 position;
    Vec3 color;
    float intensity;
    
    // Attenuation parameters
    float constant;
    float linear;
    float quadratic;
    float range;  // Culling distance
};
```

**Attenuation Formula:**
```
attenuation = 1.0 / (constant + linear * d + quadratic * d²)
```

### 2.2 Spot Light
```cpp
class SpotLight : public Light {
    Vec3 position;
    Vec3 direction;
    Vec3 color;
    float intensity;
    
    // Cone parameters
    float innerConeAngle;  // Full intensity cone
    float outerConeAngle;  // Falloff to zero
    
    // Attenuation (same as point light)
    float constant, linear, quadratic;
    float range;
};
```

**Spot Light Falloff:**
```
theta = angle between -lightDir and spotDir
epsilon = innerCone - outerCone
intensity = clamp((theta - outerCone) / epsilon, 0.0, 1.0)
```

## 3. Enhanced Shading Features

### 3.1 Multiple Light Support
- Dynamic light arrays in shaders
- Light culling based on range
- Per-object light lists
- Forward+ rendering preparation

### 3.2 Advanced Material Properties
```cpp
struct Material {
    // Existing
    Vec3 albedo;
    Vec3 specular;
    float shininess;
    
    // New properties
    float metallic;
    float roughness;
    Vec3 emissive;
    float emissiveStrength;
    
    // Texture indices
    int normalMapIndex = -1;
    int specularMapIndex = -1;
    int emissiveMapIndex = -1;
};
```

### 3.3 Normal Mapping
- Tangent space calculation
- TBN matrix construction
- Normal map sampling and transformation
- Compressed normal map support (RG only)

## 4. Shadow Mapping

### 4.1 Basic Shadow Mapping
```cpp
class ShadowMap {
    Framebuffer depthBuffer;
    Mat4 lightViewMatrix;
    Mat4 lightProjMatrix;
    int resolution;  // 512, 1024, 2048
};
```

### 4.2 Implementation Steps
1. **Shadow Pass:**
   - Render scene from light's perspective
   - Store depth only
   - Use orthographic projection for directional lights

2. **Shading Pass:**
   - Transform fragment position to light space
   - Sample shadow map
   - Apply shadow factor to lighting

### 4.3 Shadow Map Improvements
- Percentage Closer Filtering (PCF)
- Shadow bias adjustment
- Cascade shadow maps for large scenes
- Soft shadow edges

## 5. Implementation Plan

### 5.1 Phase 4A: Point and Spot Lights
**Week 1-2:**
1. Implement PointLight class
2. Add attenuation calculations
3. Modify fragment shader for multiple lights
4. Test with multiple point lights

**Week 3:**
1. Implement SpotLight class
2. Add cone attenuation
3. Optimize light culling
4. Create light visualization tools

### 5.2 Phase 4B: Normal Mapping
**Week 4:**
1. Add tangent/bitangent to Vertex struct
2. Calculate TBN matrix in vertex shader
3. Implement normal map sampling
4. Test with various normal maps

### 5.3 Phase 4C: Shadow Mapping
**Week 5-6:**
1. Create shadow map framebuffer
2. Implement shadow pass rendering
3. Add shadow sampling to fragment shader
4. Implement PCF filtering

## 6. Fragment Shader Modifications

### 6.1 Multi-Light Shader Structure
```glsl
vec3 calculateLighting(FragmentInput input) {
    vec3 result = vec3(0.0);
    
    // Ambient
    result += material.albedo * ambientLight;
    
    // Directional lights
    for (int i = 0; i < numDirLights; i++) {
        result += calcDirectionalLight(dirLights[i], input);
    }
    
    // Point lights
    for (int i = 0; i < numPointLights; i++) {
        result += calcPointLight(pointLights[i], input);
    }
    
    // Spot lights
    for (int i = 0; i < numSpotLights; i++) {
        result += calcSpotLight(spotLights[i], input);
    }
    
    return result;
}
```

## 7. Performance Considerations

### 7.1 Light Culling
- Frustum culling for lights
- Distance culling based on attenuation
- Screen-space tile culling
- Early-out for fully shadowed regions

### 7.2 Optimization Strategies
- Light sorting by contribution
- LOD for distant lights
- Deferred shading preparation
- GPU-style light indexing

## 8. Testing and Validation

### 8.1 Test Scenes
1. **Cornell Box:** Multiple colored point lights
2. **Spotlight Gallery:** Various cone angles and colors
3. **Shadow Test:** Complex shadow casters
4. **Normal Map Test:** Brick wall with normal mapping

### 8.2 Performance Targets
- 60 FPS with 8 lights on simple scenes
- 30 FPS with 16 lights and shadows
- Scalable quality settings

## 9. GUI Controls

### 9.1 Light Editor
- Add/remove lights dynamically
- Edit light properties in real-time
- Visualize light positions and cones
- Toggle shadows per light

### 9.2 Material Editor
- Metallic/roughness sliders
- Normal map strength
- Emissive controls

## 10. Future Enhancements

### 10.1 Advanced Shadow Techniques
- Variance Shadow Maps (VSM)
- Exponential Shadow Maps (ESM)
- Ray-traced shadows (future)

### 10.2 Global Illumination
- Screen-space reflections
- Basic environment mapping
- Precomputed radiance transfer

## Implementation Priority

1. **High Priority:**
   - Point lights with attenuation
   - Multiple light support
   - Basic shadow mapping

2. **Medium Priority:**
   - Spot lights
   - Normal mapping
   - PCF shadow filtering

3. **Low Priority:**
   - Advanced shadow techniques
   - HDR rendering
   - Area lights

## Success Criteria

Phase 4 is complete when:
1. ✅ Multiple point and spot lights working
2. ✅ Shadow mapping for directional lights
3. ✅ Normal mapping functional
4. ✅ Performance targets met
5. ✅ GUI controls for all features
6. ✅ Test scenes render correctly