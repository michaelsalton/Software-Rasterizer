#pragma once

#include "../Math/Vec3.h"
#include "../Math/Mat4.h"
#include "../Math/Math.h"
#include "Transform.h"

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
    Camera() 
        : projectionType(PERSPECTIVE),
          fov(60.0f),
          nearPlane(0.1f),
          farPlane(100.0f),
          aspectRatio(1.0f),
          orthoSize(5.0f),
          viewDirty(true),
          projectionDirty(true) {
        // Default camera position
        transform.setPosition(0, 0, 5);
    }
    
    // Transform access
    Transform& getTransform() { 
        viewDirty = true;
        return transform; 
    }
    
    const Transform& getTransform() const { 
        return transform; 
    }
    
    // Quick position/rotation access
    void setPosition(const Vec3& pos) {
        transform.setPosition(pos);
        viewDirty = true;
    }
    
    void setPosition(float x, float y, float z) {
        setPosition(Vec3(x, y, z));
    }
    
    Vec3 getPosition() const {
        return transform.getPosition();
    }
    
    void lookAt(const Vec3& target, const Vec3& up = Vec3(0, 1, 0)) {
        transform.lookAt(target, up);
        viewDirty = true;
    }
    
    // Projection settings
    void setPerspective(float fovDegrees, float aspect, float near, float far) {
        projectionType = PERSPECTIVE;
        fov = fovDegrees;
        aspectRatio = aspect;
        nearPlane = near;
        farPlane = far;
        projectionDirty = true;
    }
    
    void setOrthographic(float size, float aspect, float near, float far) {
        projectionType = ORTHOGRAPHIC;
        orthoSize = size;
        aspectRatio = aspect;
        nearPlane = near;
        farPlane = far;
        projectionDirty = true;
    }
    
    void setAspectRatio(float aspect) {
        aspectRatio = aspect;
        projectionDirty = true;
    }
    
    void setFieldOfView(float fovDegrees) {
        fov = fovDegrees;
        projectionDirty = true;
    }
    
    float getFieldOfView() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    
    // Get view matrix (world to camera transformation)
    const Mat4& getViewMatrix() const {
        if (viewDirty) {
            updateViewMatrix();
        }
        return viewMatrix;
    }
    
    // Get projection matrix (camera to clip space)
    const Mat4& getProjectionMatrix() const {
        if (projectionDirty) {
            updateProjectionMatrix();
        }
        return projectionMatrix;
    }
    
    // Get combined view-projection matrix
    const Mat4& getViewProjectionMatrix() const {
        if (viewDirty || projectionDirty) {
            viewProjectionMatrix = getProjectionMatrix() * getViewMatrix();
        }
        return viewProjectionMatrix;
    }
    
    // Transform point from world space to view space
    Vec3 worldToView(const Vec3& worldPoint) const {
        Vec4 point(worldPoint, 1.0f);
        Vec4 viewPoint = getViewMatrix() * point;
        return viewPoint.toVec3();
    }
    
    // Transform point from view space to world space
    Vec3 viewToWorld(const Vec3& viewPoint) const {
        Mat4 invView = getViewMatrix().inverse();
        Vec4 point(viewPoint, 1.0f);
        Vec4 worldPoint = invView * point;
        return worldPoint.toVec3();
    }
    
    // Get camera vectors in world space
    Vec3 getForward() const {
        return transform.getForward();
    }
    
    Vec3 getRight() const {
        return transform.getRight();
    }
    
    Vec3 getUp() const {
        return transform.getUp();
    }
    
    // Ray from camera through normalized device coordinates
    Vec3 getRayDirection(float ndcX, float ndcY) const {
        // Convert from NDC to view space
        float halfFovRad = Math::toRadians(fov * 0.5f);
        float tanHalfFov = tan(halfFovRad);
        
        Vec3 viewDir(
            ndcX * tanHalfFov * aspectRatio,
            ndcY * tanHalfFov,
            -1.0f  // Forward is -Z
        );
        
        // Transform to world space
        return transform.transformDirection(viewDir.normalized());
    }
    
private:
    void updateViewMatrix() const {
        // View matrix is the inverse of the camera's world transform
        // For efficiency, we construct it directly
        Vec3 pos = transform.getPosition();
        Vec3 right = transform.getRight();
        Vec3 up = transform.getUp();
        Vec3 forward = transform.getForward();
        
        // View matrix: [R^T | -R^T * p]
        //              [0   |    1     ]
        viewMatrix = Mat4();
        
        // Rotation part (transposed)
        viewMatrix.m[0] = right.x;
        viewMatrix.m[4] = right.y;
        viewMatrix.m[8] = right.z;
        
        viewMatrix.m[1] = up.x;
        viewMatrix.m[5] = up.y;
        viewMatrix.m[9] = up.z;
        
        viewMatrix.m[2] = -forward.x;
        viewMatrix.m[6] = -forward.y;
        viewMatrix.m[10] = -forward.z;
        
        // Translation part
        viewMatrix.m[12] = -right.dot(pos);
        viewMatrix.m[13] = -up.dot(pos);
        viewMatrix.m[14] = forward.dot(pos);
        
        viewDirty = false;
    }
    
    void updateProjectionMatrix() const {
        if (projectionType == PERSPECTIVE) {
            projectionMatrix = Mat4::perspective(
                Math::toRadians(fov),
                aspectRatio,
                nearPlane,
                farPlane
            );
        } else {
            float halfWidth = orthoSize * aspectRatio;
            float halfHeight = orthoSize;
            projectionMatrix = Mat4::orthographic(
                -halfWidth, halfWidth,
                -halfHeight, halfHeight,
                nearPlane, farPlane
            );
        }
        projectionDirty = false;
    }
};