#include "Game/GameManager.h"
#include "Math/Vec3.h"
#include "Core/Camera.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexShader.h"
#include "Graphics/FragmentShader.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureGenerator.h"
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
	mGraphics = Graphics::Instance();
	if (!Graphics::IsIntitialized())
	{
		mQuit = true;
	}
	mTimer = Timer::Instance();
	mRenderer = new Renderer(mGraphics->GetRenderer(), Graphics::WINDOW_WIDTH, Graphics::WINDOW_HEIGHT);
	
	// Setup camera
	mCamera = new Camera();
	mCamera->setPerspective(60.0f, (float)Graphics::WINDOW_WIDTH / Graphics::WINDOW_HEIGHT, 0.1f, 100.0f);
	mCamera->setPosition(0, 0, 5);  // Position camera in front of cube
	mCamera->lookAt(Vec3(0, 0, 0));  // Look at origin where cube is
	mRenderer->SetCamera(mCamera);
	
	// Create demo cube
	mCube = new Entity();
	mRotation = 0.0f;
	
	// Initialize FPS tracking
	mFPS = 0.0f;
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
		mTestTexture = std::make_shared<Texture>("assets/textures/uvtest.png");
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
}

GameManager::~GameManager()
{
	delete mCube;
	delete mCamera;
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
			// FPS calculation
			float deltaTime = mTimer->DeltaTime();
			mFrameTime += deltaTime;
			mFrameCount++;
			mFPSUpdateTime += deltaTime;
			
			// Update FPS display every 0.5 seconds
			if (mFPSUpdateTime >= 0.5f) {
				mFPS = mFrameCount / mFrameTime;
				mFrameCount = 0;
				mFrameTime = 0.0f;
				mFPSUpdateTime = 0.0f;
			}
			
			// Update rotation
			mRotation += 1.0f;
			
			// Clear framebuffer
			mRenderer->Clear(Framebuffer::Color(30, 30, 30)); // Dark gray background
			
			// Set culling mode to back-face culling
			mRenderer->SetCullMode(CullMode::BACK);
			
			// Enable fragment shader for textured rendering
			// Toggle comment to see colored cube without textures
			mRenderer->SetFragmentShader(mTexturedShader);
			// mRenderer->SetFragmentShader(nullptr);
			
			// Test scissor rect (uncomment to test scissor functionality)
			// mRenderer->SetScissorTest(true);
			// mRenderer->SetScissorRect(100, 100, Graphics::WINDOW_WIDTH - 100, Graphics::WINDOW_HEIGHT - 100);
			
			// Test fill mode (uncomment to test wireframe or point modes)
			// mRenderer->SetFillMode(FillMode::WIREFRAME);
			// mRenderer->SetFillMode(FillMode::POINT);
			
			// Create colored cube vertices - each face has a different solid color
			std::vector<Vertex> coloredCubeVertices = {
				// Front face - Red
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, 0, 1), Vec2(0, 1), 255, 0, 0),
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, 0, 1), Vec2(1, 1), 255, 0, 0),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(0, 0, 1), Vec2(1, 0), 255, 0, 0),
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(0, 0, 1), Vec2(0, 0), 255, 0, 0),
				// Back face - Blue
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, 0, -1), Vec2(0, 1), 0, 0, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, 0, -1), Vec2(1, 1), 0, 0, 255),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(0, 0, -1), Vec2(1, 0), 0, 0, 255),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(0, 0, -1), Vec2(0, 0), 0, 0, 255),
				// Left face - Green 
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(-1, 0, 0), Vec2(0, 1), 0, 255, 0),  // front-bottom
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(-1, 0, 0), Vec2(1, 1), 0, 255, 0),  // back-bottom
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(-1, 0, 0), Vec2(1, 0), 0, 255, 0),  // back-top
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(-1, 0, 0), Vec2(0, 0), 0, 255, 0),  // front-top
				// Right face - Magenta
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(1, 0, 0), Vec2(0, 1), 255, 0, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(1, 0, 0), Vec2(1, 1), 255, 0, 255),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(1, 0, 0), Vec2(1, 0), 255, 0, 255),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(1, 0, 0), Vec2(0, 0), 255, 0, 255),
				// Top face - Yellow
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(0, 1, 0), Vec2(0, 1), 255, 255, 0),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(0, 1, 0), Vec2(1, 1), 255, 255, 0),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(0, 1, 0), Vec2(1, 0), 255, 255, 0),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(0, 1, 0), Vec2(0, 0), 255, 255, 0),
				// Bottom face - Cyan
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, -1, 0), Vec2(0, 1), 0, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, -1, 0), Vec2(1, 1), 0, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, -1, 0), Vec2(1, 0), 0, 255, 255),
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, -1, 0), Vec2(0, 0), 0, 255, 255)
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
			
			// Draw the colored cube
			mRenderer->DrawVertexMesh(coloredCubeVertices, coloredCubeIndices,
				mCube->GetWorldMatrix(), false);
			
			// Draw face normals for visualization (cyan color, from triangle centers)
			mRenderer->DrawFaceNormals(coloredCubeVertices, coloredCubeIndices, mCube->GetWorldMatrix(), 0.5f, Framebuffer::Color(0, 255, 255));
			
			// Optionally also draw vertex normals (yellow color, from vertices)
			// mRenderer->DrawVertexNormals(coloredCubeVertices, mCube->GetWorldMatrix(), 0.3f, Framebuffer::Color(255, 255, 0));
			
			// Draw FPS counter
			char fpsText[32];
			snprintf(fpsText, sizeof(fpsText), "FPS: %.1f", mFPS);
			mRenderer->DrawText(fpsText, 10, 10, Framebuffer::Color(255, 255, 0));
			
			// Present framebuffer to screen
			mRenderer->Present();
			mGraphics->Render();
			mTimer->Reset();
		}
	}
}