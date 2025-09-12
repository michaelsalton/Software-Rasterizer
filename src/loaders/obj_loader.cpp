#include "loaders/obj_loader.h"
#include "models/model.h"
#include "models/mesh.h"
#include "rendering/material.h"
#include "rendering/texture.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

bool OBJLoader::load(const std::string& filepath, Model* outModel) {
    if (!outModel) {
        std::cerr << "OBJLoader: Output model is null" << std::endl;
        return false;
    }
    
    OBJData data;
    
    // Parse the OBJ file
    if (!parseOBJ(filepath, data)) {
        std::cerr << "OBJLoader: Failed to parse OBJ file: " << filepath << std::endl;
        return false;
    }
    
    // Generate normals if not provided
    generateNormalsIfNeeded(data);
    
    // Build the mesh from parsed data
    Mesh* mesh = buildMesh(data);
    if (!mesh) {
        std::cerr << "OBJLoader: Failed to build mesh from OBJ data" << std::endl;
        return false;
    }
    
    // Set the mesh on the model
    outModel->setMesh(std::unique_ptr<Mesh>(mesh));
    
    // Convert MTL materials to our Material format
    for (const auto& [name, mtl] : data.materials) {
        auto material = std::make_shared<Material>();
        
        // Convert MTL properties to our material system
        material->albedo = mtl.diffuse;  // Use diffuse as albedo
        material->specular = mtl.specular;
        material->emissive = mtl.emissive;
        material->shininess = mtl.shininess;
        
        // Set shading model based on illum
        if (mtl.illum >= 2) {
            material->shadingModel = Material::BLINN_PHONG;
        } else {
            material->shadingModel = Material::LAMBERT;
        }
        
        // TODO: Load textures referenced by the material
        // For now, we'll skip texture loading
        
        outModel->addMaterial(material);
    }
    
    // If no materials were loaded, add a default one
    if (outModel->getMaterialCount() == 0) {
        outModel->addMaterial(std::make_shared<Material>());
    }
    
    std::cout << "OBJLoader: Successfully loaded model from " << filepath << std::endl;
    std::cout << "  Vertices: " << mesh->getTotalVertexCount() << std::endl;
    std::cout << "  Triangles: " << mesh->getTotalTriangleCount() << std::endl;
    std::cout << "  Materials: " << outModel->getMaterialCount() << std::endl;
    
    // Debug: print first few vertices
    if (mesh->getSubMeshCount() > 0) {
        const auto& submesh = mesh->getSubMesh(0);
        std::cout << "  First 8 vertices:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(8), submesh.vertices.size()); ++i) {
            const auto& v = submesh.vertices[i];
            std::cout << "    V" << i << ": pos(" << v.position.x << ", " << v.position.y 
                      << ", " << v.position.z << ") norm(" << v.normal.x << ", " 
                      << v.normal.y << ", " << v.normal.z << ")" << std::endl;
        }
    }
    
    return true;
}

bool OBJLoader::parseOBJ(const std::string& filepath, OBJData& data) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "OBJLoader: Cannot open file: " << filepath << std::endl;
        return false;
    }
    
    // Extract directory for MTL file loading
    std::string directory;
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        directory = filepath.substr(0, lastSlash);
    } else {
        directory = ".";
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        if (token == "v") {
            // Vertex position
            Vec3 vertex;
            if (parseVertex(line, vertex)) {
                data.positions.push_back(vertex);
            } else {
                std::cerr << "OBJLoader: Invalid vertex at line " << lineNumber << std::endl;
            }
        }
        else if (token == "vt") {
            // Texture coordinate
            Vec2 texCoord;
            if (parseTexCoord(line, texCoord)) {
                data.texCoords.push_back(texCoord);
            } else {
                std::cerr << "OBJLoader: Invalid texture coordinate at line " << lineNumber << std::endl;
            }
        }
        else if (token == "vn") {
            // Normal
            Vec3 normal;
            if (parseNormal(line, normal)) {
                data.normals.push_back(normal);
            } else {
                std::cerr << "OBJLoader: Invalid normal at line " << lineNumber << std::endl;
            }
        }
        else if (token == "f") {
            // Face
            OBJData::Face face;
            face.materialName = data.currentMaterial;
            if (parseFace(line, face)) {
                // Triangulate if necessary
                if (face.vertices.size() > 3) {
                    std::vector<OBJData::Face> triangles;
                    triangulate(face, triangles);
                    for (const auto& tri : triangles) {
                        data.faces.push_back(tri);
                    }
                } else if (face.vertices.size() == 3) {
                    data.faces.push_back(face);
                }
            } else {
                std::cerr << "OBJLoader: Invalid face at line " << lineNumber << std::endl;
            }
        }
        else if (token == "mtllib") {
            // Material library
            std::string mtlFile;
            iss >> mtlFile;
            std::string mtlPath = directory + "/" + mtlFile;
            parseMTL(mtlPath, data.materials);
            
            // Build material index map
            size_t index = 0;
            for (const auto& [name, material] : data.materials) {
                data.materialIndices[name] = index++;
            }
        }
        else if (token == "usemtl") {
            // Use material
            iss >> data.currentMaterial;
        }
        else if (token == "g") {
            // Group
            iss >> data.currentGroup;
        }
        else if (token == "o") {
            // Object
            iss >> data.currentObject;
        }
        // Ignore other tokens (s, etc.)
    }
    
    file.close();
    
    if (data.positions.empty() || data.faces.empty()) {
        std::cerr << "OBJLoader: No vertices or faces found in file" << std::endl;
        return false;
    }
    
    return true;
}

