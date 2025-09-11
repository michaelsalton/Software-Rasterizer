#include "lighting/light.h"
#include "math/math.h"
#include <cmath>

// DirectionalLight implementation
DirectionalLight::DirectionalLight(const Vec3& lightDirection, const Vec3& color, float intensity)
    : Light(DIRECTIONAL, color, intensity) {
    setDirection(lightDirection);
}

void DirectionalLight::setDirection(const Vec3& dir) {
    // Store normalized direction TO the light
    // If dir is the direction the light is shining, we negate it
    direction = -dir.normalized();
}

Vec3 DirectionalLight::getDirectionToLight(const Vec3& worldPos) const {
    // For directional lights, direction is the same everywhere
    return direction;
}

float DirectionalLight::getAttenuation(const Vec3& worldPos) const {
    // No attenuation for directional lights
    return 1.0f;
}

Vec3 DirectionalLight::getLightContribution(const Vec3& worldPos) const {
    // Return color * intensity
    return color * intensity;
}

// PointLight implementation
PointLight::PointLight(const Vec3& pos, const Vec3& col, float intens,
                      float constant, float linear, float quadratic)
    : Light(POINT, col, intens), position(pos),
      attenuationConstant(constant), attenuationLinear(linear), 
      attenuationQuadratic(quadratic) {
    range = calculateRange();
}

void PointLight::setAttenuation(float constant, float linear, float quadratic) {
    attenuationConstant = constant;
    attenuationLinear = linear;
    attenuationQuadratic = quadratic;
    range = calculateRange();
}

float PointLight::calculateRange() const {
    // Calculate effective range where light contribution drops below 1/256
    float threshold = 256.0f / intensity;  // When intensity * atten < 1/256
    
    // Solve quadratic equation: atten = 1/(c + l*d + q*d²) = 1/threshold
    // So: c + l*d + q*d² = threshold
    // Rearranged: q*d² + l*d + (c - threshold) = 0
    float a = attenuationQuadratic;
    float b = attenuationLinear;
    float c = attenuationConstant - threshold;
    
    if (a > 0.0001f) {
        // Quadratic case
        float discriminant = b * b - 4 * a * c;
        if (discriminant > 0) {
            return (-b + std::sqrt(discriminant)) / (2 * a);
        }
    } else if (std::abs(b) > 0.0001f) {
        // Linear case
        return (threshold - attenuationConstant) / attenuationLinear;
    }
    
    // Constant case or fallback
    return 100.0f;  // Default large range
}

Vec3 PointLight::getDirectionToLight(const Vec3& worldPos) const {
    // Direction from surface point to light position
    Vec3 toLight = position - worldPos;
    return toLight.normalized();
}

float PointLight::getAttenuation(const Vec3& worldPos) const {
    // Calculate distance
    float distance = (position - worldPos).length();
    
    // Early out if beyond range
    if (distance > range) {
        return 0.0f;
    }
    
    // Standard attenuation formula
    float attenuation = 1.0f / (attenuationConstant + 
                               attenuationLinear * distance + 
                               attenuationQuadratic * distance * distance);
    
    return std::max(0.0f, attenuation);
}

Vec3 PointLight::getLightContribution(const Vec3& worldPos) const {
    float atten = getAttenuation(worldPos);
    return color * intensity * atten;
}

// ============================================================================
// SpotLight Implementation
// ============================================================================

SpotLight::SpotLight(const Vec3& pos, const Vec3& dir,
                     float innerAngleDeg, float outerAngleDeg,
                     const Vec3& col, float inten,
                     float constant, float linear, float quadratic)
    : Light(SPOT, col, inten)
    , position(pos)
    , direction(dir.normalized())
    , attenuationConstant(constant)
    , attenuationLinear(linear)
    , attenuationQuadratic(quadratic)
{
    setConeAngles(innerAngleDeg, outerAngleDeg);
    range = calculateRange();
}

void SpotLight::setDirection(const Vec3& dir) {
    direction = dir.normalized();
}

void SpotLight::setConeAngles(float innerDeg, float outerDeg) {
    // Ensure outer angle is larger than inner
    innerDeg = std::max(0.0f, std::min(innerDeg, 180.0f));
    outerDeg = std::max(innerDeg, std::min(outerDeg, 180.0f));
    
    // Convert to radians
    innerConeAngle = innerDeg * Math::PI / 180.0f;
    outerConeAngle = outerDeg * Math::PI / 180.0f;
    
    // Precompute cosines for efficiency
    cosInnerCone = std::cos(innerConeAngle);
    cosOuterCone = std::cos(outerConeAngle);
}

void SpotLight::setAttenuation(float constant, float linear, float quadratic) {
    attenuationConstant = std::max(0.001f, constant);
    attenuationLinear = std::max(0.0f, linear);
    attenuationQuadratic = std::max(0.0f, quadratic);
    range = calculateRange();
}

float SpotLight::calculateRange() const {
    // Same calculation as point light
    float threshold = 256.0f / intensity;
    
    float a = attenuationQuadratic;
    float b = attenuationLinear;
    float c = attenuationConstant - threshold;
    
    if (a > 0.0001f) {
        float discriminant = b * b - 4 * a * c;
        if (discriminant > 0) {
            return (-b + std::sqrt(discriminant)) / (2 * a);
        }
    } else if (std::abs(b) > 0.0001f) {
        return (threshold - attenuationConstant) / attenuationLinear;
    }
    
    return 100.0f;
}

Vec3 SpotLight::getDirectionToLight(const Vec3& worldPos) const {
    // Same as point light - direction from surface to light
    Vec3 toLight = position - worldPos;
    return toLight.normalized();
}

float SpotLight::getAttenuation(const Vec3& worldPos) const {
    // First calculate distance attenuation like point light
    float distance = (position - worldPos).length();
    
    if (distance > range) {
        return 0.0f;
    }
    
    float distAtten = 1.0f / (attenuationConstant + 
                             attenuationLinear * distance + 
                             attenuationQuadratic * distance * distance);
    
    // Now calculate cone attenuation
    Vec3 lightToSurface = (worldPos - position).normalized();
    float cosTheta = lightToSurface.dot(direction);
    
    float coneAtten;
    if (cosTheta > cosInnerCone) {
        // Inside inner cone - full intensity
        coneAtten = 1.0f;
    } else if (cosTheta > cosOuterCone) {
        // Between inner and outer - smooth falloff
        float t = (cosTheta - cosOuterCone) / (cosInnerCone - cosOuterCone);
        coneAtten = t * t; // Quadratic falloff for smooth edge
    } else {
        // Outside outer cone - no light
        coneAtten = 0.0f;
    }
    
    return std::max(0.0f, distAtten * coneAtten);
}

Vec3 SpotLight::getLightContribution(const Vec3& worldPos) const {
    float atten = getAttenuation(worldPos);
    return color * intensity * atten;
}