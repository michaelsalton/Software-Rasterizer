#include "Graphics.h"

Graphics* Graphics::sInstance = NULL;
bool Graphics::sInitialized = false;

Graphics* Graphics::Instance()
{
	if (sInstance == NULL) {
		sInstance = new Graphics();
	}
	return sInstance;
}

Graphics::Graphics()
{
	mBackBuffer = NULL;
	sInitialized = Initialize();
}

Graphics::~Graphics()
{
	SDL_DestroyWindow(mWindow);
	mWindow = NULL;
	SDL_DestroyRenderer(mRenderer);
	mRenderer = NULL;
	SDL_Quit;
}
  
void Graphics::Release()
{
	delete sInstance;
	sInstance = NULL;
	sInitialized = false;
}

SDL_Renderer* Graphics::GetRenderer()
{
	return mRenderer;
}

bool Graphics::IsIntitialized()
{
	return sInitialized;
}

bool Graphics::Initialize()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL: %s", SDL_GetError());
		return false;
	}
	mWindow = SDL_CreateWindow("Software Rasterizer", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (mWindow == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window: %s", SDL_GetError());
		return false;
	}
	mRenderer = SDL_CreateRenderer(mWindow, NULL);
	if (mRenderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create renderer: %s", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_ShowWindow(mWindow);
	mBackBuffer = SDL_GetWindowSurface(mWindow);
	return true;
}

SDL_Texture* Graphics::LoadTexture(std::string path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == NULL)
	{
		printf("Image load error: Path(%s) Error()\n", path.c_str());
		return texture;
	}
	texture = SDL_CreateTextureFromSurface(mRenderer, surface);
	if (texture == NULL)
	{
		printf("Create texture error: %s\n", SDL_GetError());
	}
	SDL_DestroySurface(surface);
	return texture;
}

void Graphics::ClearBackBuffer()
{
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mRenderer);
}

void Graphics::DrawTexture(SDL_Texture* texture)
{
	SDL_RenderTexture(mRenderer, texture, NULL, NULL);
}

void Graphics::Render()
{
	SDL_RenderPresent(mRenderer);
}