bool OBJLoader::parseMTL(const std::string& filepath, std::map<std::string, MTLMaterial>& materials) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "OBJLoader: Warning - Cannot open MTL file: " << filepath << std::endl;
        return false;
    }
    
    MTLMaterial* currentMaterial = nullptr;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        if (token == "newmtl") {
            std::string name;
            iss >> name;
            materials[name] = MTLMaterial();
            materials[name].name = name;
            currentMaterial = &materials[name];
        }
        else if (currentMaterial) {
            if (token == "Ka") {
                iss >> currentMaterial->ambient.x >> currentMaterial->ambient.y >> currentMaterial->ambient.z;
            }
            else if (token == "Kd") {
                iss >> currentMaterial->diffuse.x >> currentMaterial->diffuse.y >> currentMaterial->diffuse.z;
            }
            else if (token == "Ks") {
                iss >> currentMaterial->specular.x >> currentMaterial->specular.y >> currentMaterial->specular.z;
            }
            else if (token == "Ke") {
                iss >> currentMaterial->emissive.x >> currentMaterial->emissive.y >> currentMaterial->emissive.z;
            }
            else if (token == "Ns") {
                iss >> currentMaterial->shininess;
            }
            else if (token == "d") {
                iss >> currentMaterial->opacity;
            }
            else if (token == "Tr") {
                float transparency;
                iss >> transparency;
                currentMaterial->opacity = 1.0f - transparency;
            }
            else if (token == "Ni") {
                iss >> currentMaterial->ior;
            }
            else if (token == "illum") {
                iss >> currentMaterial->illum;
            }
            else if (token == "map_Kd") {
                iss >> currentMaterial->diffuseMap;
            }
            else if (token == "map_Ks") {
                iss >> currentMaterial->specularMap;
            }
            else if (token == "map_bump" || token == "bump") {
                iss >> currentMaterial->normalMap;
            }
            else if (token == "map_d") {
                iss >> currentMaterial->alphaMap;
            }
            else if (token == "map_Ke") {
                iss >> currentMaterial->emissiveMap;
            }
        }
    }
    
    file.close();
    return true;
}

bool OBJLoader::parseVertex(const std::string& line, Vec3& vertex) {
    std::istringstream iss(line);
    std::string token;
    iss >> token; // Skip 'v'
    
    if (!(iss >> vertex.x >> vertex.y >> vertex.z)) {
        return false;
    }
    
    // Optional w component (ignore for now)
    float w;
    if (iss >> w) {
        // Could handle homogeneous coordinates here if needed
    }
    
    return true;
}

bool OBJLoader::parseTexCoord(const std::string& line, Vec2& texCoord) {
    std::istringstream iss(line);
    std::string token;
    iss >> token; // Skip 'vt'
    
    if (!(iss >> texCoord.x >> texCoord.y)) {
        return false;
    }
    
    // Optional w component (ignore for now)
    float w;
    if (iss >> w) {
        // Could handle 3D texture coordinates here if needed
    }
    
    return true;
}

