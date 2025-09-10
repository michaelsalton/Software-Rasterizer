#pragma once

#include <memory>

class Texture;

namespace TextureGenerator {
    // Create a checkerboard texture
    std::shared_ptr<Texture> CreateCheckerboard(int size = 256, int checkerSize = 32);
    
    // Create a gradient texture
    std::shared_ptr<Texture> CreateGradient(int width = 256, int height = 256);
    
    // Create a debug grid texture with colored lines
    std::shared_ptr<Texture> CreateDebugGrid(int size = 256, int gridSize = 32);
}