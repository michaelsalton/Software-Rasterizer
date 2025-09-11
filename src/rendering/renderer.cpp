#include "rendering/renderer.h"
#include "graphics/graphics_context.h"
#include "graphics/bitmap_font.h"
#include <algorithm>
#include <iostream>
#include <cmath>

Renderer::Renderer(SDL_Renderer* renderer, int width, int height) 
    : mRenderer(renderer), mTexture(nullptr), framebuffer(nullptr), camera(nullptr), 
      width(width), height(height) {
    framebuffer = new Framebuffer(width, height);
    
    // Create texture for presenting framebuffer
    mTexture = SDL_CreateTexture(renderer, 
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height);
    
    // Setup default camera
    defaultCamera.setPerspective(60.0f, (float)width / height, 0.1f, 100.0f);
    defaultCamera.setPosition(0, 0, 5);
    defaultCamera.lookAt(Vec3(0, 0, 0));
    
    // Initialize shader system
    vertexShader = std::make_shared<DefaultVertexShader>();
    fragmentShader = std::make_shared<DefaultFragmentShader>();
    shaderUniforms.viewportWidth = width;
    shaderUniforms.viewportHeight = height;
}

Renderer::~Renderer() {
    delete framebuffer;
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
    }
}

void Renderer::Clear(const Framebuffer::Color& clearColor) {
    framebuffer->clear(clearColor);
}

void Renderer::Present() {
    // Update SDL texture with framebuffer contents
    void* pixels;
    int pitch;
    SDL_LockTexture(mTexture, NULL, &pixels, &pitch);
    
    // Copy framebuffer to texture
    const uint32_t* src = framebuffer->getColorBuffer();
    uint32_t* dst = static_cast<uint32_t*>(pixels);
    
    for (int y = 0; y < height; ++y) {
        memcpy(dst + y * (pitch / 4), src + y * width, width * sizeof(uint32_t));
    }
    
    SDL_UnlockTexture(mTexture);
    
    // Render texture to screen
    SDL_RenderTexture(mRenderer, mTexture, NULL, NULL);
}

Vec3 Renderer::Project(Vec3 point, float fov, float aspectRatio, float near, float far)
{
    if (point.z == 0)
    {
        point.z = 0.1f;
    }
    float projectedX = (point.x / point.z) * fov * aspectRatio;
    float projectedY = (point.y / point.z) * fov;

    float normalizedZ = (point.z - near) / (far - near);

    projectedX = (projectedX + 1.0f) * 0.5f * width;
    projectedY = (1.0f - projectedY) * 0.5f * height;

    projectedX = std::min(std::max(projectedX, 0.0f), (float)width - 1);
    projectedY = std::min(std::max(projectedY, 0.0f), (float)height - 1);

    return Vec3(projectedX, projectedY, normalizedZ);
}

Vec4 Renderer::TransformVertex(const Vec4& vertex, const Mat4& modelMatrix) {
    Camera* cam = camera ? camera : &defaultCamera;
    
    // Model transform (object to world)
    Vec4 worldPos = modelMatrix * vertex;
    
    // View transform (world to camera)
    Vec4 viewPos = cam->getViewMatrix() * worldPos;
    
    // Projection transform (camera to clip)
    Vec4 clipPos = cam->getProjectionMatrix() * viewPos;
    
    return clipPos;
}

Vec3 Renderer::WorldToScreen(const Vec3& worldPoint, const Mat4& modelMatrix) {
    Vec4 vertex(worldPoint, 1.0f);
    Vec4 clipPos = TransformVertex(vertex, modelMatrix);
    
    // Perspective divide
    if (clipPos.w != 0) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
        clipPos.z /= clipPos.w;
    }
    
    // Convert from NDC [-1,1] to screen coordinates
    float screenX = (clipPos.x + 1.0f) * 0.5f * width;
    float screenY = (1.0f - clipPos.y) * 0.5f * height;  // Flip Y
    
    return Vec3(screenX, screenY, clipPos.z);
}

