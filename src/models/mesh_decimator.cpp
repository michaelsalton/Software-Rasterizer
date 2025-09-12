#include "models/mesh.h"
#include <unordered_map>
#include <algorithm>

// Simple mesh decimation - reduce triangle count by skipping triangles
void decimateMesh(Mesh* mesh, float targetReduction) {
    if (!mesh || targetReduction <= 0.0f || targetReduction >= 1.0f) {
        return;
    }
    
    for (size_t i = 0; i < mesh->getSubMeshCount(); ++i) {
        Mesh::SubMesh& submesh = const_cast<Mesh::SubMesh&>(mesh->getSubMesh(i));
        
        // Create new index buffer with reduced triangles
        std::vector<uint32_t> newIndices;
        size_t skipInterval = static_cast<size_t>(1.0f / (1.0f - targetReduction));
        
        for (size_t j = 0; j < submesh.indices.size(); j += 3) {
            // Skip triangles based on interval
            if ((j / 3) % skipInterval != 0) {
                newIndices.push_back(submesh.indices[j]);
                newIndices.push_back(submesh.indices[j + 1]);
                newIndices.push_back(submesh.indices[j + 2]);
            }
        }
        
        // Replace indices
        submesh.indices = std::move(newIndices);
    }
}