#include "Graphics/FragmentShader.h"
#include "Graphics/Texture.h"

FragmentOutput TexturedFragmentShader::Shade(const FragmentInput& input,
                                           const ShaderUniforms& /*uniforms*/) {
    FragmentOutput output;
    
    if (mTexture) {
        // Sample texture at UV coordinates
        Vec4 texColor = mTexture->Sample(input.texCoord.x, input.texCoord.y);
        
        // Modulate with vertex color
        output.color = Vec4(
            texColor.x * input.color.x,
            texColor.y * input.color.y,
            texColor.z * input.color.z,
            texColor.w * input.color.w
        );
    } else {
        // No texture, use vertex color
        output.color = input.color;
    }
    
    output.depth = input.depth;
    output.discard = false;
    
    return output;
}