#include "GameManager.h"

GameManager* GameManager::sInstance = NULL;

GameManager* GameManager::Instance()
{
	if (sInstance == NULL)
	{
		sInstance = new GameManager();
	}
	return sInstance;
}

GameManager::GameManager()
{
	mQuit = false;
	mGraphics = Graphics::Instance();
	if (!Graphics::IsIntitialized)
	{
		mQuit = true;
	}
	mTimer = Timer::Instance();
	mRenderer = new Renderer(mGraphics->GetRenderer());
}

GameManager::~GameManager()
{
	Graphics::Release();
	mGraphics = NULL;
	Timer::Release();
	mTimer = NULL;
	delete mRenderer;
	mRenderer = NULL;
}

void GameManager::Release()
{
	delete sInstance;
	sInstance = NULL;
}

void GameManager::Run()
{
	while (!mQuit)
	{
		mTimer->Update();
		while (SDL_PollEvent(&mEvents) != 0)
		{
			if (mEvents.type == SDL_EVENT_QUIT)
			{
				mQuit = true;
			}
		}
		if (mTimer->DeltaTime() > 1.0f / FRAME_RATE)
		{
			mGraphics->ClearBackBuffer();
			// **** Draw Loop ****



			mRenderer->DrawFilledTriangle(Vector3(0, 0, 0), Vector3(-50, -50, -50), Vector3(-100, -150, -150), { 0, 255, 0, 255 });
			mRenderer->DrawLine(Vector3(0, 0, 0), Vector3(200, 200, 200), { 255, 0, 0, 255 });




			// *******************
			mGraphics->Render();
			mTimer->Reset();
		}
	}
}