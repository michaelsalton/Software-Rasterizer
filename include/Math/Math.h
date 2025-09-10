#pragma once

#include <cmath>
#include <algorithm>
#include <limits>

// Include vector types
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

namespace Math {

// =====================================================
// Constants
// =====================================================
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = 0.5f * PI;
constexpr float QUARTER_PI = 0.25f * PI;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;
constexpr float EPSILON = 1e-6f;

// =====================================================
// Basic Math Functions
// =====================================================

// Convert degrees to radians
inline float toRadians(float degrees) {
    return degrees * DEG_TO_RAD;
}

// Convert radians to degrees
inline float toDegrees(float radians) {
    return radians * RAD_TO_DEG;
}

// Clamp value between min and max
template<typename T>
inline T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

// Linear interpolation
template<typename T>
inline T lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}

// Smooth interpolation (ease in/out)
inline float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// Smoother interpolation (Ken Perlin's improved version)
inline float smootherstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// Map value from one range to another
inline float remap(float value, float inMin, float inMax, float outMin, float outMax) {
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

// Check if two floats are approximately equal
inline bool approxEqual(float a, float b, float epsilon = EPSILON) {
    return std::abs(a - b) < epsilon;
}

// Sign function (-1, 0, or 1)
template<typename T>
inline T sign(T value) {
    return (T(0) < value) - (value < T(0));
}

// =====================================================
// Trigonometric Functions
// =====================================================

// Normalized sincos (returns values in range [0, 1])
inline void sincos(float angle, float& s, float& c) {
    s = std::sin(angle);
    c = std::cos(angle);
}

// =====================================================
// 2D Transformations
// =====================================================

// Rotate a 2D point around origin
inline Vec2 rotate(const Vec2& v, float angleRad) {
    float s, c;
    sincos(angleRad, s, c);
    return Vec2(v.x * c - v.y * s, v.x * s + v.y * c);
}

// Rotate a 2D point around a pivot
inline Vec2 rotateAround(const Vec2& v, const Vec2& pivot, float angleRad) {
    Vec2 translated = v - pivot;
    Vec2 rotated = rotate(translated, angleRad);
    return rotated + pivot;
}

// Scale a 2D point from origin
inline Vec2 scale(const Vec2& v, float sx, float sy) {
    return Vec2(v.x * sx, v.y * sy);
}

// Scale a 2D point from a pivot
inline Vec2 scaleFrom(const Vec2& v, const Vec2& pivot, float sx, float sy) {
    Vec2 translated = v - pivot;
    return Vec2(translated.x * sx, translated.y * sy) + pivot;
}

// =====================================================
// 3D Transformations
// =====================================================

// Rotate around X axis
inline Vec3 rotateX(const Vec3& v, float angleRad) {
    float s, c;
    sincos(angleRad, s, c);
    return Vec3(v.x, v.y * c - v.z * s, v.y * s + v.z * c);
}

// Rotate around Y axis
inline Vec3 rotateY(const Vec3& v, float angleRad) {
    float s, c;
    sincos(angleRad, s, c);
    return Vec3(v.x * c + v.z * s, v.y, -v.x * s + v.z * c);
}

// Rotate around Z axis
inline Vec3 rotateZ(const Vec3& v, float angleRad) {
    float s, c;
    sincos(angleRad, s, c);
    return Vec3(v.x * c - v.y * s, v.x * s + v.y * c, v.z);
}

// Rotate around an arbitrary axis (Rodrigues' rotation formula)
inline Vec3 rotateAroundAxis(const Vec3& v, const Vec3& axis, float angleRad) {
    Vec3 k = axis.normalized();
    float s, c;
    sincos(angleRad, s, c);
    float oneMinusC = 1.0f - c;
    
    return v * c + k.cross(v) * s + k * k.dot(v) * oneMinusC;
}

// Euler rotation (apply rotations in order: X, Y, Z)
inline Vec3 rotateEuler(const Vec3& v, float xRad, float yRad, float zRad) {
    Vec3 result = rotateX(v, xRad);
    result = rotateY(result, yRad);
    result = rotateZ(result, zRad);
    return result;
}

// Look-at rotation (align vector to point at target)
inline Vec3 lookAt(const Vec3& from, const Vec3& to, const Vec3& /*up*/ = Vec3(0, 1, 0)) {
    Vec3 forward = (to - from).normalized();
    // Vec3 right = up.cross(forward).normalized(); // For future use with rotation matrix
    // Vec3 newUp = forward.cross(right); // For future use with rotation matrix
    return forward; // Return forward direction, can be extended to return full rotation matrix
}

// =====================================================
// Geometric Functions
// =====================================================

// Reflect vector around normal
inline Vec3 reflect(const Vec3& incident, const Vec3& normal) {
    return incident - normal * (2.0f * incident.dot(normal));
}

// Refract vector through surface
inline Vec3 refract(const Vec3& incident, const Vec3& normal, float eta) {
    float k = 1.0f - eta * eta * (1.0f - normal.dot(incident) * normal.dot(incident));
    if (k < 0.0f) {
        return Vec3(0, 0, 0); // Total internal reflection
    }
    return incident * eta - normal * (eta * normal.dot(incident) + std::sqrt(k));
}

// Project vector onto another vector
inline Vec3 project(const Vec3& v, const Vec3& onto) {
    float scalar = v.dot(onto) / onto.lengthSquared();
    return onto * scalar;
}

// Get perpendicular component of vector
inline Vec3 perpendicular(const Vec3& v, const Vec3& from) {
    return v - project(v, from);
}

// =====================================================
// Random Number Generation
// =====================================================

// Random float between 0 and 1
inline float random01() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// Random float between min and max
inline float random(float min, float max) {
    return min + (max - min) * random01();
}

// Random point in unit circle
inline Vec2 randomInUnitCircle() {
    float angle = random(0, TWO_PI);
    float r = std::sqrt(random01());
    return Vec2(r * std::cos(angle), r * std::sin(angle));
}

// Random point in unit sphere
inline Vec3 randomInUnitSphere() {
    float theta = random(0, TWO_PI);
    float phi = std::acos(1.0f - 2.0f * random01());
    float r = std::cbrt(random01());
    
    float sinPhi = std::sin(phi);
    return Vec3(
        r * sinPhi * std::cos(theta),
        r * sinPhi * std::sin(theta),
        r * std::cos(phi)
    );
}

// Random point on unit sphere surface
inline Vec3 randomOnUnitSphere() {
    float theta = random(0, TWO_PI);
    float phi = std::acos(1.0f - 2.0f * random01());
    
    float sinPhi = std::sin(phi);
    return Vec3(
        sinPhi * std::cos(theta),
        sinPhi * std::sin(theta),
        std::cos(phi)
    );
}

} // namespace Math