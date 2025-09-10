# Phase 3: Lighting System Specification

## Overview
The lighting system will implement a comprehensive illumination model supporting multiple light types, various shading models, and both per-vertex and per-pixel lighting calculations. This system will integrate with the existing fragment shader architecture to provide realistic lighting effects.

## Architecture Overview

### Core Components
1. **Light Base Classes** - Abstract interfaces for different light types
2. **Lighting Models** - Phong, Blinn-Phong, and extensible models
3. **Light Manager** - Scene-wide light management and culling
4. **Shader Integration** - Fragment shader lighting calculations
5. **Material Properties** - Surface properties for lighting interaction

## 1. Light System Architecture

### 1.1 Base Light Class
```cpp
class Light {
public:
    enum LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };

protected:
    LightType type;
    Vec3 color;
    float intensity;
    bool enabled;
    bool castsShadows;

public:
    virtual ~Light() = default;
    
    // Core light interface
    virtual Vec3 getDirectionToLight(const Vec3& worldPos) const = 0;
    virtual float getAttenuation(const Vec3& worldPos) const = 0;
    virtual Vec3 getLightContribution(const Vec3& worldPos) const = 0;
    
    // Common properties
    void setColor(const Vec3& color);
    void setIntensity(float intensity);
    Vec3 getColor() const { return color; }
    float getIntensity() const { return intensity; }
    LightType getType() const { return type; }
};
```

### 1.2 Directional Light
```cpp
class DirectionalLight : public Light {
private:
    Vec3 direction;  // Normalized direction TO the light
    
public:
    DirectionalLight(const Vec3& direction, const Vec3& color, float intensity);
    
    Vec3 getDirectionToLight(const Vec3& worldPos) const override {
        return -direction;  // Direction TO light source
    }
    
    float getAttenuation(const Vec3& worldPos) const override {
        return 1.0f;  // No attenuation for directional lights
    }
    
    Vec3 getLightContribution(const Vec3& worldPos) const override {
        return color * intensity;
    }
};
```

### 1.3 Point Light
```cpp
class PointLight : public Light {
private:
    Vec3 position;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float range;  // Maximum effective range
    
public:
    PointLight(const Vec3& position, const Vec3& color, float intensity);
    
    Vec3 getDirectionToLight(const Vec3& worldPos) const override {
        return (position - worldPos).normalized();
    }
    
    float getAttenuation(const Vec3& worldPos) const override {
        float distance = (position - worldPos).length();
        if (distance > range) return 0.0f;
        
        return 1.0f / (constantAttenuation + 
                      linearAttenuation * distance + 
                      quadraticAttenuation * distance * distance);
    }
    
    // Attenuation presets
    void setAttenuationLinear();     // Good for medium range
    void setAttenuationQuadratic();  // Realistic falloff
    void setAttenuationConstant();   // No falloff
};
```

### 1.4 Spot Light
```cpp
class SpotLight : public Light {
private:
    Vec3 position;
    Vec3 direction;
    float innerConeAngle;  // Full intensity within this angle
    float outerConeAngle;  // Zero intensity beyond this angle
    float range;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    
public:
    SpotLight(const Vec3& position, const Vec3& direction, 
              float innerAngle, float outerAngle,
              const Vec3& color, float intensity);
    
    float getSpotFactor(const Vec3& worldPos) const {
        Vec3 lightDir = (position - worldPos).normalized();
        float cosAngle = lightDir.dot(direction);
        float cosInner = cos(innerConeAngle);
        float cosOuter = cos(outerConeAngle);
        
        // Smooth falloff between inner and outer cone
        return Math::smoothstep(cosOuter, cosInner, cosAngle);
    }
};
```

## 2. Material System

### 2.1 Material Properties
```cpp
struct Material {
    // Base color properties
    Vec3 albedo;          // Base color/diffuse
    Vec3 specular;        // Specular color
    Vec3 emissive;        // Self-illumination
    
    // Surface properties
    float roughness;      // 0 = smooth, 1 = rough
    float metallic;       // 0 = dielectric, 1 = metal
    float shininess;      // Specular exponent for Phong
    
    // Texture indices (-1 if not used)
    int albedoTexture;
    int normalTexture;
    int specularTexture;
    int emissiveTexture;
    
    // Shader selection
    enum ShadingModel {
        PHONG,
        BLINN_PHONG,
        LAMBERT,
        UNLIT
    } shadingModel;
};
```

