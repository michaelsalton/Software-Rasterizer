#pragma once

#include <SDL3/SDL.h>

class InputManager
{
public:
	static InputManager* Instance();

private:
	static InputManager* sInstance;
	const bool* mKeyboardStates;

public:
	static void Release();
	bool KeyDown(SDL_Scancode scanCode);
	void Update();

private:
	InputManager();
	~InputManager();
};