void Renderer::DrawPoint(Vec3 point, const Framebuffer::Color& color, const Mat4& modelMatrix)
{
    Vec3 screenPos = WorldToScreen(point, modelMatrix);
    framebuffer->writePixel(screenPos.x, screenPos.y, color, screenPos.z);
}

void Renderer::DrawLine(Vec3 v1, Vec3 v2, const Framebuffer::Color& color, const Mat4& modelMatrix)
{
    Vec3 p1 = WorldToScreen(v1, modelMatrix);
    Vec3 p2 = WorldToScreen(v2, modelMatrix);
    framebuffer->drawLine(p1.x, p1.y, p2.x, p2.y, color, p1.z, p2.z, true);
}

void Renderer::DrawTriangle(Vec3 v1, Vec3 v2, Vec3 v3, const Framebuffer::Color& color, const Mat4& modelMatrix)
{
    Vec3 p1 = WorldToScreen(v1, modelMatrix);
    Vec3 p2 = WorldToScreen(v2, modelMatrix);
    Vec3 p3 = WorldToScreen(v3, modelMatrix);
    
    framebuffer->drawLine(p1.x, p1.y, p2.x, p2.y, color, p1.z, p2.z, true);
    framebuffer->drawLine(p2.x, p2.y, p3.x, p3.y, color, p2.z, p3.z, true);
    framebuffer->drawLine(p3.x, p3.y, p1.x, p1.y, color, p3.z, p1.z, true);
}

void Renderer::DrawFilledTriangle(Vec3 v1, Vec3 v2, Vec3 v3, const Framebuffer::Color& color, const Mat4& modelMatrix, bool useDepthTest)
{
    // Transform vertices to screen space
    Vec3 p1 = WorldToScreen(v1, modelMatrix);
    Vec3 p2 = WorldToScreen(v2, modelMatrix);
    Vec3 p3 = WorldToScreen(v3, modelMatrix);
    
    DrawFilledTriangleScreen(p1, p2, p3, color, useDepthTest);
}

void Renderer::DrawFilledTriangleScreen(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Framebuffer::Color& color, bool useDepthTest)
{
    // Sort vertices by Y-coordinate (ascending)
    Vec3 v1 = p1, v2 = p2, v3 = p3;
    if (v1.y > v2.y) std::swap(v1, v2);
    if (v2.y > v3.y) std::swap(v2, v3);
    if (v1.y > v2.y) std::swap(v1, v2);

    // Draw the top half of the triangle
    for (int y = v1.y; y <= v2.y; ++y) {
        int xLeft = Interpolate(v1.y, v3.y, v1.x, v3.x, y);
        int xRight = Interpolate(v1.y, v2.y, v1.x, v2.x, y);
        
        // Interpolate Z values for the scanline
        float zLeft = InterpolateFloat(v1.y, v3.y, v1.z, v3.z, y);
        float zRight = InterpolateFloat(v1.y, v2.y, v1.z, v2.z, y);
        
        if (xLeft > xRight) {
            std::swap(xLeft, xRight);
            std::swap(zLeft, zRight);
        }
        
        // Draw horizontal line with interpolated depth
        for (int x = xLeft; x <= xRight; ++x) {
            float t = (xRight - xLeft) > 0 ? (float)(x - xLeft) / (xRight - xLeft) : 0;
            float z = zLeft + (zRight - zLeft) * t;
            framebuffer->writePixel(x, y, color, z, useDepthTest);
        }
    }

    // Draw the bottom half of the triangle
    for (int y = v2.y; y <= v3.y; ++y) {
        int xLeft = Interpolate(v2.y, v3.y, v2.x, v3.x, y);
        int xRight = Interpolate(v1.y, v3.y, v1.x, v3.x, y);
        
        // Interpolate Z values for the scanline
        float zLeft = InterpolateFloat(v2.y, v3.y, v2.z, v3.z, y);
        float zRight = InterpolateFloat(v1.y, v3.y, v1.z, v3.z, y);
        
        if (xLeft > xRight) {
            std::swap(xLeft, xRight);
            std::swap(zLeft, zRight);
        }
        
        // Draw horizontal line with interpolated depth
        for (int x = xLeft; x <= xRight; ++x) {
            float t = (xRight - xLeft) > 0 ? (float)(x - xLeft) / (xRight - xLeft) : 0;
            float z = zLeft + (zRight - zLeft) * t;
            framebuffer->writePixel(x, y, color, z, useDepthTest);
        }
    }
}

