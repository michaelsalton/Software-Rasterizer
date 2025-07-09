#pragma once

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <limits>

class Framebuffer {
public:
    struct Color {
        uint8_t r, g, b, a;
        
        Color() : r(0), g(0), b(0), a(255) {}
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) 
            : r(r), g(g), b(b), a(a) {}
        
        uint32_t toUint32() const {
            return (a << 24) | (r << 16) | (g << 8) | b;
        }
        
        static Color fromUint32(uint32_t color) {
            return Color(
                (color >> 16) & 0xFF,
                (color >> 8) & 0xFF,
                color & 0xFF,
                (color >> 24) & 0xFF
            );
        }
        
        // Blend this color over another color
        Color blend(const Color& dst) const {
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
    };

private:
    uint32_t* colorBuffer;
    float* depthBuffer;
    int width;
    int height;
    int bufferSize;

public:
    Framebuffer(int width, int height) 
        : width(width), height(height), bufferSize(width * height) {
        colorBuffer = new uint32_t[bufferSize];
        depthBuffer = new float[bufferSize];
        clear();
    }
    
    ~Framebuffer() {
        delete[] colorBuffer;
        delete[] depthBuffer;
    }
    
    // Clear both buffers
    void clear(const Color& color = Color(0, 0, 0, 255)) {
        clearColor(color);
        clearDepth(1.0f);
    }
    
    // Clear color buffer only
    void clearColor(const Color& color = Color(0, 0, 0, 255)) {
        uint32_t clearValue = color.toUint32();
        // Efficient clear using memset for black, otherwise use std::fill
        if (clearValue == 0xFF000000) { // Opaque black
            memset(colorBuffer, 0, bufferSize * sizeof(uint32_t));
        } else {
            std::fill(colorBuffer, colorBuffer + bufferSize, clearValue);
        }
    }
    
    // Clear depth buffer only
    void clearDepth(float depth = 1.0f) {
        // Most common case: clearing to 1.0f
        if (depth == 1.0f) {
            // On some platforms, all bits set to 1 represents 1.0f in IEEE 754
            memset(depthBuffer, 0xFF, bufferSize * sizeof(float));
        } else {
            std::fill(depthBuffer, depthBuffer + bufferSize, depth);
        }
    }
    
    // Write pixel with bounds checking and optional depth testing
    bool writePixel(int x, int y, const Color& color, float depth = 0.0f, bool depthTest = true) {
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
    
    // Write pixel with blending
    bool writePixelBlended(int x, int y, const Color& color, float depth = 0.0f, bool depthTest = true) {
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
    
    // Get pixel color
    Color getPixel(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return Color(0, 0, 0, 0);
        }
        return Color::fromUint32(colorBuffer[y * width + x]);
    }
    
    // Get depth value
    float getDepth(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return std::numeric_limits<float>::max();
        }
        return depthBuffer[y * width + x];
    }
    
    // Direct access to buffers (for SDL rendering)
    uint32_t* getColorBuffer() { return colorBuffer; }
    const uint32_t* getColorBuffer() const { return colorBuffer; }
    
    float* getDepthBuffer() { return depthBuffer; }
    const float* getDepthBuffer() const { return depthBuffer; }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Resize framebuffer
    void resize(int newWidth, int newHeight) {
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
    
    // Fill a rectangle (useful for UI elements)
    void fillRect(int x, int y, int w, int h, const Color& color, float depth = 0.0f, bool depthTest = true) {
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
    
    // Draw a line (using Bresenham's algorithm)
    void drawLine(int x0, int y0, int x1, int y1, const Color& color, float z0 = 0.0f, float z1 = 0.0f, bool depthTest = true) {
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
};