#pragma once

#include <cstring>
#include <cmath>
#include <iostream>
#include "Vec3.h"
#include "Vec4.h"

// 4x4 Matrix class for 3D transformations
// Using column-major order (OpenGL convention)
class Mat4 {
public:
    float m[16]; // Column-major storage

    // Constructors
    Mat4() { setIdentity(); }
    
    Mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33) {
        m[0] = m00;  m[4] = m01;  m[8]  = m02;  m[12] = m03;
        m[1] = m10;  m[5] = m11;  m[9]  = m12;  this->m[13] = m13;
        m[2] = m20;  m[6] = m21;  m[10] = m22;  m[14] = m23;
        m[3] = m30;  m[7] = m31;  m[11] = m32;  m[15] = m33;
    }

    // Set to identity matrix
    void setIdentity() {
        std::memset(m, 0, sizeof(m));
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    // Element access (row, col)
    float& at(int row, int col) {
        return m[col * 4 + row];
    }

    const float& at(int row, int col) const {
        return m[col * 4 + row];
    }

    // Matrix multiplication
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                float sum = 0.0f;
                for (int k = 0; k < 4; k++) {
                    sum += at(i, k) * other.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }

    // Transform a Vec4
    Vec4 operator*(const Vec4& v) const {
        return Vec4(
            m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w,
            m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w,
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
            m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
        );
    }

    // Transform a Vec3 (assumes w = 1)
    Vec3 transformPoint(const Vec3& v) const {
        Vec4 v4(v, 1.0f);
        Vec4 result = *this * v4;
        return result.toVec3();
    }

    // Transform a Vec3 direction (assumes w = 0)
    Vec3 transformDirection(const Vec3& v) const {
        return Vec3(
            m[0] * v.x + m[4] * v.y + m[8]  * v.z,
            m[1] * v.x + m[5] * v.y + m[9]  * v.z,
            m[2] * v.x + m[6] * v.y + m[10] * v.z
        );
    }

    // Transpose
    Mat4 transpose() const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.at(i, j) = at(j, i);
            }
        }
        return result;
    }

    // Calculate determinant
    float determinant() const {
        float det = 0.0f;
        
        // Using cofactor expansion along first row
        det += m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) -
                       m[9] * (m[6] * m[15] - m[7] * m[14]) +
                       m[13] * (m[6] * m[11] - m[7] * m[10]));
        
        det -= m[4] * (m[1] * (m[10] * m[15] - m[11] * m[14]) -
                       m[9] * (m[2] * m[15] - m[3] * m[14]) +
                       m[13] * (m[2] * m[11] - m[3] * m[10]));
        
        det += m[8] * (m[1] * (m[6] * m[15] - m[7] * m[14]) -
                       m[5] * (m[2] * m[15] - m[3] * m[14]) +
                       m[13] * (m[2] * m[7] - m[3] * m[6]));
        
        det -= m[12] * (m[1] * (m[6] * m[11] - m[7] * m[10]) -
                        m[5] * (m[2] * m[11] - m[3] * m[10]) +
                        m[9] * (m[2] * m[7] - m[3] * m[6]));
        
        return det;
    }

    // Calculate inverse matrix
    Mat4 inverse() const {
        float det = determinant();
        
        // Check if matrix is invertible
        if (std::abs(det) < 1e-6f) {
            // Return identity matrix if not invertible
            return Mat4::identity();
        }
        
        Mat4 result;
        float invDet = 1.0f / det;
        
        // Calculate cofactor matrix and transpose
        result.m[0] = invDet * (m[5] * (m[10] * m[15] - m[11] * m[14]) -
                                m[9] * (m[6] * m[15] - m[7] * m[14]) +
                                m[13] * (m[6] * m[11] - m[7] * m[10]));
        
        result.m[1] = invDet * -(m[1] * (m[10] * m[15] - m[11] * m[14]) -
                                 m[9] * (m[2] * m[15] - m[3] * m[14]) +
                                 m[13] * (m[2] * m[11] - m[3] * m[10]));
        
        result.m[2] = invDet * (m[1] * (m[6] * m[15] - m[7] * m[14]) -
                                m[5] * (m[2] * m[15] - m[3] * m[14]) +
                                m[13] * (m[2] * m[7] - m[3] * m[6]));
        
        result.m[3] = invDet * -(m[1] * (m[6] * m[11] - m[7] * m[10]) -
                                 m[5] * (m[2] * m[11] - m[3] * m[10]) +
                                 m[9] * (m[2] * m[7] - m[3] * m[6]));
        
        result.m[4] = invDet * -(m[4] * (m[10] * m[15] - m[11] * m[14]) -
                                 m[8] * (m[6] * m[15] - m[7] * m[14]) +
                                 m[12] * (m[6] * m[11] - m[7] * m[10]));
        
        result.m[5] = invDet * (m[0] * (m[10] * m[15] - m[11] * m[14]) -
                                m[8] * (m[2] * m[15] - m[3] * m[14]) +
                                m[12] * (m[2] * m[11] - m[3] * m[10]));
        
        result.m[6] = invDet * -(m[0] * (m[6] * m[15] - m[7] * m[14]) -
                                 m[4] * (m[2] * m[15] - m[3] * m[14]) +
                                 m[12] * (m[2] * m[7] - m[3] * m[6]));
        
        result.m[7] = invDet * (m[0] * (m[6] * m[11] - m[7] * m[10]) -
                                m[4] * (m[2] * m[11] - m[3] * m[10]) +
                                m[8] * (m[2] * m[7] - m[3] * m[6]));
        
        result.m[8] = invDet * (m[4] * (m[9] * m[15] - m[11] * m[13]) -
                                m[8] * (m[5] * m[15] - m[7] * m[13]) +
                                m[12] * (m[5] * m[11] - m[7] * m[9]));
        
        result.m[9] = invDet * -(m[0] * (m[9] * m[15] - m[11] * m[13]) -
                                 m[8] * (m[1] * m[15] - m[3] * m[13]) +
                                 m[12] * (m[1] * m[11] - m[3] * m[9]));
        
        result.m[10] = invDet * (m[0] * (m[5] * m[15] - m[7] * m[13]) -
                                 m[4] * (m[1] * m[15] - m[3] * m[13]) +
                                 m[12] * (m[1] * m[7] - m[3] * m[5]));
        
        result.m[11] = invDet * -(m[0] * (m[5] * m[11] - m[7] * m[9]) -
                                  m[4] * (m[1] * m[11] - m[3] * m[9]) +
                                  m[8] * (m[1] * m[7] - m[3] * m[5]));
        
        result.m[12] = invDet * -(m[4] * (m[9] * m[14] - m[10] * m[13]) -
                                  m[8] * (m[5] * m[14] - m[6] * m[13]) +
                                  m[12] * (m[5] * m[10] - m[6] * m[9]));
        
        result.m[13] = invDet * (m[0] * (m[9] * m[14] - m[10] * m[13]) -
                                 m[8] * (m[1] * m[14] - m[2] * m[13]) +
                                 m[12] * (m[1] * m[10] - m[2] * m[9]));
        
        result.m[14] = invDet * -(m[0] * (m[5] * m[14] - m[6] * m[13]) -
                                  m[4] * (m[1] * m[14] - m[2] * m[13]) +
                                  m[12] * (m[1] * m[6] - m[2] * m[5]));
        
        result.m[15] = invDet * (m[0] * (m[5] * m[10] - m[6] * m[9]) -
                                 m[4] * (m[1] * m[10] - m[2] * m[9]) +
                                 m[8] * (m[1] * m[6] - m[2] * m[5]));
        
        return result;
    }

    // Static factory methods for common transformations
    static Mat4 identity() {
        return Mat4();
    }

    static Mat4 translation(float x, float y, float z) {
        Mat4 result;
        result.m[12] = x;
        result.m[13] = y;
        result.m[14] = z;
        return result;
    }

    static Mat4 translation(const Vec3& v) {
        return translation(v.x, v.y, v.z);
    }

    static Mat4 scale(float x, float y, float z) {
        Mat4 result;
        result.m[0] = x;
        result.m[5] = y;
        result.m[10] = z;
        return result;
    }

    static Mat4 scale(float s) {
        return scale(s, s, s);
    }

    static Mat4 scale(const Vec3& v) {
        return scale(v.x, v.y, v.z);
    }

    static Mat4 rotationX(float angleRad) {
        Mat4 result;
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        result.m[5] = c;
        result.m[6] = s;
        result.m[9] = -s;
        result.m[10] = c;
        return result;
    }

    static Mat4 rotationY(float angleRad) {
        Mat4 result;
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        result.m[0] = c;
        result.m[2] = -s;
        result.m[8] = s;
        result.m[10] = c;
        return result;
    }

    static Mat4 rotationZ(float angleRad) {
        Mat4 result;
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        result.m[0] = c;
        result.m[1] = s;
        result.m[4] = -s;
        result.m[5] = c;
        return result;
    }

    // Rotation around arbitrary axis
    static Mat4 rotation(const Vec3& axis, float angleRad) {
        Vec3 a = axis.normalized();
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        float t = 1.0f - c;

        return Mat4(
            t * a.x * a.x + c,       t * a.x * a.y - s * a.z, t * a.x * a.z + s * a.y, 0,
            t * a.x * a.y + s * a.z, t * a.y * a.y + c,       t * a.y * a.z - s * a.x, 0,
            t * a.x * a.z - s * a.y, t * a.y * a.z + s * a.x, t * a.z * a.z + c,       0,
            0,                       0,                       0,                       1
        );
    }

    // Look-at matrix (view matrix)
    static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 f = (target - eye).normalized();  // Forward
        Vec3 r = f.cross(up).normalized();     // Right
        Vec3 u = r.cross(f);                    // Up

        Mat4 result;
        result.m[0] = r.x;
        result.m[4] = r.y;
        result.m[8] = r.z;
        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[12] = -r.dot(eye);
        result.m[13] = -u.dot(eye);
        result.m[14] = f.dot(eye);
        return result;
    }

    // Perspective projection matrix
    static Mat4 perspective(float fovRad, float aspect, float near, float far) {
        float f = 1.0f / std::tan(fovRad * 0.5f);
        Mat4 result;
        std::memset(result.m, 0, sizeof(result.m));
        
        result.m[0] = f / aspect;
        result.m[5] = f;
        result.m[10] = (far + near) / (near - far);
        result.m[11] = -1.0f;
        result.m[14] = (2.0f * far * near) / (near - far);
        
        return result;
    }

    // Orthographic projection matrix
    static Mat4 orthographic(float left, float right, float bottom, float top, float near, float far) {
        Mat4 result;
        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = -2.0f / (far - near);
        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -(far + near) / (far - near);
        return result;
    }

    // Euler angles from rotation matrix (ZYX order)
    Vec3 toEulerAngles() const {
        Vec3 angles;
        
        // Check for gimbal lock
        float sy = std::sqrt(m[0] * m[0] + m[4] * m[4]);
        
        bool singular = sy < 1e-6f;
        
        if (!singular) {
            angles.x = std::atan2(m[9], m[10]);  // Roll
            angles.y = std::atan2(-m[8], sy);    // Pitch
            angles.z = std::atan2(m[4], m[0]);   // Yaw
        } else {
            angles.x = std::atan2(-m[6], m[5]);
            angles.y = std::atan2(-m[8], sy);
            angles.z = 0;
        }
        
        return angles;
    }

    // Create rotation matrix from Euler angles (ZYX order)
    static Mat4 fromEulerAngles(float x, float y, float z) {
        return rotationZ(z) * rotationY(y) * rotationX(x);
    }

    static Mat4 fromEulerAngles(const Vec3& angles) {
        return fromEulerAngles(angles.x, angles.y, angles.z);
    }

    // Extract scale from transformation matrix
    Vec3 extractScale() const {
        Vec3 scale;
        scale.x = Vec3(m[0], m[1], m[2]).length();
        scale.y = Vec3(m[4], m[5], m[6]).length();
        scale.z = Vec3(m[8], m[9], m[10]).length();
        return scale;
    }

    // Extract translation from transformation matrix
    Vec3 extractTranslation() const {
        return Vec3(m[12], m[13], m[14]);
    }

    // Decompose transformation matrix into translation, rotation, and scale
    void decompose(Vec3& translation, Mat4& rotation, Vec3& scale) const {
        // Extract translation
        translation = extractTranslation();
        
        // Extract scale
        scale = extractScale();
        
        // Extract rotation by removing scale from the matrix
        rotation = *this;
        rotation.m[12] = rotation.m[13] = rotation.m[14] = 0.0f;
        
        if (scale.x != 0) {
            rotation.m[0] /= scale.x;
            rotation.m[1] /= scale.x;
            rotation.m[2] /= scale.x;
        }
        
        if (scale.y != 0) {
            rotation.m[4] /= scale.y;
            rotation.m[5] /= scale.y;
            rotation.m[6] /= scale.y;
        }
        
        if (scale.z != 0) {
            rotation.m[8] /= scale.z;
            rotation.m[9] /= scale.z;
            rotation.m[10] /= scale.z;
        }
    }

    // Utility methods
    bool isIdentity(float epsilon = 1e-5f) const {
        return std::abs(m[0] - 1.0f) < epsilon && std::abs(m[5] - 1.0f) < epsilon &&
               std::abs(m[10] - 1.0f) < epsilon && std::abs(m[15] - 1.0f) < epsilon &&
               std::abs(m[1]) < epsilon && std::abs(m[2]) < epsilon && std::abs(m[3]) < epsilon &&
               std::abs(m[4]) < epsilon && std::abs(m[6]) < epsilon && std::abs(m[7]) < epsilon &&
               std::abs(m[8]) < epsilon && std::abs(m[9]) < epsilon && std::abs(m[11]) < epsilon &&
               std::abs(m[12]) < epsilon && std::abs(m[13]) < epsilon && std::abs(m[14]) < epsilon;
    }

    bool isOrthogonal() const {
        Mat4 mT = transpose();
        Mat4 shouldBeIdentity = *this * mT;
        return shouldBeIdentity.isIdentity();
    }

    // Linear interpolation between two matrices
    static Mat4 lerp(const Mat4& a, const Mat4& b, float t) {
        Mat4 result;
        for (int i = 0; i < 16; i++) {
            result.m[i] = a.m[i] + (b.m[i] - a.m[i]) * t;
        }
        return result;
    }

    // Operators
    Mat4& operator*=(const Mat4& other) {
        *this = *this * other;
        return *this;
    }

    bool operator==(const Mat4& other) const {
        for (int i = 0; i < 16; i++) {
            if (std::abs(m[i] - other.m[i]) > 1e-6f) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Mat4& other) const {
        return !(*this == other);
    }
};

// Stream output for debugging
inline std::ostream& operator<<(std::ostream& os, const Mat4& mat) {
    os << "Mat4(\n";
    for (int i = 0; i < 4; i++) {
        os << "  ";
        for (int j = 0; j < 4; j++) {
            os << mat.at(i, j) << " ";
        }
        os << "\n";
    }
    os << ")";
    return os;
}