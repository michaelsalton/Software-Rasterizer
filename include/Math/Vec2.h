#pragma once

#include <cmath>
#include <iostream>

class Vec2
{
public:
    float x, y;

    // Constructors
    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2(float value) : x(value), y(value) {}

    // Basic arithmetic operators
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    Vec2 operator/(float scalar) const {
        return Vec2(x / scalar, y / scalar);
    }

    // Compound assignment operators
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Unary minus
    Vec2 operator-() const {
        return Vec2(-x, -y);
    }

    // Equality operators
    bool operator==(const Vec2& other) const {
        return (x == other.x && y == other.y);
    }

    bool operator!=(const Vec2& other) const {
        return !(*this == other);
    }

    // Dot product
    float dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }

    // Cross product (returns the z-component of the 3D cross product)
    float cross(const Vec2& other) const {
        return x * other.y - y * other.x;
    }

    // Length/Magnitude
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

    // Normalization
    Vec2 normalized() const {
        float len = length();
        if (len > 0.0f) {
            return Vec2(x / len, y / len);
        }
        return Vec2(0.0f, 0.0f);
    }

    void normalize() {
        float len = length();
        if (len > 0.0f) {
            x /= len;
            y /= len;
        }
    }

    // Distance between two points
    static float distance(const Vec2& a, const Vec2& b) {
        return (b - a).length();
    }

    static float distanceSquared(const Vec2& a, const Vec2& b) {
        return (b - a).lengthSquared();
    }

    // Angle between two vectors (in radians)
    float angle(const Vec2& other) const {
        float d = dot(other);
        float lens = length() * other.length();
        if (lens > 0.0f) {
            return std::acos(d / lens);
        }
        return 0.0f;
    }

    // Perpendicular vector (rotated 90 degrees counter-clockwise)
    Vec2 perpendicular() const {
        return Vec2(-y, x);
    }

    // Linear interpolation
    static Vec2 lerp(const Vec2& a, const Vec2& b, float t) {
        return a + (b - a) * t;
    }

    // Array access
    float& operator[](int index) {
        return (index == 0) ? x : y;
    }

    const float& operator[](int index) const {
        return (index == 0) ? x : y;
    }
};

// Global operators
inline Vec2 operator*(float scalar, const Vec2& vec) {
    return vec * scalar;
}

// Stream output for debugging
inline std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
    os << "Vec2(" << vec.x << ", " << vec.y << ")";
    return os;
}