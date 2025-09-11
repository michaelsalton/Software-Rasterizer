#include "graphics/frame_buffer.h"
#include <cmath>

// Color implementation
Framebuffer::Color::Color() : r(0), g(0), b(0), a(255) {}

Framebuffer::Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) 
    : r(r), g(g), b(b), a(a) {}

uint32_t Framebuffer::Color::toUint32() const {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

Framebuffer::Color Framebuffer::Color::fromUint32(uint32_t color) {
    return Color(
        (color >> 16) & 0xFF,
        (color >> 8) & 0xFF,
        color & 0xFF,
        (color >> 24) & 0xFF
    );
}

Framebuffer::Color Framebuffer::Color::blend(const Color& dst) const {
    if (a == 255) return *this; // Opaque, no blending needed
    if (a == 0) return dst; // Fully transparent
    
    float alpha = a / 255.0f;
    float invAlpha = 1.0f - alpha;
    
    return Color(
        static_cast<uint8_t>(r * alpha + dst.r * invAlpha),
        static_cast<uint8_t>(g * alpha + dst.g * invAlpha),
        static_cast<uint8_t>(b * alpha + dst.b * invAlpha),
        static_cast<uint8_t>(a + dst.a * invAlpha)
    );
}

// Framebuffer implementation
Framebuffer::Framebuffer(int width, int height) 
    : width(width), height(height), bufferSize(width * height) {
    colorBuffer = new uint32_t[bufferSize];
    depthBuffer = new float[bufferSize];
    clear();
}

Framebuffer::~Framebuffer() {
    delete[] colorBuffer;
    delete[] depthBuffer;
}

void Framebuffer::clear(const Color& color) {
    clearColor(color);
    clearDepth(1.0f);
}

void Framebuffer::clearColor(const Color& color) {
    uint32_t clearValue = color.toUint32();
    // Efficient clear using memset for black, otherwise use std::fill
    if (clearValue == 0xFF000000) { // Opaque black
        memset(colorBuffer, 0, bufferSize * sizeof(uint32_t));
    } else {
        std::fill(colorBuffer, colorBuffer + bufferSize, clearValue);
    }
}

void Framebuffer::clearDepth(float depth) {
    // Most common case: clearing to 1.0f
    if (depth == 1.0f) {
        // On some platforms, all bits set to 1 represents 1.0f in IEEE 754
        memset(depthBuffer, 0xFF, bufferSize * sizeof(float));
    } else {
        std::fill(depthBuffer, depthBuffer + bufferSize, depth);
    }
}

bool Framebuffer::writePixel(int x, int y, const Color& color, float depth, bool depthTest) {
    // Bounds check
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    
    int index = y * width + x;
    
    // Depth test
    if (depthTest) {
        if (depth >= depthBuffer[index]) {
            return false; // Failed depth test
        }
        depthBuffer[index] = depth;
    }
    
    // Write color
    colorBuffer[index] = color.toUint32();
    return true;
}

bool Framebuffer::writePixelBlended(int x, int y, const Color& color, float depth, bool depthTest) {
    // Bounds check
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    
    int index = y * width + x;
    
    // Depth test
    if (depthTest) {
        if (depth >= depthBuffer[index]) {
            return false; // Failed depth test
        }
        depthBuffer[index] = depth;
    }
    
    // Blend with existing color
    Color dstColor = Color::fromUint32(colorBuffer[index]);
    Color blended = color.blend(dstColor);
    colorBuffer[index] = blended.toUint32();
    return true;
}

Framebuffer::Color Framebuffer::getPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return Color(0, 0, 0, 0);
    }
    return Color::fromUint32(colorBuffer[y * width + x]);
}

float Framebuffer::getDepth(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return std::numeric_limits<float>::max();
    }
    return depthBuffer[y * width + x];
}

uint32_t* Framebuffer::getColorBuffer() { 
    return colorBuffer; 
}

const uint32_t* Framebuffer::getColorBuffer() const { 
    return colorBuffer; 
}

float* Framebuffer::getDepthBuffer() { 
    return depthBuffer; 
}

const float* Framebuffer::getDepthBuffer() const { 
    return depthBuffer; 
}

int Framebuffer::getWidth() const { 
    return width; 
}

int Framebuffer::getHeight() const { 
    return height; 
}

void Framebuffer::resize(int newWidth, int newHeight) {
    if (newWidth == width && newHeight == height) return;
    
    delete[] colorBuffer;
    delete[] depthBuffer;
    
    width = newWidth;
    height = newHeight;
    bufferSize = width * height;
    
    colorBuffer = new uint32_t[bufferSize];
    depthBuffer = new float[bufferSize];
    clear();
}

void Framebuffer::fillRect(int x, int y, int w, int h, const Color& color, float depth, bool depthTest) {
    int x0 = std::max(0, x);
    int y0 = std::max(0, y);
    int x1 = std::min(width, x + w);
    int y1 = std::min(height, y + h);
    
    for (int py = y0; py < y1; ++py) {
        for (int px = x0; px < x1; ++px) {
            writePixel(px, py, color, depth, depthTest);
        }
    }
}

void Framebuffer::drawLine(int x0, int y0, int x1, int y1, const Color& color, float z0, float z1, bool depthTest) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    float totalDist = sqrt(dx * dx + dy * dy);
    if (totalDist == 0) totalDist = 1;
    
    while (true) {
        // Interpolate depth
        float t = 0;
        if (dx > dy) {
            t = (x0 - (x1 < x0 ? x1 : x0)) / float(dx);
        } else {
            t = (y0 - (y1 < y0 ? y1 : y0)) / float(dy);
        }
        float z = z0 + (z1 - z0) * t;
        
        writePixel(x0, y0, color, z, depthTest);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}