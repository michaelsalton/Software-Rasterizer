#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "../Math/Vec3.h"

class Renderer
{
public:
	Renderer(SDL_Renderer* renderer);
	~Renderer();

	void Clear();
	Vec3 Project(Vec3 point, float fov, float aspectRatio, float near, float far);
	void DrawPoint(Vec3 point, SDL_Color color);
	void DrawLine(Vec3 v1, Vec3 v2, SDL_Color color);
	void DrawTriangle(Vec3 v1, Vec3 v2, Vec3 v3, SDL_Color color);
	void DrawFilledTriangle(Vec3 v1, Vec3 v2, Vec3 v3, SDL_Color color);

private:
	SDL_Renderer* mRenderer;
	std::vector<float> mDepthBuffer;
	int Interpolate(int y1, int y2, int x1, int x2, int y);
};

