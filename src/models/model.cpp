#include "models/model.h"
#include "rendering/renderer.h"
#include "loaders/obj_loader.h"
#include "pipeline/fragment_shader.h"
#include <iostream>
#include <algorithm>

BoundingBox Model::mEmptyBounds;

Model::Model() {
}

Model::~Model() {
}

bool Model::loadFromFile(const std::string& filepath) {
    // Extract directory from filepath
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        mDirectory = filepath.substr(0, lastSlash);
    } else {
        mDirectory = ".";
    }
    
    // Extract filename for model name
    size_t nameStart = (lastSlash != std::string::npos) ? lastSlash + 1 : 0;
    size_t extPos = filepath.find_last_of('.');
    if (extPos != std::string::npos && extPos > nameStart) {
        mName = filepath.substr(nameStart, extPos - nameStart);
    } else {
        mName = filepath.substr(nameStart);
    }
    
    // Check file extension
    std::string extension;
    if (extPos != std::string::npos) {
        extension = filepath.substr(extPos);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }
    
    // Load based on extension
    if (extension == ".obj") {
        return OBJLoader::load(filepath, this);
    } else {
        std::cerr << "Model: Unsupported file format: " << extension << std::endl;
        return false;
    }
}

void Model::setMesh(std::unique_ptr<Mesh> mesh) {
    mMesh = std::move(mesh);
}

void Model::addMaterial(std::shared_ptr<Material> material) {
    mMaterials.push_back(material);
}

Material* Model::getMaterial(size_t index) {
    if (index >= mMaterials.size()) {
        return nullptr;
    }
    return mMaterials[index].get();
}

const Material* Model::getMaterial(size_t index) const {
    if (index >= mMaterials.size()) {
        return nullptr;
    }
    return mMaterials[index].get();
}

void Model::setMaterial(size_t index, std::shared_ptr<Material> material) {
    if (index >= mMaterials.size()) {
        mMaterials.resize(index + 1);
    }
    mMaterials[index] = material;
}

void Model::addTexture(const std::string& name, std::shared_ptr<Texture> texture) {
    mTextures[name] = texture;
}

std::shared_ptr<Texture> Model::getTexture(const std::string& name) const {
    auto it = mTextures.find(name);
    if (it != mTextures.end()) {
        return it->second;
    }
    return nullptr;
}

void Model::draw(Renderer* renderer, const Mat4& transform) {
    if (!mMesh || !renderer) {
        return;
    }
    
    // For each submesh in the mesh
    for (size_t i = 0; i < mMesh->getSubMeshCount(); ++i) {
        const Mesh::SubMesh& submesh = mMesh->getSubMesh(i);
        
        // Get the material for this submesh
        Material* material = getMaterial(submesh.materialIndex);
        
        // Set the material and texture on the renderer (if we have one)
        if (material && renderer->GetFragmentShader()) {
            // If we have a texture for this material, set it
            if (!mTextures.empty()) {
                // Get the first texture (in this case, the diffuse map)
                auto textureIt = mTextures.begin();
                if (textureIt->second) {
                    // Try to set the texture on the shader
                    auto texturedShader = dynamic_cast<TexturedFragmentShader*>(renderer->GetFragmentShader().get());
                    if (texturedShader) {
                        texturedShader->SetTexture(textureIt->second.get());
                    }
                    
                    auto litShader = dynamic_cast<LitFragmentShader*>(renderer->GetFragmentShader().get());
                    if (litShader) {
                        litShader->SetAlbedoTexture(textureIt->second.get());
                    }
                }
            }
        }
        
        // Draw the submesh
        // Convert indices from uint32_t to int
        std::vector<int> intIndices(submesh.indices.begin(), submesh.indices.end());
        
        renderer->DrawVertexMesh(submesh.vertices, intIndices, transform, false);
    }
}