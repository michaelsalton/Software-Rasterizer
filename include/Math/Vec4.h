#pragma once

#include <cmath>
#include <iostream>
#include "Math/Vec3.h"

class Vec4
{
public:
    float x, y, z, w;

    // Constructors
    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4(float value) : x(value), y(value), z(value), w(value) {}
    Vec4(const Vec3& v3, float w) : x(v3.x), y(v3.y), z(v3.z), w(w) {}

    // Basic arithmetic operators
    Vec4 operator+(const Vec4& other) const {
        return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Vec4 operator-(const Vec4& other) const {
        return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Vec4 operator*(float scalar) const {
        return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Vec4 operator/(float scalar) const {
        return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    // Component-wise multiplication
    Vec4 operator*(const Vec4& other) const {
        return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    // Compound assignment operators
    Vec4& operator+=(const Vec4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Vec4& operator-=(const Vec4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    Vec4& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    Vec4& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    // Unary minus
    Vec4 operator-() const {
        return Vec4(-x, -y, -z, -w);
    }

    // Equality operators
    bool operator==(const Vec4& other) const {
        return (x == other.x && y == other.y && z == other.z && w == other.w);
    }

    bool operator!=(const Vec4& other) const {
        return !(*this == other);
    }

    // Dot product
    float dot(const Vec4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    // Length/Magnitude
    float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    float lengthSquared() const {
        return x * x + y * y + z * z + w * w;
    }

    // Normalization
    Vec4 normalized() const {
        float len = length();
        if (len > 0.0f) {
            return Vec4(x / len, y / len, z / len, w / len);
        }
        return Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    void normalize() {
        float len = length();
        if (len > 0.0f) {
            x /= len;
            y /= len;
            z /= len;
            w /= len;
        }
    }

    // Convert to Vec3 (for homogeneous coordinates)
    Vec3 toVec3() const {
        if (w != 0.0f) {
            return Vec3(x / w, y / w, z / w);
        }
        return Vec3(x, y, z);
    }

    // Get XYZ components as Vec3
    Vec3 xyz() const {
        return Vec3(x, y, z);
    }

    // Linear interpolation
    static Vec4 lerp(const Vec4& a, const Vec4& b, float t) {
        return a + (b - a) * t;
    }

    // Array access
    float& operator[](int index) {
        switch(index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
        }
    }

    const float& operator[](int index) const {
        switch(index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
        }
    }
};

// Global operators
inline Vec4 operator*(float scalar, const Vec4& vec) {
    return vec * scalar;
}

// Stream output for debugging
inline std::ostream& operator<<(std::ostream& os, const Vec4& vec) {
    os << "Vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    return os;
}