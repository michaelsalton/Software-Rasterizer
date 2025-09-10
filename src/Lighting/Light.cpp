#include "Lighting/Light.h"

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