void Renderer::DrawMesh(const std::vector<Vec3>& vertices, const std::vector<int>& indices, 
                       const Framebuffer::Color& color, const Mat4& modelMatrix, bool wireframe)
{
    // Draw triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vec3 v1 = vertices[indices[i]];
        Vec3 v2 = vertices[indices[i + 1]];
        Vec3 v3 = vertices[indices[i + 2]];
        
        if (wireframe) {
            DrawTriangle(v1, v2, v3, color, modelMatrix);
        } else {
            DrawFilledTriangle(v1, v2, v3, color, modelMatrix, true);
        }
    }
}

void Renderer::DrawAxis(const Mat4& modelMatrix, float length)
{
    // Draw axes in both positive and negative directions
    Vec3 xAxisPos(length, 0, 0);
    Vec3 xAxisNeg(-length, 0, 0);
    Vec3 yAxisPos(0, length, 0);
    Vec3 yAxisNeg(0, -length, 0);
    Vec3 zAxisPos(0, 0, length);
    Vec3 zAxisNeg(0, 0, -length);
    
    // X axis - Red (full line from -X to +X)
    DrawLine(xAxisNeg, xAxisPos, Framebuffer::Color(255, 0, 0), modelMatrix);
    
    // Y axis - Green (full line from -Y to +Y)
    DrawLine(yAxisNeg, yAxisPos, Framebuffer::Color(0, 255, 0), modelMatrix);
    
    // Z axis - Blue (full line from -Z to +Z)
    DrawLine(zAxisNeg, zAxisPos, Framebuffer::Color(0, 0, 255), modelMatrix);
}

void Renderer::DrawGrid(int size, float spacing, const Framebuffer::Color& color)
{
    float halfSize = size * spacing * 0.5f;
    
    // Draw lines parallel to X axis
    for (int z = -size/2; z <= size/2; ++z) {
        DrawLine(
            Vec3(-halfSize, 0, z * spacing),
            Vec3(halfSize, 0, z * spacing),
            color
        );
    }
    
    // Draw lines parallel to Z axis
    for (int x = -size/2; x <= size/2; ++x) {
        DrawLine(
            Vec3(x * spacing, 0, -halfSize),
            Vec3(x * spacing, 0, halfSize),
            color
        );
    }
}

void Renderer::DrawVertexNormals(const std::vector<Vertex>& vertices, const Mat4& modelMatrix, float normalLength, const Framebuffer::Color& color)
{
    // Transform vertices to world space to get proper normal visualization
    Mat4 normalMatrix = modelMatrix.inverse().transpose(); // For transforming normals
    
    for (const auto& vertex : vertices) {
        // Transform vertex position to world space
        Vec4 worldPos = modelMatrix * Vec4(vertex.position, 1.0f);
        Vec3 startPos(worldPos.x, worldPos.y, worldPos.z);
        
        // Transform normal to world space (using normal matrix to handle non-uniform scaling)
        Vec4 worldNormal = normalMatrix * Vec4(vertex.normal, 0.0f);
        Vec3 normalDir = Vec3(worldNormal.x, worldNormal.y, worldNormal.z).normalized();
        
        // Calculate end position of normal line
        Vec3 endPos = startPos + normalDir * normalLength;
        
        // Draw the normal as a line
        DrawLine(startPos, endPos, color);
        
        // Optional: Draw a small arrow head at the end
        Vec3 right = Vec3(1, 0, 0);
        if (std::abs(normalDir.dot(right)) > 0.9f) {
            right = Vec3(0, 1, 0);
        }
        Vec3 perpendicular = normalDir.cross(right).normalized();
        Vec3 arrowBase = endPos - normalDir * normalLength * 0.2f;
        
        DrawLine(endPos, arrowBase + perpendicular * normalLength * 0.1f, color);
        DrawLine(endPos, arrowBase - perpendicular * normalLength * 0.1f, color);
    }
}