## 3. Lighting Models Implementation

### 3.1 Phong Lighting Model
```cpp
class PhongLighting {
public:
    static Vec3 calculateLighting(
        const Vec3& worldPos,
        const Vec3& worldNormal,
        const Vec3& viewDir,
        const Material& material,
        const std::vector<Light*>& lights,
        const Vec3& ambientLight
    ) {
        Vec3 result = material.albedo * ambientLight;
        
        for (const Light* light : lights) {
            if (!light->isEnabled()) continue;
            
            // Get light properties
            Vec3 lightDir = light->getDirectionToLight(worldPos);
            float attenuation = light->getAttenuation(worldPos);
            Vec3 lightColor = light->getLightContribution(worldPos);
            
            // Diffuse component (Lambertian)
            float NdotL = Math::max(0.0f, worldNormal.dot(lightDir));
            Vec3 diffuse = material.albedo * NdotL;
            
            // Specular component (Phong)
            Vec3 reflectDir = reflect(-lightDir, worldNormal);
            float RdotV = Math::max(0.0f, reflectDir.dot(viewDir));
            float specFactor = pow(RdotV, material.shininess);
            Vec3 specular = material.specular * specFactor;
            
            // Combine with attenuation
            result += (diffuse + specular) * lightColor * attenuation;
        }
        
        // Add emissive
        result += material.emissive;
        
        return result;
    }
    
private:
    static Vec3 reflect(const Vec3& incident, const Vec3& normal) {
        return incident - normal * 2.0f * incident.dot(normal);
    }
};
```

### 3.2 Blinn-Phong Lighting Model
```cpp
class BlinnPhongLighting {
public:
    static Vec3 calculateLighting(
        const Vec3& worldPos,
        const Vec3& worldNormal,
        const Vec3& viewDir,
        const Material& material,
        const std::vector<Light*>& lights,
        const Vec3& ambientLight
    ) {
        Vec3 result = material.albedo * ambientLight;
        
        for (const Light* light : lights) {
            if (!light->isEnabled()) continue;
            
            Vec3 lightDir = light->getDirectionToLight(worldPos);
            float attenuation = light->getAttenuation(worldPos);
            Vec3 lightColor = light->getLightContribution(worldPos);
            
            // Diffuse
            float NdotL = Math::max(0.0f, worldNormal.dot(lightDir));
            Vec3 diffuse = material.albedo * NdotL;
            
            // Specular (Blinn-Phong)
            Vec3 halfVector = (lightDir + viewDir).normalized();
            float NdotH = Math::max(0.0f, worldNormal.dot(halfVector));
            float specFactor = pow(NdotH, material.shininess * 4.0f);
            Vec3 specular = material.specular * specFactor;
            
            result += (diffuse + specular) * lightColor * attenuation;
        }
        
        result += material.emissive;
        return result;
    }
};
```

## 4. Light Manager

### 4.1 Scene Light Management
```cpp
class LightManager {
private:
    std::vector<std::unique_ptr<Light>> lights;
    Vec3 ambientLight;
    
    // Light limits
    static constexpr size_t MAX_LIGHTS = 16;
    static constexpr size_t MAX_ACTIVE_LIGHTS = 8;
    
public:
    // Light management
    void addLight(std::unique_ptr<Light> light);
    void removeLight(size_t index);
    void clear();
    
    // Ambient light
    void setAmbientLight(const Vec3& ambient) { ambientLight = ambient; }
    Vec3 getAmbientLight() const { return ambientLight; }
    
    // Get lights affecting a position (with culling)
    std::vector<Light*> getLightsAffectingPosition(const Vec3& worldPos) const;
    
    // Get all active lights
    const std::vector<std::unique_ptr<Light>>& getAllLights() const { 
        return lights; 
    }
};
```

## 5. Fragment Shader Integration