bool OBJLoader::parseNormal(const std::string& line, Vec3& normal) {
    std::istringstream iss(line);
    std::string token;
    iss >> token; // Skip 'vn'
    
    if (!(iss >> normal.x >> normal.y >> normal.z)) {
        return false;
    }
    
    normal = normal.normalized();
    return true;
}

bool OBJLoader::parseFace(const std::string& line, OBJData::Face& face) {
    std::istringstream iss(line);
    std::string token;
    iss >> token; // Skip 'f'
    
    std::string vertexStr;
    while (iss >> vertexStr) {
        OBJData::Face::VertexIndices indices;
        if (!parseVertexIndices(vertexStr, indices)) {
            return false;
        }
        face.vertices.push_back(indices);
    }
    
    return face.vertices.size() >= 3;
}

bool OBJLoader::parseVertexIndices(const std::string& vertexStr, OBJData::Face::VertexIndices& indices) {
    std::vector<std::string> parts;
    std::stringstream ss(vertexStr);
    std::string part;
    
    // Split by '/' to get position/texcoord/normal indices
    while (std::getline(ss, part, '/')) {
        parts.push_back(part);
    }
    
    if (parts.empty() || parts.size() > 3) {
        return false;
    }
    
    // Parse position index (required)
    if (!parts[0].empty()) {
        indices.positionIndex = std::stoi(parts[0]) - 1; // OBJ uses 1-based indexing
    } else {
        return false;
    }
    
    // Parse texture coordinate index (optional)
    if (parts.size() > 1 && !parts[1].empty()) {
        indices.texCoordIndex = std::stoi(parts[1]) - 1;
    }
    
    // Parse normal index (optional)
    if (parts.size() > 2 && !parts[2].empty()) {
        indices.normalIndex = std::stoi(parts[2]) - 1;
    }
    
    return true;
}

void OBJLoader::generateNormalsIfNeeded(OBJData& data) {
    // Check if we need to generate normals
    bool needsNormals = false;
    for (const auto& face : data.faces) {
        for (const auto& vertex : face.vertices) {
            if (vertex.normalIndex < 0) {
                needsNormals = true;
                break;
            }
        }
        if (needsNormals) break;
    }
    
    if (!needsNormals && !data.normals.empty()) {
        return; // Normals already provided
    }
    
    // Clear existing normals and generate new ones
    data.normals.clear();
    data.normals.resize(data.positions.size(), Vec3(0, 0, 0));
    
    // Calculate face normals and accumulate to vertices
    for (auto& face : data.faces) {
        if (face.vertices.size() < 3) continue;
        
        // Get first triangle of the face
        const Vec3& v0 = data.positions[face.vertices[0].positionIndex];
        const Vec3& v1 = data.positions[face.vertices[1].positionIndex];
        const Vec3& v2 = data.positions[face.vertices[2].positionIndex];
        
        // Calculate face normal
        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;
        Vec3 faceNormal = edge1.cross(edge2).normalized();
        
        // Accumulate to all vertices of the face
        for (auto& vertex : face.vertices) {
            data.normals[vertex.positionIndex] += faceNormal;
            vertex.normalIndex = vertex.positionIndex; // Use position index as normal index
        }
    }
    
    // Normalize all vertex normals
    for (auto& normal : data.normals) {
        normal = normal.normalized();
    }
}

void OBJLoader::triangulate(const OBJData::Face& face, std::vector<OBJData::Face>& triangles) {
    // Simple fan triangulation from first vertex
    for (size_t i = 2; i < face.vertices.size(); ++i) {
        OBJData::Face triangle;
        triangle.materialName = face.materialName;
        triangle.vertices.push_back(face.vertices[0]);
        triangle.vertices.push_back(face.vertices[i - 1]);
        triangle.vertices.push_back(face.vertices[i]);
        triangles.push_back(triangle);
    }
}