void Renderer::DrawFaceNormals(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const Mat4& modelMatrix, float normalLength, const Framebuffer::Color& color)
{
    // Process each triangle
    for (size_t i = 0; i < indices.size(); i += 3) {
        // Get the three vertices of the triangle
        const Vertex& v0 = vertices[indices[i]];
        const Vertex& v1 = vertices[indices[i + 1]];
        const Vertex& v2 = vertices[indices[i + 2]];
        
        // Transform vertices to world space
        Vec4 worldPos0 = modelMatrix * Vec4(v0.position, 1.0f);
        Vec4 worldPos1 = modelMatrix * Vec4(v1.position, 1.0f);
        Vec4 worldPos2 = modelMatrix * Vec4(v2.position, 1.0f);
        
        Vec3 p0(worldPos0.x, worldPos0.y, worldPos0.z);
        Vec3 p1(worldPos1.x, worldPos1.y, worldPos1.z);
        Vec3 p2(worldPos2.x, worldPos2.y, worldPos2.z);
        
        // Calculate face center (centroid)
        Vec3 center = (p0 + p1 + p2) * (1.0f / 3.0f);
        
        // Calculate face normal using cross product
        Vec3 edge1 = p1 - p0;
        Vec3 edge2 = p2 - p0;
        Vec3 faceNormal = edge2.cross(edge1).normalized();
        
        // Offset the center slightly along the normal to avoid z-fighting
        Vec3 offsetCenter = center + faceNormal * 0.01f;
        
        // Calculate end position of normal line
        Vec3 endPos = offsetCenter + faceNormal * normalLength;
        
        // Draw the normal as a line from face center
        // Pass identity matrix since positions are already in world space
        DrawLine(offsetCenter, endPos, color, Mat4());
        
        // Draw arrow head
        Vec3 right = Vec3(1, 0, 0);
        if (std::abs(faceNormal.dot(right)) > 0.9f) {
            right = Vec3(0, 1, 0);
        }
        Vec3 perpendicular = faceNormal.cross(right).normalized();
        Vec3 arrowBase = endPos - faceNormal * normalLength * 0.2f;
        
        DrawLine(endPos, arrowBase + perpendicular * normalLength * 0.1f, color, Mat4());
        DrawLine(endPos, arrowBase - perpendicular * normalLength * 0.1f, color, Mat4());
    }
}

int Renderer::Interpolate(int y1, int y2, int x1, int x2, int y)
{
    if (y1 == y2)
    {
        return x1;
    }
    return x1 + (x2 - x1) * (y - y1) / (y2 - y1);
}

float Renderer::InterpolateFloat(float y1, float y2, float x1, float x2, float y)
{
    if (y1 == y2)
    {
        return x1;
    }
    return x1 + (x2 - x1) * (y - y1) / (y2 - y1);
}

Vec3 Renderer::ComputeBarycentric(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c)
{
    Vec3 v0 = Vec3(c.x - a.x, c.y - a.y, 0);
    Vec3 v1 = Vec3(b.x - a.x, b.y - a.y, 0);
    Vec3 v2 = Vec3(p.x - a.x, p.y - a.y, 0);
    
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);
    
    float denom = d00 * d11 - d01 * d01;
    if (fabs(denom) < 1e-8) {
        return Vec3(-1, -1, -1); // Triangle is degenerate
    }
    
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    
    return Vec3(u, w, v);
}

