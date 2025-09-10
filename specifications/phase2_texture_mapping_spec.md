# Phase 2: Texture Mapping System - Comprehensive Specification

## Executive Summary

This document outlines the complete implementation specification for adding texture mapping capabilities to the software rasterizer. The system will support loading images from disk, generating mipmaps, perspective-correct texture sampling, and multiple filtering modes.

---

## 1. Architecture Overview

### 1.1 Core Components

```
┌─────────────────────┐
│   Image Loader      │ ← Load PNG/JPG files
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│   Texture Class     │ ← Manage texture data, mipmaps
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│  Texture Sampler    │ ← Sample textures with filtering
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│ Fragment Pipeline   │ ← Apply textures in rasterizer
└─────────────────────┘
```

### 1.2 Integration Points

- **Vertex Structure**: Already contains `Vec2 texCoord` for UV coordinates
- **TransformedVertex**: Already propagates texCoords through pipeline
- **Rasterizer**: Add texture sampling in pixel shading loop
- **Fragment Shader**: New system for per-pixel operations

---

## 2. Texture Class Design

### 2.1 Core Texture Class

```cpp
// include/Graphics/Texture.h
#pragma once

#include <vector>
#include <memory>
#include <cstdint>

enum class TextureFormat {
    RGB8,      // 24-bit RGB
    RGBA8,     // 32-bit RGBA
    GRAY8,     // 8-bit grayscale
    FLOAT32    // HDR textures
};

enum class TextureFilter {
    NEAREST,   // Point sampling
    BILINEAR,  // Linear interpolation
    TRILINEAR  // Mipmap + bilinear
};

enum class TextureWrap {
    REPEAT,    // Tile texture
    CLAMP,     // Clamp to edge
    MIRROR,    // Mirror repeat
    BORDER     // Border color
};

class Texture {
public:
    // Constructors
    Texture(int width, int height, TextureFormat format);
    Texture(const std::string& filename); // Load from file
    
    // Mipmap generation
    void GenerateMipmaps();
    bool HasMipmaps() const { return mMipLevels.size() > 1; }
    
    // Sampling interface
    Vec4 Sample(float u, float v, float mipLevel = 0.0f) const;
    Vec4 SampleNearest(float u, float v, int mipLevel = 0) const;
    Vec4 SampleBilinear(float u, float v, int mipLevel = 0) const;
    Vec4 SampleTrilinear(float u, float v, float mipLevel) const;
    
    // Properties
    int GetWidth(int mipLevel = 0) const;
    int GetHeight(int mipLevel = 0) const;
    TextureFormat GetFormat() const { return mFormat; }
    
    // Filter and wrap modes
    void SetFilter(TextureFilter filter) { mFilter = filter; }
    void SetWrapU(TextureWrap wrap) { mWrapU = wrap; }
    void SetWrapV(TextureWrap wrap) { mWrapV = wrap; }
    void SetBorderColor(const Vec4& color) { mBorderColor = color; }
    
    // Direct pixel access
    void SetPixel(int x, int y, const Vec4& color, int mipLevel = 0);
    Vec4 GetPixel(int x, int y, int mipLevel = 0) const;
    
    // Raw data access (for uploading)
    uint8_t* GetData(int mipLevel = 0);
    const uint8_t* GetData(int mipLevel = 0) const;

private:
    struct MipLevel {
        int width;
        int height;
        std::vector<uint8_t> data;
    };
    
    TextureFormat mFormat;
    TextureFilter mFilter;
    TextureWrap mWrapU, mWrapV;
    Vec4 mBorderColor;
    std::vector<MipLevel> mMipLevels;
    
    // Internal helpers
    int WrapCoordinate(int coord, int size, TextureWrap wrap) const;
    Vec4 ConvertToVec4(const uint8_t* pixel) const;
    void ConvertFromVec4(uint8_t* pixel, const Vec4& color) const;
    void DownsampleMipLevel(int srcLevel, int dstLevel);
};
```

### 2.2 Texture Manager