Mesh* OBJLoader::buildMesh(const OBJData& data) {
    Mesh* mesh = new Mesh();
    
    // Group faces by material
    std::unordered_map<size_t, std::vector<const OBJData::Face*>> materialFaces;
    
    for (const auto& face : data.faces) {
        size_t materialIndex = 0;
        auto it = data.materialIndices.find(face.materialName);
        if (it != data.materialIndices.end()) {
            materialIndex = it->second;
        }
        materialFaces[materialIndex].push_back(&face);
    }
    
    // Create a submesh for each material
    for (const auto& [materialIndex, faces] : materialFaces) {
        Mesh::SubMesh submesh;
        submesh.materialIndex = materialIndex;
        
        // Build vertex buffer with unique vertices
        std::unordered_map<std::string, uint32_t> uniqueVertices;
        
        for (const OBJData::Face* face : faces) {
            for (const auto& vertexIndices : face->vertices) {
                // Create a key for this unique vertex combination
                std::string key = std::to_string(vertexIndices.positionIndex) + "/" +
                                 std::to_string(vertexIndices.texCoordIndex) + "/" +
                                 std::to_string(vertexIndices.normalIndex);
                
                // Check if we've already added this vertex
                auto it = uniqueVertices.find(key);
                uint32_t index;
                
                if (it != uniqueVertices.end()) {
                    index = it->second;
                } else {
                    // Create new vertex
                    Vertex vertex;
                    
                    // Position (required)
                    if (vertexIndices.positionIndex >= 0 && 
                        vertexIndices.positionIndex < static_cast<int>(data.positions.size())) {
                        vertex.position = data.positions[vertexIndices.positionIndex];
                    }
                    
                    // Texture coordinate (optional)
                    if (vertexIndices.texCoordIndex >= 0 && 
                        vertexIndices.texCoordIndex < static_cast<int>(data.texCoords.size())) {
                        vertex.texCoord = data.texCoords[vertexIndices.texCoordIndex];
                    } else {
                        vertex.texCoord = Vec2(0, 0);
                    }
                    
                    // Normal (optional)
                    if (vertexIndices.normalIndex >= 0 && 
                        vertexIndices.normalIndex < static_cast<int>(data.normals.size())) {
                        vertex.normal = data.normals[vertexIndices.normalIndex];
                    } else {
                        vertex.normal = Vec3(0, 1, 0); // Default up normal
                    }
                    
                    // Default color (white)
                    vertex.color = Framebuffer::Color(255, 255, 255, 255);
                    
                    index = static_cast<uint32_t>(submesh.vertices.size());
                    submesh.vertices.push_back(vertex);
                    uniqueVertices[key] = index;
                    
                    // Update bounds
                    submesh.bounds.expand(vertex.position);
                }
                
                submesh.indices.push_back(index);
            }
        }
        
        mesh->addSubMesh(std::move(submesh));
    }
    
    // If no submeshes were created, create a default one
    if (mesh->getSubMeshCount() == 0) {
        Mesh::SubMesh submesh;
        
        // Build a simple vertex buffer without deduplication
        for (const auto& face : data.faces) {
            // All faces should already be triangulated by this point
            if (face.vertices.size() != 3) {
                std::cerr << "OBJLoader: Warning - non-triangular face found with " 
                          << face.vertices.size() << " vertices" << std::endl;
                continue;
            }
            
            uint32_t baseIndex = static_cast<uint32_t>(submesh.vertices.size());
            
            for (size_t i = 0; i < face.vertices.size(); ++i) {
                const auto& vertexIndices = face.vertices[i];
                
                Vertex vertex;
                
                // Position
                if (vertexIndices.positionIndex >= 0 && 
                    vertexIndices.positionIndex < static_cast<int>(data.positions.size())) {
                    vertex.position = data.positions[vertexIndices.positionIndex];
                }
                
                // Texture coordinate
                if (vertexIndices.texCoordIndex >= 0 && 
                    vertexIndices.texCoordIndex < static_cast<int>(data.texCoords.size())) {
                    vertex.texCoord = data.texCoords[vertexIndices.texCoordIndex];
                } else {
                    vertex.texCoord = Vec2(0, 0);
                }
                
                // Normal
                if (vertexIndices.normalIndex >= 0 && 
                    vertexIndices.normalIndex < static_cast<int>(data.normals.size())) {
                    vertex.normal = data.normals[vertexIndices.normalIndex];
                } else {
                    vertex.normal = Vec3(0, 1, 0);
                }
                
                // Default color
                vertex.color = Framebuffer::Color(255, 255, 255, 255);
                
                submesh.vertices.push_back(vertex);
                submesh.bounds.expand(vertex.position);
            }
            
            // Add indices for the triangle
            submesh.indices.push_back(baseIndex + 0);
            submesh.indices.push_back(baseIndex + 1);
            submesh.indices.push_back(baseIndex + 2);
        }
        
        mesh->addSubMesh(std::move(submesh));
    }
    
    return mesh;
}