void Renderer::DrawVertexMesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const Mat4& modelMatrix, bool wireframe)
{
    // Process all vertices first
    std::vector<TransformedVertex> transformedVertices;
    transformedVertices.reserve(vertices.size());
    
    Camera* cam = camera ? camera : &defaultCamera;
    
    for (const auto& vertex : vertices) {
        TransformedVertex tv = VertexProcessor::ProcessVertex(
            vertex, 
            modelMatrix, 
            cam->getViewMatrix(), 
            cam->getProjectionMatrix(),
            width,
            height
        );
        transformedVertices.push_back(tv);
    }
    
    // Draw triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        const TransformedVertex& v0 = transformedVertices[indices[i]];
        const TransformedVertex& v1 = transformedVertices[indices[i + 1]];
        const TransformedVertex& v2 = transformedVertices[indices[i + 2]];
        
        if (wireframe) {
            // Draw wireframe using vertex colors
            framebuffer->drawLine(
                v0.screenPosition.x, v0.screenPosition.y,
                v1.screenPosition.x, v1.screenPosition.y,
                v0.color, v0.screenPosition.z, v1.screenPosition.z, true
            );
            framebuffer->drawLine(
                v1.screenPosition.x, v1.screenPosition.y,
                v2.screenPosition.x, v2.screenPosition.y,
                v1.color, v1.screenPosition.z, v2.screenPosition.z, true
            );
            framebuffer->drawLine(
                v2.screenPosition.x, v2.screenPosition.y,
                v0.screenPosition.x, v0.screenPosition.y,
                v2.color, v2.screenPosition.z, v0.screenPosition.z, true
            );
        } else {
            // Clip triangle against view frustum
            std::vector<TransformedVertex> clippedPolygon = Clipper::ClipTriangle(v0, v1, v2);
            
            // Draw the clipped polygon
            if (!clippedPolygon.empty()) {
                DrawClippedPolygon(clippedPolygon);
            }
        }
    }
}

void Renderer::DrawVertexTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Mat4& modelMatrix, bool wireframe)
{
    Camera* cam = camera ? camera : &defaultCamera;
    
    // Transform vertices
    TransformedVertex tv0 = VertexProcessor::ProcessVertex(v0, modelMatrix, cam->getViewMatrix(), cam->getProjectionMatrix(), width, height);
    TransformedVertex tv1 = VertexProcessor::ProcessVertex(v1, modelMatrix, cam->getViewMatrix(), cam->getProjectionMatrix(), width, height);
    TransformedVertex tv2 = VertexProcessor::ProcessVertex(v2, modelMatrix, cam->getViewMatrix(), cam->getProjectionMatrix(), width, height);
    
    if (wireframe) {
        framebuffer->drawLine(
            tv0.screenPosition.x, tv0.screenPosition.y,
            tv1.screenPosition.x, tv1.screenPosition.y,
            tv0.color, tv0.screenPosition.z, tv1.screenPosition.z, true
        );
        framebuffer->drawLine(
            tv1.screenPosition.x, tv1.screenPosition.y,
            tv2.screenPosition.x, tv2.screenPosition.y,
            tv1.color, tv1.screenPosition.z, tv2.screenPosition.z, true
        );
        framebuffer->drawLine(
            tv2.screenPosition.x, tv2.screenPosition.y,
            tv0.screenPosition.x, tv0.screenPosition.y,
            tv2.color, tv2.screenPosition.z, tv0.screenPosition.z, true
        );
    } else {
        // Clip triangle against view frustum
        std::vector<TransformedVertex> clippedPolygon = Clipper::ClipTriangle(tv0, tv1, tv2);
        
        // Draw the clipped polygon
        if (!clippedPolygon.empty()) {
            DrawClippedPolygon(clippedPolygon);
        }
    }
}

void Renderer::DrawWireframeVertexTriangle(const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2)
{
    // Draw triangle edges with depth testing
    framebuffer->drawLine(
        v0.screenPosition.x, v0.screenPosition.y,
        v1.screenPosition.x, v1.screenPosition.y,
        v0.color, v0.screenPosition.z, v1.screenPosition.z, 
        pipelineState.depthStencil.depthEnable
    );
    framebuffer->drawLine(
        v1.screenPosition.x, v1.screenPosition.y,
        v2.screenPosition.x, v2.screenPosition.y,
        v1.color, v1.screenPosition.z, v2.screenPosition.z,
        pipelineState.depthStencil.depthEnable
    );
    framebuffer->drawLine(
        v2.screenPosition.x, v2.screenPosition.y,
        v0.screenPosition.x, v0.screenPosition.y,
        v2.color, v2.screenPosition.z, v0.screenPosition.z,
        pipelineState.depthStencil.depthEnable
    );
}

