#include "game/game_manager.h"
#include "math/vec3.h"
#include "math/math.h"
#include "core/camera.h"
#include "pipeline/vertex_processor.h"
#include "pipeline/vertex_shader.h"
#include "pipeline/fragment_shader.h"
#include "rendering/texture.h"
#include "graphics/texture_generator.h"
#include "lighting/light.h"
#include "rendering/material.h"
#include "imgui.h"
#include <cmath>
#include <cstdio>

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
	mGraphics = GraphicsContext::Instance();
	if (!GraphicsContext::IsIntitialized())
	{
		mQuit = true;
	}
	mTimer = Timer::Instance();
	mRenderer = new Renderer(mGraphics->GetRenderer(), GraphicsContext::WINDOW_WIDTH, GraphicsContext::WINDOW_HEIGHT);
	
	// Initialize GUI
	mGUIManager = new UIManager();
	if (!mGUIManager->Initialize(mGraphics->GetWindow(), mGraphics->GetRenderer())) {
		printf("Failed to initialize GUI!\n");
		delete mGUIManager;
		mGUIManager = nullptr;
	}
	
	// Setup camera
	mCamera = new Camera();
	mCamera->setPerspective(60.0f, (float)GraphicsContext::WINDOW_WIDTH / GraphicsContext::WINDOW_HEIGHT, 0.1f, 100.0f);
	mCamera->setPosition(0, 0, 5);  // Position camera in front of cube
	mCamera->lookAt(Vec3(0, 0, 0));  // Look at origin where cube is
	mRenderer->SetCamera(mCamera);
	
	// Setup camera controller
	mCameraController = new CameraController(mCamera);
	mCameraController->SetControlMode(CameraController::FPS); // Start in FPS mode
	
	// Create demo cube
	mCube = new Entity();
	mRotation = 0.0f;
	mRotateCube = true;
	
	// Initialize FPS tracking
	mFPS = 60.0f;  // Start with expected FPS
	mFrameTime = 0.0f;
	mFrameCount = 0;
	mFPSUpdateTime = 0.0f;
	
	// Create test texture
	// Try different texture patterns - uncomment one to test
	// mTestTexture = TextureGenerator::CreateCheckerboard(256, 32);
	// mTestTexture = TextureGenerator::CreateGradient(256, 256);
	// mTestTexture = TextureGenerator::CreateDebugGrid(256, 32);
	
	// Load texture from file
	try {
		mTestTexture = std::make_shared<Texture>("assets/textures/metal.jpg");
		printf("Loaded texture from file!\n");
	} catch (const std::exception& e) {
		printf("Failed to load texture: %s\n", e.what());
		printf("Using procedural texture instead.\n");
		mTestTexture = TextureGenerator::CreateDebugGrid(256, 32);
	}
	
	mTestTexture->SetFilter(TextureFilter::BILINEAR);
	mTestTexture->SetWrapU(TextureWrap::REPEAT);
	mTestTexture->SetWrapV(TextureWrap::REPEAT);
	mTestTexture->GenerateMipmaps();
	
	// Create textured fragment shader
	mTexturedShader = std::make_shared<TexturedFragmentShader>();
	mTexturedShader->SetTexture(mTestTexture.get());
	
	// Create lit fragment shader
	mLitShader = std::make_shared<LitFragmentShader>();
	mLitShader->SetAlbedoTexture(mTestTexture.get());
	
	// Set up a basic material
	Material cubeMaterial = Material::Plastic(Vec3(1, 1, 1), 32.0f);
	cubeMaterial.albedoTexture = 0; // Use first texture
	mLitShader->SetMaterial(cubeMaterial);
	
	// Create a directional light (sun)
	mSunLight = std::make_shared<DirectionalLight>(
		Vec3(0, -1, -1).normalized(),  // Direction the light is shining
		Vec3(1.0f, 0.95f, 0.8f),      // Warm white color
		0.5f                          // Reduced intensity for multi-light setup
	);
	mLitShader->AddLight(mSunLight);
	
	// Create point lights
	// Red point light on the left
	auto redLight = std::make_shared<PointLight>(
		Vec3(-2.0f, 0.0f, 2.0f),   // Position
		Vec3(1.0f, 0.2f, 0.2f),    // Red color
		2.0f,                      // Intensity
		1.0f, 0.35f, 0.44f         // Attenuation (constant, linear, quadratic)
	);
	mPointLights.push_back(redLight);
	mLitShader->AddLight(redLight);
	
	// Green point light on the right
	auto greenLight = std::make_shared<PointLight>(
		Vec3(2.0f, 0.0f, 2.0f),    // Position
		Vec3(0.2f, 1.0f, 0.2f),    // Green color
		2.0f,                      // Intensity
		1.0f, 0.35f, 0.44f         // Attenuation
	);
	mPointLights.push_back(greenLight);
	mLitShader->AddLight(greenLight);
	
	// Blue point light above
	auto blueLight = std::make_shared<PointLight>(
		Vec3(0.0f, 2.0f, 0.0f),    // Position
		Vec3(0.2f, 0.2f, 1.0f),    // Blue color
		2.0f,                      // Intensity
		1.0f, 0.35f, 0.44f         // Attenuation
	);
	mPointLights.push_back(blueLight);
	mLitShader->AddLight(blueLight);
	
	// Create a spotlight (like a flashlight)
	mSpotLight = std::make_shared<SpotLight>(
		Vec3(0.0f, 3.0f, 3.0f),    // Position: above and in front
		Vec3(0.0f, -0.7f, -0.7f).normalized(),  // Direction: pointing down at cube
		15.0f,                      // Inner cone angle (degrees)
		30.0f,                      // Outer cone angle (degrees)
		Vec3(1.0f, 0.9f, 0.7f),    // Warm white color
		3.0f,                      // Intensity
		1.0f, 0.09f, 0.032f        // Attenuation
	);
	mLitShader->AddLight(mSpotLight);
	
	// Set ambient light (reduced for better light contrast)
	mLitShader->SetAmbientLight(Vec3(0.1f, 0.1f, 0.12f)); // Slightly blue ambient
	
	mLightAngle = 0.0f;
	mAnimatePointLights = false;
}

