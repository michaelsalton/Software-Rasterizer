#include "Core/Transform.h"

Transform::Transform() 
    : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1),
      isDirty(true), parent(nullptr) {}

void Transform::setPosition(const Vec3& pos) {
    position = pos;
    isDirty = true;
}

void Transform::setPosition(float x, float y, float z) {
    position = Vec3(x, y, z);
    isDirty = true;
}

const Vec3& Transform::getPosition() const { 
    return position; 
}

void Transform::translate(const Vec3& delta) {
    position = position + delta;
    isDirty = true;
}

void Transform::translate(float x, float y, float z) {
    translate(Vec3(x, y, z));
}

void Transform::setRotation(const Vec3& rot) {
    rotation = rot;
    isDirty = true;
}

void Transform::setRotation(float x, float y, float z) {
    rotation = Vec3(x, y, z);
    isDirty = true;
}

const Vec3& Transform::getRotation() const { 
    return rotation; 
}

void Transform::rotate(const Vec3& delta) {
    rotation = rotation + delta;
    isDirty = true;
}

void Transform::rotate(float x, float y, float z) {
    rotate(Vec3(x, y, z));
}

void Transform::setScale(const Vec3& s) {
    scale = s;
    isDirty = true;
}

void Transform::setScale(float s) {
    scale = Vec3(s, s, s);
    isDirty = true;
}

void Transform::setScale(float x, float y, float z) {
    scale = Vec3(x, y, z);
    isDirty = true;
}

const Vec3& Transform::getScale() const { 
    return scale; 
}

const Mat4& Transform::getModelMatrix() const {
    if (isDirty) {
        updateModelMatrix();
    }
    return modelMatrix;
}

Mat4 Transform::getWorldMatrix() const {
    if (parent) {
        return parent->getWorldMatrix() * getModelMatrix();
    }
    return getModelMatrix();
}

Vec3 Transform::getWorldPosition() const {
    if (parent) {
        Mat4 worldMat = getWorldMatrix();
        return Vec3(worldMat.m[12], worldMat.m[13], worldMat.m[14]);
    }
    return position;
}

Vec3 Transform::transformPoint(const Vec3& localPoint) const {
    Vec4 point(localPoint, 1.0f);
    Vec4 worldPoint = getWorldMatrix() * point;
    return worldPoint.toVec3();
}

Vec3 Transform::transformDirection(const Vec3& localDir) const {
    Vec4 dir(localDir, 0.0f);
    Vec4 worldDir = getWorldMatrix() * dir;
    return worldDir.xyz().normalized();
}

Vec3 Transform::getForward() const {
    return transformDirection(Vec3(0, 0, -1)); // -Z is forward
}

Vec3 Transform::getRight() const {
    return transformDirection(Vec3(1, 0, 0));
}

Vec3 Transform::getUp() const {
    return transformDirection(Vec3(0, 1, 0));
}

void Transform::lookAt(const Vec3& target, const Vec3& up) {
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

void Transform::setParent(Transform* p) {
    parent = p;
    isDirty = true;
}

Transform* Transform::getParent() const { 
    return parent; 
}

void Transform::reset() {
    position = Vec3(0, 0, 0);
    rotation = Vec3(0, 0, 0);
    scale = Vec3(1, 1, 1);
    isDirty = true;
}

void Transform::updateModelMatrix() const {
    // Build transformation matrix: T * R * S
    Mat4 T = Mat4::translation(position);
    Mat4 R = Mat4::fromEulerAngles(rotation);
    Mat4 S = Mat4::scale(scale);
    
    modelMatrix = T * R * S;
    isDirty = false;
}