void Renderer::DrawFilledVertexTriangle(const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2)
{
    // Create scissor rect if enabled
    ScissorRect* scissor = nullptr;
    ScissorRect scissorRect;
    if (pipelineState.rasterizer.scissorEnable) {
        scissorRect.enabled = true;
        scissorRect.left = pipelineState.rasterizer.scissorRect.left;
        scissorRect.top = pipelineState.rasterizer.scissorRect.top;
        scissorRect.right = pipelineState.rasterizer.scissorRect.right;
        scissorRect.bottom = pipelineState.rasterizer.scissorRect.bottom;
        scissor = &scissorRect;
    }
    
    // Use fragment shader if available, otherwise use standard rasterization
    if (fragmentShader) {
        Rasterizer::RasterizeTriangleWithShader(v0, v1, v2, framebuffer, fragmentShader.get(), shaderUniforms, rasterAlgorithm, scissor);
    } else {
        Rasterizer::RasterizeTriangle(v0, v1, v2, framebuffer, rasterAlgorithm, scissor);
    }
    return;
    
    // Keep old implementation below for reference (will be removed later)
    // Get screen positions
    Vec3 p0 = v0.screenPosition;
    Vec3 p1 = v1.screenPosition;
    Vec3 p2 = v2.screenPosition;
    
    // Sort vertices by Y-coordinate
    const TransformedVertex* verts[3] = { &v0, &v1, &v2 };
    Vec3 positions[3] = { p0, p1, p2 };
    
    // Bubble sort by Y
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2 - i; j++) {
            if (positions[j].y > positions[j + 1].y) {
                std::swap(positions[j], positions[j + 1]);
                std::swap(verts[j], verts[j + 1]);
            }
        }
    }
    
    // Scanline rasterization
    int y0 = (int)positions[0].y;
    int y1 = (int)positions[1].y;
    int y2 = (int)positions[2].y;
    
    // Rasterize top half of triangle
    for (int y = y0; y <= y1; y++) {
        if (y < 0 || y >= height) continue;
        
        float t1 = (y1 - y0) > 0 ? (float)(y - y0) / (y1 - y0) : 0;
        float t2 = (y2 - y0) > 0 ? (float)(y - y0) / (y2 - y0) : 0;
        
        int x1 = (int)(positions[0].x + (positions[1].x - positions[0].x) * t1);
        int x2 = (int)(positions[0].x + (positions[2].x - positions[0].x) * t2);
        
        if (x1 > x2) std::swap(x1, x2);
        
        RasterizeScanline(y, x1, x2, *verts[0], *verts[1], *verts[2], positions[0], positions[1], positions[2]);
    }
    
    // Rasterize bottom half of triangle
    for (int y = y1 + 1; y <= y2; y++) {
        if (y < 0 || y >= height) continue;
        
        float t1 = (y2 - y1) > 0 ? (float)(y - y1) / (y2 - y1) : 0;
        float t2 = (y2 - y0) > 0 ? (float)(y - y0) / (y2 - y0) : 0;
        
        int x1 = (int)(positions[1].x + (positions[2].x - positions[1].x) * t1);
        int x2 = (int)(positions[0].x + (positions[2].x - positions[0].x) * t2);
        
        if (x1 > x2) std::swap(x1, x2);
        
        RasterizeScanline(y, x1, x2, *verts[0], *verts[1], *verts[2], positions[0], positions[1], positions[2]);
    }
}

void Renderer::RasterizeScanline(int y, int x1, int x2, 
                                const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2,
                                const Vec3& screenPos0, const Vec3& screenPos1, const Vec3& screenPos2)
{
    x1 = std::max(0, x1);
    x2 = std::min(width - 1, x2);
    
    for (int x = x1; x <= x2; x++) {
        // Compute barycentric coordinates
        Vec3 bary = ComputeBarycentric(Vec3(x, y, 0), screenPos0, screenPos1, screenPos2);
        
        if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue;
        
        // Interpolate vertex attributes
        TransformedVertex interpolated = VertexProcessor::InterpolateVertex(v0, v1, v2, bary.x, bary.y, bary.z);
        
        // Write pixel with interpolated color and depth
        framebuffer->writePixel(x, y, interpolated.color, interpolated.screenPosition.z, true);
    }
}

