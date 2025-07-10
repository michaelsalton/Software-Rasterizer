#include "Game/GameManager.h"
#include "Math/Vec3.h"
#include "Core/Camera.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexShader.h"
#include <cmath>

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
	mCamera->setPosition(-5, -1, -7);
	mCamera->lookAt(Vec3(0, -1, -1));  // Look down negative Z axis
	mRenderer->SetCamera(mCamera);
	
	// Create demo cube
	mCube = new Entity();
	mRotation = 0.0f;
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
			// Update rotation
			mRotation += 1.0f;
			
			// Clear framebuffer
			mRenderer->Clear(Framebuffer::Color(30, 30, 30)); // Dark gray background
			
			// **** Draw Loop ****
			
			// Draw grid to show ground plane
			mRenderer->DrawGrid(20, 0.5f, Framebuffer::Color(64, 64, 64));
			
			// Draw world axis at origin
			mRenderer->DrawAxis(Mat4(), 2.0f);
			
			// Draw rotating cube
			// Define cube vertices
			std::vector<Vec3> cubeVertices = {
				// Front face
				Vec3(-0.5f, -0.5f,  0.5f),
				Vec3( 0.5f, -0.5f,  0.5f),
				Vec3( 0.5f,  0.5f,  0.5f),
				Vec3(-0.5f,  0.5f,  0.5f),
				// Back face
				Vec3(-0.5f, -0.5f, -0.5f),
				Vec3( 0.5f, -0.5f, -0.5f),
				Vec3( 0.5f,  0.5f, -0.5f),
				Vec3(-0.5f,  0.5f, -0.5f)
			};
			
			// Create colored cube using new vertex system
			std::vector<Vertex> coloredCubeVertices = {
				// Front face - red gradient
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, 0, 1), Vec2(0, 1), 255, 0, 0),
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, 0, 1), Vec2(1, 1), 255, 128, 0),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(0, 0, 1), Vec2(1, 0), 255, 255, 0),
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(0, 0, 1), Vec2(0, 0), 255, 128, 128),
				// Back face - blue gradient
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, 0, -1), Vec2(0, 1), 0, 0, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, 0, -1), Vec2(1, 1), 0, 128, 255),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(0, 0, -1), Vec2(1, 0), 0, 255, 255),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(0, 0, -1), Vec2(0, 0), 128, 128, 255),
				// Left face - green gradient
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(-1, 0, 0), Vec2(0, 1), 0, 255, 0),
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(-1, 0, 0), Vec2(1, 1), 128, 255, 0),
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(-1, 0, 0), Vec2(1, 0), 255, 255, 128),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(-1, 0, 0), Vec2(0, 0), 128, 255, 128),
				// Right face - magenta gradient
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(1, 0, 0), Vec2(0, 1), 255, 0, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(1, 0, 0), Vec2(1, 1), 255, 128, 255),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(1, 0, 0), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(1, 0, 0), Vec2(0, 0), 255, 128, 255),
				// Top face - yellow gradient
				Vertex(Vec3(-0.5f,  0.5f,  0.5f), Vec3(0, 1, 0), Vec2(0, 1), 255, 255, 0),
				Vertex(Vec3( 0.5f,  0.5f,  0.5f), Vec3(0, 1, 0), Vec2(1, 1), 255, 255, 128),
				Vertex(Vec3( 0.5f,  0.5f, -0.5f), Vec3(0, 1, 0), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3(-0.5f,  0.5f, -0.5f), Vec3(0, 1, 0), Vec2(0, 0), 255, 255, 64),
				// Bottom face - cyan gradient
				Vertex(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, -1, 0), Vec2(0, 1), 0, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, -1, 0), Vec2(1, 1), 128, 255, 255),
				Vertex(Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, -1, 0), Vec2(1, 0), 255, 255, 255),
				Vertex(Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, -1, 0), Vec2(0, 0), 64, 255, 255)
			};
			
			std::vector<int> cubeIndices = {
				// Front face
				0, 1, 2,  2, 3, 0,
				// Back face
				5, 4, 7,  7, 6, 5,
				// Left face
				4, 0, 3,  3, 7, 4,
				// Right face
				1, 5, 6,  6, 2, 1,
				// Top face
				3, 2, 6,  6, 7, 3,
				// Bottom face
				4, 5, 1,  1, 0, 4
			};
			
			// Indices for colored cube (24 vertices)
			std::vector<int> coloredCubeIndices = {
				// Front face
				0, 1, 2,  2, 3, 0,
				// Back face
				4, 5, 6,  6, 7, 4,
				// Left face
				8, 9, 10,  10, 11, 8,
				// Right face
				12, 13, 14,  14, 15, 12,
				// Top face
				16, 17, 18,  18, 19, 16,
				// Bottom face
				20, 21, 22,  22, 23, 20
			};
			
			// Update cube transform
			mCube->GetTransform().setRotation(
				Math::toRadians(mRotation * 0.7f),
				Math::toRadians(mRotation),
				Math::toRadians(mRotation * 0.3f)
			);
			mCube->GetTransform().setPosition(0, 0.5f, 0);
			
			// Draw cube (wireframe)
			mRenderer->DrawMesh(cubeVertices, cubeIndices, 
				Framebuffer::Color(0, 255, 255), 
				mCube->GetWorldMatrix(), true);
			
			// Draw cube's local coordinate system
			mRenderer->DrawAxis(mCube->GetWorldMatrix(), 1.0f);
			
			// Draw a second cube that's a child of the first
			Entity childCube;
			childCube.Parent(mCube);
			childCube.GetTransform().setPosition(2, 0, 0);
			childCube.GetTransform().setScale(0.5f);
			childCube.GetTransform().setRotation(0, Math::toRadians(-mRotation * 2), 0);
			
			// Draw child cube with vertex colors (filled)
			mRenderer->DrawVertexMesh(coloredCubeVertices, coloredCubeIndices,
				childCube.GetWorldMatrix(), false);
			
			// Draw child's coordinate system
			mRenderer->DrawAxis(childCube.GetWorldMatrix(), 0.5f);
			
			// Draw a third cube to show vertex color interpolation
			Entity vertexColorCube;
			vertexColorCube.GetTransform().setPosition(-2, 0, 0);
			vertexColorCube.GetTransform().setScale(1.5f);
			vertexColorCube.GetTransform().setRotation(
				Math::toRadians(-mRotation * 0.5f),
				Math::toRadians(mRotation * 0.8f),
				0
			);
			
			// Create a simple triangle with vertex colors for clear interpolation demo
			std::vector<Vertex> triangleVertices = {
				Vertex(Vec3(0, 1, 0), Vec3(0, 0, 1), Vec2(0.5f, 0), 255, 0, 0),      // Red top
				Vertex(Vec3(-0.866f, -0.5f, 0), Vec3(0, 0, 1), Vec2(0, 1), 0, 255, 0), // Green bottom left
				Vertex(Vec3(0.866f, -0.5f, 0), Vec3(0, 0, 1), Vec2(1, 1), 0, 0, 255)   // Blue bottom right
			};
			
			// Draw the triangle to show color interpolation
			mRenderer->DrawVertexTriangle(triangleVertices[0], triangleVertices[1], triangleVertices[2],
				vertexColorCube.GetWorldMatrix(), false);
			
			// **** Vertex Shader Demonstrations ****
			
			// Get shader uniforms reference once
			ShaderUniforms& uniforms = mRenderer->GetShaderUniforms();
			
			// Update shader time uniform
			uniforms.time = SDL_GetTicks() * 0.001f; // Convert to seconds
			
			// Wave shader demo
			Entity waveCube;
			waveCube.GetTransform().setPosition(3, 0, -2);
			waveCube.GetTransform().setScale(1.2f);
			waveCube.GetTransform().setRotation(0, Math::toRadians(mRotation * 0.5f), 0);
			
			auto waveShader = std::make_shared<WaveVertexShader>();
			mRenderer->SetVertexShader(waveShader);
			uniforms.SetFloat("waveAmplitude", 0.15f);
			uniforms.SetFloat("waveFrequency", 3.0f);
			uniforms.SetFloat("waveSpeed", 2.0f);
			mRenderer->DrawVertexMeshShaded(coloredCubeVertices, coloredCubeIndices,
				waveCube.GetWorldMatrix(), false);
			
			// Twist shader demo
			Entity twistCube;
			twistCube.GetTransform().setPosition(-3, 0, -2);
			twistCube.GetTransform().setScale(0.8f, 2.0f, 0.8f);
			
			auto twistShader = std::make_shared<TwistVertexShader>();
			mRenderer->SetVertexShader(twistShader);
			uniforms.SetFloat("twistAmount", Math::toRadians(180.0f));
			uniforms.SetFloat("twistHeight", 2.0f);
			mRenderer->DrawVertexMeshShaded(coloredCubeVertices, coloredCubeIndices,
				twistCube.GetWorldMatrix(), false);
			
			// Explode shader demo
			Entity explodeCube;
			explodeCube.GetTransform().setPosition(0, 2, -3);
			explodeCube.GetTransform().setRotation(
				Math::toRadians(mRotation * 0.3f),
				Math::toRadians(mRotation * 0.7f),
				0
			);
			
			auto explodeShader = std::make_shared<ExplodeVertexShader>();
			mRenderer->SetVertexShader(explodeShader);
			uniforms.SetFloat("explodeAmount", 0.3f);
			mRenderer->DrawVertexMeshShaded(coloredCubeVertices, coloredCubeIndices,
				explodeCube.GetWorldMatrix(), false);
			
			// Spherize shader demo
			Entity spherizeCube;
			spherizeCube.GetTransform().setPosition(0, -2, -1);
			spherizeCube.GetTransform().setScale(1.5f);
			spherizeCube.GetTransform().setRotation(0, Math::toRadians(mRotation), 0);
			
			auto spherizeShader = std::make_shared<SpherizeVertexShader>();
			mRenderer->SetVertexShader(spherizeShader);
			uniforms.SetFloat("spherizeAmount", 0.8f);
			uniforms.SetFloat("spherizeRadius", 0.7f);
			mRenderer->DrawVertexMeshShaded(coloredCubeVertices, coloredCubeIndices,
				spherizeCube.GetWorldMatrix(), false);
			
			// Reset to default shader for normal rendering
			mRenderer->SetVertexShader(std::make_shared<DefaultVertexShader>());
			
			// **** Primitive Assembly and Culling Demo ****
			
			// Triangle strip demo - creates a ribbon
			std::vector<Vertex> stripVertices;
			const int stripSegments = 20;
			for (int i = 0; i <= stripSegments; ++i) {
				float t = (float)i / stripSegments;
				float x = (t - 0.5f) * 4.0f;
				float z = sin(t * Math::TWO_PI) * 0.5f;
				
				// Top vertex
				stripVertices.push_back(Vertex(
					Vec3(x, 0.5f, z),
					Vec3(0, 1, 0),
					Vec2(t, 0),
					255, static_cast<uint8_t>(t * 255), 0
				));
				
				// Bottom vertex
				stripVertices.push_back(Vertex(
					Vec3(x, -0.5f, z),
					Vec3(0, 1, 0),
					Vec2(t, 1),
					0, static_cast<uint8_t>(t * 255), 255
				));
			}
			
			Entity stripEntity;
			stripEntity.GetTransform().setPosition(0, 3, 0);
			stripEntity.GetTransform().setRotation(0, Math::toRadians(mRotation * 0.5f), 0);
			
			// Draw as triangle strip
			mRenderer->DrawPrimitives(stripVertices, PrimitiveType::TRIANGLE_STRIP, stripEntity.GetWorldMatrix());
			
			// Triangle fan demo - creates a cone/circle
			std::vector<Vertex> fanVertices;
			fanVertices.push_back(Vertex(Vec3(0, 1, 0), Vec3(0, 1, 0), Vec2(0.5f, 0.5f), 255, 255, 255)); // Center
			
			const int fanSegments = 16;
			for (int i = 0; i <= fanSegments; ++i) {
				float angle = ((float)i / fanSegments) * Math::TWO_PI;
				float x = cos(angle) * 0.5f;
				float z = sin(angle) * 0.5f;
				
				fanVertices.push_back(Vertex(
					Vec3(x, 0, z),
					Vec3(0, 1, 0),
					Vec2((x + 1) * 0.5f, (z + 1) * 0.5f),
					static_cast<uint8_t>(128 + x * 127),
					static_cast<uint8_t>(128 + z * 127),
					128
				));
			}
			
			Entity fanEntity;
			fanEntity.GetTransform().setPosition(-4, 1, 0);
			fanEntity.GetTransform().setScale(2.0f);
			fanEntity.GetTransform().setRotation(Math::toRadians(45), 0, 0);
			
			// Draw as triangle fan
			mRenderer->DrawPrimitives(fanVertices, PrimitiveType::TRIANGLE_FAN, fanEntity.GetWorldMatrix());
			
			// Culling demo - draw two cubes with different culling modes
			Entity cullTestCube1;
			cullTestCube1.GetTransform().setPosition(4, 1, 0);
			cullTestCube1.GetTransform().setRotation(0, Math::toRadians(mRotation), 0);
			
			// Enable back-face culling (default)
			mRenderer->SetCullMode(CullMode::BACK);
			mRenderer->DrawIndexedPrimitives(coloredCubeVertices, coloredCubeIndices, 
				PrimitiveType::TRIANGLES, cullTestCube1.GetWorldMatrix());
			
			// Draw wireframe overlay to show culled faces
			mRenderer->SetCullMode(CullMode::NONE);
			mRenderer->DrawVertexMesh(coloredCubeVertices, coloredCubeIndices,
				cullTestCube1.GetWorldMatrix(), true);
			
			// Frustum culling demo - create objects that move in and out of view
			Entity frustumTestCube;
			float frustumX = sin(SDL_GetTicks() * 0.001f) * 10.0f; // Move side to side
			frustumTestCube.GetTransform().setPosition(frustumX, 1, -5);
			frustumTestCube.GetTransform().setScale(1.5f);
			frustumTestCube.GetTransform().setRotation(
				Math::toRadians(mRotation),
				Math::toRadians(mRotation * 0.7f),
				Math::toRadians(mRotation * 0.3f)
			);
			
			// This cube will be frustum culled when it moves outside the view
			mRenderer->SetCullMode(CullMode::BACK);
			mRenderer->DrawIndexedPrimitives(coloredCubeVertices, coloredCubeIndices,
				PrimitiveType::TRIANGLES, frustumTestCube.GetWorldMatrix());
			
			// Near plane clipping demo - cube that intersects near plane
			Entity nearPlaneCube;
			float nearZ = -2.0f + sin(SDL_GetTicks() * 0.0005f) * 3.0f; // Move forward/back
			nearPlaneCube.GetTransform().setPosition(0, -3, nearZ);
			nearPlaneCube.GetTransform().setScale(1.2f);
			nearPlaneCube.GetTransform().setRotation(
				Math::toRadians(mRotation * 0.5f),
				Math::toRadians(mRotation),
				0
			);
			
			// This cube will be clipped when it intersects the near plane
			mRenderer->DrawIndexedPrimitives(coloredCubeVertices, coloredCubeIndices,
				PrimitiveType::TRIANGLES, nearPlaneCube.GetWorldMatrix());
			
			// Reset culling mode
			mRenderer->SetCullMode(CullMode::BACK);
			
			// **** Rasterization Algorithm Comparison Demo ****
			
			// Cycle through rasterization algorithms based on time
			int algorithmIndex = (SDL_GetTicks() / 3000) % 3; // Change every 3 seconds
			Rasterizer::Algorithm currentAlgorithm;
			std::string algorithmName;
			
			switch (algorithmIndex) {
				case 0:
					currentAlgorithm = Rasterizer::Algorithm::SCANLINE;
					algorithmName = "SCANLINE";
					break;
				case 1:
					currentAlgorithm = Rasterizer::Algorithm::EDGE_EQUATION;
					algorithmName = "EDGE_EQUATION";
					break;
				case 2:
					currentAlgorithm = Rasterizer::Algorithm::HIERARCHICAL;
					algorithmName = "HIERARCHICAL";
					break;
			}
			
			mRenderer->SetRasterizationAlgorithm(currentAlgorithm);
			
			// Create a large triangle to test rasterization
			std::vector<Vertex> largeTriangle = {
				Vertex(Vec3(-2, -1, 0), Vec3(0, 0, 1), Vec2(0, 1), 255, 0, 0),     // Red
				Vertex(Vec3(2, -1, 0), Vec3(0, 0, 1), Vec2(1, 1), 0, 255, 0),      // Green
				Vertex(Vec3(0, 2, 0), Vec3(0, 0, 1), Vec2(0.5f, 0), 0, 0, 255)     // Blue
			};
			
			Entity triangleEntity;
			triangleEntity.GetTransform().setPosition(0, -4, -2);
			triangleEntity.GetTransform().setScale(2.0f);
			triangleEntity.GetTransform().setRotation(0, Math::toRadians(mRotation * 0.2f), 0);
			
			// Draw the triangle with current algorithm
			mRenderer->DrawPrimitives(largeTriangle, PrimitiveType::TRIANGLES, triangleEntity.GetWorldMatrix());
			
			// Create a mesh with many small triangles to stress-test rasterization
			std::vector<Vertex> tessellatedPlane;
			std::vector<int> planeIndices;
			
			const int gridSize = 10;
			const float spacing = 0.2f;
			
			// Generate vertices
			for (int y = 0; y <= gridSize; ++y) {
				for (int x = 0; x <= gridSize; ++x) {
					float fx = (x - gridSize/2) * spacing;
					float fy = 0;
					float fz = (y - gridSize/2) * spacing;
					
					// Add some wave displacement
					fy = sin(fx * 2 + SDL_GetTicks() * 0.001f) * 0.1f + 
					     sin(fz * 2 + SDL_GetTicks() * 0.001f) * 0.1f;
					
					uint8_t r = static_cast<uint8_t>((float)x / gridSize * 255);
					uint8_t b = static_cast<uint8_t>((float)y / gridSize * 255);
					
					tessellatedPlane.push_back(Vertex(
						Vec3(fx, fy, fz),
						Vec3(0, 1, 0),
						Vec2((float)x / gridSize, (float)y / gridSize),
						r, 128, b
					));
				}
			}
			
			// Generate indices for triangles
			for (int y = 0; y < gridSize; ++y) {
				for (int x = 0; x < gridSize; ++x) {
					int topLeft = y * (gridSize + 1) + x;
					int topRight = topLeft + 1;
					int bottomLeft = topLeft + (gridSize + 1);
					int bottomRight = bottomLeft + 1;
					
					// First triangle
					planeIndices.push_back(topLeft);
					planeIndices.push_back(bottomLeft);
					planeIndices.push_back(topRight);
					
					// Second triangle
					planeIndices.push_back(topRight);
					planeIndices.push_back(bottomLeft);
					planeIndices.push_back(bottomRight);
				}
			}
			
			Entity planeEntity;
			planeEntity.GetTransform().setPosition(5, -2, -3);
			planeEntity.GetTransform().setRotation(Math::toRadians(-30), 0, 0);
			
			// Draw tessellated plane with many triangles
			mRenderer->DrawIndexedPrimitives(tessellatedPlane, planeIndices, 
				PrimitiveType::TRIANGLES, planeEntity.GetWorldMatrix());
			
			// *******************
			
			// Present framebuffer to screen
			mRenderer->Present();
			mGraphics->Render();
			mTimer->Reset();
		}
	}
}