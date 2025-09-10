#include "Graphics/Rasterizer.h"
#include "Graphics/FragmentShader.h"
#include <cmath>

void Rasterizer::RasterizeTriangle(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    Framebuffer* framebuffer,
    Algorithm algorithm,
    const ::ScissorRect* scissor) {
    
    switch (algorithm) {
        case Algorithm::SCANLINE:
            RasterizeTriangleScanline(v0, v1, v2, framebuffer, scissor);
            break;
        case Algorithm::EDGE_EQUATION:
            RasterizeTriangleEdgeEquation(v0, v1, v2, framebuffer, scissor);
            break;
        case Algorithm::HIERARCHICAL:
            RasterizeTriangleHierarchical(v0, v1, v2, framebuffer, 8, scissor);
            break;
    }
}

void Rasterizer::RasterizeTriangleEdgeEquation(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    Framebuffer* framebuffer,
    const ::ScissorRect* scissor) {
    
    // Setup triangle
    TriangleSetup setup;
    setup.setup(v0.screenPosition, v1.screenPosition, v2.screenPosition, 
                framebuffer->getWidth(), framebuffer->getHeight(), scissor);
    
    // Skip degenerate triangles
    if (!setup.isValid()) {
        return;
    }
    
    // Iterate through all pixels in bounding box
    for (int y = setup.minY; y <= setup.maxY; y++) {
        for (int x = setup.minX; x <= setup.maxX; x++) {
            // Test if pixel center is inside all three edges
            float px = x + 0.5f;
            float py = y + 0.5f;
            
            if (setup.edge0.inside(px, py) && 
                setup.edge1.inside(px, py) && 
                setup.edge2.inside(px, py)) {
                
                // Compute barycentric coordinates
                Vec3 bary = ComputeBarycentricFast(px, py, setup);
                
                // Rasterize the pixel
                RasterizePixel(x, y, bary, v0, v1, v2, framebuffer);
            }
        }
    }
}

void Rasterizer::RasterizeTriangleScanline(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    Framebuffer* framebuffer,
    const ::ScissorRect* scissor) {
    
    // Get screen positions
    Vec3 p0 = v0.screenPosition;
    Vec3 p1 = v1.screenPosition;
    Vec3 p2 = v2.screenPosition;
    
    // Sort vertices by Y coordinate
    if (p0.y > p1.y) { std::swap(p0, p1); std::swap(const_cast<TransformedVertex&>(v0), const_cast<TransformedVertex&>(v1)); }
    if (p1.y > p2.y) { std::swap(p1, p2); std::swap(const_cast<TransformedVertex&>(v1), const_cast<TransformedVertex&>(v2)); }
    if (p0.y > p1.y) { std::swap(p0, p1); std::swap(const_cast<TransformedVertex&>(v0), const_cast<TransformedVertex&>(v1)); }
    
    // Calculate inverse slopes
    float invSlope1 = 0, invSlope2 = 0;
    
    if (p1.y - p0.y > 0) {
        invSlope1 = (p1.x - p0.x) / (p1.y - p0.y);
    }
    if (p2.y - p0.y > 0) {
        invSlope2 = (p2.x - p0.x) / (p2.y - p0.y);
    }
    
    // Rasterize upper part of triangle
    if (p1.y - p0.y > 0) {
        for (int y = (int)p0.y; y <= (int)p1.y; y++) {
            if (y < 0 || y >= framebuffer->getHeight()) continue;
            if (scissor && scissor->enabled && (y < scissor->top || y > scissor->bottom)) continue;
            
            // float t = (y - p0.y) / (p1.y - p0.y);
            int x1 = (int)(p0.x + (y - p0.y) * invSlope1);
            int x2 = (int)(p0.x + (y - p0.y) * invSlope2);
            
            if (x1 > x2) std::swap(x1, x2);
            
            for (int x = x1; x <= x2; x++) {
                if (x < 0 || x >= framebuffer->getWidth()) continue;
                if (scissor && scissor->enabled && (x < scissor->left || x > scissor->right)) continue;
                
                // Compute barycentric coordinates
                Vec3 bary = ComputeBarycentric(
                    Vec2(x + 0.5f, y + 0.5f),
                    Vec2(p0.x, p0.y),
                    Vec2(p1.x, p1.y),
                    Vec2(p2.x, p2.y)
                );
                
                RasterizePixel(x, y, bary, v0, v1, v2, framebuffer);
            }
        }
    }
    
    // Calculate inverse slope for bottom part
    if (p2.y - p1.y > 0) {
        invSlope1 = (p2.x - p1.x) / (p2.y - p1.y);
    }
    
    // Rasterize lower part of triangle
    if (p2.y - p1.y > 0) {
        for (int y = (int)p1.y + 1; y <= (int)p2.y; y++) {
            if (y < 0 || y >= framebuffer->getHeight()) continue;
            if (scissor && scissor->enabled && (y < scissor->top || y > scissor->bottom)) continue;
            
            int x1 = (int)(p1.x + (y - p1.y) * invSlope1);
            int x2 = (int)(p0.x + (y - p0.y) * invSlope2);
            
            if (x1 > x2) std::swap(x1, x2);
            
            for (int x = x1; x <= x2; x++) {
                if (x < 0 || x >= framebuffer->getWidth()) continue;
                if (scissor && scissor->enabled && (x < scissor->left || x > scissor->right)) continue;
                
                // Compute barycentric coordinates
                Vec3 bary = ComputeBarycentric(
                    Vec2(x + 0.5f, y + 0.5f),
                    Vec2(p0.x, p0.y),
                    Vec2(p1.x, p1.y),
                    Vec2(p2.x, p2.y)
                );
                
                RasterizePixel(x, y, bary, v0, v1, v2, framebuffer);
            }
        }
    }
}

