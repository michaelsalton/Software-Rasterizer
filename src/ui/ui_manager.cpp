#include "ui/ui_manager.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

UIManager::UIManager() : mInitialized(false), mContext(nullptr), mInteractionEnabled(true) {
}

UIManager::~UIManager() {
    if (mInitialized) {
        Shutdown();
    }
}

bool UIManager::Initialize(SDL_Window* window, SDL_Renderer* renderer) {
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

void UIManager::Shutdown() {
    if (mInitialized) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(mContext);
        mContext = nullptr;
        mInitialized = false;
    }
}

void UIManager::ProcessEvent(const SDL_Event& event) {
    if (mInitialized && mInteractionEnabled) {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }
}

void UIManager::BeginFrame() {
    if (mInitialized) {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
}

void UIManager::DrawControlPanel(RenderSettings& settings) {
    if (!mInitialized) return;
    
    // When interaction is disabled, push a disabled style
    if (!mInteractionEnabled) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f); // Make it semi-transparent
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
    }
    
    // Create control panel window
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Render Controls", nullptr, ImGuiWindowFlags_NoCollapse)) {
        // Show camera control instructions
        if (!mInteractionEnabled) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f)); // Yellow
            ImGui::Text("Camera Mode Active - Press ESC to release");
            ImGui::PopStyleColor();
            ImGui::Separator();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 1.0f, 0.7f, 1.0f)); // Light green
            ImGui::Text("Press E to activate camera control");
            ImGui::PopStyleColor();
            ImGui::Separator();
        }
        
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
                
                ImGui::Separator();
                ImGui::Checkbox("Show Spot Light", &settings.showSpotLight);
                if (settings.showSpotLight) {
                    ImGui::Checkbox("Animate Spot Light", &settings.animateSpotLight);
                    if (settings.animateSpotLight) {
                        ImGui::SliderFloat("Spot Light Angle", &settings.spotLightAngle, 0.0f, 360.0f);
                    }
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
    
    // Pop the disabled style if it was pushed
    if (!mInteractionEnabled) {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
}

void UIManager::EndFrame(SDL_Renderer* renderer) {
    if (mInitialized) {
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
}

void UIManager::DrawControlsWindow(bool cameraActive, bool isOrbitMode) {
    if (!mInitialized) return;
    
    // Position window in bottom right corner
    ImGuiIO& io = ImGui::GetIO();
    float windowWidth = 250.0f;
    float windowHeight = cameraActive ? 180.0f : 120.0f;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - windowWidth - 10, 
                                  io.DisplaySize.y - windowHeight - 10), 
                           ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
    
    // Make the window non-interactive and semi-transparent
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | 
                                  ImGuiWindowFlags_NoResize | 
                                  ImGuiWindowFlags_NoMove | 
                                  ImGuiWindowFlags_NoScrollbar | 
                                  ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav;
    
    if (ImGui::Begin("##Controls", nullptr, windowFlags)) {
        ImGui::Text("Controls");
        ImGui::Separator();
        
        if (cameraActive) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.5f, 1.0f)); // Yellow
            ImGui::Text("Camera Mode Active");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            if (isOrbitMode) {
                ImGui::Text("Mouse: Rotate camera");
                ImGui::Text("W/S: Zoom in/out");
                ImGui::Text("Mouse Wheel: Zoom");
            } else {
                ImGui::Text("Mouse: Look around");
                ImGui::Text("W: Move forward");
                ImGui::Text("S: Move backward");
                ImGui::Text("A: Strafe left");
                ImGui::Text("D: Strafe right");
            }
            ImGui::Spacing();
            ImGui::Text("ESC: Release camera");
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 0.5f, 1.0f)); // Green
            ImGui::Text("UI Mode Active");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            ImGui::Text("E: Activate camera");
            ImGui::Text("Mouse: Interact with UI");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("R: Toggle cube rotation");
        ImGui::Text("C: Switch camera mode");
    }
    ImGui::End();
}

bool UIManager::IsMouseOverGUI() const {
    if (!mInitialized || !mInteractionEnabled) return false;
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}