GameManager::~GameManager()
{
	delete mCube;
	delete mCameraController;
	delete mCamera;
	delete mGUIManager;
	GraphicsContext::Release();
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
			
			// Pass events to GUI first (it should have priority)
			bool guiWantsMouse = false;
			if (mGUIManager) {
				mGUIManager->ProcessEvent(mEvents);
				guiWantsMouse = mGUIManager->IsMouseOverGUI();
			}
			
			// Only pass mouse events to camera controller if GUI doesn't want them
			bool isMouseEvent = (mEvents.type == SDL_EVENT_MOUSE_MOTION || 
			                    mEvents.type == SDL_EVENT_MOUSE_BUTTON_DOWN || 
			                    mEvents.type == SDL_EVENT_MOUSE_BUTTON_UP ||
			                    mEvents.type == SDL_EVENT_MOUSE_WHEEL);
			
			if (!isMouseEvent || !guiWantsMouse) {
				mCameraController->HandleEvent(mEvents);
			}
			
			// Toggle cube rotation with R key
			if (mEvents.type == SDL_EVENT_KEY_DOWN && mEvents.key.key == SDLK_R) {
				mRotateCube = !mRotateCube;
			}
			
			// Switch camera modes with C key
			if (mEvents.type == SDL_EVENT_KEY_DOWN && mEvents.key.key == SDLK_C) {
				if (mCameraController->GetControlMode() == CameraController::FPS) {
					mCameraController->SetControlMode(CameraController::ORBIT);
				} else {
					mCameraController->SetControlMode(CameraController::FPS);
				}
			}
		}
		if (mTimer->DeltaTime() > 1.0f / FRAME_RATE)
		{
			// FPS calculation
			float deltaTime = mTimer->DeltaTime();
			mFrameTime += deltaTime;
			mFrameCount++;
			mFPSUpdateTime += deltaTime;
			
			// Update FPS display every 0.1 seconds
			if (mFPSUpdateTime >= 0.1f) {
				mFPS = mFrameCount / mFrameTime;
				// printf("Updated FPS: %.1f (frames: %d, time: %.3f)\n", mFPS, mFrameCount, mFrameTime);
				mFrameCount = 0;
				mFrameTime = 0.0f;
				mFPSUpdateTime = 0.0f;
			}
			
			// Update camera controller
			mCameraController->Update(deltaTime);
			
			// Update rotation if enabled
			if (mRotateCube) {
				mRotation += 60.0f * deltaTime;  // 60 degrees per second
			}
			
			// Update light animation
			if (mRenderSettings.enableLighting && mRenderSettings.animateLight) {
				mLightAngle += 30.0f * deltaTime; // 30 degrees per second
				
				// Rotate light direction around Y axis
				float radians = Math::toRadians(mLightAngle);
				Vec3 lightDir(sin(radians), -0.7f, cos(radians));
				mSunLight->setDirection(lightDir.normalized());
			}
			
			// Update point lights
			if (mRenderSettings.enableLighting && mRenderSettings.showPointLights) {
				// Enable/disable point lights based on GUI
				for (auto& light : mPointLights) {
					light->setEnabled(true);
					light->setIntensity(mRenderSettings.pointLightIntensity);
				}
				
				// Animate point lights if enabled
				if (mRenderSettings.animatePointLights) {
					float time = SDL_GetTicks() / 1000.0f;
					
					// Orbit the lights around the cube
					if (mPointLights.size() >= 3) {
						// Red light orbits in XZ plane
						float angle1 = time * 0.5f;
						mPointLights[0]->setPosition(Vec3(cos(angle1) * 2.0f, 0.0f, sin(angle1) * 2.0f + 2.0f));
						
						// Green light orbits in XZ plane (opposite phase)
						float angle2 = time * 0.5f + Math::PI;
						mPointLights[1]->setPosition(Vec3(cos(angle2) * 2.0f, 0.0f, sin(angle2) * 2.0f + 2.0f));
						
						// Blue light moves up and down
						float height = sin(time * 2.0f) * 1.0f + 1.0f;
						mPointLights[2]->setPosition(Vec3(0.0f, height, 0.0f));
					}
				}
			} else {
				// Disable point lights when not shown
				for (auto& light : mPointLights) {
					light->setEnabled(false);
				}
			}
			
			// Update spotlight
			if (mRenderSettings.enableLighting && mRenderSettings.showSpotLight) {
				mSpotLight->setEnabled(true);
				
				// Animate spotlight if enabled
				if (mRenderSettings.animateSpotLight) {
					// Rotate spotlight around Y axis
					float radians = Math::toRadians(mRenderSettings.spotLightAngle);
					float radius = 4.0f;
					Vec3 newPos(sin(radians) * radius, 3.0f, cos(radians) * radius);
					mSpotLight->setPosition(newPos);
					
					// Always point at the cube
					Vec3 toTarget = -newPos.normalized();
					mSpotLight->setDirection(toTarget);
				}
			} else {
				mSpotLight->setEnabled(false);
			}
			
			// Update shader material based on GUI settings
			if (mLitShader) {
				Material mat = Material::Plastic(Vec3(1, 1, 1), 32.0f);
				mat.albedoTexture = 0;
				
				// Set shading model based on GUI
				switch (mRenderSettings.shadingModel) {
					case 0: mat.shadingModel = Material::PHONG; break;
					case 1: mat.shadingModel = Material::BLINN_PHONG; break;
					case 2: mat.shadingModel = Material::LAMBERT; break;
				}
				
				mLitShader->SetMaterial(mat);
			}
			
			// Start GUI frame
			if (mGUIManager) {
				mGUIManager->BeginFrame();
			}
			
			// Clear framebuffer
			mRenderer->Clear(Framebuffer::Color(1, 1, 1)); // Dark gray background
			
			// Apply GUI settings
			// Cull mode
			switch (mRenderSettings.cullMode) {
				case 0: mRenderer->SetCullMode(CullMode::NONE); break;
				case 1: mRenderer->SetCullMode(CullMode::BACK); break;
				case 2: mRenderer->SetCullMode(CullMode::FRONT); break;
			}
			
			// Fill mode
			switch (mRenderSettings.fillMode) {
				case 0: mRenderer->SetFillMode(FillMode::SOLID); break;
				case 1: mRenderer->SetFillMode(FillMode::WIREFRAME); break;
				case 2: mRenderer->SetFillMode(FillMode::POINT); break;
			}
			
			// Shader selection based on lighting and texture settings
			if (mRenderSettings.enableLighting) {
				mRenderer->SetFragmentShader(mLitShader);
			} else if (mRenderSettings.enableTextures) {
				mRenderer->SetFragmentShader(mTexturedShader);
			} else {
				mRenderer->SetFragmentShader(nullptr);
			}
			
			// Update texture filter
			if (mTestTexture && mRenderSettings.enableTextures) {
				switch (mRenderSettings.textureFilter) {
					case 0: mTestTexture->SetFilter(TextureFilter::NEAREST); break;
					case 1: mTestTexture->SetFilter(TextureFilter::BILINEAR); break;
					case 2: mTestTexture->SetFilter(TextureFilter::TRILINEAR); break;
				}
			}
			
			// Rasterization algorithm
			switch (mRenderSettings.rasterAlgorithm) {
				case 0: mRenderer->SetRasterizationAlgorithm(Rasterizer::Algorithm::SCANLINE); break;
				case 1: mRenderer->SetRasterizationAlgorithm(Rasterizer::Algorithm::EDGE_EQUATION); break;
				case 2: mRenderer->SetRasterizationAlgorithm(Rasterizer::Algorithm::HIERARCHICAL); break;
			}
			
			// Other settings
			mRenderer->SetDepthTest(mRenderSettings.enableDepthTest);
			mRenderer->SetScissorTest(mRenderSettings.enableScissor);
			if (mRenderSettings.enableScissor) {
				mRenderer->SetScissorRect(100, 100, GraphicsContext::WINDOW_WIDTH - 100, GraphicsContext::WINDOW_HEIGHT - 100);
			}
			
			// Test scissor rect (uncomment to test scissor functionality)
			// mRenderer->SetScissorTest(true);
			// mRenderer->SetScissorRect(100, 100, GraphicsContext::WINDOW_WIDTH - 100, GraphicsContext::WINDOW_HEIGHT - 100);
			
			// Test fill mode (uncomment to test wireframe or point modes)
			// mRenderer->SetFillMode(FillMode::WIREFRAME);
			// mRenderer->SetFillMode(FillMode::POINT);
			
			// Create textured cube vertices - white color for all vertices (texture only)
			std::vector<Vertex> coloredCubeVertices = {
				// Front face
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, 0, 1), Vec2(0, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, 0, 1), Vec2(1, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(0, 0, 1), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(0, 0, 1), Vec2(0, 0), 255, 255, 255),
				// Back face
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, 0, -1), Vec2(0, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, 0, -1), Vec2(1, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(0, 0, -1), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(0, 0, -1), Vec2(0, 0), 255, 255, 255),
				// Left face
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(-1, 0, 0), Vec2(0, 1), 255, 255, 255),
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(-1, 0, 0), Vec2(1, 1), 255, 255, 255),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(-1, 0, 0), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(-1, 0, 0), Vec2(0, 0), 255, 255, 255),
				// Right face
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(1, 0, 0), Vec2(0, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(1, 0, 0), Vec2(1, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(1, 0, 0), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(1, 0, 0), Vec2(0, 0), 255, 255, 255),
				// Top face
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(0, 1, 0), Vec2(0, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(0, 1, 0), Vec2(1, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(0, 1, 0), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(0, 1, 0), Vec2(0, 0), 255, 255, 255),
				// Bottom face
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, -1, 0), Vec2(0, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, -1, 0), Vec2(1, 1), 255, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, -1, 0), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, -1, 0), Vec2(0, 0), 255, 255, 255)
			};
			
			// Indices for colored cube (24 vertices)
			std::vector<int> coloredCubeIndices = {
				// Front face (CCW in screen space with Y-down)
				0, 3, 2,  2, 1, 0,
				// Back face
				4, 5, 6,  6, 7, 4,
				// Left face
				8, 9, 10,  10, 11, 8,
				// Right face
				13, 12, 15,  15, 14, 13,
				// Top face
				17, 16, 19,  19, 18, 17,
				// Bottom face
				20, 21, 22,  22, 23, 20
			};
			
			// Update cube transform - position at center
			mCube->GetTransform().setRotation(
				Math::toRadians(mRotation * 0.7f),
				Math::toRadians(mRotation),
				Math::toRadians(mRotation * 0.3f)
			);
			mCube->GetTransform().setPosition(0, 0, 0); // Center of screen
			
			// Draw axis if enabled
			if (mRenderSettings.showAxis) {
				mRenderer->DrawAxis(Mat4(), mRenderSettings.axisLength);
			}
			
			
			// Draw the colored cube
			mRenderer->DrawVertexMesh(coloredCubeVertices, coloredCubeIndices,
				mCube->GetWorldMatrix(), false);
			
			// Draw normals based on GUI settings
			if (mRenderSettings.showNormals) {
				// Draw face normals (cyan color, from triangle centers)
				mRenderer->DrawFaceNormals(coloredCubeVertices, coloredCubeIndices, 
					mCube->GetWorldMatrix(), mRenderSettings.normalLength, 
					Framebuffer::Color(0, 255, 255));
			}
			
			if (mRenderSettings.showVertexNormals) {
				// Draw vertex normals (yellow color, from vertices)
				mRenderer->DrawVertexNormals(coloredCubeVertices, 
					mCube->GetWorldMatrix(), mRenderSettings.normalLength * 0.7f, 
					Framebuffer::Color(255, 255, 0));
			}
			
			// Draw wireframe overlay if enabled
			if (mRenderSettings.showWireframe && mRenderSettings.fillMode == 0) {
				mRenderer->SetFillMode(FillMode::WIREFRAME);
				mRenderer->DrawVertexMesh(coloredCubeVertices, coloredCubeIndices,
					mCube->GetWorldMatrix(), true);
				mRenderer->SetFillMode(FillMode::SOLID);
			}
			
			// Draw FPS counter if enabled
			if (mRenderSettings.showFPS) {
				char fpsText[32];
				// Use ImGui's FPS calculation for consistency with the control panel
				float imguiFPS = mGUIManager ? ImGui::GetIO().Framerate : mFPS;
				snprintf(fpsText, sizeof(fpsText), "FPS: %.1f", imguiFPS);
				mRenderer->DrawText(fpsText, 10, 10, Framebuffer::Color(255, 255, 0));
			}
			
			// Present framebuffer to screen
			mRenderer->Present();
			
			// Draw GUI on top
			if (mGUIManager) {
				mGUIManager->DrawControlPanel(mRenderSettings);
				mGUIManager->EndFrame(mGraphics->GetRenderer());
			}
			
			mGraphics->Render();
			mTimer->Reset();
		}
	}
}