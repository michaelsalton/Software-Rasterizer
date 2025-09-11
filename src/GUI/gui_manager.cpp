#include "GUI/gui_manager.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

GUIManager::GUIManager() : mInitialized(false), mContext(nullptr) {
}

GUIManager::~GUIManager() {
    if (mInitialized) {
        Shutdown();
    }
}

bool GUIManager::Initialize(SDL_Window* window, SDL_Renderer* renderer) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    mContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(mContext);
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Scale UI for high DPI displays
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(1.0f);
    
    // Setup Platform/Renderer backends
    if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
        return false;
    }
    if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
        ImGui_ImplSDL3_Shutdown();
        return false;
    }
    
    mInitialized = true;
    return true;
}

void GUIManager::Shutdown() {
    if (mInitialized) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(mContext);
        mContext = nullptr;
        mInitialized = false;
    }
}

void GUIManager::ProcessEvent(const SDL_Event& event) {
    if (mInitialized) {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }
}

void GUIManager::BeginFrame() {
    if (mInitialized) {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
}

void GUIManager::DrawControlPanel(RenderSettings& settings) {
    if (!mInitialized) return;
    
    // Create control panel window
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Render Controls", nullptr, ImGuiWindowFlags_NoCollapse)) {
        // Display options
        if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Show Axis", &settings.showAxis);
            ImGui::Checkbox("Show Face Normals", &settings.showNormals);
            ImGui::Checkbox("Show Vertex Normals", &settings.showVertexNormals);
            ImGui::Checkbox("Show Wireframe Overlay", &settings.showWireframe);
            ImGui::Checkbox("Enable Textures", &settings.enableTextures);
            ImGui::Checkbox("Show FPS", &settings.showFPS);
            
            if (settings.showAxis) {
                ImGui::SliderFloat("Axis Length", &settings.axisLength, 0.5f, 5.0f);
            }
            ImGui::SliderFloat("Normal Length", &settings.normalLength, 0.1f, 2.0f);
        }
        
        // Render modes
        if (ImGui::CollapsingHeader("Render Modes")) {
            const char* fillModes[] = { "Solid", "Wireframe", "Points" };
            ImGui::Combo("Fill Mode", &settings.fillMode, fillModes, 3);
            
            const char* cullModes[] = { "None", "Back Face", "Front Face" };
            ImGui::Combo("Cull Mode", &settings.cullMode, cullModes, 3);
            
            const char* filterModes[] = { "Nearest", "Bilinear", "Trilinear" };
            ImGui::Combo("Texture Filter", &settings.textureFilter, filterModes, 3);
        }
        
        // Lighting
        if (ImGui::CollapsingHeader("Lighting")) {
            ImGui::Checkbox("Enable Lighting", &settings.enableLighting);
            
            if (settings.enableLighting) {
                ImGui::Checkbox("Animate Directional Light", &settings.animateLight);
                
                const char* shadingModels[] = { "Phong", "Blinn-Phong", "Lambert" };
                ImGui::Combo("Shading Model", &settings.shadingModel, shadingModels, 3);
                
                ImGui::Separator();
                ImGui::Checkbox("Show Point Lights", &settings.showPointLights);
                if (settings.showPointLights) {
                    ImGui::Checkbox("Animate Point Lights", &settings.animatePointLights);
                    ImGui::SliderFloat("Point Light Intensity", &settings.pointLightIntensity, 0.0f, 5.0f);
                }
            }
        }
        
        // Pipeline settings
        if (ImGui::CollapsingHeader("Pipeline")) {
            ImGui::Checkbox("Depth Test", &settings.enableDepthTest);
            ImGui::Checkbox("Scissor Test", &settings.enableScissor);
            
            const char* rasterAlgos[] = { "Scanline", "Edge Equation", "Hierarchical" };
            ImGui::Combo("Raster Algorithm", &settings.rasterAlgorithm, rasterAlgos, 3);
        }
        
        // Stats
        if (ImGui::CollapsingHeader("Statistics")) {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);
            ImGui::Text("FPS: %.1f", io.Framerate);
            ImGui::Separator();
            ImGui::Text("Mouse Pos: (%.0f, %.0f)", io.MousePos.x, io.MousePos.y);
        }
        
    }
    ImGui::End();
}

void GUIManager::EndFrame(SDL_Renderer* renderer) {
    if (mInitialized) {
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
}

bool GUIManager::IsMouseOverGUI() const {
    if (!mInitialized) return false;
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}