```cpp
// include/Graphics/TextureManager.h
class TextureManager {
public:
    static TextureManager& Instance();
    
    // Load and cache textures
    std::shared_ptr<Texture> LoadTexture(const std::string& filename);
    std::shared_ptr<Texture> CreateTexture(const std::string& name, 
                                          int width, int height, 
                                          TextureFormat format);
    
    // Bind for rendering
    void BindTexture(int slot, std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> GetBoundTexture(int slot) const;
    
    // Clear cache
    void ClearCache();
    
private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> mTextureCache;
    std::array<std::shared_ptr<Texture>, 16> mBoundTextures; // 16 texture slots
};
```

---

## 3. Image Loading System

### 3.1 STB Image Integration

```cpp
// src/Graphics/ImageLoader.cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class ImageLoader {
public:
    struct ImageData {
        int width;
        int height;
        int channels;
        std::unique_ptr<uint8_t[]> pixels;
    };
    
    static ImageData LoadFromFile(const std::string& filename) {
        ImageData data;
        int reqChannels = 4; // Force RGBA
        
        uint8_t* pixels = stbi_load(filename.c_str(), 
                                    &data.width, 
                                    &data.height, 
                                    &data.channels, 
                                    reqChannels);
        
        if (!pixels) {
            throw std::runtime_error("Failed to load image: " + filename);
        }
        
        data.pixels.reset(pixels);
        data.channels = reqChannels;
        return data;
    }
    
    static void SaveToFile(const std::string& filename,
                          const uint8_t* pixels,
                          int width, int height, int channels);
};
```

### 3.2 Supported Formats

- **Input**: PNG, JPG/JPEG, BMP, TGA, PSD, GIF, HDR, PIC
- **Internal**: RGB8, RGBA8, GRAY8, FLOAT32
- **Output**: PNG, BMP, TGA, JPG

---

## 4. Mipmap Generation

### 4.1 Mipmap Chain Creation

```cpp
void Texture::GenerateMipmaps() {
    if (mMipLevels.empty()) return;
    
    int mipWidth = mMipLevels[0].width;
    int mipHeight = mMipLevels[0].height;
    
    // Calculate number of mip levels
    int numLevels = 1 + std::floor(std::log2(std::max(mipWidth, mipHeight)));
    mMipLevels.resize(numLevels);
    
    // Generate each mip level
    for (int level = 1; level < numLevels; ++level) {
        mipWidth = std::max(1, mipWidth / 2);
        mipHeight = std::max(1, mipHeight / 2);
        
        mMipLevels[level].width = mipWidth;
        mMipLevels[level].height = mipHeight;
        mMipLevels[level].data.resize(mipWidth * mipHeight * GetBytesPerPixel());
        
        DownsampleMipLevel(level - 1, level);
    }
}
```

### 4.2 Box Filter Downsampling

```cpp
void Texture::DownsampleMipLevel(int srcLevel, int dstLevel) {
    const MipLevel& src = mMipLevels[srcLevel];
    MipLevel& dst = mMipLevels[dstLevel];
    
    for (int y = 0; y < dst.height; ++y) {
        for (int x = 0; x < dst.width; ++x) {
            // Sample 2x2 block from source
            Vec4 color(0, 0, 0, 0);
            for (int dy = 0; dy < 2; ++dy) {
                for (int dx = 0; dx < 2; ++dx) {
                    int sx = std::min(x * 2 + dx, src.width - 1);
                    int sy = std::min(y * 2 + dy, src.height - 1);
                    color += GetPixel(sx, sy, srcLevel);
                }
            }
            color *= 0.25f; // Average
            SetPixel(x, y, color, dstLevel);
        }
    }
}
```

---

## 5. UV Coordinate Pipeline

### 5.1 Vertex UV Storage

```cpp
// Already implemented in Vertex.h
struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;  // UV coordinates [0,1]
    Framebuffer::Color color;
};
```

### 5.2 UV Interpolation in Rasterizer

```cpp
// In Rasterizer::RasterizeTriangle
// After calculating barycentric coordinates:

// Perspective-correct UV interpolation
float w0 = alpha / v0.clipPosition.w;
float w1 = beta / v1.clipPosition.w;
float w2 = gamma / v2.clipPosition.w;
float wSum = w0 + w1 + w2;

float u = (v0.texCoord.x * w0 + v1.texCoord.x * w1 + v2.texCoord.x * w2) / wSum;
float v = (v0.texCoord.y * w0 + v1.texCoord.y * w1 + v2.texCoord.y * w2) / wSum;
```

