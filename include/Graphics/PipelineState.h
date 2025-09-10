#pragma once

#include <cstdint>
#include "Graphics/PrimitiveAssembler.h"  // For CullMode and WindingOrder

// Fill mode for rasterization
enum class FillMode {
    SOLID,      // Fill triangles with solid color
    WIREFRAME,  // Draw only triangle edges
    POINT       // Draw only vertices
};

// Comparison functions for depth/stencil testing
enum class ComparisonFunc {
    NEVER,          // Test never passes
    LESS,           // Pass if source < dest
    EQUAL,          // Pass if source == dest
    LESS_EQUAL,     // Pass if source <= dest
    GREATER,        // Pass if source > dest
    NOT_EQUAL,      // Pass if source != dest
    GREATER_EQUAL,  // Pass if source >= dest
    ALWAYS          // Test always passes
};

// Stencil operations
enum class StencilOp {
    KEEP,       // Keep current value
    ZERO,       // Set to 0
    REPLACE,    // Replace with reference value
    INCREMENT,  // Increment (clamp to max)
    DECREMENT,  // Decrement (clamp to 0)
    INVERT,     // Bitwise invert
    INCREMENT_WRAP, // Increment with wrap
    DECREMENT_WRAP  // Decrement with wrap
};

// Blend factors for color blending
enum class BlendFactor {
    ZERO,               // 0
    ONE,                // 1
    SRC_COLOR,          // Source color
    ONE_MINUS_SRC_COLOR,// 1 - source color
    DST_COLOR,          // Destination color
    ONE_MINUS_DST_COLOR,// 1 - destination color
    SRC_ALPHA,          // Source alpha
    ONE_MINUS_SRC_ALPHA,// 1 - source alpha
    DST_ALPHA,          // Destination alpha
    ONE_MINUS_DST_ALPHA // 1 - destination alpha
};

// Blend operations
enum class BlendOp {
    ADD,            // Result = source * srcFactor + dest * dstFactor
    SUBTRACT,       // Result = source * srcFactor - dest * dstFactor
    REVERSE_SUBTRACT,// Result = dest * dstFactor - source * srcFactor
    MIN,            // Result = min(source, dest)
    MAX             // Result = max(source, dest)
};

// Rasterizer state configuration
struct RasterizerState {
    FillMode fillMode = FillMode::SOLID;
    CullMode cullMode = CullMode::BACK;
    WindingOrder frontFace = WindingOrder::CCW;
    
    bool scissorEnable = false;
    bool depthClamp = false;  // Clamp depth to [0,1] instead of clipping
    
    float depthBias = 0.0f;           // Constant depth bias
    float slopeScaledDepthBias = 0.0f;// Slope-dependent depth bias
    float depthBiasClamp = 0.0f;      // Maximum depth bias
    
    bool antialiasedLineEnable = false;
    bool multisampleEnable = false;
    
    // Scissor rectangle (if enabled)
    struct ScissorRect {
        int left = 0;
        int top = 0;
        int right = 0;
        int bottom = 0;
    } scissorRect;
    
    // Constructor with defaults
    RasterizerState() = default;
};

// Depth-stencil state configuration
struct DepthStencilState {
    // Depth testing
    bool depthEnable = true;
    bool depthWriteEnable = true;
    ComparisonFunc depthFunc = ComparisonFunc::LESS;
    
    // Stencil testing
    bool stencilEnable = false;
    uint8_t stencilReadMask = 0xFF;
    uint8_t stencilWriteMask = 0xFF;
    
    // Front face stencil operations
    struct StencilOpDesc {
        StencilOp stencilFailOp = StencilOp::KEEP;      // Op when stencil test fails
        StencilOp stencilDepthFailOp = StencilOp::KEEP; // Op when stencil passes but depth fails
        StencilOp stencilPassOp = StencilOp::KEEP;      // Op when both pass
        ComparisonFunc stencilFunc = ComparisonFunc::ALWAYS;
    };
    
    StencilOpDesc frontFace;
    StencilOpDesc backFace;
    
    // Constructor with defaults
    DepthStencilState() = default;
};

// Blend state configuration
struct BlendState {
    bool blendEnable = false;
    
    // Color blending
    BlendFactor srcBlend = BlendFactor::ONE;
    BlendFactor dstBlend = BlendFactor::ZERO;
    BlendOp blendOp = BlendOp::ADD;
    
    // Alpha blending
    BlendFactor srcBlendAlpha = BlendFactor::ONE;
    BlendFactor dstBlendAlpha = BlendFactor::ZERO;
    BlendOp blendOpAlpha = BlendOp::ADD;
    
    // Color write mask
    struct ColorWriteMask {
        bool red = true;
        bool green = true;
        bool blue = true;
        bool alpha = true;
    } colorWriteMask;
    
    // Constructor with defaults
    BlendState() = default;
    
    // Common presets
    static BlendState AlphaBlend() {
        BlendState state;
        state.blendEnable = true;
        state.srcBlend = BlendFactor::SRC_ALPHA;
        state.dstBlend = BlendFactor::ONE_MINUS_SRC_ALPHA;
        return state;
    }
    
    static BlendState Additive() {
        BlendState state;
        state.blendEnable = true;
        state.srcBlend = BlendFactor::ONE;
        state.dstBlend = BlendFactor::ONE;
        return state;
    }
    
    static BlendState Multiply() {
        BlendState state;
        state.blendEnable = true;
        state.srcBlend = BlendFactor::DST_COLOR;
        state.dstBlend = BlendFactor::ZERO;
        return state;
    }
};

// Complete pipeline state
struct PipelineState {
    RasterizerState rasterizer;
    DepthStencilState depthStencil;
    BlendState blend;
    
    // Constructor with defaults
    PipelineState() = default;
    
    // Factory methods for common configurations
    static PipelineState Default() {
        return PipelineState();
    }
    
    static PipelineState Wireframe() {
        PipelineState state;
        state.rasterizer.fillMode = FillMode::WIREFRAME;
        return state;
    }
    
    static PipelineState NoCulling() {
        PipelineState state;
        state.rasterizer.cullMode = CullMode::NONE;
        return state;
    }
    
    static PipelineState NoDepth() {
        PipelineState state;
        state.depthStencil.depthEnable = false;
        state.depthStencil.depthWriteEnable = false;
        return state;
    }
};