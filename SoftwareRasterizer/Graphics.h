#pragma once

#include <SDL3/SDL.h>
#include <SDL3_Image/SDL_image.h>
#include <stdio.h>
#include <string>

class Graphics
{
public:
	static Graphics* Instance();

	static const int WINDOW_WIDTH = 800;
	static const int WINDOW_HEIGHT = 600;

private:
	static Graphics* sInstance;
	static bool sInitialized;

	SDL_Window* mWindow;
	SDL_Surface* mBackBuffer;
	SDL_Renderer* mRenderer;

public:
	static void Release();
	static bool IsIntitialized();

	SDL_Renderer* GetRenderer();
	SDL_Texture* LoadTexture(std::string path);
	void ClearBackBuffer();
	void DrawTexture(SDL_Texture* texture);
	void Render();

private:
	Graphics();
	~Graphics();

	bool Initialize();
};
