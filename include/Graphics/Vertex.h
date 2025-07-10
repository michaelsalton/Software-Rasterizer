#pragma once

#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Graphics/Framebuffer.h"

// Comprehensive vertex structure with all common attributes
struct Vertex {
    Vec3 position;      // 3D position in object space
    Vec3 normal;        // Surface normal for lighting
    Vec2 texCoord;      // Texture coordinates (UV)
    Framebuffer::Color color;  // Vertex color (RGBA)
    
    // Default constructor
    Vertex() 
        : position(0, 0, 0)
        , normal(0, 1, 0)
        , texCoord(0, 0)
        , color(255, 255, 255, 255) {}
    
    // Position-only constructor
    explicit Vertex(const Vec3& pos)
        : position(pos)
        , normal(0, 1, 0)
        , texCoord(0, 0)
        , color(255, 255, 255, 255) {}
    
    // Full constructor
    Vertex(const Vec3& pos, const Vec3& norm, const Vec2& uv, const Framebuffer::Color& col)
        : position(pos)
        , normal(norm)
        , texCoord(uv)
        , color(col) {}
    
    // Convenience constructor with RGB color
    Vertex(const Vec3& pos, const Vec3& norm, const Vec2& uv, 
           uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : position(pos)
        , normal(norm)
        , texCoord(uv)
        , color(r, g, b, a) {}
};

// Transformed vertex after vertex shader stage
// Contains all interpolatable attributes in clip space
struct TransformedVertex {
    Vec4 clipPosition;   // Position in clip space (before perspective divide)
    Vec3 worldPosition;  // Position in world space (for lighting)
    Vec3 worldNormal;    // Normal in world space (for lighting)
    Vec2 texCoord;       // Texture coordinates (passed through)
    Framebuffer::Color color;  // Vertex color (passed through)
    
    // Screen space position after perspective divide and viewport transform
    Vec3 screenPosition; // x, y in pixels, z is depth [0,1]
};

// Vertex processing pipeline functions
namespace VertexProcessor {
    // Transform a vertex through the full MVP pipeline
    TransformedVertex ProcessVertex(const Vertex& vertex, 
                                  const Mat4& modelMatrix,
                                  const Mat4& viewMatrix,
                                  const Mat4& projectionMatrix,
                                  int viewportWidth,
                                  int viewportHeight);
    
    // Transform position from object space to clip space
    Vec4 TransformToClipSpace(const Vec3& position,
                             const Mat4& modelMatrix,
                             const Mat4& viewMatrix,
                             const Mat4& projectionMatrix);
    
    // Transform normal from object space to world space
    Vec3 TransformNormal(const Vec3& normal, const Mat4& modelMatrix);
    
    // Perform perspective divide and viewport transformation
    Vec3 ClipToScreen(const Vec4& clipPos, int viewportWidth, int viewportHeight);
    
    // Interpolate vertex attributes for rasterization
    TransformedVertex InterpolateVertex(const TransformedVertex& v0,
                                      const TransformedVertex& v1,
                                      const TransformedVertex& v2,
                                      float w0, float w1, float w2);
}