void Rasterizer::RasterizeTriangleHierarchical(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    Framebuffer* framebuffer,
    int tileSize,
    const ::ScissorRect* scissor) {
    
    // Setup triangle
    TriangleSetup setup;
    setup.setup(v0.screenPosition, v1.screenPosition, v2.screenPosition,
                framebuffer->getWidth(), framebuffer->getHeight(), scissor);
    
    if (!setup.isValid()) {
        return;
    }
    
    // Process tiles
    for (int tileY = setup.minY / tileSize; tileY <= setup.maxY / tileSize; tileY++) {
        for (int tileX = setup.minX / tileSize; tileX <= setup.maxX / tileSize; tileX++) {
            int tileMinX = tileX * tileSize;
            int tileMinY = tileY * tileSize;
            int tileMaxX = tileMinX + tileSize - 1;
            int tileMaxY = tileMinY + tileSize - 1;
            
            // Check if tile overlaps triangle
            if (TileOverlapsTriangle(tileMinX, tileMinY, tileMaxX, tileMaxY, setup)) {
                // Rasterize pixels in tile
                int startX = std::max(tileMinX, setup.minX);
                int startY = std::max(tileMinY, setup.minY);
                int endX = std::min(tileMaxX, setup.maxX);
                int endY = std::min(tileMaxY, setup.maxY);
                
                for (int y = startY; y <= endY; y++) {
                    for (int x = startX; x <= endX; x++) {
                        float px = x + 0.5f;
                        float py = y + 0.5f;
                        
                        if (setup.edge0.inside(px, py) && 
                            setup.edge1.inside(px, py) && 
                            setup.edge2.inside(px, py)) {
                            
                            Vec3 bary = ComputeBarycentricFast(px, py, setup);
                            RasterizePixel(x, y, bary, v0, v1, v2, framebuffer);
                        }
                    }
                }
            }
        }
    }
}

