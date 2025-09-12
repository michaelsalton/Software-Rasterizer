# Phase 5: 3D Model Loading System Specification

## Overview
Phase 5 introduces the ability to load and render 3D models from standard file formats, starting with OBJ files and later expanding to glTF 2.0. This phase will transform the renderer from a hardcoded cube demo to a full 3D model viewer.

## Goals
1. Load 3D models from industry-standard file formats
2. Support materials and textures from model files
3. Implement efficient mesh management
4. Create a scene graph for complex models
5. Enable rendering of real-world 3D assets

## 5.1 Core Components

### 5.1.1 Mesh Class
```cpp
class Mesh {
public:
    struct SubMesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        uint32_t materialIndex;
        BoundingBox bounds;
    };

    // Construction
    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    
    // SubMesh management
    void addSubMesh(const SubMesh& submesh);
    const SubMesh& getSubMesh(size_t index) const;
    size_t getSubMeshCount() const;
    
    // Bounds
    void calculateBounds();
    const BoundingBox& getBounds() const;
    
    // Statistics
    size_t getTotalVertexCount() const;
    size_t getTotalTriangleCount() const;
    
private:
    std::vector<SubMesh> mSubMeshes;
    BoundingBox mBounds;
    std::string mName;
};
```

### 5.1.2 Model Class
```cpp
class Model {
public:
    Model();
    ~Model();
    
    // Loading
    bool loadFromFile(const std::string& filepath);
    
    // Rendering
    void draw(Renderer* renderer, const Mat4& transform);
    
    // Material access
    Material* getMaterial(size_t index);
    void setMaterial(size_t index, std::shared_ptr<Material> material);
    
    // Mesh access
    const Mesh* getMesh() const { return mMesh.get(); }
    
    // Bounds
    const BoundingBox& getBounds() const { return mMesh->getBounds(); }
    
private:
    std::unique_ptr<Mesh> mMesh;
    std::vector<std::shared_ptr<Material>> mMaterials;
    std::map<std::string, std::shared_ptr<Texture>> mTextures;
    std::string mDirectory; // Base directory for relative paths
};
```

### 5.1.3 Enhanced Material System
```cpp
class Material {
public:
    enum ShadingModel {
        LAMBERT,
        PHONG,
        BLINN_PHONG,
        PBR_METALLIC_ROUGHNESS
    };
    
    // Basic properties
    Vec3 ambient = Vec3(0.1f, 0.1f, 0.1f);
    Vec3 diffuse = Vec3(0.8f, 0.8f, 0.8f);
    Vec3 specular = Vec3(1.0f, 1.0f, 1.0f);
    Vec3 emissive = Vec3(0.0f, 0.0f, 0.0f);
    float shininess = 32.0f;
    float opacity = 1.0f;
    
    // PBR properties
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f; // Ambient occlusion
    
    // Textures
    std::shared_ptr<Texture> diffuseMap;
    std::shared_ptr<Texture> specularMap;
    std::shared_ptr<Texture> normalMap;
    std::shared_ptr<Texture> emissiveMap;
    std::shared_ptr<Texture> aoMap;
    std::shared_ptr<Texture> metallicRoughnessMap;
    
    // Rendering properties
    ShadingModel shadingModel = BLINN_PHONG;
    bool doubleSided = false;
    bool alphaTest = false;
    float alphaTestThreshold = 0.5f;
    
    // Factory methods
    static Material Default();
    static Material FromMTL(const MTLMaterial& mtl);
    static Material FromGLTF(const GLTFMaterial& gltf);
};
```

## 5.2 File Format Support

### 5.2.1 OBJ Loader (Priority 1)
```cpp
class OBJLoader {
public:
    struct OBJData {
        std::vector<Vec3> positions;
        std::vector<Vec2> texCoords;
        std::vector<Vec3> normals;
        
        struct Face {
            struct Vertex {
                int positionIndex;
                int texCoordIndex;
                int normalIndex;
            };
            std::vector<Vertex> vertices;
            std::string materialName;
        };
        std::vector<Face> faces;
        
        std::map<std::string, MTLMaterial> materials;
    };
    
    static bool load(const std::string& filepath, Model* outModel);
    
private:
    static bool parseOBJ(const std::string& filepath, OBJData& data);
    static bool parseMTL(const std::string& filepath, std::map<std::string, MTLMaterial>& materials);
    static void generateNormals(OBJData& data);
    static void triangulate(const OBJData::Face& face, std::vector<OBJData::Face>& triangles);
    static Mesh* buildMesh(const OBJData& data);
};
```

