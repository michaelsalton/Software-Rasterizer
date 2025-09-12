#pragma once

#include "math/vec3.h"
#include "math/vec2.h"
#include <string>
#include <vector>
#include <map>

// Forward declarations
class Model;
class Mesh;

// Material properties from MTL file
struct MTLMaterial {
    std::string name;
    Vec3 ambient = Vec3(0.1f, 0.1f, 0.1f);      // Ka
    Vec3 diffuse = Vec3(0.8f, 0.8f, 0.8f);      // Kd
    Vec3 specular = Vec3(1.0f, 1.0f, 1.0f);     // Ks
    Vec3 emissive = Vec3(0.0f, 0.0f, 0.0f);     // Ke
    float shininess = 32.0f;                     // Ns
    float opacity = 1.0f;                        // d or Tr
    float ior = 1.0f;                            // Ni (index of refraction)
    int illum = 2;                               // Illumination model
    
    std::string diffuseMap;     // map_Kd
    std::string specularMap;    // map_Ks
    std::string normalMap;      // map_bump or bump
    std::string alphaMap;       // map_d
    std::string emissiveMap;    // map_Ke
};

class OBJLoader {
public:
    // Main loading function
    static bool load(const std::string& filepath, Model* outModel);
    
private:
    struct OBJData {
        // Vertex data pools
        std::vector<Vec3> positions;
        std::vector<Vec2> texCoords;
        std::vector<Vec3> normals;
        
        // Face data
        struct Face {
            struct VertexIndices {
                int positionIndex = -1;
                int texCoordIndex = -1;
                int normalIndex = -1;
            };
            std::vector<VertexIndices> vertices;
            std::string materialName;
        };
        std::vector<Face> faces;
        
        // Materials
        std::map<std::string, MTLMaterial> materials;
        std::map<std::string, size_t> materialIndices; // material name -> index
        
        // Groups/objects (for future use)
        std::string currentGroup;
        std::string currentObject;
        std::string currentMaterial;
    };
    
    // Parsing functions
    static bool parseOBJ(const std::string& filepath, OBJData& data);
    static bool parseMTL(const std::string& filepath, std::map<std::string, MTLMaterial>& materials);
    
    // Helper functions
    static bool parseVertex(const std::string& line, Vec3& vertex);
    static bool parseTexCoord(const std::string& line, Vec2& texCoord);
    static bool parseNormal(const std::string& line, Vec3& normal);
    static bool parseFace(const std::string& line, OBJData::Face& face);
    static bool parseVertexIndices(const std::string& vertexStr, OBJData::Face::VertexIndices& indices);
    
    // Mesh building
    static Mesh* buildMesh(const OBJData& data);
    static void generateNormalsIfNeeded(OBJData& data);
    static void triangulate(const OBJData::Face& face, std::vector<OBJData::Face>& triangles);
};