#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "GameManager.h"

int main(int argc, char* argv[])
{
	GameManager* game = GameManager::Instance();
	
	game->Run();

	GameManager::Release();

	game = NULL;

	return 0;
}