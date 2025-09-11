#pragma once

#include <SDL3/SDL.h>
// #include <SDL3_image/SDL_image.h> // Commented out - not available on this system
#include <stdio.h>
#include <string>

class GraphicsContext
{
public:
	static GraphicsContext* Instance();

	static const int WINDOW_WIDTH = 800;
	static const int WINDOW_HEIGHT = 600;

private:
	static GraphicsContext* sInstance;
	static bool sInitialized;

	SDL_Window* mWindow;
	SDL_Surface* mBackBuffer;
	SDL_Renderer* mRenderer;

public:
	static void Release();
	static bool IsIntitialized();

	SDL_Window* GetWindow();
	SDL_Renderer* GetRenderer();
	SDL_Texture* LoadTexture(std::string path);
	void ClearBackBuffer();
	void DrawTexture(SDL_Texture* texture);
	void Render();

private:
	GraphicsContext();
	~GraphicsContext();

	bool Initialize();
};
