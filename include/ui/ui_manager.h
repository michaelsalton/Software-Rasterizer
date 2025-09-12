#pragma once

#include <SDL3/SDL.h>

// Forward declarations
struct ImGuiContext;

struct RenderSettings {
    // Display toggles
    bool showNormals = false;
    bool showVertexNormals = false;
    bool showWireframe = false;
    bool enableTextures = true;
    bool showFPS = true;
    bool showAxis = false;
    
    // Render modes
    int fillMode = 0; // 0=Solid, 1=Wireframe, 2=Points
    int cullMode = 1; // 0=None, 1=Back, 2=Front
    int textureFilter = 1; // 0=Nearest, 1=Bilinear, 2=Trilinear
    
    // Lighting
    bool enableLighting = true;
    bool animateLight = true;
    int shadingModel = 0; // 0=Phong, 1=Blinn-Phong, 2=Lambert
    bool showPointLights = true;
    bool animatePointLights = false;
    float pointLightIntensity = 2.0f;
    bool showSpotLight = true;
    bool animateSpotLight = false;
    float spotLightAngle = 0.0f;
    
    // Debug options
    bool enableDepthTest = true;
    bool enableScissor = false;
    float normalLength = 0.5f;
    float axisLength = 2.0f;
    
    // Performance
    int rasterAlgorithm = 1; // 0=Scanline, 1=EdgeEquation, 2=Hierarchical
    
    // Model loading
    bool useLoadedModel = true; // Toggle between loaded model and hardcoded cube
};

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    // Initialize ImGui with SDL
    bool Initialize(SDL_Window* window, SDL_Renderer* renderer);
    void Shutdown();
    
    // Process SDL events
    void ProcessEvent(const SDL_Event& event);
    
    // Start new frame
    void BeginFrame();
    
    // Render control panel
    void DrawControlPanel(RenderSettings& settings);
    
    // Render controls help window
    void DrawControlsWindow(bool cameraActive, bool isOrbitMode);
    
    // End frame and render
    void EndFrame(SDL_Renderer* renderer);
    
    // Check if mouse is over GUI
    bool IsMouseOverGUI() const;
    
    // Enable/disable UI interaction
    void SetInteractionEnabled(bool enabled) { mInteractionEnabled = enabled; }
    
private:
    bool mInitialized;
    ImGuiContext* mContext;
    bool mInteractionEnabled;
};