### 5.1 Lit Fragment Shader Base
```cpp
class LitFragmentShader : public FragmentShader {
protected:
    const LightManager* lightManager;
    Material material;
    
public:
    Vec4 shade(const FragmentInput& input) override {
        // Get interpolated attributes
        Vec3 worldPos = input.worldPosition;
        Vec3 worldNormal = input.worldNormal.normalized();
        Vec3 viewDir = (cameraPosition - worldPos).normalized();
        
        // Get texture samples if available
        Vec3 albedo = material.albedo;
        if (material.albedoTexture >= 0 && input.hasTexCoords) {
            albedo = sampleTexture(material.albedoTexture, input.texCoords);
        }
        
        // Calculate lighting based on material's shading model
        Vec3 finalColor;
        switch (material.shadingModel) {
            case Material::PHONG:
                finalColor = PhongLighting::calculateLighting(
                    worldPos, worldNormal, viewDir, material,
                    lightManager->getLightsAffectingPosition(worldPos),
                    lightManager->getAmbientLight()
                );
                break;
                
            case Material::BLINN_PHONG:
                finalColor = BlinnPhongLighting::calculateLighting(
                    worldPos, worldNormal, viewDir, material,
                    lightManager->getLightsAffectingPosition(worldPos),
                    lightManager->getAmbientLight()
                );
                break;
                
            case Material::LAMBERT:
                // Simple diffuse only
                finalColor = calculateLambertian(worldPos, worldNormal);
                break;
                
            case Material::UNLIT:
                finalColor = albedo + material.emissive;
                break;
        }
        
        // Tone mapping and gamma correction
        finalColor = toneMap(finalColor);
        finalColor = gammaCorrect(finalColor);
        
        return Vec4(finalColor, 1.0f);
    }
    
private:
    Vec3 toneMap(const Vec3& color) {
        // Simple Reinhard tone mapping
        return color / (color + Vec3(1.0f));
    }
    
    Vec3 gammaCorrect(const Vec3& color) {
        const float gamma = 2.2f;
        return Vec3(
            pow(color.x, 1.0f / gamma),
            pow(color.y, 1.0f / gamma),
            pow(color.z, 1.0f / gamma)
        );
    }
};
```

## 6. Advanced Features

### 6.1 Normal Mapping
```cpp
class NormalMappedShader : public LitFragmentShader {
    Vec3 perturbNormal(const Vec3& worldNormal, const Vec3& tangent, 
                      const Vec2& texCoords) {
        // Sample normal map
        Vec3 normalMapSample = sampleTexture(material.normalTexture, texCoords);
        
        // Convert from [0,1] to [-1,1]
        normalMapSample = normalMapSample * 2.0f - Vec3(1.0f);
        
        // Build TBN matrix
        Vec3 N = worldNormal.normalized();
        Vec3 T = tangent.normalized();
        Vec3 B = N.cross(T);
        
        // Transform normal from tangent space to world space
        return Mat3(T, B, N) * normalMapSample;
    }
};
```

### 6.2 Shadow Mapping (Phase 3.5)
```cpp
struct ShadowMap {
    Framebuffer depthBuffer;
    Mat4 lightViewProjection;
    float bias;
    
    bool isInShadow(const Vec3& worldPos) const {
        // Transform to light space
        Vec4 lightSpacePos = lightViewProjection * Vec4(worldPos, 1.0f);
        
        // Perspective divide
        Vec3 projCoords = lightSpacePos.xyz() / lightSpacePos.w;
        
        // Transform to [0,1] range
        projCoords = projCoords * 0.5f + 0.5f;
        
        // Sample shadow map
        float closestDepth = depthBuffer.sampleDepth(projCoords.x, projCoords.y);
        float currentDepth = projCoords.z;
        
        // Check if current fragment is in shadow
        return currentDepth - bias > closestDepth;
    }
};
```

## 7. Implementation Plan

### Phase 3.1: Basic Lighting Infrastructure
**Week 1-2:**
1. Implement Light base class and DirectionalLight
2. Create basic Material struct
3. Implement Lambertian diffuse shading
4. Integrate with existing fragment shader system

**Deliverables:**
- Working directional light
- Basic diffuse shading
- Simple test scene with rotating light

### Phase 3.2: Complete Light Types
**Week 3:**
1. Implement PointLight with attenuation
2. Implement SpotLight with cone falloff
3. Create LightManager for scene lights
4. Add light culling for performance

