#include "Graphics/VertexShader.h"
#include <cmath>

// Default vertex shader implementation
TransformedVertex DefaultVertexShader::Process(const Vertex& input, const ShaderUniforms& uniforms) {
    TransformedVertex output;
    
    // Transform position through MVP pipeline
    Vec4 worldPos = uniforms.modelMatrix * Vec4(input.position, 1.0f);
    output.worldPosition = worldPos.xyz();
    
    // Transform to clip space
    output.clipPosition = uniforms.mvpMatrix * Vec4(input.position, 1.0f);
    
    // Transform normal to world space
    Vec3 transformedNormal = uniforms.normalMatrix.transformDirection(input.normal);
    output.worldNormal = transformedNormal.normalized();
    
    // Pass through texture coordinates and color
    output.texCoord = input.texCoord;
    output.color = input.color;
    
    // Convert to screen space
    output.screenPosition = VertexProcessor::ClipToScreen(
        output.clipPosition, 
        uniforms.viewportWidth, 
        uniforms.viewportHeight
    );
    
    // Calculate inverse W for perspective-correct interpolation
    output.invW = (output.clipPosition.w != 0.0f) ? 1.0f / output.clipPosition.w : 1.0f;
    
    return output;
}

// Wave vertex shader - creates wave motion based on position and time
TransformedVertex WaveVertexShader::Process(const Vertex& input, const ShaderUniforms& uniforms) {
    TransformedVertex output;
    
    // Apply wave displacement
    Vertex modifiedInput = input;
    float amplitude = uniforms.GetFloat("waveAmplitude", 0.2f);
    float frequency = uniforms.GetFloat("waveFrequency", 2.0f);
    float speed = uniforms.GetFloat("waveSpeed", 1.0f);
    
    // Wave along XZ plane affecting Y
    float wave = amplitude * sin(frequency * (input.position.x + input.position.z) + uniforms.time * speed);
    modifiedInput.position.y += wave;
    
    // Recalculate normal (approximate)
    float dx = amplitude * frequency * cos(frequency * (input.position.x + input.position.z) + uniforms.time * speed);
    float dz = dx; // Same derivative for Z
    Vec3 tangentX(1, dx, 0);
    Vec3 tangentZ(0, dz, 1);
    modifiedInput.normal = tangentZ.cross(tangentX).normalized();
    
    // Transform position through MVP pipeline
    Vec4 worldPos = uniforms.modelMatrix * Vec4(modifiedInput.position, 1.0f);
    output.worldPosition = worldPos.xyz();
    
    output.clipPosition = uniforms.mvpMatrix * Vec4(modifiedInput.position, 1.0f);
    
    // Transform modified normal
    Vec3 transformedNormal = uniforms.normalMatrix.transformDirection(modifiedInput.normal);
    output.worldNormal = transformedNormal.normalized();
    
    // Pass through attributes
    output.texCoord = input.texCoord;
    output.color = input.color;
    
    // Convert to screen space
    output.screenPosition = VertexProcessor::ClipToScreen(
        output.clipPosition, 
        uniforms.viewportWidth, 
        uniforms.viewportHeight
    );
    
    // Calculate inverse W for perspective-correct interpolation
    output.invW = (output.clipPosition.w != 0.0f) ? 1.0f / output.clipPosition.w : 1.0f;
    
    return output;
}

// Twist vertex shader - twists geometry around Y axis
TransformedVertex TwistVertexShader::Process(const Vertex& input, const ShaderUniforms& uniforms) {
    TransformedVertex output;
    
    // Apply twist based on Y position
    Vertex modifiedInput = input;
    float twistAmount = uniforms.GetFloat("twistAmount", 1.0f);
    float twistHeight = uniforms.GetFloat("twistHeight", 2.0f);
    
    float angle = (input.position.y / twistHeight) * twistAmount;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    
    // Rotate around Y axis
    float newX = input.position.x * cosAngle - input.position.z * sinAngle;
    float newZ = input.position.x * sinAngle + input.position.z * cosAngle;
    modifiedInput.position.x = newX;
    modifiedInput.position.z = newZ;
    
    // Rotate normal as well
    float normalX = input.normal.x * cosAngle - input.normal.z * sinAngle;
    float normalZ = input.normal.x * sinAngle + input.normal.z * cosAngle;
    modifiedInput.normal.x = normalX;
    modifiedInput.normal.z = normalZ;
    
    // Transform position through MVP pipeline
    Vec4 worldPos = uniforms.modelMatrix * Vec4(modifiedInput.position, 1.0f);
    output.worldPosition = worldPos.xyz();
    
    output.clipPosition = uniforms.mvpMatrix * Vec4(modifiedInput.position, 1.0f);
    
    // Transform normal
    Vec3 transformedNormal = uniforms.normalMatrix.transformDirection(modifiedInput.normal);
    output.worldNormal = transformedNormal.normalized();
    
    // Pass through attributes
    output.texCoord = input.texCoord;
    output.color = input.color;
    
    // Convert to screen space
    output.screenPosition = VertexProcessor::ClipToScreen(
        output.clipPosition, 
        uniforms.viewportWidth, 
        uniforms.viewportHeight
    );
    
    // Calculate inverse W for perspective-correct interpolation
    output.invW = (output.clipPosition.w != 0.0f) ? 1.0f / output.clipPosition.w : 1.0f;
    
    return output;
}

