#pragma once

#include "Math/Vec3.h"
#include "Math/Mat4.h"
#include "Math/Math.h"
#include "Core/Transform.h"

class Camera {
public:
    enum ProjectionType {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };
    
private:
    Transform transform;
    
    // Projection parameters
    ProjectionType projectionType;
    float fov;           // Field of view in degrees (for perspective)
    float nearPlane;
    float farPlane;
    float aspectRatio;
    
    // Orthographic parameters
    float orthoSize;     // Half-height of orthographic view
    
    // Cached matrices
    mutable Mat4 viewMatrix;
    mutable Mat4 projectionMatrix;
    mutable Mat4 viewProjectionMatrix;
    mutable bool viewDirty;
    mutable bool projectionDirty;
    
public:
    Camera();
    
    // Transform access
    Transform& getTransform();
    const Transform& getTransform() const;
    
    // Quick position/rotation access
    void setPosition(const Vec3& pos);
    void setPosition(float x, float y, float z);
    Vec3 getPosition() const;
    void lookAt(const Vec3& target, const Vec3& up = Vec3(0, 1, 0));
    
    // Projection settings
    void setPerspective(float fovDegrees, float aspect, float near, float far);
    void setOrthographic(float size, float aspect, float near, float far);
    void setAspectRatio(float aspect);
    void setFieldOfView(float fovDegrees);
    
    float getFieldOfView() const;
    float getAspectRatio() const;
    float getNearPlane() const;
    float getFarPlane() const;
    
    // Get view matrix (world to camera transformation)
    const Mat4& getViewMatrix() const;
    
    // Get projection matrix (camera to clip space)
    const Mat4& getProjectionMatrix() const;
    
    // Get combined view-projection matrix
    const Mat4& getViewProjectionMatrix() const;
    
    // Transform point from world space to view space
    Vec3 worldToView(const Vec3& worldPoint) const;
    
    // Transform point from view space to world space
    Vec3 viewToWorld(const Vec3& viewPoint) const;
    
    // Get camera vectors in world space
    Vec3 getForward() const;
    Vec3 getRight() const;
    Vec3 getUp() const;
    
    // Ray from camera through normalized device coordinates
    Vec3 getRayDirection(float ndcX, float ndcY) const;
    
private:
    void updateViewMatrix() const;
    void updateProjectionMatrix() const;
};