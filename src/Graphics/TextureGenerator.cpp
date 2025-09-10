#include "Graphics/Texture.h"
#include <memory>

namespace TextureGenerator {
    
    std::shared_ptr<Texture> CreateCheckerboard(int size, int checkerSize) {
        auto texture = std::make_shared<Texture>(size, size, TextureFormat::RGBA8);
        
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                bool isWhite = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
                Vec4 color = isWhite ? Vec4(1, 1, 1, 1) : Vec4(0, 0, 0, 1);
                texture->SetPixel(x, y, color);
            }
        }
        
        return texture;
    }
    
    std::shared_ptr<Texture> CreateGradient(int width, int height) {
        auto texture = std::make_shared<Texture>(width, height, TextureFormat::RGBA8);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float u = float(x) / (width - 1);
                float v = float(y) / (height - 1);
                Vec4 color(u, v, 1.0f - u, 1.0f);
                texture->SetPixel(x, y, color);
            }
        }
        
        return texture;
    }
    
    std::shared_ptr<Texture> CreateDebugGrid(int size, int gridSize) {
        auto texture = std::make_shared<Texture>(size, size, TextureFormat::RGBA8);
        
        // Fill with white
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                texture->SetPixel(x, y, Vec4(1, 1, 1, 1));
            }
        }
        
        // Draw grid lines
        for (int i = 0; i < size; i += gridSize) {
            // Vertical lines
            for (int y = 0; y < size; y++) {
                texture->SetPixel(i, y, Vec4(0, 0, 1, 1)); // Blue
            }
            // Horizontal lines
            for (int x = 0; x < size; x++) {
                texture->SetPixel(x, i, Vec4(1, 0, 0, 1)); // Red
            }
        }
        
        // Draw border
        for (int i = 0; i < size; i++) {
            texture->SetPixel(i, 0, Vec4(0, 1, 0, 1)); // Green
            texture->SetPixel(i, size-1, Vec4(0, 1, 0, 1));
            texture->SetPixel(0, i, Vec4(0, 1, 0, 1));
            texture->SetPixel(size-1, i, Vec4(0, 1, 0, 1));
        }
        
        return texture;
    }
}