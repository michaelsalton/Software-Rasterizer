#pragma once

#include "../Math/Vec3.h"
#include "../Math/Mat4.h"
#include "../Math/Math.h"

class Transform {
private:
    Vec3 position;
    Vec3 rotation; // Euler angles in radians
    Vec3 scale;
    
    // Cached matrices
    mutable Mat4 modelMatrix;
    mutable bool isDirty;
    
    // Hierarchy
    Transform* parent;
    
public:
    Transform() 
        : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1),
          isDirty(true), parent(nullptr) {}
    
    // Position
    void setPosition(const Vec3& pos) {
        position = pos;
        isDirty = true;
    }
    
    void setPosition(float x, float y, float z) {
        position = Vec3(x, y, z);
        isDirty = true;
    }
    
    const Vec3& getPosition() const { return position; }
    
    void translate(const Vec3& delta) {
        position = position + delta;
        isDirty = true;
    }
    
    void translate(float x, float y, float z) {
        translate(Vec3(x, y, z));
    }
    
    // Rotation (Euler angles)
    void setRotation(const Vec3& rot) {
        rotation = rot;
        isDirty = true;
    }
    
    void setRotation(float x, float y, float z) {
        rotation = Vec3(x, y, z);
        isDirty = true;
    }
    
    const Vec3& getRotation() const { return rotation; }
    
    void rotate(const Vec3& delta) {
        rotation = rotation + delta;
        isDirty = true;
    }
    
    void rotate(float x, float y, float z) {
        rotate(Vec3(x, y, z));
    }
    
    // Scale
    void setScale(const Vec3& s) {
        scale = s;
        isDirty = true;
    }
    
    void setScale(float s) {
        scale = Vec3(s, s, s);
        isDirty = true;
    }
    
    void setScale(float x, float y, float z) {
        scale = Vec3(x, y, z);
        isDirty = true;
    }
    
    const Vec3& getScale() const { return scale; }
    
    // Get the model matrix (object to world transformation)
    const Mat4& getModelMatrix() const {
        if (isDirty) {
            updateModelMatrix();
        }
        return modelMatrix;
    }
    
    // Get world transformation (includes parent transforms)
    Mat4 getWorldMatrix() const {
        if (parent) {
            return parent->getWorldMatrix() * getModelMatrix();
        }
        return getModelMatrix();
    }
    
    // Get world position (considering parent transforms)
    Vec3 getWorldPosition() const {
        if (parent) {
            Mat4 worldMat = getWorldMatrix();
            return Vec3(worldMat.m[12], worldMat.m[13], worldMat.m[14]);
        }
        return position;
    }
    
    // Transform a point from local space to world space
    Vec3 transformPoint(const Vec3& localPoint) const {
        Vec4 point(localPoint, 1.0f);
        Vec4 worldPoint = getWorldMatrix() * point;
        return worldPoint.toVec3();
    }
    
    // Transform a direction from local space to world space (no translation)
    Vec3 transformDirection(const Vec3& localDir) const {
        Vec4 dir(localDir, 0.0f);
        Vec4 worldDir = getWorldMatrix() * dir;
        return worldDir.xyz().normalized();
    }
    
    // Get forward, right, up vectors in world space
    Vec3 getForward() const {
        return transformDirection(Vec3(0, 0, -1)); // -Z is forward
    }
    
    Vec3 getRight() const {
        return transformDirection(Vec3(1, 0, 0));
    }
    
    Vec3 getUp() const {
        return transformDirection(Vec3(0, 1, 0));
    }
    
    // Look at target
    void lookAt(const Vec3& target, const Vec3& up = Vec3(0, 1, 0)) {
        Vec3 pos = getWorldPosition();
        Vec3 forward = (target - pos).normalized();
        Vec3 right = up.cross(forward).normalized();
        Vec3 newUp = forward.cross(right);
        
        // Create rotation matrix
        Mat4 rotMat;
        rotMat.m[0] = right.x;    rotMat.m[4] = right.y;    rotMat.m[8] = right.z;
        rotMat.m[1] = newUp.x;    rotMat.m[5] = newUp.y;    rotMat.m[9] = newUp.z;
        rotMat.m[2] = -forward.x; rotMat.m[6] = -forward.y; rotMat.m[10] = -forward.z;
        
        // Extract Euler angles
        rotation = rotMat.toEulerAngles();
        isDirty = true;
    }
    
    // Parent-child hierarchy
    void setParent(Transform* p) {
        parent = p;
        isDirty = true;
    }
    
    Transform* getParent() const { return parent; }
    
    // Reset to identity
    void reset() {
        position = Vec3(0, 0, 0);
        rotation = Vec3(0, 0, 0);
        scale = Vec3(1, 1, 1);
        isDirty = true;
    }
    
private:
    void updateModelMatrix() const {
        // Build transformation matrix: T * R * S
        Mat4 T = Mat4::translation(position);
        Mat4 R = Mat4::fromEulerAngles(rotation);
        Mat4 S = Mat4::scale(scale);
        
        modelMatrix = T * R * S;
        isDirty = false;
    }
};