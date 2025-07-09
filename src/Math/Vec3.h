#pragma once

#include <cmath>
#include <iostream>

class Vec3
{
public:
    float x, y, z;

    // Constructors
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3(float value) : x(value), y(value), z(value) {}

    // Basic arithmetic operators
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    // Component-wise multiplication
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    // Compound assignment operators
    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Unary minus
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    // Equality operators
    bool operator==(const Vec3& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    bool operator!=(const Vec3& other) const {
        return !(*this == other);
    }

    // Dot product
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Length/Magnitude
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float lengthSquared() const {
        return x * x + y * y + z * z;
    }

    // Normalization
    Vec3 normalized() const {
        float len = length();
        if (len > 0.0f) {
            return Vec3(x / len, y / len, z / len);
        }
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    void normalize() {
        float len = length();
        if (len > 0.0f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }

    // Distance between two points
    static float distance(const Vec3& a, const Vec3& b) {
        return (b - a).length();
    }

    static float distanceSquared(const Vec3& a, const Vec3& b) {
        return (b - a).lengthSquared();
    }

    // Angle between two vectors (in radians)
    float angle(const Vec3& other) const {
        float d = dot(other);
        float lens = length() * other.length();
        if (lens > 0.0f) {
            return std::acos(d / lens);
        }
        return 0.0f;
    }

    // Reflection
    Vec3 reflect(const Vec3& normal) const {
        return *this - normal * (2.0f * dot(normal));
    }

    // Linear interpolation
    static Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
        return a + (b - a) * t;
    }

    // Array access
    float& operator[](int index) {
        if (index == 0) return x;
        else if (index == 1) return y;
        else return z;
    }

    const float& operator[](int index) const {
        if (index == 0) return x;
        else if (index == 1) return y;
        else return z;
    }

};

// Global operators
inline Vec3 operator*(float scalar, const Vec3& vec) {
    return vec * scalar;
}

// Stream output for debugging
inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
    os << "Vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}