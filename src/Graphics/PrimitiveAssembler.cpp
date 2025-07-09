#include "PrimitiveAssembler.h"
#include <stdexcept>

PrimitiveAssembler::PrimitiveAssembler()
    : cullMode(CullMode::BACK)
    , windingOrder(WindingOrder::CCW) {
}

std::vector<Triangle> PrimitiveAssembler::AssembleIndexed(
    const std::vector<TransformedVertex>& vertices,
    const std::vector<int>& indices,
    PrimitiveType primitiveType) {
    
    std::vector<Triangle> triangles;
    
    switch (primitiveType) {
        case PrimitiveType::TRIANGLES:
            triangles = AssembleTriangles(vertices, &indices);
            break;
        case PrimitiveType::TRIANGLE_STRIP:
            triangles = AssembleTriangleStrip(vertices, &indices);
            break;
        case PrimitiveType::TRIANGLE_FAN:
            triangles = AssembleTriangleFan(vertices, &indices);
            break;
        default:
            // Other primitive types not implemented yet
            break;
    }
    
    // Perform culling if enabled
    if (cullMode != CullMode::NONE) {
        triangles = CullTriangles(triangles);
    }
    
    return triangles;
}

std::vector<Triangle> PrimitiveAssembler::AssembleNonIndexed(
    const std::vector<TransformedVertex>& vertices,
    PrimitiveType primitiveType) {
    
    std::vector<Triangle> triangles;
    
    switch (primitiveType) {
        case PrimitiveType::TRIANGLES:
            triangles = AssembleTriangles(vertices, nullptr);
            break;
        case PrimitiveType::TRIANGLE_STRIP:
            triangles = AssembleTriangleStrip(vertices, nullptr);
            break;
        case PrimitiveType::TRIANGLE_FAN:
            triangles = AssembleTriangleFan(vertices, nullptr);
            break;
        default:
            // Other primitive types not implemented yet
            break;
    }
    
    // Perform culling if enabled
    if (cullMode != CullMode::NONE) {
        triangles = CullTriangles(triangles);
    }
    
    return triangles;
}

std::vector<Triangle> PrimitiveAssembler::AssembleTriangles(
    const std::vector<TransformedVertex>& vertices,
    const std::vector<int>* indices) {
    
    std::vector<Triangle> triangles;
    
    if (indices) {
        // Indexed mode
        if (indices->size() < 3 || indices->size() % 3 != 0) {
            return triangles; // Invalid index count
        }
        
        for (size_t i = 0; i < indices->size(); i += 3) {
            Triangle tri(
                vertices[(*indices)[i]],
                vertices[(*indices)[i + 1]],
                vertices[(*indices)[i + 2]]
            );
            
            tri.faceNormal = CalculateFaceNormal(tri);
            tri.isFrontFacing = IsFrontFacing(tri);
            triangles.push_back(tri);
        }
    } else {
        // Non-indexed mode
        if (vertices.size() < 3 || vertices.size() % 3 != 0) {
            return triangles; // Invalid vertex count
        }
        
        for (size_t i = 0; i < vertices.size(); i += 3) {
            Triangle tri(
                vertices[i],
                vertices[i + 1],
                vertices[i + 2]
            );
            
            tri.faceNormal = CalculateFaceNormal(tri);
            tri.isFrontFacing = IsFrontFacing(tri);
            triangles.push_back(tri);
        }
    }
    
    return triangles;
}

std::vector<Triangle> PrimitiveAssembler::AssembleTriangleStrip(
    const std::vector<TransformedVertex>& vertices,
    const std::vector<int>* indices) {
    
    std::vector<Triangle> triangles;
    size_t count = indices ? indices->size() : vertices.size();
    
    if (count < 3) {
        return triangles; // Not enough vertices
    }
    
    for (size_t i = 0; i < count - 2; ++i) {
        Triangle tri;
        
        // Alternate winding order for even/odd triangles
        if (i % 2 == 0) {
            tri.vertices[0] = GetVertex(vertices, indices, i);
            tri.vertices[1] = GetVertex(vertices, indices, i + 1);
            tri.vertices[2] = GetVertex(vertices, indices, i + 2);
        } else {
            tri.vertices[0] = GetVertex(vertices, indices, i + 1);
            tri.vertices[1] = GetVertex(vertices, indices, i);
            tri.vertices[2] = GetVertex(vertices, indices, i + 2);
        }
        
        tri.faceNormal = CalculateFaceNormal(tri);
        tri.isFrontFacing = IsFrontFacing(tri);
        triangles.push_back(tri);
    }
    
    return triangles;
}

std::vector<Triangle> PrimitiveAssembler::AssembleTriangleFan(
    const std::vector<TransformedVertex>& vertices,
    const std::vector<int>* indices) {
    
    std::vector<Triangle> triangles;
    size_t count = indices ? indices->size() : vertices.size();
    
    if (count < 3) {
        return triangles; // Not enough vertices
    }
    
    // First vertex is the center of the fan
    const TransformedVertex& center = GetVertex(vertices, indices, 0);
    
    for (size_t i = 1; i < count - 1; ++i) {
        Triangle tri(
            center,
            GetVertex(vertices, indices, i),
            GetVertex(vertices, indices, i + 1)
        );
        
        tri.faceNormal = CalculateFaceNormal(tri);
        tri.isFrontFacing = IsFrontFacing(tri);
        triangles.push_back(tri);
    }
    
    return triangles;
}

std::vector<Triangle> PrimitiveAssembler::CullTriangles(const std::vector<Triangle>& triangles) {
    std::vector<Triangle> culledTriangles;
    
    for (const auto& tri : triangles) {
        if (!ShouldCull(tri)) {
            culledTriangles.push_back(tri);
        }
    }
    
    return culledTriangles;
}

const TransformedVertex& PrimitiveAssembler::GetVertex(
    const std::vector<TransformedVertex>& vertices,
    const std::vector<int>* indices,
    size_t index) const {
    
    if (indices) {
        return vertices[(*indices)[index]];
    } else {
        return vertices[index];
    }
}

Vec3 PrimitiveAssembler::CalculateFaceNormal(const Triangle& triangle) const {
    // Use screen space positions for culling
    const Vec3& v0 = triangle.vertices[0].screenPosition;
    const Vec3& v1 = triangle.vertices[1].screenPosition;
    const Vec3& v2 = triangle.vertices[2].screenPosition;
    
    // Calculate edge vectors
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    
    // Cross product gives normal (pointing towards viewer if CCW)
    return edge1.cross(edge2).normalized();
}

bool PrimitiveAssembler::IsFrontFacing(const Triangle& triangle) const {
    // In screen space, positive Z points towards viewer
    // For CCW winding, cross product points towards viewer (positive Z)
    // For CW winding, cross product points away (negative Z)
    
    bool isCCW = triangle.faceNormal.z > 0;
    
    if (windingOrder == WindingOrder::CCW) {
        return isCCW;
    } else {
        return !isCCW;
    }
}

bool PrimitiveAssembler::ShouldCull(const Triangle& triangle) const {
    switch (cullMode) {
        case CullMode::NONE:
            return false;
            
        case CullMode::BACK:
            return !triangle.isFrontFacing;
            
        case CullMode::FRONT:
            return triangle.isFrontFacing;
            
        case CullMode::FRONT_AND_BACK:
            return true;
            
        default:
            return false;
    }
}