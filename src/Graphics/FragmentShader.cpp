#include "Graphics/FragmentShader.h"
#include "Graphics/Texture.h"
#include "Math/Math.h"
#include <algorithm>

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

FragmentOutput LitFragmentShader::Shade(const FragmentInput& input,
                                       const ShaderUniforms& uniforms) {
    FragmentOutput output;
    
    // Get normalized normal
    Vec3 normal = input.worldNormal.normalized();
    
    // Get view direction (from fragment to camera)
    Vec3 viewDir = (uniforms.cameraPosition - input.worldPos).normalized();
    
    // Get base color from vertex color or texture
    Vec3 albedo = material.albedo;
    if (albedoTexture && material.albedoTexture >= 0) {
        Vec4 texColor = albedoTexture->Sample(input.texCoord.x, input.texCoord.y);
        albedo = Vec3(texColor.x, texColor.y, texColor.z);
    } else {
        // Modulate material albedo with vertex color
        albedo = Vec3(
            material.albedo.x * input.color.x,
            material.albedo.y * input.color.y,
            material.albedo.z * input.color.z
        );
    }
    
    // Update material albedo for lighting calculations
    Material lightingMat = material;
    lightingMat.albedo = albedo;
    
    // Calculate lighting based on shading model
    Vec3 finalColor;
    switch (material.shadingModel) {
        case Material::LAMBERT:
            finalColor = LightingCalculations::calculateLambertian(
                normal, lightingMat, lights, ambientLight
            );
            break;
            
        case Material::PHONG:
            finalColor = LightingCalculations::calculatePhong(
                input.worldPos, normal, viewDir, lightingMat, lights, ambientLight
            );
            break;
            
        case Material::BLINN_PHONG:
            finalColor = LightingCalculations::calculateBlinnPhong(
                input.worldPos, normal, viewDir, lightingMat, lights, ambientLight
            );
            break;
            
        case Material::UNLIT:
            finalColor = albedo + material.emissive;
            break;
    }
    
    // Clamp to [0,1] range
    finalColor.x = std::clamp(finalColor.x, 0.0f, 1.0f);
    finalColor.y = std::clamp(finalColor.y, 0.0f, 1.0f);
    finalColor.z = std::clamp(finalColor.z, 0.0f, 1.0f);
    
    output.color = Vec4(finalColor.x, finalColor.y, finalColor.z, input.color.w);
    output.depth = input.depth;
    output.discard = false;
    
    return output;
}

FragmentOutput TexturedLitFragmentShader::Shade(const FragmentInput& input,
                                               const ShaderUniforms& uniforms) {
    // For now, just use the base class implementation
    // Later we can add normal mapping, etc.
    return LitFragmentShader::Shade(input, uniforms);
}