**Deliverables:**
- All three light types working
- Multi-light scene support
- Light distance culling

### Phase 3.3: Advanced Shading Models
**Week 4:**
1. Implement Phong specular shading
2. Implement Blinn-Phong shading
3. Add material property system
4. Create different material presets

**Deliverables:**
- Complete Phong/Blinn-Phong shaders
- Material system with presets
- Shiny vs matte material demos

### Phase 3.4: Normal Mapping
**Week 5:**
1. Add tangent space calculations
2. Implement normal map sampling
3. Create normal mapped shader variant
4. Test with various normal maps

**Deliverables:**
- Working normal mapping
- Tangent space calculations
- Visual quality improvements

### Phase 3.5: Shadow Mapping (Optional)
**Week 6:**
1. Implement shadow map generation
2. Add shadow sampling with PCF
3. Handle shadow acne and peter-panning
4. Optimize shadow map resolution

**Deliverables:**
- Basic shadow mapping
- Soft shadows with PCF
- Shadow quality settings

## 8. Performance Considerations

### 8.1 Optimization Strategies
1. **Light Culling**
   - Distance-based culling for point/spot lights
   - Screen-space tile culling
   - Maximum light count per pixel

2. **Shader Variants**
   - Compile different shaders for different light counts
   - Specialized shaders for single light
   - Unlit shader for distant objects

3. **LOD System**
   - Per-vertex lighting for distant objects
   - Simplified shading models at distance
   - Automatic shader LOD selection

### 8.2 Performance Targets
- 60 FPS with 8 active lights
- 30 FPS with 16 active lights
- < 5ms lighting calculation overhead
- Minimal memory overhead (< 10MB for light data)

## 9. Testing and Validation

### 9.1 Test Scenes
1. **Cornell Box** - Validate light transport
2. **Sphere Grid** - Test different materials
3. **Sponza Atrium** - Complex lighting scenario
4. **Character Model** - Skin shading test

### 9.2 Visual Tests
1. Diffuse-only sphere
2. Specular highlight validation
3. Light attenuation curves
4. Shadow map quality
5. Normal map effectiveness

### 9.3 Performance Benchmarks
1. Single light performance
2. Multi-light scaling
3. Shadow map overhead
4. Fragment shader complexity impact

## 10. API Usage Example

```cpp
// Create lights
auto sunLight = std::make_unique<DirectionalLight>(
    Vec3(0, -1, -1).normalized(),  // Direction
    Vec3(1.0f, 0.95f, 0.8f),      // Warm white
    1.0f                          // Intensity
);

auto pointLight = std::make_unique<PointLight>(
    Vec3(5, 10, 5),               // Position
    Vec3(1, 0, 0),                // Red
    100.0f                        // Intensity
);
pointLight->setAttenuationQuadratic();
pointLight->setRange(50.0f);

// Setup light manager
LightManager lightManager;
lightManager.addLight(std::move(sunLight));
lightManager.addLight(std::move(pointLight));
lightManager.setAmbientLight(Vec3(0.1f, 0.1f, 0.15f));

// Create material
Material goldMaterial;
goldMaterial.albedo = Vec3(1.0f, 0.765f, 0.336f);
goldMaterial.specular = Vec3(1.0f, 0.9f, 0.7f);
goldMaterial.shininess = 128.0f;
goldMaterial.shadingModel = Material::BLINN_PHONG;

// Create shader
auto shader = std::make_unique<LitFragmentShader>();
shader->setLightManager(&lightManager);
shader->setMaterial(goldMaterial);

// Use in renderer
renderer.setFragmentShader(shader.get());
```

## Success Criteria

1. **Visual Quality**
   - Smooth shading across surfaces
   - Correct specular highlights
   - Realistic light falloff
   - No lighting artifacts

2. **Performance**
   - Maintains 60 FPS target
   - Scales well with light count
   - Efficient memory usage

3. **Flexibility**
   - Easy to add new light types
   - Simple material authoring
   - Extensible shading models

4. **Integration**
   - Works with existing pipeline
   - Compatible with texture system
   - Supports future features

This comprehensive specification provides a complete roadmap for implementing a professional-quality lighting system in the software rasterizer.