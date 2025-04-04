#pragma once

#include <math.h>
#include "Vector3.h"

#define PI 3.14159265
#define DEG_TO_RAD PI / 180.0f

inline Vector3 operator + (const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Vector3 operator - (const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Vector3 RotateVector(Vector3& vector, float angle)
{
    float radianAngle = (float)(angle * DEG_TO_RAD);
    return Vector3(
        (float)(vector.x * cos(radianAngle) - vector.y * sin(radianAngle)),
        (float)(vector.x * sin(radianAngle) + vector.y * cos(radianAngle)),
        vector.z
    );
}

inline Vector3 RotateVector3D(Vector3& vector, float angleX, float angleY, float angleZ)
{
    float radianAngleX = (float)(angleX * DEG_TO_RAD);
    float radianAngleY = (float)(angleY * DEG_TO_RAD);
    float radianAngleZ = (float)(angleZ * DEG_TO_RAD);

    float cosX = cos(radianAngleX), sinX = sin(radianAngleX);
    float cosY = cos(radianAngleY), sinY = sin(radianAngleY);
    float cosZ = cos(radianAngleZ), sinZ = sin(radianAngleZ);

    float tempY = vector.y * cosX - vector.z * sinX;
    float tempZ = vector.y * sinX + vector.z * cosX;

    float tempX = vector.x * cosY + tempZ * sinY;
    tempZ = -vector.x * sinY + tempZ * cosY;

    vector.x = tempX * cosZ - tempY * sinZ;
    vector.y = tempX * sinZ + tempY * cosZ;
    vector.z = tempZ;

    return vector;
}
