#include "rendering/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb/stb_image.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

Texture::Texture(int width, int height, TextureFormat format)
    : mFormat(format)
    , mFilter(TextureFilter::NEAREST)
    , mWrapU(TextureWrap::REPEAT)
    , mWrapV(TextureWrap::REPEAT)
    , mBorderColor(0, 0, 0, 1)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Invalid texture dimensions");
    }
    
    // Create base mip level
    size_t dataSize = width * height * GetBytesPerPixel(format);
    mMipLevels.emplace_back(width, height, dataSize);
    
    // Initialize to black
    std::fill(mMipLevels[0].data.begin(), mMipLevels[0].data.end(), 0);
}

Texture::Texture(const std::string& filename)
    : mFormat(TextureFormat::RGBA8)
    , mFilter(TextureFilter::NEAREST)
    , mWrapU(TextureWrap::REPEAT)
    , mWrapV(TextureWrap::REPEAT)
    , mBorderColor(0, 0, 0, 1)
{
    LoadFromFile(filename);
}

Texture::~Texture() = default;

Texture::Texture(Texture&& other) noexcept
    : mFormat(other.mFormat)
    , mFilter(other.mFilter)
    , mWrapU(other.mWrapU)
    , mWrapV(other.mWrapV)
    , mBorderColor(other.mBorderColor)
    , mMipLevels(std::move(other.mMipLevels))
{
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        mFormat = other.mFormat;
        mFilter = other.mFilter;
        mWrapU = other.mWrapU;
        mWrapV = other.mWrapV;
        mBorderColor = other.mBorderColor;
        mMipLevels = std::move(other.mMipLevels);
    }
    return *this;
}

void Texture::LoadFromFile(const std::string& filename) {
    int width, height, channels;
    int reqChannels = 4; // Force RGBA
    
    // Load image data
    unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &channels, reqChannels);
    if (!pixels) {
        throw std::runtime_error("Failed to load texture: " + filename + " - " + stbi_failure_reason());
    }
    
    // Create texture data
    mFormat = TextureFormat::RGBA8;
    size_t dataSize = width * height * 4;
    mMipLevels.clear();
    mMipLevels.emplace_back(width, height, dataSize);
    
    // Copy pixel data
    std::memcpy(mMipLevels[0].data.data(), pixels, dataSize);
    
    // Free loaded image
    stbi_image_free(pixels);
}

int Texture::GetWidth(int mipLevel) const {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        return 0;
    }
    return mMipLevels[mipLevel].width;
}

int Texture::GetHeight(int mipLevel) const {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        return 0;
    }
    return mMipLevels[mipLevel].height;
}

int Texture::GetBytesPerPixel(TextureFormat format) {
    switch (format) {
        case TextureFormat::GRAY8: return 1;
        case TextureFormat::RGB8: return 3;
        case TextureFormat::RGBA8: return 4;
        case TextureFormat::FLOAT32: return 16; // 4 floats
        default: return 4;
    }
}

uint8_t* Texture::GetData(int mipLevel) {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        return nullptr;
    }
    return mMipLevels[mipLevel].data.data();
}

const uint8_t* Texture::GetData(int mipLevel) const {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        return nullptr;
    }
    return mMipLevels[mipLevel].data.data();
}

Vec4 Texture::GetPixel(int x, int y, int mipLevel) const {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        return mBorderColor;
    }
    
    const MipLevel& mip = mMipLevels[mipLevel];
    
    // Bounds check
    if (x < 0 || x >= mip.width || y < 0 || y >= mip.height) {
        return mBorderColor;
    }
    
    const uint8_t* pixel = &mip.data[(y * mip.width + x) * GetBytesPerPixel(mFormat)];
    return ConvertToVec4(pixel);
}

void Texture::SetPixel(int x, int y, const Vec4& color, int mipLevel) {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        return;
    }
    
    MipLevel& mip = mMipLevels[mipLevel];
    
    // Bounds check
    if (x < 0 || x >= mip.width || y < 0 || y >= mip.height) {
        return;
    }
    
    uint8_t* pixel = &mip.data[(y * mip.width + x) * GetBytesPerPixel(mFormat)];
    ConvertFromVec4(pixel, color);
}

Vec4 Texture::ConvertToVec4(const uint8_t* pixel) const {
    switch (mFormat) {
        case TextureFormat::GRAY8:
            return Vec4(pixel[0] / 255.0f, pixel[0] / 255.0f, pixel[0] / 255.0f, 1.0f);
            
        case TextureFormat::RGB8:
            return Vec4(pixel[0] / 255.0f, pixel[1] / 255.0f, pixel[2] / 255.0f, 1.0f);
            
        case TextureFormat::RGBA8:
            return Vec4(pixel[0] / 255.0f, pixel[1] / 255.0f, pixel[2] / 255.0f, pixel[3] / 255.0f);
            
        default:
            return Vec4(0, 0, 0, 1);
    }
}

void Texture::ConvertFromVec4(uint8_t* pixel, const Vec4& color) const {
    // Clamp values to [0,1]
    float r = std::max(0.0f, std::min(1.0f, color.x));
    float g = std::max(0.0f, std::min(1.0f, color.y));
    float b = std::max(0.0f, std::min(1.0f, color.z));
    float a = std::max(0.0f, std::min(1.0f, color.w));
    
    switch (mFormat) {
        case TextureFormat::GRAY8:
            pixel[0] = static_cast<uint8_t>((r * 0.299f + g * 0.587f + b * 0.114f) * 255);
            break;
            
        case TextureFormat::RGB8:
            pixel[0] = static_cast<uint8_t>(r * 255);
            pixel[1] = static_cast<uint8_t>(g * 255);
            pixel[2] = static_cast<uint8_t>(b * 255);
            break;
            
        case TextureFormat::RGBA8:
            pixel[0] = static_cast<uint8_t>(r * 255);
            pixel[1] = static_cast<uint8_t>(g * 255);
            pixel[2] = static_cast<uint8_t>(b * 255);
            pixel[3] = static_cast<uint8_t>(a * 255);
            break;
            
        default:
            break;
    }
}

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
            
        default:
            return 0;
    }
}