### 5.3 UV Coordinate Conventions

- **Origin**: Top-left (0,0) to bottom-right (1,1)
- **Range**: [0,1] for standard mapping
- **Out-of-bounds**: Handled by wrap modes

---

## 6. Perspective-Correct Interpolation

### 6.1 Mathematical Foundation

For perspective-correct interpolation:
```
attribute_screen = (a0/w0 + a1/w1 + a2/w2) / (1/w0 + 1/w1 + 1/w2)
```

Where:
- a0, a1, a2 = attribute values at vertices
- w0, w1, w2 = clip space W coordinates

### 6.2 Implementation

```cpp
struct PerspectiveInterpolator {
    float w0, w1, w2;  // Reciprocal W values
    float wSum;        // Sum of reciprocals
    
    void Setup(const TransformedVertex& v0, 
               const TransformedVertex& v1,
               const TransformedVertex& v2,
               float alpha, float beta, float gamma) {
        w0 = alpha / v0.clipPosition.w;
        w1 = beta / v1.clipPosition.w;
        w2 = gamma / v2.clipPosition.w;
        wSum = w0 + w1 + w2;
    }
    
    Vec2 InterpolateUV(const Vec2& uv0, const Vec2& uv1, const Vec2& uv2) {
        float u = (uv0.x * w0 + uv1.x * w1 + uv2.x * w2) / wSum;
        float v = (uv0.y * w0 + uv1.y * w1 + uv2.y * w2) / wSum;
        return Vec2(u, v);
    }
    
    float InterpolateDepth(float z0, float z1, float z2) {
        return (z0 * w0 + z1 * w1 + z2 * w2) / wSum;
    }
};
```

---

## 7. Texture Sampling Methods

### 7.1 Nearest Neighbor (Point) Sampling

```cpp
Vec4 Texture::SampleNearest(float u, float v, int mipLevel) const {
    const MipLevel& mip = mMipLevels[mipLevel];
    
    // Convert UV to texel coordinates
    int x = static_cast<int>(u * mip.width);
    int y = static_cast<int>(v * mip.height);
    
    // Apply wrap mode
    x = WrapCoordinate(x, mip.width, mWrapU);
    y = WrapCoordinate(y, mip.height, mWrapV);
    
    return GetPixel(x, y, mipLevel);
}
```

### 7.2 Bilinear Filtering

```cpp
Vec4 Texture::SampleBilinear(float u, float v, int mipLevel) const {
    const MipLevel& mip = mMipLevels[mipLevel];
    
    // Convert to texel space
    float fx = u * mip.width - 0.5f;
    float fy = v * mip.height - 0.5f;
    
    // Get integer coordinates
    int x0 = static_cast<int>(std::floor(fx));
    int y0 = static_cast<int>(std::floor(fy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    
    // Fractional parts
    float sx = fx - x0;
    float sy = fy - y0;
    
    // Apply wrap mode to all coordinates
    x0 = WrapCoordinate(x0, mip.width, mWrapU);
    x1 = WrapCoordinate(x1, mip.width, mWrapU);
    y0 = WrapCoordinate(y0, mip.height, mWrapV);
    y1 = WrapCoordinate(y1, mip.height, mWrapV);
    
    // Bilinear interpolation
    Vec4 c00 = GetPixel(x0, y0, mipLevel);
    Vec4 c10 = GetPixel(x1, y0, mipLevel);
    Vec4 c01 = GetPixel(x0, y1, mipLevel);
    Vec4 c11 = GetPixel(x1, y1, mipLevel);
    
    Vec4 c0 = Vec4::lerp(c00, c10, sx);
    Vec4 c1 = Vec4::lerp(c01, c11, sx);
    
    return Vec4::lerp(c0, c1, sy);
}
```

### 7.3 Trilinear Filtering

```cpp
Vec4 Texture::SampleTrilinear(float u, float v, float mipLevel) const {
    // Clamp mip level
    mipLevel = std::max(0.0f, std::min(mipLevel, 
                        static_cast<float>(mMipLevels.size() - 1)));
    
    // Get integer mip levels
    int mip0 = static_cast<int>(std::floor(mipLevel));
    int mip1 = std::min(mip0 + 1, static_cast<int>(mMipLevels.size() - 1));
    
    // Fractional part
    float t = mipLevel - mip0;
    
    // Sample both mip levels
    Vec4 c0 = SampleBilinear(u, v, mip0);
    Vec4 c1 = SampleBilinear(u, v, mip1);
    
    // Lerp between mip levels
    return Vec4::lerp(c0, c1, t);
}
```