### 5.2.2 MTL Material Support
```cpp
struct MTLMaterial {
    std::string name;
    Vec3 ambient;           // Ka
    Vec3 diffuse;           // Kd
    Vec3 specular;          // Ks
    Vec3 emissive;          // Ke
    float shininess;        // Ns
    float opacity;          // d or Tr
    float ior;              // Ni (index of refraction)
    int illum;              // Illumination model
    
    std::string diffuseMap;     // map_Kd
    std::string specularMap;    // map_Ks
    std::string normalMap;      // map_bump or bump
    std::string alphaMap;       // map_d
    std::string emissiveMap;    // map_Ke
};
```

### 5.2.3 glTF 2.0 Loader (Priority 2)
```cpp
class GLTFLoader {
public:
    static bool load(const std::string& filepath, Model* outModel);
    
private:
    struct GLTFData {
        // JSON document
        json document;
        
        // Binary buffer data
        std::vector<std::vector<uint8_t>> buffers;
        
        // Parsed components
        std::vector<GLTFMesh> meshes;
        std::vector<GLTFMaterial> materials;
        std::vector<GLTFTexture> textures;
        std::vector<GLTFNode> nodes;
    };
    
    static bool parseGLTF(const std::string& filepath, GLTFData& data);
    static bool loadBuffers(const GLTFData& data);
    static Mesh* buildMesh(const GLTFData& data, int meshIndex);
};
```

## 5.3 Scene Graph System

### 5.3.1 Scene Node
```cpp
class SceneNode {
public:
    SceneNode(const std::string& name = "");
    virtual ~SceneNode();
    
    // Transform
    void setLocalTransform(const Mat4& transform);
    Mat4 getWorldTransform() const;
    
    // Hierarchy
    void addChild(std::shared_ptr<SceneNode> child);
    void removeChild(std::shared_ptr<SceneNode> child);
    SceneNode* getParent() const { return mParent; }
    
    // Rendering
    virtual void draw(Renderer* renderer, const Mat4& parentTransform);
    virtual void updateBounds();
    
    // Properties
    void setVisible(bool visible) { mVisible = visible; }
    bool isVisible() const { return mVisible; }
    
protected:
    std::string mName;
    SceneNode* mParent = nullptr;
    std::vector<std::shared_ptr<SceneNode>> mChildren;
    
    Mat4 mLocalTransform;
    bool mTransformDirty = true;
    mutable Mat4 mCachedWorldTransform;
    
    bool mVisible = true;
    BoundingBox mLocalBounds;
    BoundingBox mWorldBounds;
};
```

### 5.3.2 Model Node
```cpp
class ModelNode : public SceneNode {
public:
    ModelNode(std::shared_ptr<Model> model);
    
    void draw(Renderer* renderer, const Mat4& parentTransform) override;
    void updateBounds() override;
    
    void setModel(std::shared_ptr<Model> model);
    Model* getModel() const { return mModel.get(); }
    
private:
    std::shared_ptr<Model> mModel;
};
```

### 5.3.3 Scene Manager
```cpp
class Scene {
public:
    Scene();
    ~Scene();
    
    // Node management
    void addNode(std::shared_ptr<SceneNode> node);
    void removeNode(std::shared_ptr<SceneNode> node);
    void clear();
    
    // Model loading helpers
    std::shared_ptr<ModelNode> loadModel(const std::string& filepath);
    
    // Rendering
    void draw(Renderer* renderer);
    
    // Scene queries
    std::vector<SceneNode*> getNodesInFrustum(const Frustum& frustum);
    SceneNode* pickNode(const Ray& ray);
    
    // Lighting (integrate with existing system)
    void addLight(std::shared_ptr<Light> light);
    void removeLight(std::shared_ptr<Light> light);
    const std::vector<std::shared_ptr<Light>>& getLights() const { return mLights; }
    
private:
    std::shared_ptr<SceneNode> mRootNode;
    std::vector<std::shared_ptr<Light>> mLights;
    std::unordered_map<std::string, std::shared_ptr<Model>> mModelCache;
};
```

## 5.4 Resource Management

### 5.4.1 Model Cache
```cpp
class ModelCache {
public:
    static ModelCache& getInstance();
    
    std::shared_ptr<Model> loadModel(const std::string& filepath);
    std::shared_ptr<Texture> loadTexture(const std::string& filepath);
    
    void clear();
    void releaseUnused();
    
    size_t getModelCount() const { return mModels.size(); }
    size_t getTextureCount() const { return mTextures.size(); }
    size_t getMemoryUsage() const;
    
private:
    std::unordered_map<std::string, std::weak_ptr<Model>> mModels;
    std::unordered_map<std::string, std::weak_ptr<Texture>> mTextures;
};
```

