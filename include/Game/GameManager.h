#pragma once

#include "Graphics/Graphics.h"
#include "Utils/Timer.h"
#include "Game/Entity.h"
#include "Graphics/Renderer.h"
#include "GUI/GUIManager.h"
#include "Core/CameraController.h"
#include <memory>

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
	CameraController* mCameraController;
	Entity* mCube;
	float mRotation;
	bool mRotateCube;  // Toggle for cube rotation
	
	// FPS tracking
	float mFPS;
	float mFrameTime;
	int mFrameCount;
	float mFPSUpdateTime;
	
	// Texture demo
	std::shared_ptr<class Texture> mTestTexture;
	std::shared_ptr<class TexturedFragmentShader> mTexturedShader;
	
	// Lighting demo
	std::shared_ptr<class LitFragmentShader> mLitShader;
	std::shared_ptr<class DirectionalLight> mSunLight;
	float mLightAngle;
	
	// GUI
	GUIManager* mGUIManager;
	RenderSettings mRenderSettings;

public:
	static void Release();
	void Run();

private:
	GameManager();
	~GameManager();
};