Vec3 Rasterizer::ComputeBarycentricFast(float x, float y, const TriangleSetup& setup) {
    // Use edge equations for fast barycentric computation
    float w0 = setup.edge0.evaluate(x, y);
    float w1 = setup.edge1.evaluate(x, y);
    float w2 = setup.edge2.evaluate(x, y);
    
    // Normalize
    float sum = w0 + w1 + w2;
    if (sum > 0) {
        return Vec3(w1 / sum, w2 / sum, w0 / sum);
    }
    
    return Vec3(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f);
}

Vec3 Rasterizer::ComputeBarycentric(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
    Vec2 v0 = b - a;
    Vec2 v1 = c - a;
    Vec2 v2 = p - a;
    
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);
    
    float denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 0.0001f) {
        return Vec3(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f);
    }
    
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    
    return Vec3(u, v, w);
}

TransformedVertex Rasterizer::InterpolateVertex(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    const Vec3& barycentric) {
    
    return VertexProcessor::InterpolateVertex(v0, v1, v2, 
                                             barycentric.x, barycentric.y, barycentric.z);
}

void Rasterizer::RasterizePixel(
    int x, int y,
    const Vec3& barycentric,
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    Framebuffer* framebuffer) {
    
    // Skip invalid barycentric coordinates
    if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) {
        return;
    }
    
    // Interpolate vertex attributes
    TransformedVertex interpolated = InterpolateVertex(v0, v1, v2, barycentric);
    
    // Write pixel with depth test
    framebuffer->writePixel(x, y, interpolated.color, interpolated.screenPosition.z, true);
}

void Rasterizer::RasterizeTriangleWithShader(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    Framebuffer* framebuffer,
    FragmentShader* shader,
    const ShaderUniforms& uniforms,
    Algorithm algorithm,
    const ::ScissorRect* scissor) {
    
    switch (algorithm) {
        case Algorithm::EDGE_EQUATION:
            RasterizeTriangleEdgeEquationWithShader(v0, v1, v2, framebuffer, shader, uniforms, scissor);
            break;
        default:
            // Fall back to edge equation for now
            RasterizeTriangleEdgeEquationWithShader(v0, v1, v2, framebuffer, shader, uniforms, scissor);
            break;
    }
}