// Explode vertex shader - moves vertices along their normals
TransformedVertex ExplodeVertexShader::Process(const Vertex& input, const ShaderUniforms& uniforms) {
    TransformedVertex output;
    
    // Apply explosion based on time
    float explodeAmount = uniforms.GetFloat("explodeAmount", 0.5f);
    float explodeTime = fmod(uniforms.time * 0.5f, 2.0f); // Loop every 2 seconds
    
    // Smoothstep for nice animation
    float t = explodeTime < 1.0f ? explodeTime : 2.0f - explodeTime;
    t = t * t * (3.0f - 2.0f * t); // Smoothstep
    
    // Move along normal
    Vertex modifiedInput = input;
    modifiedInput.position = input.position + input.normal * (explodeAmount * t);
    
    // Transform position through MVP pipeline
    Vec4 worldPos = uniforms.modelMatrix * Vec4(modifiedInput.position, 1.0f);
    output.worldPosition = worldPos.xyz();
    
    output.clipPosition = uniforms.mvpMatrix * Vec4(modifiedInput.position, 1.0f);
    
    // Transform normal (unchanged)
    Vec3 transformedNormal = uniforms.normalMatrix.transformDirection(input.normal);
    output.worldNormal = transformedNormal.normalized();
    
    // Pass through attributes
    output.texCoord = input.texCoord;
    output.color = input.color;
    
    // Convert to screen space
    output.screenPosition = VertexProcessor::ClipToScreen(
        output.clipPosition, 
        uniforms.viewportWidth, 
        uniforms.viewportHeight
    );
    
    // Calculate inverse W for perspective-correct interpolation
    output.invW = (output.clipPosition.w != 0.0f) ? 1.0f / output.clipPosition.w : 1.0f;
    
    return output;
}

// Spherize vertex shader - morphs geometry towards a sphere
TransformedVertex SpherizeVertexShader::Process(const Vertex& input, const ShaderUniforms& uniforms) {
    TransformedVertex output;
    
    // Spherize based on time
    float spherizeAmount = uniforms.GetFloat("spherizeAmount", 1.0f);
    float spherizeRadius = uniforms.GetFloat("spherizeRadius", 1.0f);
    float t = (sin(uniforms.time) + 1.0f) * 0.5f; // Oscillate between 0 and 1
    
    // Normalize position to get sphere position
    Vec3 spherePos = input.position.normalized() * spherizeRadius;
    
    // Interpolate between original and sphere position
    Vertex modifiedInput = input;
    modifiedInput.position = input.position * (1.0f - t * spherizeAmount) + spherePos * (t * spherizeAmount);
    
    // Interpolate normal too
    Vec3 sphereNormal = spherePos.normalized();
    modifiedInput.normal = (input.normal * (1.0f - t * spherizeAmount) + sphereNormal * (t * spherizeAmount)).normalized();
    
    // Transform position through MVP pipeline
    Vec4 worldPos = uniforms.modelMatrix * Vec4(modifiedInput.position, 1.0f);
    output.worldPosition = worldPos.xyz();
    
    output.clipPosition = uniforms.mvpMatrix * Vec4(modifiedInput.position, 1.0f);
    
    // Transform normal
    Vec3 transformedNormal = uniforms.normalMatrix.transformDirection(modifiedInput.normal);
    output.worldNormal = transformedNormal.normalized();
    
    // Pass through attributes
    output.texCoord = input.texCoord;
    output.color = input.color;
    
    // Convert to screen space
    output.screenPosition = VertexProcessor::ClipToScreen(
        output.clipPosition, 
        uniforms.viewportWidth, 
        uniforms.viewportHeight
    );
    
    // Calculate inverse W for perspective-correct interpolation
    output.invW = (output.clipPosition.w != 0.0f) ? 1.0f / output.clipPosition.w : 1.0f;
    
    return output;
}