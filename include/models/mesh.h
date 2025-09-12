#pragma once

#include "pipeline/vertex_processor.h"
#include "math/vec3.h"
#include <vector>
#include <cstdint>

// Axis-aligned bounding box
struct BoundingBox {
    Vec3 min;
    Vec3 max;
    
    BoundingBox() : min(1e30f, 1e30f, 1e30f), max(-1e30f, -1e30f, -1e30f) {}
    
    void expand(const Vec3& point) {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);
        
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }
    
    Vec3 getCenter() const {
        return (min + max) * 0.5f;
    }
    
    Vec3 getSize() const {
        return max - min;
    }
    
    float getRadius() const {
        return getSize().length() * 0.5f;
    }
};

class Mesh {
public:
    struct SubMesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        uint32_t materialIndex = 0;
        BoundingBox bounds;
        
        size_t getTriangleCount() const {
            return indices.size() / 3;
        }
    };
    
    // Construction
    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();
    
    // SubMesh management
    void addSubMesh(const SubMesh& submesh);
    void addSubMesh(SubMesh&& submesh);
    const SubMesh& getSubMesh(size_t index) const;
    SubMesh& getSubMesh(size_t index);
    size_t getSubMeshCount() const { return mSubMeshes.size(); }
    
    // Direct access for simple meshes (single submesh)
    const std::vector<Vertex>& getVertices() const;
    const std::vector<uint32_t>& getIndices() const;
    
    // Bounds
    void calculateBounds();
    const BoundingBox& getBounds() const { return mBounds; }
    
    // Statistics
    size_t getTotalVertexCount() const;
    size_t getTotalTriangleCount() const;
    size_t getTotalIndexCount() const;
    
    // Utilities
    void generateNormals();
    void generateTangents(); // For future normal mapping
    
    // Name
    void setName(const std::string& name) { mName = name; }
    const std::string& getName() const { return mName; }
    
private:
    std::vector<SubMesh> mSubMeshes;
    BoundingBox mBounds;
    std::string mName;
};