### 7.4 Wrap Mode Implementation

```cpp
int Texture::WrapCoordinate(int coord, int size, TextureWrap wrap) const {
    switch (wrap) {
        case TextureWrap::REPEAT:
            return ((coord % size) + size) % size;
            
        case TextureWrap::CLAMP:
            return std::max(0, std::min(coord, size - 1));
            
        case TextureWrap::MIRROR: {
            int period = size * 2;
            coord = ((coord % period) + period) % period;
            if (coord >= size) {
                coord = period - coord - 1;
            }
            return coord;
        }
        
        case TextureWrap::BORDER:
            if (coord < 0 || coord >= size) {
                return -1; // Special value for border
            }
            return coord;
    }
}
```

---

## 8. Fragment Shader System

### 8.1 Fragment Shader Interface

```cpp
// include/Graphics/FragmentShader.h
class FragmentShader {
public:
    struct FragmentInput {
        Vec2 screenPos;      // Pixel position
        float depth;         // Interpolated depth
        Vec3 worldPos;       // World position
        Vec3 worldNormal;    // World normal
        Vec2 texCoord;       // UV coordinates
        Vec4 color;          // Vertex color
    };
    
    struct FragmentOutput {
        Vec4 color;          // Final pixel color [0,1]
        float depth;         // Modified depth (optional)
        bool discard;        // Discard this fragment
    };
    
    // Main shader function
    virtual FragmentOutput Shade(const FragmentInput& input,
                                const ShaderUniforms& uniforms) = 0;
};

// Basic textured shader
class TexturedShader : public FragmentShader {
public:
    FragmentOutput Shade(const FragmentInput& input,
                        const ShaderUniforms& uniforms) override {
        FragmentOutput output;
        
        // Sample texture
        auto texture = TextureManager::Instance().GetBoundTexture(0);
        if (texture) {
            output.color = texture->Sample(input.texCoord.x, 
                                         input.texCoord.y);
        } else {
            output.color = Vec4(input.color) / 255.0f;
        }
        
        output.depth = input.depth;
        output.discard = false;
        return output;
    }
};
```

### 8.2 Rasterizer Integration

```cpp
// In Rasterizer::RasterizeTriangle
void Rasterizer::RasterizeTriangleWithShading(/* ... */) {
    // ... existing setup ...
    
    // Get active fragment shader
    FragmentShader* shader = mPipeline->GetFragmentShader();
    if (!shader) {
        shader = &mDefaultTexturedShader;
    }
    
    // For each pixel in triangle
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            // ... barycentric calculation ...
            
            if (inside) {
                // Setup fragment input
                FragmentShader::FragmentInput fragInput;
                fragInput.screenPos = Vec2(x, y);
                fragInput.depth = interpolatedDepth;
                fragInput.worldPos = interpolatedWorldPos;
                fragInput.worldNormal = interpolatedNormal.normalized();
                fragInput.texCoord = interpolatedUV;
                fragInput.color = interpolatedColor;
                
                // Run fragment shader
                auto fragOutput = shader->Shade(fragInput, mUniforms);
                
                if (!fragOutput.discard && 
                    mFramebuffer->DepthTest(x, y, fragOutput.depth)) {
                    // Convert color to 8-bit
                    Framebuffer::Color finalColor(
                        static_cast<uint8_t>(fragOutput.color.x * 255),
                        static_cast<uint8_t>(fragOutput.color.y * 255),
                        static_cast<uint8_t>(fragOutput.color.z * 255),
                        static_cast<uint8_t>(fragOutput.color.w * 255)
                    );
                    
                    mFramebuffer->writePixel(x, y, finalColor, 
                                           fragOutput.depth, true);
                }
            }
        }
    }
}
```

---

## 9. Performance Considerations

### 9.1 Texture Cache Optimization