void Rasterizer::RasterizeTriangleEdgeEquationWithShader(
    const TransformedVertex& v0,
    const TransformedVertex& v1,
    const TransformedVertex& v2,
    Framebuffer* framebuffer,
    FragmentShader* shader,
    const ShaderUniforms& uniforms,
    const ::ScissorRect* scissor) {
    
    // Setup triangle
    TriangleSetup setup;
    setup.setup(v0.screenPosition, v1.screenPosition, v2.screenPosition, 
                framebuffer->getWidth(), framebuffer->getHeight(), scissor);
    
    // Skip degenerate triangles
    if (!setup.isValid()) {
        return;
    }
    
    // Perspective-correct interpolation setup
    float w0_inv = 1.0f / v0.clipPosition.w;
    float w1_inv = 1.0f / v1.clipPosition.w;
    float w2_inv = 1.0f / v2.clipPosition.w;
    
    // Iterate through all pixels in bounding box
    for (int y = setup.minY; y <= setup.maxY; y++) {
        for (int x = setup.minX; x <= setup.maxX; x++) {
            // Test if pixel center is inside all three edges
            float px = x + 0.5f;
            float py = y + 0.5f;
            
            if (setup.edge0.inside(px, py) && 
                setup.edge1.inside(px, py) && 
                setup.edge2.inside(px, py)) {
                
                // Compute barycentric coordinates
                Vec3 bary = ComputeBarycentricFast(px, py, setup);
                
                // Skip invalid coordinates
                if (bary.x < 0 || bary.y < 0 || bary.z < 0) {
                    continue;
                }
                
                // Perspective-correct interpolation
                float w0 = bary.x * w0_inv;
                float w1 = bary.y * w1_inv;
                float w2 = bary.z * w2_inv;
                float w_sum = w0 + w1 + w2;
                
                // Setup fragment input
                FragmentInput fragInput;
                fragInput.screenPos = Vec2(x, y);
                
                // Interpolate depth
                fragInput.depth = (v0.screenPosition.z * w0 + 
                                  v1.screenPosition.z * w1 + 
                                  v2.screenPosition.z * w2) / w_sum;
                
                // Interpolate world position
                fragInput.worldPos = (v0.worldPosition * w0 + 
                                     v1.worldPosition * w1 + 
                                     v2.worldPosition * w2) * (1.0f / w_sum);
                
                // Interpolate world normal
                fragInput.worldNormal = (v0.worldNormal * w0 + 
                                        v1.worldNormal * w1 + 
                                        v2.worldNormal * w2) * (1.0f / w_sum);
                fragInput.worldNormal.normalize();
                
                // Interpolate texture coordinates
                fragInput.texCoord = Vec2(
                    (v0.texCoord.x * w0 + v1.texCoord.x * w1 + v2.texCoord.x * w2) / w_sum,
                    (v0.texCoord.y * w0 + v1.texCoord.y * w1 + v2.texCoord.y * w2) / w_sum
                );
                
                // Interpolate color and convert to [0,1] range
                Framebuffer::Color c0 = v0.color;
                Framebuffer::Color c1 = v1.color;
                Framebuffer::Color c2 = v2.color;
                
                float r = (c0.r * w0 + c1.r * w1 + c2.r * w2) / w_sum / 255.0f;
                float g = (c0.g * w0 + c1.g * w1 + c2.g * w2) / w_sum / 255.0f;
                float b = (c0.b * w0 + c1.b * w1 + c2.b * w2) / w_sum / 255.0f;
                float a = (c0.a * w0 + c1.a * w1 + c2.a * w2) / w_sum / 255.0f;
                
                fragInput.color = Vec4(r, g, b, a);
                
                // Run fragment shader
                FragmentOutput fragOutput = shader->Shade(fragInput, uniforms);
                
                // Apply fragment shader output
                if (!fragOutput.discard) {
                    // Convert color back to 8-bit
                    Framebuffer::Color finalColor(
                        static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, fragOutput.color.x)) * 255),
                        static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, fragOutput.color.y)) * 255),
                        static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, fragOutput.color.z)) * 255),
                        static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, fragOutput.color.w)) * 255)
                    );
                    
                    // writePixel will handle depth testing
                    framebuffer->writePixel(x, y, finalColor, fragOutput.depth, true);
                }
            }
        }
    }
}

bool Rasterizer::TileOverlapsTriangle(
    int tileMinX, int tileMinY,
    int tileMaxX, int tileMaxY,
    const TriangleSetup& setup) {
    
    // Check if tile bounding box overlaps triangle bounding box
    if (tileMaxX < setup.minX || tileMinX > setup.maxX ||
        tileMaxY < setup.minY || tileMinY > setup.maxY) {
        return false;
    }
    
    // Check tile corners against edge equations
    // If all corners are outside the same edge, tile doesn't overlap
    for (int i = 0; i < 3; i++) {
        const EdgeEquation* edge = nullptr;
        switch (i) {
            case 0: edge = &setup.edge0; break;
            case 1: edge = &setup.edge1; break;
            case 2: edge = &setup.edge2; break;
        }
        
        bool allOutside = true;
        for (int cy = 0; cy <= 1; cy++) {
            for (int cx = 0; cx <= 1; cx++) {
                float px = (cx == 0) ? tileMinX : tileMaxX + 1;
                float py = (cy == 0) ? tileMinY : tileMaxY + 1;
                if (edge->inside(px, py)) {
                    allOutside = false;
                    break;
                }
            }
            if (!allOutside) break;
        }
        
        if (allOutside) {
            return false;
        }
    }
    
    return true;
}