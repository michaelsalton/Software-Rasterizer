#pragma once

#include "graphics/graphics_context.h"
#include "utils/timer.h"
#include "scene/entity.h"
#include "rendering/renderer.h"
#include "ui/ui_manager.h"
#include "core/camera_controller.h"
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
	GraphicsContext* mGraphics;
	SDL_Event mEvents;
	Timer* mTimer;
	
	// Demo objects
	class Camera* mCamera;
	CameraController* mCameraController;
	Entity* mCube;
	float mRotation;
	bool mRotateCube;  // Toggle for cube rotation
	
	// Model loading demo
	std::shared_ptr<class Model> mLoadedModel;
	
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
	std::vector<std::shared_ptr<class PointLight>> mPointLights;
	std::shared_ptr<class SpotLight> mSpotLight;
	float mLightAngle;
	bool mAnimatePointLights;
	
	// GUI
	UIManager* mGUIManager;
	RenderSettings mRenderSettings;

public:
	static void Release();
	void Run();

private:
	GameManager();
	~GameManager();
};
