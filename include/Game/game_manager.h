#pragma once

#include "Graphics/graphics.h"
#include "Utils/timer.h"
#include "Game/entity.h"
#include "Graphics/renderer.h"
#include "GUI/gui_manager.h"
#include "Core/camera_controller.h"
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
