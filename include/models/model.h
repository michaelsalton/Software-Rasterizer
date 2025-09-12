#pragma once

#include "models/mesh.h"
#include "rendering/material.h"
#include "rendering/texture.h"
#include "math/mat4.h"
#include <memory>
#include <vector>
#include <map>
#include <string>

// Forward declarations
class Renderer;

class Model {
public:
    Model();
    ~Model();
    
    // Loading
    bool loadFromFile(const std::string& filepath);
    
    // Mesh access
    void setMesh(std::unique_ptr<Mesh> mesh);
    const Mesh* getMesh() const { return mMesh.get(); }
    Mesh* getMesh() { return mMesh.get(); }
    
    // Material management
    void addMaterial(std::shared_ptr<Material> material);
    Material* getMaterial(size_t index);
    const Material* getMaterial(size_t index) const;
    void setMaterial(size_t index, std::shared_ptr<Material> material);
    size_t getMaterialCount() const { return mMaterials.size(); }
    
    // Texture management
    void addTexture(const std::string& name, std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> getTexture(const std::string& name) const;
    
    // Rendering
    void draw(Renderer* renderer, const Mat4& transform);
    
    // Bounds
    const BoundingBox& getBounds() const { 
        return mMesh ? mMesh->getBounds() : mEmptyBounds; 
    }
    
    // Info
    const std::string& getName() const { return mName; }
    void setName(const std::string& name) { mName = name; }
    const std::string& getDirectory() const { return mDirectory; }
    
    // Statistics
    size_t getVertexCount() const { 
        return mMesh ? mMesh->getTotalVertexCount() : 0; 
    }
    size_t getTriangleCount() const { 
        return mMesh ? mMesh->getTotalTriangleCount() : 0; 
    }
    
private:
    std::unique_ptr<Mesh> mMesh;
    std::vector<std::shared_ptr<Material>> mMaterials;
    std::map<std::string, std::shared_ptr<Texture>> mTextures;
    
    std::string mName;
    std::string mDirectory; // Base directory for relative texture paths
    
    static BoundingBox mEmptyBounds;
};