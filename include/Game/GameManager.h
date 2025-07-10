#pragma once

#include "Graphics/Graphics.h"
#include "Utils/Timer.h"
#include "Game/Entity.h"
#include "Graphics/Renderer.h"

class GameManager
{
public:
	static GameManager* Instance();

private:
	static GameManager* sInstance;
	Renderer* mRenderer;
	const int FRAME_RATE = 60;
	bool mQuit;
	Graphics* mGraphics;
	SDL_Event mEvents;
	Timer* mTimer;
	
	// Demo objects
	class Camera* mCamera;
	Entity* mCube;
	float mRotation;

public:
	static void Release();
	void Run();

private:
	GameManager();
	~GameManager();
};
