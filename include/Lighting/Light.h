#pragma once

#include "Math/Vec3.h"

// Base class for all light types
class Light {
public:
    enum LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };

protected:
    LightType type;
    Vec3 color;
    float intensity;
    bool enabled;

public:
    Light(LightType lightType, const Vec3& lightColor, float lightIntensity)
        : type(lightType), color(lightColor), intensity(lightIntensity), enabled(true) {}
    
    virtual ~Light() = default;
    
    // Core light interface
    virtual Vec3 getDirectionToLight(const Vec3& worldPos) const = 0;
    virtual float getAttenuation(const Vec3& worldPos) const = 0;
    virtual Vec3 getLightContribution(const Vec3& worldPos) const = 0;
    
    // Common properties
    void setColor(const Vec3& c) { color = c; }
    void setIntensity(float i) { intensity = i; }
    void setEnabled(bool e) { enabled = e; }
    
    Vec3 getColor() const { return color; }
    float getIntensity() const { return intensity; }
    bool isEnabled() const { return enabled; }
    LightType getType() const { return type; }
};

// Directional light (like the sun)
class DirectionalLight : public Light {
private:
    Vec3 direction;  // Direction TO the light source
    
public:
    DirectionalLight(const Vec3& lightDirection, const Vec3& color = Vec3(1, 1, 1), float intensity = 1.0f);
    
    // Set direction the light is coming FROM (will be negated internally)
    void setDirection(const Vec3& dir);
    Vec3 getDirection() const { return direction; }
    
    // Light interface implementation
    Vec3 getDirectionToLight(const Vec3& worldPos) const override;
    float getAttenuation(const Vec3& worldPos) const override;
    Vec3 getLightContribution(const Vec3& worldPos) const override;
};