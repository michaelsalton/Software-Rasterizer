#pragma once

#include "math/vec4.h"
#include "pipeline/vertex_processor.h"
#include <vector>
#include <array>

// Clipping plane representation
struct ClipPlane {
    Vec4 normal;    // Plane normal in homogeneous coordinates
    float d;        // Distance from origin
    
    ClipPlane() : normal(0, 0, 0, 0), d(0) {}
    ClipPlane(const Vec4& n, float dist) : normal(n), d(dist) {}
    
    // Test if a point is inside the plane (positive side)
    float Distance(const Vec4& point) const {
        return normal.dot(point) + d;
    }
    
    bool IsInside(const Vec4& point) const {
        return Distance(point) >= 0;
    }
};

// View frustum with 6 clipping planes
struct ViewFrustum {
    enum PlaneType {
        NEAR_PLANE = 0,
        FAR_PLANE = 1,
        LEFT_PLANE = 2,
        RIGHT_PLANE = 3,
        TOP_PLANE = 4,
        BOTTOM_PLANE = 5,
        PLANE_COUNT = 6
    };
    
    std::array<ClipPlane, PLANE_COUNT> planes;
    
    // Initialize frustum planes in clip space
    // In clip space, the view frustum is the unit cube: -w <= x,y,z <= w
    void InitializeClipSpace() {
        // Near plane: z >= -w  =>  0*x + 0*y + 1*z + 1*w >= 0
        planes[NEAR_PLANE] = ClipPlane(Vec4(0, 0, 1, 1), 0);
        
        // Far plane: z <= w  =>  0*x + 0*y + -1*z + 1*w >= 0
        planes[FAR_PLANE] = ClipPlane(Vec4(0, 0, -1, 1), 0);
        
        // Left plane: x >= -w  =>  1*x + 0*y + 0*z + 1*w >= 0
        planes[LEFT_PLANE] = ClipPlane(Vec4(1, 0, 0, 1), 0);
        
        // Right plane: x <= w  =>  -1*x + 0*y + 0*z + 1*w >= 0
        planes[RIGHT_PLANE] = ClipPlane(Vec4(-1, 0, 0, 1), 0);
        
        // Bottom plane: y >= -w  =>  0*x + 1*y + 0*z + 1*w >= 0
        planes[BOTTOM_PLANE] = ClipPlane(Vec4(0, 1, 0, 1), 0);
        
        // Top plane: y <= w  =>  0*x + -1*y + 0*z + 1*w >= 0
        planes[TOP_PLANE] = ClipPlane(Vec4(0, -1, 0, 1), 0);
    }
};

// Polygon clipper using Sutherland-Hodgman algorithm
class Clipper {
public:
    // Test if a triangle is completely outside the frustum (early rejection)
    static bool IsTriangleOutsideFrustum(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2);
    
    // Clip a triangle against the view frustum
    // Returns a list of vertices forming the clipped polygon (0 to 9 vertices)
    static std::vector<TransformedVertex> ClipTriangle(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        const TransformedVertex& v2);
    
    // Clip a polygon against a single plane
    static std::vector<TransformedVertex> ClipPolygonAgainstPlane(
        const std::vector<TransformedVertex>& polygon,
        const ClipPlane& plane);
    
private:
    // Interpolate between two vertices based on clipping
    static TransformedVertex InterpolateVertices(
        const TransformedVertex& v0,
        const TransformedVertex& v1,
        float t);
    
    // Calculate intersection point between edge and plane
    static float CalculateIntersectionRatio(
        const Vec4& p0,
        const Vec4& p1,
        const ClipPlane& plane);
    
    // Static frustum for clipping
    static ViewFrustum sFrustum;
    static bool sFrustumInitialized;
    
    // Initialize frustum if needed
    static void EnsureFrustumInitialized();
};