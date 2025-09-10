#pragma once

#include "Math/vec3.h"
#include "Math/math.h"
#include "Lighting/light.h"
#include "Lighting/material.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

class LightingCalculations {
public:
    // Basic Lambertian diffuse lighting
    static Vec3 calculateLambertian(
        const Vec3& worldNormal,
        const Material& material,
        const std::vector<std::shared_ptr<Light>>& lights,
        const Vec3& ambientLight = Vec3(0.1f, 0.1f, 0.1f)
    ) {
        // Start with ambient
        Vec3 result = material.albedo * ambientLight;
        
        // Add contribution from each light
        for (const auto& light : lights) {
            if (!light->isEnabled()) continue;
            
            // Get light direction and contribution
            Vec3 lightDir = light->getDirectionToLight(Vec3(0, 0, 0)); // Position doesn't matter for directional
            float attenuation = light->getAttenuation(Vec3(0, 0, 0));
            Vec3 lightColor = light->getLightContribution(Vec3(0, 0, 0));
            
            // Calculate diffuse (N dot L)
            float NdotL = std::max(0.0f, worldNormal.dot(lightDir));
            
            // Add diffuse contribution
            result += material.albedo * lightColor * NdotL * attenuation;
        }
        
        // Add emissive
        result += material.emissive;
        
        return result;
    }
    
    // Full Phong lighting model
    static Vec3 calculatePhong(
        const Vec3& worldPos,
        const Vec3& worldNormal,
        const Vec3& viewDir,
        const Material& material,
        const std::vector<std::shared_ptr<Light>>& lights,
        const Vec3& ambientLight = Vec3(0.1f, 0.1f, 0.1f)
    ) {
        // Start with ambient
        Vec3 result = material.albedo * ambientLight;
        
        for (const auto& light : lights) {
            if (!light->isEnabled()) continue;
            
            Vec3 lightDir = light->getDirectionToLight(worldPos);
            float attenuation = light->getAttenuation(worldPos);
            Vec3 lightColor = light->getLightContribution(worldPos);
            
            // Diffuse component
            float NdotL = std::max(0.0f, worldNormal.dot(lightDir));
            Vec3 diffuse = material.albedo * NdotL;
            
            // Specular component
            Vec3 specular(0, 0, 0);
            if (NdotL > 0.0f) {
                Vec3 reflectDir = reflect(-lightDir, worldNormal);
                float RdotV = std::max(0.0f, reflectDir.dot(viewDir));
                float specFactor = pow(RdotV, material.shininess);
                specular = material.specular * specFactor;
            }
            
            // Combine
            result += (diffuse + specular) * lightColor * attenuation;
        }
        
        result += material.emissive;
        return result;
    }
    
    // Blinn-Phong lighting model
    static Vec3 calculateBlinnPhong(
        const Vec3& worldPos,
        const Vec3& worldNormal,
        const Vec3& viewDir,
        const Material& material,
        const std::vector<std::shared_ptr<Light>>& lights,
        const Vec3& ambientLight = Vec3(0.1f, 0.1f, 0.1f)
    ) {
        Vec3 result = material.albedo * ambientLight;
        
        for (const auto& light : lights) {
            if (!light->isEnabled()) continue;
            
            Vec3 lightDir = light->getDirectionToLight(worldPos);
            float attenuation = light->getAttenuation(worldPos);
            Vec3 lightColor = light->getLightContribution(worldPos);
            
            // Diffuse
            float NdotL = std::max(0.0f, worldNormal.dot(lightDir));
            Vec3 diffuse = material.albedo * NdotL;
            
            // Specular (Blinn-Phong)
            Vec3 specular(0, 0, 0);
            if (NdotL > 0.0f) {
                Vec3 halfVector = (lightDir + viewDir).normalized();
                float NdotH = std::max(0.0f, worldNormal.dot(halfVector));
                // Blinn-Phong typically uses 4x the Phong shininess
                float specFactor = pow(NdotH, material.shininess * 4.0f);
                specular = material.specular * specFactor;
            }
            
            result += (diffuse + specular) * lightColor * attenuation;
        }
        
        result += material.emissive;
        return result;
    }
    
private:
    static Vec3 reflect(const Vec3& incident, const Vec3& normal) {
        return incident - normal * 2.0f * incident.dot(normal);
    }
};