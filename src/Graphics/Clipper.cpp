#include "Clipper.h"
#include <algorithm>

// Static member initialization
ViewFrustum Clipper::sFrustum;
bool Clipper::sFrustumInitialized = false;

void Clipper::EnsureFrustumInitialized() {
    if (!sFrustumInitialized) {
        sFrustum.InitializeClipSpace();
        sFrustumInitialized = true;
    }
}

bool Clipper::IsTriangleOutsideFrustum(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2) {
    
    EnsureFrustumInitialized();
    
    // For each frustum plane, check if all vertices are on the negative side
    for (int i = 0; i < ViewFrustum::PLANE_COUNT; ++i) {
        const ClipPlane& plane = sFrustum.planes[i];
        
        float d0 = plane.Distance(v0.clipPosition);
        float d1 = plane.Distance(v1.clipPosition);
        float d2 = plane.Distance(v2.clipPosition);
        
        // If all vertices are outside this plane, triangle is outside frustum
        if (d0 < 0 && d1 < 0 && d2 < 0) {
            return true;
        }
    }
    
    // Triangle is at least partially inside the frustum
    return false;
}

std::vector<TransformedVertex> Clipper::ClipTriangle(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2) {
    
    EnsureFrustumInitialized();
    
    // Start with the input triangle
    std::vector<TransformedVertex> polygon = { v0, v1, v2 };
    
    // Clip against each frustum plane
    for (int i = 0; i < ViewFrustum::PLANE_COUNT; ++i) {
        polygon = ClipPolygonAgainstPlane(polygon, sFrustum.planes[i]);
        
        // Early out if polygon is completely clipped
        if (polygon.empty()) {
            return polygon;
        }
    }
    
    return polygon;
}

std::vector<TransformedVertex> Clipper::ClipPolygonAgainstPlane(
    const std::vector<TransformedVertex>& polygon,
    const ClipPlane& plane) {
    
    if (polygon.empty()) {
        return polygon;
    }
    
    std::vector<TransformedVertex> outputPolygon;
    
    // Process each edge of the polygon
    for (size_t i = 0; i < polygon.size(); ++i) {
        const TransformedVertex& currentVertex = polygon[i];
        const TransformedVertex& nextVertex = polygon[(i + 1) % polygon.size()];
        
        // Calculate distances to plane
        float currentDist = plane.Distance(currentVertex.clipPosition);
        float nextDist = plane.Distance(nextVertex.clipPosition);
        
        bool currentInside = currentDist >= 0;
        bool nextInside = nextDist >= 0;
        
        // Case 1: Both vertices inside - add next vertex
        if (currentInside && nextInside) {
            outputPolygon.push_back(nextVertex);
        }
        // Case 2: Current inside, next outside - add intersection
        else if (currentInside && !nextInside) {
            float t = CalculateIntersectionRatio(
                currentVertex.clipPosition,
                nextVertex.clipPosition,
                plane
            );
            outputPolygon.push_back(InterpolateVertices(currentVertex, nextVertex, t));
        }
        // Case 3: Current outside, next inside - add intersection and next
        else if (!currentInside && nextInside) {
            float t = CalculateIntersectionRatio(
                currentVertex.clipPosition,
                nextVertex.clipPosition,
                plane
            );
            outputPolygon.push_back(InterpolateVertices(currentVertex, nextVertex, t));
            outputPolygon.push_back(nextVertex);
        }
        // Case 4: Both outside - add nothing
    }
    
    return outputPolygon;
}

float Clipper::CalculateIntersectionRatio(
    const Vec4& p0,
    const Vec4& p1,
    const ClipPlane& plane) {
    
    float d0 = plane.Distance(p0);
    float d1 = plane.Distance(p1);
    
    // Avoid division by zero
    if (std::abs(d0 - d1) < 0.0001f) {
        return 0.0f;
    }
    
    // Calculate interpolation parameter
    // t = 0 at p0, t = 1 at p1
    return d0 / (d0 - d1);
}

TransformedVertex Clipper::InterpolateVertices(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    float t) {
    
    TransformedVertex result;
    
    // Interpolate clip space position
    result.clipPosition = v0.clipPosition * (1.0f - t) + v1.clipPosition * t;
    
    // Interpolate world space attributes
    result.worldPosition = v0.worldPosition * (1.0f - t) + v1.worldPosition * t;
    result.worldNormal = (v0.worldNormal * (1.0f - t) + v1.worldNormal * t).normalized();
    
    // Interpolate texture coordinates
    result.texCoord = v0.texCoord * (1.0f - t) + v1.texCoord * t;
    
    // Interpolate color
    float r = v0.color.r * (1.0f - t) + v1.color.r * t;
    float g = v0.color.g * (1.0f - t) + v1.color.g * t;
    float b = v0.color.b * (1.0f - t) + v1.color.b * t;
    float a = v0.color.a * (1.0f - t) + v1.color.a * t;
    
    result.color.r = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, r)));
    result.color.g = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, g)));
    result.color.b = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, b)));
    result.color.a = static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, a)));
    
    // Note: screenPosition will be recalculated after clipping
    result.screenPosition = Vec3(0, 0, 0);
    
    return result;
}