#include "models/mesh.h"
#include <stdexcept>

Mesh::Mesh() {
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    SubMesh submesh;
    submesh.vertices = vertices;
    submesh.indices = indices;
    submesh.materialIndex = 0;
    
    // Calculate bounds for the submesh
    for (const auto& vertex : vertices) {
        submesh.bounds.expand(vertex.position);
    }
    
    mSubMeshes.push_back(std::move(submesh));
    mBounds = mSubMeshes[0].bounds;
}

Mesh::~Mesh() {
}

void Mesh::addSubMesh(const SubMesh& submesh) {
    mSubMeshes.push_back(submesh);
    
    // Update overall bounds
    for (const auto& vertex : submesh.vertices) {
        mBounds.expand(vertex.position);
    }
}

void Mesh::addSubMesh(SubMesh&& submesh) {
    // Update overall bounds before moving
    for (const auto& vertex : submesh.vertices) {
        mBounds.expand(vertex.position);
    }
    
    mSubMeshes.push_back(std::move(submesh));
}

const Mesh::SubMesh& Mesh::getSubMesh(size_t index) const {
    if (index >= mSubMeshes.size()) {
        throw std::out_of_range("SubMesh index out of range");
    }
    return mSubMeshes[index];
}

Mesh::SubMesh& Mesh::getSubMesh(size_t index) {
    if (index >= mSubMeshes.size()) {
        throw std::out_of_range("SubMesh index out of range");
    }
    return mSubMeshes[index];
}

const std::vector<Vertex>& Mesh::getVertices() const {
    if (mSubMeshes.empty()) {
        static std::vector<Vertex> empty;
        return empty;
    }
    return mSubMeshes[0].vertices;
}

const std::vector<uint32_t>& Mesh::getIndices() const {
    if (mSubMeshes.empty()) {
        static std::vector<uint32_t> empty;
        return empty;
    }
    return mSubMeshes[0].indices;
}

void Mesh::calculateBounds() {
    mBounds = BoundingBox();
    
    for (const auto& submesh : mSubMeshes) {
        for (const auto& vertex : submesh.vertices) {
            mBounds.expand(vertex.position);
        }
    }
}

size_t Mesh::getTotalVertexCount() const {
    size_t count = 0;
    for (const auto& submesh : mSubMeshes) {
        count += submesh.vertices.size();
    }
    return count;
}

size_t Mesh::getTotalTriangleCount() const {
    size_t count = 0;
    for (const auto& submesh : mSubMeshes) {
        count += submesh.getTriangleCount();
    }
    return count;
}

size_t Mesh::getTotalIndexCount() const {
    size_t count = 0;
    for (const auto& submesh : mSubMeshes) {
        count += submesh.indices.size();
    }
    return count;
}

void Mesh::generateNormals() {
    for (auto& submesh : mSubMeshes) {
        // First, zero out all normals
        for (auto& vertex : submesh.vertices) {
            vertex.normal = Vec3(0, 0, 0);
        }
        
        // Calculate face normals and accumulate to vertices
        for (size_t i = 0; i < submesh.indices.size(); i += 3) {
            uint32_t i0 = submesh.indices[i];
            uint32_t i1 = submesh.indices[i + 1];
            uint32_t i2 = submesh.indices[i + 2];
            
            Vec3& v0 = submesh.vertices[i0].position;
            Vec3& v1 = submesh.vertices[i1].position;
            Vec3& v2 = submesh.vertices[i2].position;
            
            // Calculate face normal
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 faceNormal = edge1.cross(edge2).normalized();
            
            // Add to vertex normals
            submesh.vertices[i0].normal += faceNormal;
            submesh.vertices[i1].normal += faceNormal;
            submesh.vertices[i2].normal += faceNormal;
        }
        
        // Normalize all vertex normals
        for (auto& vertex : submesh.vertices) {
            vertex.normal = vertex.normal.normalized();
        }
    }
}

void Mesh::generateTangents() {
    // TODO: Implement tangent generation for normal mapping
    // This will be needed in a future phase
}