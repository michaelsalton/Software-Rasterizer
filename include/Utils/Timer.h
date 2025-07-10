#pragma once

#include <SDL3/SDL.h>

class Timer
{
public:
	static Timer* Instance();

private:
	static Timer* sInstance;
	unsigned int mStartTicks;
	unsigned int mElapsedTicks;
	float mDeltaTime;
	float mTimeScale;

public:
	static void Release();
	void Reset();
	float DeltaTime();
	void TimeScale(float timeScale);
	float TimeScale();
	void Update();

private:
	Timer();
	~Timer();
};
