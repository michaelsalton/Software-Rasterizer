#pragma once

#include "math/vec2.h"
#include "math/vec4.h"
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

enum class TextureFormat {
    RGB8,      // 24-bit RGB
    RGBA8,     // 32-bit RGBA
    GRAY8,     // 8-bit grayscale
    FLOAT32    // HDR textures (not implemented yet)
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
    Texture(int width, int height, TextureFormat format = TextureFormat::RGBA8);
    Texture(const std::string& filename); // Load from file
    ~Texture();
    
    // Disable copy, allow move
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    
    // Mipmap generation
    void GenerateMipmaps();
    bool HasMipmaps() const { return mMipLevels.size() > 1; }
    int GetMipLevelCount() const { return static_cast<int>(mMipLevels.size()); }
    
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
    
    TextureFilter GetFilter() const { return mFilter; }
    TextureWrap GetWrapU() const { return mWrapU; }
    TextureWrap GetWrapV() const { return mWrapV; }
    
    // Direct pixel access
    void SetPixel(int x, int y, const Vec4& color, int mipLevel = 0);
    Vec4 GetPixel(int x, int y, int mipLevel = 0) const;
    
    // Raw data access (for uploading)
    uint8_t* GetData(int mipLevel = 0);
    const uint8_t* GetData(int mipLevel = 0) const;
    
    // Utility
    static int GetBytesPerPixel(TextureFormat format);
    
private:
    struct MipLevel {
        int width;
        int height;
        std::vector<uint8_t> data;
        
        MipLevel() : width(0), height(0) {}
        MipLevel(int w, int h, size_t dataSize) : width(w), height(h), data(dataSize) {}
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
    
    // Load texture data from file
    void LoadFromFile(const std::string& filename);
};