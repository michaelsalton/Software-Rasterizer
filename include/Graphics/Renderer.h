#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexShader.h"
#include "Graphics/FragmentShader.h"
#include "Graphics/Clipper.h"
#include "Graphics/PrimitiveAssembler.h"
#include "Graphics/Rasterizer.h"
#include "Graphics/PipelineState.h"
#include "Core/Camera.h"

class Renderer
{
public:
	Renderer(SDL_Renderer* renderer, int width, int height);
	~Renderer();

	void Clear(const Framebuffer::Color& clearColor = Framebuffer::Color(0, 0, 0));
	void Present(); // Copy framebuffer to SDL renderer
	
	// Camera management
	void SetCamera(Camera* cam) { camera = cam; }
	Camera* GetCamera() { return camera; }
	
	// Transform pipeline
	Vec3 Project(Vec3 point, float fov, float aspectRatio, float near, float far); // Legacy
	Vec4 TransformVertex(const Vec4& vertex, const Mat4& modelMatrix); // Full MVP transform
	Vec3 WorldToScreen(const Vec3& worldPoint, const Mat4& modelMatrix = Mat4());
	
	// Drawing with model matrix
	void DrawPoint(Vec3 point, const Framebuffer::Color& color, const Mat4& modelMatrix = Mat4());
	void DrawLine(Vec3 v1, Vec3 v2, const Framebuffer::Color& color, const Mat4& modelMatrix = Mat4());
	void DrawTriangle(Vec3 v1, Vec3 v2, Vec3 v3, const Framebuffer::Color& color, const Mat4& modelMatrix = Mat4());
	void DrawFilledTriangle(Vec3 v1, Vec3 v2, Vec3 v3, const Framebuffer::Color& color, const Mat4& modelMatrix = Mat4(), bool useDepthTest = true);
	
	// Mesh rendering
	void DrawMesh(const std::vector<Vec3>& vertices, const std::vector<int>& indices, const Framebuffer::Color& color, const Mat4& modelMatrix = Mat4(), bool wireframe = false);
	
	// Vertex-based mesh rendering with full attribute support
	void DrawVertexMesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const Mat4& modelMatrix = Mat4(), bool wireframe = false);
	void DrawVertexTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Mat4& modelMatrix = Mat4(), bool wireframe = false);
	
	// Shader-based rendering
	void SetVertexShader(std::shared_ptr<VertexShader> shader) { vertexShader = shader; }
	std::shared_ptr<VertexShader> GetVertexShader() { return vertexShader; }
	void SetFragmentShader(std::shared_ptr<FragmentShader> shader) { fragmentShader = shader; }
	std::shared_ptr<FragmentShader> GetFragmentShader() { return fragmentShader; }
	void DrawVertexMeshShaded(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const Mat4& modelMatrix = Mat4(), bool wireframe = false);
	
	// Shader uniforms
	ShaderUniforms& GetShaderUniforms() { return shaderUniforms; }
	void UpdateShaderUniforms(const Mat4& modelMatrix);
	
	// Primitive assembly and culling
	PrimitiveAssembler& GetPrimitiveAssembler() { return primitiveAssembler; }
	void SetCullMode(CullMode mode) { primitiveAssembler.SetCullMode(mode); }
	void SetWindingOrder(WindingOrder order) { primitiveAssembler.SetWindingOrder(order); }
	
	// Advanced rendering with primitive types
	void DrawPrimitives(const std::vector<Vertex>& vertices, PrimitiveType primitiveType, const Mat4& modelMatrix = Mat4());
	void DrawIndexedPrimitives(const std::vector<Vertex>& vertices, const std::vector<int>& indices, PrimitiveType primitiveType, const Mat4& modelMatrix = Mat4());
	
	// Rasterization algorithm selection
	void SetRasterizationAlgorithm(Rasterizer::Algorithm algorithm) { rasterAlgorithm = algorithm; }
	Rasterizer::Algorithm GetRasterizationAlgorithm() const { return rasterAlgorithm; }
	
	// Pipeline state management
	void SetPipelineState(const PipelineState& state);
	const PipelineState& GetPipelineState() const { return pipelineState; }
	
	// Quick state setters (for convenience)
	void SetFillMode(FillMode mode) { pipelineState.rasterizer.fillMode = mode; }
	void SetDepthTest(bool enable) { pipelineState.depthStencil.depthEnable = enable; }
	void SetDepthWrite(bool enable) { pipelineState.depthStencil.depthWriteEnable = enable; }
	void SetScissorTest(bool enable) { pipelineState.rasterizer.scissorEnable = enable; }
	void SetScissorRect(int left, int top, int right, int bottom);
	
	// Debug visualization
	void DrawAxis(const Mat4& modelMatrix = Mat4(), float length = 1.0f);
	void DrawGrid(int size = 10, float spacing = 1.0f, const Framebuffer::Color& color = Framebuffer::Color(128, 128, 128));
	void DrawVertexNormals(const std::vector<Vertex>& vertices, const Mat4& modelMatrix = Mat4(), float normalLength = 0.3f, const Framebuffer::Color& color = Framebuffer::Color(255, 255, 0));
	void DrawFaceNormals(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const Mat4& modelMatrix = Mat4(), float normalLength = 0.3f, const Framebuffer::Color& color = Framebuffer::Color(0, 255, 255));
	
	// Text rendering
	void DrawText(const std::string& text, int x, int y, const Framebuffer::Color& color = Framebuffer::Color(255, 255, 255));
	
	// Access to framebuffer for advanced operations
	Framebuffer* GetFramebuffer() { return framebuffer; }

private:
	SDL_Renderer* mRenderer;
	SDL_Texture* mTexture;
	Framebuffer* framebuffer;
	Camera* camera;
	int width;
	int height;
	
	// Default camera if none is set
	Camera defaultCamera;
	
	// Shader system
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<FragmentShader> fragmentShader;
	ShaderUniforms shaderUniforms;
	
	// Primitive assembly
	PrimitiveAssembler primitiveAssembler;
	
	// Rasterization algorithm
	Rasterizer::Algorithm rasterAlgorithm = Rasterizer::Algorithm::EDGE_EQUATION;
	
	// Pipeline state
	PipelineState pipelineState;
	
	// Helper functions
	int Interpolate(int y1, int y2, int x1, int x2, int y);
	float InterpolateFloat(float y1, float y2, float x1, float x2, float y);
	
	// Barycentric coordinates for interpolation
	Vec3 ComputeBarycentric(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c);
	
	// Internal drawing functions that work in screen space
	void DrawFilledTriangleScreen(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Framebuffer::Color& color, bool useDepthTest);
	
	// Vertex attribute interpolation for rasterization
	void DrawFilledVertexTriangle(const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2);
	void DrawWireframeVertexTriangle(const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2);
	void DrawClippedPolygon(const std::vector<TransformedVertex>& polygon);
	void RasterizeScanline(int y, int x1, int x2, const TransformedVertex& v0, const TransformedVertex& v1, const TransformedVertex& v2, const Vec3& screenPos0, const Vec3& screenPos1, const Vec3& screenPos2);
};

