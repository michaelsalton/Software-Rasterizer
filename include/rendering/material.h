#pragma once

#include "math/vec3.h"

struct Material {
    // Base color properties
    Vec3 albedo;          // Base color/diffuse (0-1 range)
    Vec3 specular;        // Specular color (0-1 range)
    Vec3 emissive;        // Self-illumination (0-1 range)
    
    // Surface properties
    float shininess;      // Specular exponent for Phong (1-256)
    
    // Texture indices (-1 if not used)
    int albedoTexture;
    int normalTexture;
    int specularTexture;
    int emissiveTexture;
    
    // Shading model
    enum ShadingModel {
        PHONG,
        BLINN_PHONG,
        LAMBERT,     // Diffuse only
        UNLIT        // No lighting
    } shadingModel;
    
    // Default constructor - white diffuse material
    Material() 
        : albedo(1, 1, 1),
          specular(0.3f, 0.3f, 0.3f),
          emissive(0, 0, 0),
          shininess(32.0f),
          albedoTexture(-1),
          normalTexture(-1),
          specularTexture(-1),
          emissiveTexture(-1),
          shadingModel(PHONG) {}
    
    // Helper factory methods
    static Material Diffuse(const Vec3& color) {
        Material mat;
        mat.albedo = color;
        mat.specular = Vec3(0, 0, 0);
        mat.shadingModel = LAMBERT;
        return mat;
    }
    
    static Material Plastic(const Vec3& color, float shininess = 32.0f) {
        Material mat;
        mat.albedo = color;
        mat.specular = Vec3(0.3f, 0.3f, 0.3f);
        mat.shininess = shininess;
        return mat;
    }
    
    static Material Metal(const Vec3& color, float shininess = 128.0f) {
        Material mat;
        mat.albedo = color * 0.3f; // Darker diffuse for metals
        mat.specular = color;       // Specular matches color
        mat.shininess = shininess;
        return mat;
    }
    
    static Material Unlit(const Vec3& color) {
        Material mat;
        mat.albedo = color;
        mat.shadingModel = UNLIT;
        return mat;
    }
};