void Renderer::UpdateShaderUniforms(const Mat4& modelMatrix) {
    Camera* cam = camera ? camera : &defaultCamera;
    
    shaderUniforms.modelMatrix = modelMatrix;
    shaderUniforms.viewMatrix = cam->getViewMatrix();
    shaderUniforms.projectionMatrix = cam->getProjectionMatrix();
    shaderUniforms.cameraPosition = cam->getPosition();
    shaderUniforms.UpdateDerivedMatrices();
}

void Renderer::DrawVertexMeshShaded(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const Mat4& modelMatrix, bool wireframe) {
    if (!vertexShader) {
        // Fall back to regular vertex mesh drawing
        DrawVertexMesh(vertices, indices, modelMatrix, wireframe);
        return;
    }
    
    // Update shader uniforms
    UpdateShaderUniforms(modelMatrix);
    
    // Notify shader of batch begin
    vertexShader->BeginBatch(shaderUniforms);
    
    // Process all vertices through the vertex shader
    std::vector<TransformedVertex> transformedVertices;
    transformedVertices.reserve(vertices.size());
    
    for (const auto& vertex : vertices) {
        transformedVertices.push_back(vertexShader->Process(vertex, shaderUniforms));
    }
    
    // Draw triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        const TransformedVertex& v0 = transformedVertices[indices[i]];
        const TransformedVertex& v1 = transformedVertices[indices[i + 1]];
        const TransformedVertex& v2 = transformedVertices[indices[i + 2]];
        
        // Simple frustum culling - if all vertices are outside clip space, skip
        bool allOutside = true;
        for (int j = 0; j < 3; j++) {
            const TransformedVertex& v = (j == 0) ? v0 : (j == 1) ? v1 : v2;
            if (v.clipPosition.x >= -v.clipPosition.w && v.clipPosition.x <= v.clipPosition.w &&
                v.clipPosition.y >= -v.clipPosition.w && v.clipPosition.y <= v.clipPosition.w &&
                v.clipPosition.z >= 0 && v.clipPosition.z <= v.clipPosition.w) {
                allOutside = false;
                break;
            }
        }
        
        if (allOutside) continue;
        
        if (wireframe) {
            // Draw triangle edges
            DrawLine(v0.worldPosition, v1.worldPosition, v0.color, modelMatrix);
            DrawLine(v1.worldPosition, v2.worldPosition, v1.color, modelMatrix);
            DrawLine(v2.worldPosition, v0.worldPosition, v2.color, modelMatrix);
        } else {
            // Clip triangle against view frustum
            std::vector<TransformedVertex> clippedPolygon = Clipper::ClipTriangle(v0, v1, v2);
            
            // Draw the clipped polygon
            if (!clippedPolygon.empty()) {
                DrawClippedPolygon(clippedPolygon);
            }
        }
    }
    
    // Notify shader of batch end
    vertexShader->EndBatch();
}

