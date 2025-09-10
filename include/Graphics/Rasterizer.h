#pragma once

#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Graphics/Vertex.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/FragmentShader.h"
#include "Graphics/VertexShader.h"
#include <algorithm>
#include <functional>

// Scissor rectangle for clipping
struct ScissorRect {
    int left, top, right, bottom;
    bool enabled;
    
    ScissorRect() : left(0), top(0), right(0), bottom(0), enabled(false) {}
    ScissorRect(int l, int t, int r, int b, bool e = true) 
        : left(l), top(t), right(r), bottom(b), enabled(e) {}
};

// Edge equation for half-space testing
struct EdgeEquation {
    float a, b, c;  // ax + by + c = 0
    
    EdgeEquation() : a(0), b(0), c(0) {}
    
    // Create edge equation from two points (counter-clockwise order)
    EdgeEquation(const Vec2& v0, const Vec2& v1) {
        a = v0.y - v1.y;
        b = v1.x - v0.x;
        c = -(a * v0.x + b * v0.y);
        
        // Normalize for consistent evaluation
        float len = sqrt(a * a + b * b);
        if (len > 0) {
            a /= len;
            b /= len;
            c /= len;
        }
    }
    
    // Evaluate edge equation at point (x, y)
    float evaluate(float x, float y) const {
        return a * x + b * y + c;
    }
    
    // Test if point is inside (positive side of edge)
    bool inside(float x, float y) const {
        return evaluate(x, y) >= 0;
    }
};

// Triangle setup data for rasterization
struct TriangleSetup {
    // Screen space vertices
    Vec3 v0, v1, v2;
    
    // Edge equations
    EdgeEquation edge0, edge1, edge2;
    
    // Bounding box
    int minX, minY, maxX, maxY;
    
    // Area of triangle (2x the actual area)
    float area2;
    
    // Setup triangle for rasterization
    void setup(const Vec3& p0, const Vec3& p1, const Vec3& p2, int screenWidth, int screenHeight, 
               const ScissorRect* scissor = nullptr) {
        v0 = p0;
        v1 = p1;
        v2 = p2;
        
        // Setup edge equations (counter-clockwise)
        edge0 = EdgeEquation(Vec2(v1.x, v1.y), Vec2(v2.x, v2.y));
        edge1 = EdgeEquation(Vec2(v2.x, v2.y), Vec2(v0.x, v0.y));
        edge2 = EdgeEquation(Vec2(v0.x, v0.y), Vec2(v1.x, v1.y));
        
        // Calculate area (2x for efficiency)
        area2 = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);
        
        // Calculate bounding box
        minX = std::max(0, (int)std::min({v0.x, v1.x, v2.x}));
        minY = std::max(0, (int)std::min({v0.y, v1.y, v2.y}));
        maxX = std::min(screenWidth - 1, (int)std::max({v0.x, v1.x, v2.x}));
        maxY = std::min(screenHeight - 1, (int)std::max({v0.y, v1.y, v2.y}));
        
        // Apply scissor rect if enabled
        if (scissor && scissor->enabled) {
            minX = std::max(minX, scissor->left);
            minY = std::max(minY, scissor->top);
            maxX = std::min(maxX, scissor->right);
            maxY = std::min(maxY, scissor->bottom);
        }
    }
    
    // Check if triangle is degenerate or back-facing
    bool isValid() const {
        return area2 > 0.01f; // Small epsilon for numerical stability
    }
};

// Advanced triangle rasterizer with multiple algorithms
class Rasterizer {
public:
    // Rasterization algorithm types
    enum class Algorithm {
        SCANLINE,       // Traditional scanline algorithm
        EDGE_EQUATION,  // Half-space edge equation method
        HIERARCHICAL    // Hierarchical with tiles
    };
    
    // Callback function for pixel shading
    using PixelShader = std::function<void(int x, int y, const Vec3& barycentric, 
                                          const TransformedVertex& v0, 
                                          const TransformedVertex& v1, 
                                          const TransformedVertex& v2)>;
    
    // Main rasterization functions
    static void RasterizeTriangle(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        Framebuffer* framebuffer,
        Algorithm algorithm = Algorithm::EDGE_EQUATION,
        const ::ScissorRect* scissor = nullptr);
    
    // Rasterization with fragment shader
    static void RasterizeTriangleWithShader(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        Framebuffer* framebuffer,
        FragmentShader* shader,
        const ShaderUniforms& uniforms,
        Algorithm algorithm = Algorithm::EDGE_EQUATION,
        const ::ScissorRect* scissor = nullptr);
    
    // Edge equation-based rasterization
    static void RasterizeTriangleEdgeEquation(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        Framebuffer* framebuffer,
        const ::ScissorRect* scissor = nullptr);
    
    // Edge equation-based rasterization with shader
    static void RasterizeTriangleEdgeEquationWithShader(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        Framebuffer* framebuffer,
        FragmentShader* shader,
        const ShaderUniforms& uniforms,
        const ::ScissorRect* scissor = nullptr);
    
    // Scanline rasterization (existing method)
    static void RasterizeTriangleScanline(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        Framebuffer* framebuffer,
        const ::ScissorRect* scissor = nullptr);
    
    // Hierarchical rasterization with tiles
    static void RasterizeTriangleHierarchical(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        Framebuffer* framebuffer,
        int tileSize = 8,
        const ::ScissorRect* scissor = nullptr);
    
    // Compute optimized barycentric coordinates using edge equations
    static Vec3 ComputeBarycentricFast(float x, float y, const TriangleSetup& setup);
    
    // Compute standard barycentric coordinates
    static Vec3 ComputeBarycentric(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c);
    
private:
    // Helper to interpolate vertex attributes
    static TransformedVertex InterpolateVertex(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        const Vec3& barycentric);
    
    // Rasterize a single pixel
    static void RasterizePixel(
        int x, int y,
        const Vec3& barycentric,
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2,
        Framebuffer* framebuffer);
    
    // Check if tile overlaps triangle (for hierarchical)
    static bool TileOverlapsTriangle(
        int tileMinX, int tileMinY,
        int tileMaxX, int tileMaxY,
        const TriangleSetup& setup);
};