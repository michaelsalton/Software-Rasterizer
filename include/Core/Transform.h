#pragma once

#include "Math/vec3.h"
#include "Math/mat4.h"
#include "Math/math.h"

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
    Transform();
    
    // Position
    void setPosition(const Vec3& pos);
    void setPosition(float x, float y, float z);
    const Vec3& getPosition() const;
    void translate(const Vec3& delta);
    void translate(float x, float y, float z);
    
    // Rotation (Euler angles)
    void setRotation(const Vec3& rot);
    void setRotation(float x, float y, float z);
    const Vec3& getRotation() const;
    void rotate(const Vec3& delta);
    void rotate(float x, float y, float z);
    
    // Scale
    void setScale(const Vec3& s);
    void setScale(float s);
    void setScale(float x, float y, float z);
    const Vec3& getScale() const;
    
    // Get the model matrix (object to world transformation)
    const Mat4& getModelMatrix() const;
    
    // Get world transformation (includes parent transforms)
    Mat4 getWorldMatrix() const;
    
    // Get world position (considering parent transforms)
    Vec3 getWorldPosition() const;
    
    // Transform a point from local space to world space
    Vec3 transformPoint(const Vec3& localPoint) const;
    
    // Transform a direction from local space to world space (no translation)
    Vec3 transformDirection(const Vec3& localDir) const;
    
    // Get forward, right, up vectors in world space
    Vec3 getForward() const;
    Vec3 getRight() const;
    Vec3 getUp() const;
    
    // Look at target
    void lookAt(const Vec3& target, const Vec3& up = Vec3(0, 1, 0));
    
    // Parent-child hierarchy
    void setParent(Transform* p);
    Transform* getParent() const;
    
    // Reset to identity
    void reset();
    
private:
    void updateModelMatrix() const;
};