## 5.5 Implementation Plan

### Phase 5.1: Basic OBJ Loading (Week 1)
1. Implement basic OBJ parser
   - Parse vertices, normals, texture coordinates
   - Parse faces with vertex/texcoord/normal indices
   - Handle triangulation of polygons
2. Create Mesh class
   - Store vertex and index data
   - Calculate bounding boxes
3. Update Renderer to accept Mesh objects
   - Add DrawMesh() method
   - Handle multiple submeshes

### Phase 5.2: Material Support (Week 2)
1. Implement MTL parser
   - Parse material properties
   - Handle texture references
2. Enhance Material class
   - Add texture map support
   - Integrate with existing lighting system
3. Update fragment shaders
   - Support material properties
   - Handle texture mapping from files

### Phase 5.3: Scene Graph (Week 3)
1. Implement SceneNode hierarchy
   - Transform inheritance
   - Bounds propagation
2. Create ModelNode class
   - Render models with transforms
3. Basic Scene class
   - Manage node hierarchy
   - Integrate with existing camera/lighting

### Phase 5.4: Advanced Features (Week 4)
1. Model instancing
   - Share mesh data between instances
   - Per-instance transforms
2. Level-of-detail (LOD) support
   - Multiple mesh resolutions
   - Distance-based switching
3. Async loading
   - Background model loading
   - Progress callbacks

### Phase 5.5: glTF Support (Weeks 5-6)
1. JSON parsing with nlohmann/json
2. Binary buffer loading
3. glTF material conversion
4. Animation support (basic)

## 5.6 Testing Strategy

### Unit Tests
- OBJ parser edge cases
- MTL material parsing
- Transform hierarchy calculations
- Bounding box computations

### Integration Tests
- Load standard test models:
  - Stanford Bunny (69K triangles)
  - Utah Teapot (6K triangles)
  - Sponza Palace (262K triangles)
- Verify render output matches reference

### Performance Tests
- Large model loading time
- Memory usage tracking
- Rendering performance with multiple instances

## 5.7 Example Usage

```cpp
// Simple model loading
Scene scene;
auto dragonModel = scene.loadModel("assets/models/dragon.obj");
dragonModel->setLocalTransform(Mat4::scale(2.0f) * Mat4::rotateY(45.0f));

// Manual model creation
auto model = std::make_shared<Model>();
if (model->loadFromFile("assets/models/car.obj")) {
    auto carNode = std::make_shared<ModelNode>(model);
    carNode->setLocalTransform(Mat4::translate(10, 0, 0));
    scene.addNode(carNode);
}

// Instancing
auto treeModel = ModelCache::getInstance().loadModel("assets/models/tree.obj");
for (int i = 0; i < 100; i++) {
    auto treeNode = std::make_shared<ModelNode>(treeModel);
    treeNode->setLocalTransform(Mat4::translate(
        random(-50, 50), 0, random(-50, 50)
    ));
    scene.addNode(treeNode);
}

// Render
scene.draw(renderer);
```

## 5.8 File Structure

```
/src/
  /models/
    model.cpp
    model.h
    mesh.cpp
    mesh.h
    scene_node.cpp
    scene_node.h
    scene.cpp
    scene.h
  /loaders/
    obj_loader.cpp
    obj_loader.h
    mtl_parser.cpp
    mtl_parser.h
    gltf_loader.cpp
    gltf_loader.h
  /resources/
    model_cache.cpp
    model_cache.h
    
/tests/
  /models/
    test_obj_loader.cpp
    test_scene_graph.cpp
    test_model_cache.cpp
```

## Success Criteria

1. Successfully load and render OBJ files with materials
2. Support hierarchical scene graphs with proper transforms
3. Efficient rendering of multiple model instances
4. Memory-efficient resource sharing
5. Maintain 60 FPS with moderate complexity scenes (10-20 models)
6. Clean API that integrates well with existing renderer

## Dependencies

- Existing: Math library, Renderer, Texture, Material, Lighting system
- New: None required for OBJ (custom parser)
- Future: nlohmann/json for glTF support

This phase will transform the renderer into a practical 3D model viewer, setting the foundation for more complex scenes and eventually full 3D applications.