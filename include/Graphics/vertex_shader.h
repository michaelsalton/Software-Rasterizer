#pragma once

#include "Math/mat4.h"
#include "Math/vec4.h"
#include "Graphics/vertex.h"
#include <unordered_map>
#include <string>
#include <any>

// Forward declaration
struct ShaderUniforms;

// Base class for vertex shaders
class VertexShader {
public:
    virtual ~VertexShader() = default;
    
    // Main vertex shader function - transforms input vertex to output vertex
    virtual TransformedVertex Process(const Vertex& input, const ShaderUniforms& uniforms) = 0;
    
    // Optional: Called once before processing a batch of vertices
    virtual void BeginBatch(const ShaderUniforms& /*uniforms*/) {}
    
    // Optional: Called after processing a batch of vertices
    virtual void EndBatch() {}
};

// Container for shader uniform variables
struct ShaderUniforms {
    Mat4 modelMatrix;
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
    Mat4 modelViewMatrix;      // Pre-computed model * view
    Mat4 mvpMatrix;            // Pre-computed model * view * projection
    Mat4 normalMatrix;         // Inverse transpose of model matrix for normals
    
    // Camera info
    Vec3 cameraPosition;       // World space camera position
    
    // Viewport info
    int viewportWidth;
    int viewportHeight;
    
    // Time for animated effects
    float time;
    
    // Custom uniforms storage
    std::unordered_map<std::string, std::any> customUniforms;
    
    // Helper methods
    void SetFloat(const std::string& name, float value) {
        customUniforms[name] = value;
    }
    
    void SetVec3(const std::string& name, const Vec3& value) {
        customUniforms[name] = value;
    }
    
    void SetMat4(const std::string& name, const Mat4& value) {
        customUniforms[name] = value;
    }
    
    float GetFloat(const std::string& name, float defaultValue = 0.0f) const {
        auto it = customUniforms.find(name);
        if (it != customUniforms.end()) {
            try {
                return std::any_cast<float>(it->second);
            } catch(...) {}
        }
        return defaultValue;
    }
    
    Vec3 GetVec3(const std::string& name, const Vec3& defaultValue = Vec3()) const {
        auto it = customUniforms.find(name);
        if (it != customUniforms.end()) {
            try {
                return std::any_cast<Vec3>(it->second);
            } catch(...) {}
        }
        return defaultValue;
    }
    
    Mat4 GetMat4(const std::string& name, const Mat4& defaultValue = Mat4()) const {
        auto it = customUniforms.find(name);
        if (it != customUniforms.end()) {
            try {
                return std::any_cast<Mat4>(it->second);
            } catch(...) {}
        }
        return defaultValue;
    }
    
    // Pre-compute commonly used matrices
    void UpdateDerivedMatrices() {
        modelViewMatrix = viewMatrix * modelMatrix;
        mvpMatrix = projectionMatrix * modelViewMatrix;
        
        // Calculate proper normal matrix (inverse transpose of model matrix)
        normalMatrix = modelMatrix.normalMatrix();
    }
};

// Default vertex shader - standard MVP transformation
class DefaultVertexShader : public VertexShader {
public:
    TransformedVertex Process(const Vertex& input, const ShaderUniforms& uniforms) override;
};

// Wave vertex shader - creates wave motion
class WaveVertexShader : public VertexShader {
public:
    TransformedVertex Process(const Vertex& input, const ShaderUniforms& uniforms) override;
};

// Twist vertex shader - twists geometry around Y axis
class TwistVertexShader : public VertexShader {
public:
    TransformedVertex Process(const Vertex& input, const ShaderUniforms& uniforms) override;
};

// Explode vertex shader - moves vertices along their normals
class ExplodeVertexShader : public VertexShader {
public:
    TransformedVertex Process(const Vertex& input, const ShaderUniforms& uniforms) override;
};

// Spherize vertex shader - morphs geometry towards a sphere
class SpherizeVertexShader : public VertexShader {
public:
    TransformedVertex Process(const Vertex& input, const ShaderUniforms& uniforms) override;
};