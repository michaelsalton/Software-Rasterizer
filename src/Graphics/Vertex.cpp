#include "Graphics/vertex.h"
#include <algorithm>

namespace VertexProcessor {

TransformedVertex ProcessVertex(const Vertex& vertex, 
                              const Mat4& modelMatrix,
                              const Mat4& viewMatrix,
                              const Mat4& projectionMatrix,
                              int viewportWidth,
                              int viewportHeight) {
    TransformedVertex result;
    
    // Transform position through MVP pipeline
    Vec4 worldPos = modelMatrix * Vec4(vertex.position, 1.0f);
    result.worldPosition = worldPos.xyz();
    
    Vec4 viewPos = viewMatrix * worldPos;
    result.clipPosition = projectionMatrix * viewPos;
    
    // Transform normal to world space (using inverse transpose of model matrix)
    // For now, we'll use the regular model matrix (works for uniform scaling)
    result.worldNormal = TransformNormal(vertex.normal, modelMatrix);
    
    // Pass through texture coordinates and color
    result.texCoord = vertex.texCoord;
    result.color = vertex.color;
    
    // Convert to screen space
    result.screenPosition = ClipToScreen(result.clipPosition, viewportWidth, viewportHeight);
    
    // Calculate inverse W for perspective-correct interpolation
    result.invW = (result.clipPosition.w != 0.0f) ? 1.0f / result.clipPosition.w : 1.0f;
    
    return result;
}

Vec4 TransformToClipSpace(const Vec3& position,
                         const Mat4& modelMatrix,
                         const Mat4& viewMatrix,
                         const Mat4& projectionMatrix) {
    Vec4 worldPos = modelMatrix * Vec4(position, 1.0f);
    Vec4 viewPos = viewMatrix * worldPos;
    return projectionMatrix * viewPos;
}

Vec3 TransformNormal(const Vec3& normal, const Mat4& normalMatrix) {
    // Transform normal using the normal matrix (inverse transpose of model matrix)
    // This correctly handles non-uniform scaling
    Vec3 transformedNormal = normalMatrix.transformDirection(normal);
    return transformedNormal.normalized();
}

Vec3 ClipToScreen(const Vec4& clipPos, int viewportWidth, int viewportHeight) {
    // Perspective divide
    Vec3 ndcPos;
    if (clipPos.w != 0) {
        ndcPos.x = clipPos.x / clipPos.w;
        ndcPos.y = clipPos.y / clipPos.w;
        ndcPos.z = clipPos.z / clipPos.w;
    } else {
        ndcPos = clipPos.xyz();
    }
    
    // Viewport transformation
    float screenX = (ndcPos.x + 1.0f) * 0.5f * viewportWidth;
    float screenY = (1.0f - ndcPos.y) * 0.5f * viewportHeight;  // Flip Y
    
    // Clamp to viewport bounds
    screenX = std::max(0.0f, std::min(screenX, (float)(viewportWidth - 1)));
    screenY = std::max(0.0f, std::min(screenY, (float)(viewportHeight - 1)));
    
    // Z remains in [0, 1] range for depth testing
    float screenZ = (ndcPos.z + 1.0f) * 0.5f;
    screenZ = std::max(0.0f, std::min(screenZ, 1.0f));
    
    return Vec3(screenX, screenY, screenZ);
}

TransformedVertex InterpolateVertex(const TransformedVertex& v0,
                                  const TransformedVertex& v1,
                                  const TransformedVertex& v2,
                                  float w0, float w1, float w2) {
    TransformedVertex result;
    
    // For perspective-correct interpolation, we need to:
    // 1. Interpolate 1/w
    // 2. Interpolate attributes divided by w
    // 3. Divide by interpolated 1/w to get final values
    
    // Interpolate inverse W
    float interpolatedInvW = v0.invW * w0 + v1.invW * w1 + v2.invW * w2;
    result.invW = interpolatedInvW;
    
    // Calculate the reciprocal for perspective correction
    float perspectiveCorrection = 1.0f / interpolatedInvW;
    
    // Interpolate positions (world space doesn't need perspective correction)
    result.worldPosition = v0.worldPosition * w0 + v1.worldPosition * w1 + v2.worldPosition * w2;
    result.worldNormal = (v0.worldNormal * w0 + v1.worldNormal * w1 + v2.worldNormal * w2).normalized();
    
    // Perspective-correct texture coordinate interpolation
    Vec2 texU = v0.texCoord * (w0 * v0.invW) + 
                v1.texCoord * (w1 * v1.invW) + 
                v2.texCoord * (w2 * v2.invW);
    result.texCoord = texU * perspectiveCorrection;
    
    // Perspective-correct color interpolation
    float r = v0.color.r * (w0 * v0.invW) + 
              v1.color.r * (w1 * v1.invW) + 
              v2.color.r * (w2 * v2.invW);
    float g = v0.color.g * (w0 * v0.invW) + 
              v1.color.g * (w1 * v1.invW) + 
              v2.color.g * (w2 * v2.invW);
    float b = v0.color.b * (w0 * v0.invW) + 
              v1.color.b * (w1 * v1.invW) + 
              v2.color.b * (w2 * v2.invW);
    float a = v0.color.a * (w0 * v0.invW) + 
              v1.color.a * (w1 * v1.invW) + 
              v2.color.a * (w2 * v2.invW);
    
    // Apply perspective correction
    r *= perspectiveCorrection;
    g *= perspectiveCorrection;
    b *= perspectiveCorrection;
    a *= perspectiveCorrection;
    
    result.color.r = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, r)));
    result.color.g = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, g)));
    result.color.b = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, b)));
    result.color.a = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, a)));
    
    // Screen position interpolation (already in screen space, linear is correct)
    result.screenPosition = v0.screenPosition * w0 + v1.screenPosition * w1 + v2.screenPosition * w2;
    
    // Clip position interpolation (if needed for further processing)
    result.clipPosition = v0.clipPosition * w0 + v1.clipPosition * w1 + v2.clipPosition * w2;
    
    return result;
}

} // namespace VertexProcessor