void Renderer::DrawClippedPolygon(const std::vector<TransformedVertex>& polygon) {
    if (polygon.size() < 3) return;
    
    // Recalculate screen positions for all vertices after clipping
    std::vector<TransformedVertex> screenPolygon = polygon;
    for (auto& vertex : screenPolygon) {
        vertex.screenPosition = VertexProcessor::ClipToScreen(
            vertex.clipPosition, width, height
        );
    }
    
    // Triangulate the polygon using fan triangulation
    // Take first vertex as the fan center
    const TransformedVertex& v0 = screenPolygon[0];
    
    for (size_t i = 1; i < screenPolygon.size() - 1; ++i) {
        const TransformedVertex& v1 = screenPolygon[i];
        const TransformedVertex& v2 = screenPolygon[i + 1];
        
        // Draw based on fill mode
        switch (pipelineState.rasterizer.fillMode) {
            case FillMode::SOLID:
                DrawFilledVertexTriangle(v0, v1, v2);
                break;
                
            case FillMode::WIREFRAME:
                DrawWireframeVertexTriangle(v0, v1, v2);
                break;
                
            case FillMode::POINT:
                // Draw vertices as points
                framebuffer->writePixel(v0.screenPosition.x, v0.screenPosition.y, v0.color, v0.screenPosition.z, pipelineState.depthStencil.depthEnable);
                framebuffer->writePixel(v1.screenPosition.x, v1.screenPosition.y, v1.color, v1.screenPosition.z, pipelineState.depthStencil.depthEnable);
                framebuffer->writePixel(v2.screenPosition.x, v2.screenPosition.y, v2.color, v2.screenPosition.z, pipelineState.depthStencil.depthEnable);
                break;
        }
    }
}

void Renderer::DrawPrimitives(const std::vector<Vertex>& vertices, PrimitiveType primitiveType, const Mat4& modelMatrix) {
    if (!vertexShader) {
        vertexShader = std::make_shared<DefaultVertexShader>();
    }
    
    UpdateShaderUniforms(modelMatrix);
    
    // Transform all vertices
    std::vector<TransformedVertex> transformedVertices;
    transformedVertices.reserve(vertices.size());
    
    for (const auto& vertex : vertices) {
        transformedVertices.push_back(vertexShader->Process(vertex, shaderUniforms));
    }
    
    // Assemble primitives
    std::vector<Triangle> triangles = primitiveAssembler.AssembleNonIndexed(transformedVertices, primitiveType);
    
    // Draw all triangles
    for (const auto& tri : triangles) {
        // Clip triangle
        std::vector<TransformedVertex> clippedPolygon = Clipper::ClipTriangle(
            tri.vertices[0], tri.vertices[1], tri.vertices[2]
        );
        
        if (!clippedPolygon.empty()) {
            DrawClippedPolygon(clippedPolygon);
        }
    }
}

void Renderer::DrawIndexedPrimitives(const std::vector<Vertex>& vertices, const std::vector<int>& indices, 
                                    PrimitiveType primitiveType, const Mat4& modelMatrix) {
    if (!vertexShader) {
        vertexShader = std::make_shared<DefaultVertexShader>();
    }
    
    UpdateShaderUniforms(modelMatrix);
    
    // Transform all vertices
    std::vector<TransformedVertex> transformedVertices;
    transformedVertices.reserve(vertices.size());
    
    for (const auto& vertex : vertices) {
        transformedVertices.push_back(vertexShader->Process(vertex, shaderUniforms));
    }
    
    // Assemble primitives
    std::vector<Triangle> triangles = primitiveAssembler.AssembleIndexed(transformedVertices, indices, primitiveType);
    
    // Draw all triangles
    for (const auto& tri : triangles) {
        // Clip triangle
        std::vector<TransformedVertex> clippedPolygon = Clipper::ClipTriangle(
            tri.vertices[0], tri.vertices[1], tri.vertices[2]
        );
        
        if (!clippedPolygon.empty()) {
            DrawClippedPolygon(clippedPolygon);
        }
    }
}

void Renderer::SetPipelineState(const PipelineState& state) {
    pipelineState = state;
    
    // Apply rasterizer state to primitive assembler
    primitiveAssembler.SetCullMode(state.rasterizer.cullMode);
    primitiveAssembler.SetWindingOrder(state.rasterizer.frontFace);
    
    // Other state will be used during rendering
}

void Renderer::SetScissorRect(int left, int top, int right, int bottom) {
    pipelineState.rasterizer.scissorRect.left = left;
    pipelineState.rasterizer.scissorRect.top = top;
    pipelineState.rasterizer.scissorRect.right = right;
    pipelineState.rasterizer.scissorRect.bottom = bottom;
}

void Renderer::DrawText(const std::string& text, int x, int y, const Framebuffer::Color& color) {
    BitmapFont::DrawString(framebuffer, text, x, y, color);
}