```cpp
class TextureCache {
    struct CacheLine {
        int x, y, mipLevel;
        Vec4 color;
    };
    
    // Direct-mapped cache for texture samples
    std::array<CacheLine, 1024> mCache;
    
    Vec4 CachedSample(const Texture* tex, int x, int y, int mip) {
        uint32_t hash = Hash(tex, x, y, mip);
        uint32_t index = hash & 1023;
        
        CacheLine& line = mCache[index];
        if (line.x == x && line.y == y && line.mipLevel == mip) {
            return line.color; // Cache hit
        }
        
        // Cache miss - fetch and update
        line.x = x;
        line.y = y;
        line.mipLevel = mip;
        line.color = tex->GetPixel(x, y, mip);
        return line.color;
    }
};
```

### 9.2 Mip Level Selection

```cpp
float CalculateMipLevel(const Vec2& uv0, const Vec2& uv1, const Vec2& uv2,
                       float area, int textureWidth, int textureHeight) {
    // Calculate texture area covered by triangle
    Vec2 duv1 = uv1 - uv0;
    Vec2 duv2 = uv2 - uv0;
    float uvArea = std::abs(duv1.x * duv2.y - duv1.y * duv2.x);
    
    // Texels per pixel ratio
    float texelsPerPixel = uvArea * textureWidth * textureHeight / area;
    
    // Mip level = log2 of texel/pixel ratio
    return std::max(0.0f, 0.5f * std::log2(texelsPerPixel));
}
```

### 9.3 SIMD Texture Sampling

```cpp
// Process 4 pixels at once
void SampleTexture4x(const Texture* tex, 
                    const float u[4], const float v[4],
                    Vec4 output[4]) {
    __m128 u_vec = _mm_load_ps(u);
    __m128 v_vec = _mm_load_ps(v);
    
    // Convert to texel coordinates
    __m128 fx = _mm_mul_ps(u_vec, _mm_set1_ps(tex->GetWidth()));
    __m128 fy = _mm_mul_ps(v_vec, _mm_set1_ps(tex->GetHeight()));
    
    // ... SIMD bilinear sampling ...
}
```

---

## 10. Testing Plan

### 10.1 Unit Tests

```cpp
// tests/Graphics/TextureTest.cpp
TEST(Texture, LoadPNG) {
    Texture tex("assets/test.png");
    EXPECT_EQ(tex.GetWidth(), 256);
    EXPECT_EQ(tex.GetHeight(), 256);
}

TEST(Texture, BilinearSampling) {
    Texture tex(2, 2, TextureFormat::RGBA8);
    tex.SetPixel(0, 0, Vec4(0, 0, 0, 1));
    tex.SetPixel(1, 0, Vec4(1, 0, 0, 1));
    tex.SetPixel(0, 1, Vec4(0, 1, 0, 1));
    tex.SetPixel(1, 1, Vec4(1, 1, 0, 1));
    
    Vec4 center = tex.SampleBilinear(0.5f, 0.5f);
    EXPECT_NEAR(center.x, 0.5f, 0.001f);
    EXPECT_NEAR(center.y, 0.5f, 0.001f);
}

TEST(Texture, MipmapGeneration) {
    Texture tex(128, 128, TextureFormat::RGBA8);
    tex.GenerateMipmaps();
    EXPECT_EQ(tex.GetMipLevelCount(), 8); // log2(128) + 1
}
```

### 10.2 Visual Tests

1. **Checkerboard Pattern**: Verify correct UV mapping
2. **Gradient Texture**: Test interpolation quality
3. **Mipmap Visualization**: Color each mip level differently
4. **Wrap Mode Test**: Show all wrap modes side-by-side

### 10.3 Performance Benchmarks

- Texture sampling throughput (samples/second)
- Cache hit rate monitoring
- Mipmap level selection accuracy
- Fragment shader overhead

---

## 11. Implementation Milestones

### Milestone 1: Basic Texture Loading (Week 1)
- [ ] Integrate stb_image
- [ ] Implement Texture class constructor
- [ ] Load PNG/JPG files
- [ ] Basic nearest-neighbor sampling

### Milestone 2: UV Pipeline (Week 1-2)
- [ ] Verify UV coordinate flow through pipeline
- [ ] Implement perspective-correct interpolation
- [ ] Add UV visualization mode
- [ ] Test with textured quad

### Milestone 3: Filtering (Week 2)
- [ ] Implement bilinear filtering
- [ ] Add wrap modes
- [ ] Implement mipmap generation
- [ ] Add trilinear filtering

