#include "graphics/graphics_context.h"

GraphicsContext* GraphicsContext::sInstance = NULL;
bool GraphicsContext::sInitialized = false;

GraphicsContext* GraphicsContext::Instance()
{
	if (sInstance == NULL) {
		sInstance = new GraphicsContext();
	}
	return sInstance;
}

GraphicsContext::GraphicsContext()
{
	mBackBuffer = NULL;
	sInitialized = Initialize();
}

GraphicsContext::~GraphicsContext()
{
	SDL_DestroyWindow(mWindow);
	mWindow = NULL;
	SDL_DestroyRenderer(mRenderer);
	mRenderer = NULL;
	SDL_Quit();
}
  
void GraphicsContext::Release()
{
	delete sInstance;
	sInstance = NULL;
	sInitialized = false;
}

SDL_Window* GraphicsContext::GetWindow()
{
	return mWindow;
}

SDL_Renderer* GraphicsContext::GetRenderer()
{
	return mRenderer;
}

bool GraphicsContext::IsIntitialized()
{
	return sInitialized;
}

bool GraphicsContext::Initialize()
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

SDL_Texture* GraphicsContext::LoadTexture(std::string path)
{
	// SDL_image not available on this system
	printf("LoadTexture: SDL_image not available, cannot load %s\n", path.c_str());
	return NULL;
	/* Original code:
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
	*/
}

void GraphicsContext::ClearBackBuffer()
{
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mRenderer);
}

void GraphicsContext::DrawTexture(SDL_Texture* texture)
{
	SDL_RenderTexture(mRenderer, texture, NULL, NULL);
}

void GraphicsContext::Render()
{
	SDL_RenderPresent(mRenderer);
}
