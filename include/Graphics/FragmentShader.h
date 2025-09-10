#pragma once

#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Graphics/VertexShader.h"

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

// Simple lit fragment shader
class LitFragmentShader : public FragmentShader {
public:
    FragmentOutput Shade(const FragmentInput& input,
                        const ShaderUniforms& /*uniforms*/) override {
        FragmentOutput output;
        
        // Simple diffuse lighting
        Vec3 lightDir = Vec3(0.5f, 1.0f, 0.5f).normalized();
        float NdotL = std::max(0.0f, input.worldNormal.dot(lightDir));
        
        // Ambient + diffuse
        Vec3 ambient = Vec3(0.2f, 0.2f, 0.2f);
        Vec3 diffuse = Vec3(input.color.x, input.color.y, input.color.z) * NdotL;
        
        Vec3 finalColor = ambient + diffuse;
        output.color = Vec4(finalColor.x, finalColor.y, finalColor.z, input.color.w);
        output.depth = input.depth;
        output.discard = false;
        
        return output;
    }
};