### Milestone 4: Fragment Shaders (Week 3)
- [ ] Design fragment shader interface
- [ ] Create basic textured shader
- [ ] Integrate with rasterizer
- [ ] Add shader hot-reloading

### Milestone 5: Optimization (Week 3-4)
- [ ] Add texture cache
- [ ] Implement SIMD sampling
- [ ] Profile and optimize
- [ ] Add debugging visualizations

### Milestone 6: Polish (Week 4)
- [ ] Multiple texture support
- [ ] Texture manager with caching
- [ ] Advanced shaders (lit textured)
- [ ] Documentation and examples

---

## 12. File Structure

```
Software-Rasterizer/
├── include/Graphics/
│   ├── Texture.h
│   ├── TextureManager.h
│   ├── FragmentShader.h
│   └── ImageLoader.h
├── src/Graphics/
│   ├── Texture.cpp
│   ├── TextureManager.cpp
│   ├── FragmentShader.cpp
│   └── ImageLoader.cpp
├── assets/textures/
│   ├── test_checkerboard.png
│   ├── test_gradient.png
│   └── uvgrid.png
└── third_party/
    └── stb/
        └── stb_image.h
```

---

## 13. Example Usage

### Basic Texture Loading and Rendering

```cpp
// Load texture
auto texture = TextureManager::Instance().LoadTexture("assets/brick.png");
texture->SetFilter(TextureFilter::BILINEAR);
texture->SetWrapU(TextureWrap::REPEAT);
texture->SetWrapV(TextureWrap::REPEAT);
texture->GenerateMipmaps();

// Bind for rendering
TextureManager::Instance().BindTexture(0, texture);

// Create textured cube
std::vector<Vertex> vertices = {
    // Front face with UVs
    Vertex(Vec3(-0.5f, -0.5f, 0.5f), Vec3(0,0,1), Vec2(0,1)),
    Vertex(Vec3( 0.5f, -0.5f, 0.5f), Vec3(0,0,1), Vec2(1,1)),
    Vertex(Vec3( 0.5f,  0.5f, 0.5f), Vec3(0,0,1), Vec2(1,0)),
    Vertex(Vec3(-0.5f,  0.5f, 0.5f), Vec3(0,0,1), Vec2(0,0)),
    // ... other faces
};

// Set fragment shader
renderer->SetFragmentShader(std::make_unique<TexturedShader>());

// Render
renderer->DrawIndexedMesh(vertices, indices);
```

---

## 14. Success Criteria

The texture mapping system will be considered complete when:

1. ✅ Can load common image formats (PNG, JPG)
2. ✅ UV coordinates flow correctly through pipeline
3. ✅ Perspective-correct texture mapping works
4. ✅ All filtering modes implemented and tested
5. ✅ Mipmapping reduces aliasing effectively
6. ✅ Fragment shader system integrated
7. ✅ Performance meets targets (>30 FPS for textured scene)
8. ✅ Memory usage is reasonable (<100MB for 10 textures)
9. ✅ Visual quality matches reference renderer
10. ✅ All unit tests pass

---

## 15. Dependencies

### External Libraries
- **stb_image.h**: Single-header image loading library
  - Version: 2.28 or later
  - License: MIT/Public Domain
  - Source: https://github.com/nothings/stb

### Internal Dependencies
- Existing Math library (Vec2, Vec3, Vec4, Mat4)
- Framebuffer class for pixel writing
- Vertex structures with UV support
- Rasterizer with barycentric coordinates

---

## Appendix A: Common Pitfalls

1. **UV Orientation**: Ensure consistent UV space (Y-up vs Y-down)
2. **Premultiplied Alpha**: Handle correctly in blending
3. **Power-of-Two**: Non-POT texture support
4. **Precision**: Use float UVs, not fixed-point
5. **Cache Misses**: Optimize texture access patterns

---

## Appendix B: Future Extensions

1. **Compressed Textures**: DXT1/5, ETC2, ASTC
2. **Cube Maps**: For environment mapping
3. **3D Textures**: For volumetric effects
4. **Texture Arrays**: For batching
5. **Virtual Texturing**: For massive textures
6. **Anisotropic Filtering**: Full implementation
7. **Texture Streaming**: LOD management