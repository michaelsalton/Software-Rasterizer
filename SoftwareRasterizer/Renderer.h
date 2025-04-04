#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "Vector3.h"

class Renderer
{
public:
	Renderer(SDL_Renderer* renderer);
	~Renderer();

	void Clear();
	Vector3 Project(Vector3 point, float fov, float aspectRatio, float near, float far);
	void DrawPoint(Vector3 point, SDL_Color color);
	void DrawLine(Vector3 v1, Vector3 v2, SDL_Color color);
	void DrawTriangle(Vector3 v1, Vector3 v2, Vector3 v3, SDL_Color color);
	void DrawFilledTriangle(Vector3 v1, Vector3 v2, Vector3 v3, SDL_Color color);

private:
	SDL_Renderer* mRenderer;
	std::vector<float> mDepthBuffer;
	int Interpolate(int y1, int y2, int x1, int x2, int y);
};

