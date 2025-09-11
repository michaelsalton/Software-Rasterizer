#pragma once

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "pipeline/vertex_shader.h"
#include "lighting/light.h"
#include "rendering/material.h"
#include "lighting/lighting_calculations.h"
#include <vector>
#include <memory>

// Forward declarations
class Texture;

// Fragment shader input - all interpolated data
struct FragmentInput {
    Vec2 screenPos;      // Pixel position
    float depth;         // Interpolated depth
    Vec3 worldPos;       // World position
    Vec3 worldNormal;    // World normal
    Vec2 texCoord;       // UV coordinates
    Vec4 color;          // Vertex color [0,1]
};

// Fragment shader output
struct FragmentOutput {
    Vec4 color;          // Final pixel color [0,1]
    float depth;         // Modified depth (optional)
    bool discard;        // Discard this fragment
    
    FragmentOutput() : color(0, 0, 0, 1), depth(0), discard(false) {}
};

// Base class for fragment shaders
class FragmentShader {
public:
    virtual ~FragmentShader() = default;
    
    // Main shader function
    virtual FragmentOutput Shade(const FragmentInput& input,
                                const ShaderUniforms& uniforms) = 0;
    
    // Optional: Called once before rendering a batch
    virtual void BeginBatch(const ShaderUniforms& /*uniforms*/) {}
    
    // Optional: Called after rendering a batch
    virtual void EndBatch() {}
};

// Default fragment shader - vertex color only
class DefaultFragmentShader : public FragmentShader {
public:
    FragmentOutput Shade(const FragmentInput& input,
                        const ShaderUniforms& /*uniforms*/) override {
        FragmentOutput output;
        output.color = input.color;
        output.depth = input.depth;
        output.discard = false;
        return output;
    }
};

// Textured fragment shader
class TexturedFragmentShader : public FragmentShader {
public:
    void SetTexture(Texture* texture) { mTexture = texture; }
    
    FragmentOutput Shade(const FragmentInput& input,
                        const ShaderUniforms& uniforms) override;
    
private:
    Texture* mTexture = nullptr;
};

// Enhanced lit fragment shader with material and light support
class LitFragmentShader : public FragmentShader {
protected:
    std::vector<std::shared_ptr<Light>> lights;
    Material material;
    Vec3 ambientLight;
    Texture* albedoTexture;
    
public:
    LitFragmentShader() 
        : ambientLight(0.1f, 0.1f, 0.1f), 
          albedoTexture(nullptr) {}
    
    // Setup functions
    void SetMaterial(const Material& mat) { material = mat; }
    void SetAmbientLight(const Vec3& ambient) { ambientLight = ambient; }
    void SetAlbedoTexture(Texture* texture) { albedoTexture = texture; }
    void AddLight(std::shared_ptr<Light> light) { lights.push_back(light); }
    void ClearLights() { lights.clear(); }
    
    FragmentOutput Shade(const FragmentInput& input,
                        const ShaderUniforms& uniforms) override;
};

// Textured lit fragment shader
class TexturedLitFragmentShader : public LitFragmentShader {
public:
    FragmentOutput Shade(const FragmentInput& input,
                        const ShaderUniforms& uniforms) override;
};