#pragma once

#include <string>
#include "graphics/frame_buffer.h"

class BitmapFont {
public:
    // Simple 8x8 bitmap font for digits and basic characters
    static void DrawChar(Framebuffer* framebuffer, char c, int x, int y, const Framebuffer::Color& color);
    static void DrawString(Framebuffer* framebuffer, const std::string& text, int x, int y, const Framebuffer::Color& color);
    
private:
    // 8x8 bitmap font data for digits 0-9, period, and 'FPS'
    static const uint8_t fontData[256][8];
};