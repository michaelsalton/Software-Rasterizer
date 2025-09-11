#pragma once

#include "math/vec3.h"
#include <algorithm>
#include <cmath>

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

// Point light (lamp, candle, etc)
class PointLight : public Light {
private:
    Vec3 position;
    
    // Attenuation parameters: 1 / (constant + linear * d + quadratic * d²)
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    float range;  // Maximum effective range
    
    // Calculate effective range based on attenuation parameters
    float calculateRange() const;
    
public:
    PointLight(const Vec3& position, const Vec3& color = Vec3(1, 1, 1), float intensity = 1.0f,
               float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);
    
    // Point light specific methods
    void setPosition(const Vec3& pos) { position = pos; }
    Vec3 getPosition() const { return position; }
    
    // Set attenuation parameters
    void setAttenuation(float constant, float linear, float quadratic);
    void getAttenuation(float& constant, float& linear, float& quadratic) const {
        constant = attenuationConstant;
        linear = attenuationLinear;
        quadratic = attenuationQuadratic;
    }
    
    float getRange() const { return range; }
    
    // Light interface implementation
    Vec3 getDirectionToLight(const Vec3& worldPos) const override;
    float getAttenuation(const Vec3& worldPos) const override;
    Vec3 getLightContribution(const Vec3& worldPos) const override;
};

// Spot light (flashlight, stage light, etc)
class SpotLight : public Light {
private:
    Vec3 position;
    Vec3 direction;  // Direction the spotlight is pointing
    
    // Attenuation parameters (same as point light)
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    float range;
    
    // Spotlight cone parameters
    float innerConeAngle;  // Inner cone angle in radians (full intensity)
    float outerConeAngle;  // Outer cone angle in radians (falloff to zero)
    float cosInnerCone;    // Precomputed cos of inner angle
    float cosOuterCone;    // Precomputed cos of outer angle
    
    // Calculate effective range based on attenuation
    float calculateRange() const;
    
public:
    SpotLight(const Vec3& position, const Vec3& direction,
              float innerAngleDeg, float outerAngleDeg,
              const Vec3& color = Vec3(1, 1, 1), float intensity = 1.0f,
              float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);
    
    // Spotlight specific methods
    void setPosition(const Vec3& pos) { position = pos; }
    void setDirection(const Vec3& dir);
    void setConeAngles(float innerDeg, float outerDeg);
    
    Vec3 getPosition() const { return position; }
    Vec3 getDirection() const { return direction; }
    float getInnerAngle() const { return innerConeAngle; }
    float getOuterAngle() const { return outerConeAngle; }
    float getRange() const { return range; }
    
    // Set attenuation parameters
    void setAttenuation(float constant, float linear, float quadratic);
    
    // Light interface implementation
    Vec3 getDirectionToLight(const Vec3& worldPos) const override;
    float getAttenuation(const Vec3& worldPos) const override;
    Vec3 getLightContribution(const Vec3& worldPos) const override;
};