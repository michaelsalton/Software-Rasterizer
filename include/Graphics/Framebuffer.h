#pragma once

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <limits>

class Framebuffer {
public:
    struct Color {
        uint8_t r, g, b, a;
        
        Color();
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        
        uint32_t toUint32() const;
        static Color fromUint32(uint32_t color);
        
        // Blend this color over another color
        Color blend(const Color& dst) const;
    };

private:
    uint32_t* colorBuffer;
    float* depthBuffer;
    int width;
    int height;
    int bufferSize;

public:
    Framebuffer(int width, int height);
    ~Framebuffer();
    
    // Clear both buffers
    void clear(const Color& color = Color(0, 0, 0, 255));
    
    // Clear color buffer only
    void clearColor(const Color& color = Color(0, 0, 0, 255));
    
    // Clear depth buffer only
    void clearDepth(float depth = 1.0f);
    
    // Write pixel with bounds checking and optional depth testing
    bool writePixel(int x, int y, const Color& color, float depth = 0.0f, bool depthTest = true);
    
    // Write pixel with blending
    bool writePixelBlended(int x, int y, const Color& color, float depth = 0.0f, bool depthTest = true);
    
    // Get pixel color
    Color getPixel(int x, int y) const;
    
    // Get depth value
    float getDepth(int x, int y) const;
    
    // Direct access to buffers (for SDL rendering)
    uint32_t* getColorBuffer();
    const uint32_t* getColorBuffer() const;
    
    float* getDepthBuffer();
    const float* getDepthBuffer() const;
    
    int getWidth() const;
    int getHeight() const;
    
    // Resize framebuffer
    void resize(int newWidth, int newHeight);
    
    // Fill a rectangle (useful for UI elements)
    void fillRect(int x, int y, int w, int h, const Color& color, float depth = 0.0f, bool depthTest = true);
    
    // Draw a line (using Bresenham's algorithm)
    void drawLine(int x0, int y0, int x1, int y1, const Color& color, float z0 = 0.0f, float z1 = 0.0f, bool depthTest = true);
};