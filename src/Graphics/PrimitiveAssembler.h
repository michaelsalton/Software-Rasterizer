#pragma once

#include "Vertex.h"
#include "../Math/Vec3.h"
#include <vector>
#include <array>

// Primitive topology types
enum class PrimitiveType {
    TRIANGLES,          // Every 3 vertices form a triangle
    TRIANGLE_STRIP,     // Each vertex forms a triangle with previous 2
    TRIANGLE_FAN,       // First vertex is shared by all triangles
    LINES,              // Every 2 vertices form a line
    LINE_STRIP,         // Each vertex connects to the previous
    LINE_LOOP,          // Like strip but last connects to first
    POINTS              // Each vertex is a point
};

// Face culling modes
enum class CullMode {
    NONE,               // No culling
    BACK,               // Cull back-facing triangles (default)
    FRONT,              // Cull front-facing triangles
    FRONT_AND_BACK      // Cull all triangles (useful for z-pass)
};

// Winding order for front face determination
enum class WindingOrder {
    CCW,                // Counter-clockwise (default OpenGL)
    CW                  // Clockwise
};

// Triangle primitive after assembly
struct Triangle {
    std::array<TransformedVertex, 3> vertices;
    Vec3 faceNormal;    // Normal in screen space for culling
    bool isFrontFacing;
    
    Triangle() : isFrontFacing(true) {}
    Triangle(const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2)
        : vertices{v0, v1, v2}, isFrontFacing(true) {}
};

// Primitive assembler - converts vertex streams into triangles
class PrimitiveAssembler {
public:
    PrimitiveAssembler();
    
    // Set culling mode
    void SetCullMode(CullMode mode) { cullMode = mode; }
    CullMode GetCullMode() const { return cullMode; }
    
    // Set winding order for front faces
    void SetWindingOrder(WindingOrder order) { windingOrder = order; }
    WindingOrder GetWindingOrder() const { return windingOrder; }
    
    // Assemble primitives from indexed vertices
    std::vector<Triangle> AssembleIndexed(
        const std::vector<TransformedVertex>& vertices,
        const std::vector<int>& indices,
        PrimitiveType primitiveType = PrimitiveType::TRIANGLES);
    
    // Assemble primitives from non-indexed vertices
    std::vector<Triangle> AssembleNonIndexed(
        const std::vector<TransformedVertex>& vertices,
        PrimitiveType primitiveType = PrimitiveType::TRIANGLES);
    
    // Perform culling on assembled triangles
    std::vector<Triangle> CullTriangles(const std::vector<Triangle>& triangles);
    
private:
    CullMode cullMode;
    WindingOrder windingOrder;
    
    // Triangle assembly functions for different primitive types
    std::vector<Triangle> AssembleTriangles(
        const std::vector<TransformedVertex>& vertices,
        const std::vector<int>* indices = nullptr);
    
    std::vector<Triangle> AssembleTriangleStrip(
        const std::vector<TransformedVertex>& vertices,
        const std::vector<int>* indices = nullptr);
    
    std::vector<Triangle> AssembleTriangleFan(
        const std::vector<TransformedVertex>& vertices,
        const std::vector<int>* indices = nullptr);
    
    // Helper to get vertex by index (handles both indexed and non-indexed)
    const TransformedVertex& GetVertex(
        const std::vector<TransformedVertex>& vertices,
        const std::vector<int>* indices,
        size_t index) const;
    
    // Determine if triangle is front-facing based on winding order
    bool IsFrontFacing(const Triangle& triangle) const;
    
    // Calculate face normal in screen space
    Vec3 CalculateFaceNormal(const Triangle& triangle) const;
    
    // Check if triangle should be culled
    bool ShouldCull(const Triangle& triangle) const;
};