Vec4 Texture::Sample(float u, float v, float mipLevel) const {
    switch (mFilter) {
        case TextureFilter::NEAREST:
            return SampleNearest(u, v, static_cast<int>(mipLevel));
            
        case TextureFilter::BILINEAR:
            return SampleBilinear(u, v, static_cast<int>(mipLevel));
            
        case TextureFilter::TRILINEAR:
            if (HasMipmaps()) {
                return SampleTrilinear(u, v, mipLevel);
            } else {
                return SampleBilinear(u, v, 0);
            }
            
        default:
            return Vec4(0, 0, 0, 1);
    }
}

Vec4 Texture::SampleNearest(float u, float v, int mipLevel) const {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        mipLevel = 0;
    }
    
    const MipLevel& mip = mMipLevels[mipLevel];
    
    // Convert UV to texel coordinates
    int x = static_cast<int>(u * mip.width);
    int y = static_cast<int>(v * mip.height);
    
    // Apply wrap mode
    x = WrapCoordinate(x, mip.width, mWrapU);
    y = WrapCoordinate(y, mip.height, mWrapV);
    
    // Check for border
    if (x == -1 || y == -1) {
        return mBorderColor;
    }
    
    return GetPixel(x, y, mipLevel);
}

Vec4 Texture::SampleBilinear(float u, float v, int mipLevel) const {
    if (mipLevel < 0 || mipLevel >= static_cast<int>(mMipLevels.size())) {
        mipLevel = 0;
    }
    
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
    
    // Sample four pixels
    Vec4 c00 = (x0 == -1 || y0 == -1) ? mBorderColor : GetPixel(x0, y0, mipLevel);
    Vec4 c10 = (x1 == -1 || y0 == -1) ? mBorderColor : GetPixel(x1, y0, mipLevel);
    Vec4 c01 = (x0 == -1 || y1 == -1) ? mBorderColor : GetPixel(x0, y1, mipLevel);
    Vec4 c11 = (x1 == -1 || y1 == -1) ? mBorderColor : GetPixel(x1, y1, mipLevel);
    
    // Bilinear interpolation
    Vec4 c0 = c00 * (1 - sx) + c10 * sx;
    Vec4 c1 = c01 * (1 - sx) + c11 * sx;
    
    return c0 * (1 - sy) + c1 * sy;
}

Vec4 Texture::SampleTrilinear(float u, float v, float mipLevel) const {
    // Clamp mip level
    mipLevel = std::max(0.0f, std::min(mipLevel, static_cast<float>(mMipLevels.size() - 1)));
    
    // Get integer mip levels
    int mip0 = static_cast<int>(std::floor(mipLevel));
    int mip1 = std::min(mip0 + 1, static_cast<int>(mMipLevels.size() - 1));
    
    // Fractional part
    float t = mipLevel - mip0;
    
    // Sample both mip levels
    Vec4 c0 = SampleBilinear(u, v, mip0);
    Vec4 c1 = SampleBilinear(u, v, mip1);
    
    // Lerp between mip levels
    return c0 * (1 - t) + c1 * t;
}

void Texture::GenerateMipmaps() {
    if (mMipLevels.empty()) return;
    
    int mipWidth = mMipLevels[0].width;
    int mipHeight = mMipLevels[0].height;
    
    // Calculate number of mip levels
    int numLevels = 1 + static_cast<int>(std::floor(std::log2(std::max(mipWidth, mipHeight))));
    
    // Reserve space for all mip levels
    mMipLevels.reserve(numLevels);
    
    // Generate each mip level
    for (int level = 1; level < numLevels; ++level) {
        mipWidth = std::max(1, mipWidth / 2);
        mipHeight = std::max(1, mipHeight / 2);
        
        size_t dataSize = mipWidth * mipHeight * GetBytesPerPixel(mFormat);
        mMipLevels.emplace_back(mipWidth, mipHeight, dataSize);
        
        DownsampleMipLevel(level - 1, level);
    }
}

void Texture::DownsampleMipLevel(int srcLevel, int dstLevel) {
    const MipLevel& src = mMipLevels[srcLevel];
    MipLevel& dst = mMipLevels[dstLevel];
    
    for (int y = 0; y < dst.height; ++y) {
        for (int x = 0; x < dst.width; ++x) {
            // Sample 2x2 block from source
            Vec4 color(0, 0, 0, 0);
            int numSamples = 0;
            
            for (int dy = 0; dy < 2; ++dy) {
                for (int dx = 0; dx < 2; ++dx) {
                    int sx = std::min(x * 2 + dx, src.width - 1);
                    int sy = std::min(y * 2 + dy, src.height - 1);
                    color = color + GetPixel(sx, sy, srcLevel);
                    numSamples++;
                }
            }
            
            // Average the samples
            if (numSamples > 0) {
                color = color * (1.0f / numSamples);
            }
            
            SetPixel(x, y, color, dstLevel);
        }
    }
}