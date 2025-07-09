#include "Renderer.h"
#include "Graphics.h"
#include <algorithm>
#include <iostream>

Renderer::Renderer(SDL_Renderer* renderer) : mRenderer(renderer) {}

Renderer::~Renderer() {}

Vec3 Renderer::Project(Vec3 point, float fov, float aspectRatio, float near, float far)
{
    if (point.z == 0)
    {
        point.z = 0.1f;
    }
    float projectedX = (point.x / point.z) * fov * aspectRatio;
    float projectedY = (point.y / point.z) * fov;

    float normalizedZ = (point.z - near) / (far - near);

    projectedX = (projectedX + 1.0f) * 0.5f * Graphics::WINDOW_WIDTH;
    projectedY = (1.0f - projectedY) * 0.5f * Graphics::WINDOW_HEIGHT;

    projectedX = std::min(std::max(projectedX, 0.0f), (float)Graphics::WINDOW_WIDTH - 1);
    projectedY = std::min(std::max(projectedY, 0.0f), (float)Graphics::WINDOW_HEIGHT - 1);

    return Vec3(projectedX, projectedY, point.z);
}

void Renderer::DrawPoint(Vec3 point, SDL_Color color)
{
    Vec3 projected = Project(point, 1.0f, 1.0f, 0.1f, 100.0f);
    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(mRenderer, projected.x, projected.y);
}

void Renderer::DrawLine(Vec3 v1, Vec3 v2, SDL_Color color)
{
    Vec3 projected1 = Project(v1, 1.0f, 1.0f, 0.1f, 100.0f);
    Vec3 projected2 = Project(v2, 1.0f, 1.0f, 0.1f, 100.0f);
    SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderLine(mRenderer, projected1.x, projected1.y, projected2.x, projected2.y);
}

void Renderer::DrawTriangle(Vec3 v1, Vec3 v2, Vec3 v3, SDL_Color color)
{
	SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderLine(mRenderer, v1.x, v1.y, v2.x, v2.y);
	SDL_RenderLine(mRenderer, v2.x, v2.y, v3.x, v3.y);
	SDL_RenderLine(mRenderer, v3.x, v3.y, v1.x, v1.y);
}

void Renderer::DrawFilledTriangle(Vec3 v1, Vec3 v2, Vec3 v3, SDL_Color color)
{
    // Sort vertices by Y-coordinate (ascending)
    if (v1.y > v2.y) std::swap(v1, v2);
    if (v2.y > v3.y) std::swap(v2, v3);
    if (v1.y > v2.y) std::swap(v1, v2);

    // Project the vertices into 2D space
    Vec3 p1 = Project(v1, 1.0f, 1.0f, 0.1f, 100.0f);
    Vec3 p2 = Project(v2, 1.0f, 1.0f, 0.1f, 100.0f);
    Vec3 p3 = Project(v3, 1.0f, 1.0f, 0.1f, 100.0f);

    std::cout << "p1: " << p1.x << ", " << p1.y << std::endl;
    std::cout << "p2: " << p2.x << ", " << p2.y << std::endl;
    std::cout << "p3: " << p3.x << ", " << p3.y << std::endl;


    // Interpolate and draw horizontal lines between left and right edges
    auto DrawHorizontalLine = [this, &color](int y, int x1, int x2) {
        for (int x = x1; x <= x2; ++x) 
        {
            SDL_RenderPoint(mRenderer, x, y);
        }
    };

    // Draw the top half of the triangle
    for (int y = p1.y; y <= p2.y; ++y) {
        int xLeft = Interpolate(p1.y, p3.y, p1.x, p3.x, y);
        int xRight = Interpolate(p1.y, p2.y, p1.x, p2.x, y);
        DrawHorizontalLine(y, xLeft, xRight);
    }

    // Draw the bottom half of the triangle
    for (int y = p2.y; y <= p3.y; ++y) {
        int xLeft = Interpolate(p2.y, p3.y, p2.x, p3.x, y);
        int xRight = Interpolate(p1.y, p3.y, p1.x, p3.x, y);
        DrawHorizontalLine(y, xLeft, xRight);
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

