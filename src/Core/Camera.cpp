#include "core/camera.h"
#include <cmath>

Camera::Camera() 
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

Transform& Camera::getTransform() { 
    viewDirty = true;
    return transform; 
}

const Transform& Camera::getTransform() const { 
    return transform; 
}

void Camera::setPosition(const Vec3& pos) {
    transform.setPosition(pos);
    viewDirty = true;
}

void Camera::setPosition(float x, float y, float z) {
    setPosition(Vec3(x, y, z));
}

Vec3 Camera::getPosition() const {
    return transform.getPosition();
}

void Camera::lookAt(const Vec3& target, const Vec3& up) {
    transform.lookAt(target, up);
    viewDirty = true;
}

void Camera::setPerspective(float fovDegrees, float aspect, float near, float far) {
    projectionType = PERSPECTIVE;
    fov = fovDegrees;
    aspectRatio = aspect;
    nearPlane = near;
    farPlane = far;
    projectionDirty = true;
}

void Camera::setOrthographic(float size, float aspect, float near, float far) {
    projectionType = ORTHOGRAPHIC;
    orthoSize = size;
    aspectRatio = aspect;
    nearPlane = near;
    farPlane = far;
    projectionDirty = true;
}

void Camera::setAspectRatio(float aspect) {
    aspectRatio = aspect;
    projectionDirty = true;
}

void Camera::setFieldOfView(float fovDegrees) {
    fov = fovDegrees;
    projectionDirty = true;
}

float Camera::getFieldOfView() const { 
    return fov; 
}

float Camera::getAspectRatio() const { 
    return aspectRatio; 
}

float Camera::getNearPlane() const { 
    return nearPlane; 
}

float Camera::getFarPlane() const { 
    return farPlane; 
}

const Mat4& Camera::getViewMatrix() const {
    if (viewDirty) {
        updateViewMatrix();
    }
    return viewMatrix;
}

const Mat4& Camera::getProjectionMatrix() const {
    if (projectionDirty) {
        updateProjectionMatrix();
    }
    return projectionMatrix;
}

const Mat4& Camera::getViewProjectionMatrix() const {
    if (viewDirty || projectionDirty) {
        viewProjectionMatrix = getProjectionMatrix() * getViewMatrix();
    }
    return viewProjectionMatrix;
}

Vec3 Camera::worldToView(const Vec3& worldPoint) const {
    Vec4 point(worldPoint, 1.0f);
    Vec4 viewPoint = getViewMatrix() * point;
    return viewPoint.toVec3();
}

Vec3 Camera::viewToWorld(const Vec3& viewPoint) const {
    Mat4 invView = getViewMatrix().inverse();
    Vec4 point(viewPoint, 1.0f);
    Vec4 worldPoint = invView * point;
    return worldPoint.toVec3();
}

Vec3 Camera::getForward() const {
    return transform.getForward();
}

Vec3 Camera::getRight() const {
    return transform.getRight();
}

Vec3 Camera::getUp() const {
    return transform.getUp();
}

Vec3 Camera::getRayDirection(float ndcX, float ndcY) const {
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

void Camera::updateViewMatrix() const {
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

void Camera::updateProjectionMatrix() const {
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

