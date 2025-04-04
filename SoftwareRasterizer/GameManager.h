#pragma once

#include "Graphics.h"
#include "Timer.h"
#include "Entity.h"
#include "Renderer.h"

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

public:
	static void Release();
	void Run();

private:
	GameManager();
	~GameManager();
};
