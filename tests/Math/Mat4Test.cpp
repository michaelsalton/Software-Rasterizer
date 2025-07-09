#include "../TestFramework.h"
#include "../../src/Math/Mat4.h"
#include "../../src/Math/Vec3.h"
#include "../../src/Math/Vec4.h"
#include "../../src/Math/Math.h"

void testMat4() {
    TEST_SUITE("Mat4");

    TEST("Identity Matrix") {
        Mat4 m = Mat4::identity();
        ASSERT_TRUE(m.isIdentity());
        
        // Check diagonal elements
        ASSERT_FLOAT_EQ(1.0f, m.at(0, 0));
        ASSERT_FLOAT_EQ(1.0f, m.at(1, 1));
        ASSERT_FLOAT_EQ(1.0f, m.at(2, 2));
        ASSERT_FLOAT_EQ(1.0f, m.at(3, 3));
        
        // Check off-diagonal elements
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i != j) {
                    ASSERT_FLOAT_EQ(0.0f, m.at(i, j));
                }
            }
        }
    });

    TEST("Constructor With Values") {
        Mat4 m(1, 2, 3, 4,
               5, 6, 7, 8,
               9, 10, 11, 12,
               13, 14, 15, 16);
        
        ASSERT_FLOAT_EQ(1.0f, m.at(0, 0));
        ASSERT_FLOAT_EQ(2.0f, m.at(0, 1));
        ASSERT_FLOAT_EQ(5.0f, m.at(1, 0));
        ASSERT_FLOAT_EQ(16.0f, m.at(3, 3));
    });

    TEST("Translation Matrix") {
        Mat4 trans = Mat4::translation(10.0f, 20.0f, 30.0f);
        Vec3 point(1.0f, 2.0f, 3.0f);
        Vec3 result = trans.transformPoint(point);
        
        ASSERT_FLOAT_EQ(11.0f, result.x);
        ASSERT_FLOAT_EQ(22.0f, result.y);
        ASSERT_FLOAT_EQ(33.0f, result.z);
        
        // Translation should not affect directions
        Vec3 direction(1.0f, 0.0f, 0.0f);
        Vec3 dirResult = trans.transformDirection(direction);
        ASSERT_FLOAT_EQ(1.0f, dirResult.x);
        ASSERT_FLOAT_EQ(0.0f, dirResult.y);
        ASSERT_FLOAT_EQ(0.0f, dirResult.z);
    });

    TEST("Scale Matrix") {
        Mat4 scale = Mat4::scale(2.0f, 3.0f, 4.0f);
        Vec3 point(1.0f, 1.0f, 1.0f);
        Vec3 result = scale.transformPoint(point);
        
        ASSERT_FLOAT_EQ(2.0f, result.x);
        ASSERT_FLOAT_EQ(3.0f, result.y);
        ASSERT_FLOAT_EQ(4.0f, result.z);
        
        // Test uniform scale
        Mat4 uniformScale = Mat4::scale(2.0f);
        result = uniformScale.transformPoint(point);
        ASSERT_FLOAT_EQ(2.0f, result.x);
        ASSERT_FLOAT_EQ(2.0f, result.y);
        ASSERT_FLOAT_EQ(2.0f, result.z);
        
        // Test Vec3 scale
        Vec3 scaleVec(2.0f, 3.0f, 4.0f);
        Mat4 vecScale = Mat4::scale(scaleVec);
        result = vecScale.transformPoint(point);
        ASSERT_FLOAT_EQ(2.0f, result.x);
        ASSERT_FLOAT_EQ(3.0f, result.y);
        ASSERT_FLOAT_EQ(4.0f, result.z);
    });

    TEST("Rotation Matrices") {
        // Test X rotation (90 degrees)
        Mat4 rotX = Mat4::rotationX(Math::toRadians(90.0f));
        Vec3 point(0.0f, 1.0f, 0.0f);
        Vec3 result = rotX.transformPoint(point);
        TestFramework::getInstance().setEpsilon(1e-4f);
        ASSERT_FLOAT_EQ(0.0f, result.x);
        ASSERT_FLOAT_EQ(0.0f, result.y);
        ASSERT_FLOAT_EQ(1.0f, result.z);
        
        // Test Y rotation (90 degrees)
        Mat4 rotY = Mat4::rotationY(Math::toRadians(90.0f));
        point = Vec3(1.0f, 0.0f, 0.0f);
        result = rotY.transformPoint(point);
        ASSERT_FLOAT_EQ(0.0f, result.x);
        ASSERT_FLOAT_EQ(0.0f, result.y);
        ASSERT_FLOAT_EQ(-1.0f, result.z);
        
        // Test Z rotation (90 degrees)
        Mat4 rotZ = Mat4::rotationZ(Math::toRadians(90.0f));
        point = Vec3(1.0f, 0.0f, 0.0f);
        result = rotZ.transformPoint(point);
        ASSERT_FLOAT_EQ(0.0f, result.x);
        ASSERT_FLOAT_EQ(1.0f, result.y);
        ASSERT_FLOAT_EQ(0.0f, result.z);
        TestFramework::getInstance().setEpsilon(1e-5f);
    });

    TEST("Matrix Multiplication") {
        Mat4 trans = Mat4::translation(10.0f, 0.0f, 0.0f);
        Mat4 scale = Mat4::scale(2.0f);
        
        // Order matters: first scale, then translate
        Mat4 combined1 = trans * scale;
        Vec3 point(1.0f, 1.0f, 1.0f);
        Vec3 result1 = combined1.transformPoint(point);
        ASSERT_FLOAT_EQ(12.0f, result1.x);  // 1*2 + 10
        ASSERT_FLOAT_EQ(2.0f, result1.y);   // 1*2
        ASSERT_FLOAT_EQ(2.0f, result1.z);   // 1*2
        
        // Different order: first translate, then scale
        Mat4 combined2 = scale * trans;
        Vec3 result2 = combined2.transformPoint(point);
        ASSERT_FLOAT_EQ(22.0f, result2.x);  // (1+10)*2
        ASSERT_FLOAT_EQ(2.0f, result2.y);   // 1*2
        ASSERT_FLOAT_EQ(2.0f, result2.z);   // 1*2
    });

    TEST("Transpose") {
        Mat4 m(1, 2, 3, 4,
               5, 6, 7, 8,
               9, 10, 11, 12,
               13, 14, 15, 16);
        
        Mat4 mT = m.transpose();
        
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                ASSERT_FLOAT_EQ(m.at(i, j), mT.at(j, i));
            }
        }
    });

    TEST("Determinant and Inverse") {
        // Test with a known invertible matrix
        Mat4 m = Mat4::translation(5, 10, 15) * 
                 Mat4::rotationY(Math::toRadians(45)) * 
                 Mat4::scale(2);
        
        float det = m.determinant();
        ASSERT_FLOAT_EQ(8.0f, det);  // 2^3 for scale
        
        Mat4 mInv = m.inverse();
        Mat4 identity = m * mInv;
        
        TestFramework::getInstance().setEpsilon(1e-4f);
        ASSERT_TRUE(identity.isIdentity());
        TestFramework::getInstance().setEpsilon(1e-5f);
        
        // Test non-invertible matrix
        Mat4 singular(0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 1);
        ASSERT_FLOAT_EQ(0.0f, singular.determinant());
        Mat4 singularInv = singular.inverse();
        ASSERT_TRUE(singularInv.isIdentity());  // Returns identity for non-invertible
    });

    TEST("Euler Angles") {
        float x = Math::toRadians(30);
        float y = Math::toRadians(45);
        float z = Math::toRadians(60);
        
        Mat4 m = Mat4::fromEulerAngles(x, y, z);
        Vec3 extracted = m.toEulerAngles();
        
        // Due to gimbal lock and multiple solutions, we test by reconstruction
        Mat4 reconstructed = Mat4::fromEulerAngles(extracted);
        
        TestFramework::getInstance().setEpsilon(1e-3f);
        for (int i = 0; i < 16; i++) {
            ASSERT_FLOAT_EQ(m.m[i], reconstructed.m[i]);
        }
        TestFramework::getInstance().setEpsilon(1e-5f);
    });

    TEST("Matrix Decomposition") {
        Vec3 origTrans(5, 10, 15);
        Vec3 origScale(2, 3, 4);
        float angle = Math::toRadians(45);
        
        Mat4 m = Mat4::translation(origTrans) * 
                 Mat4::rotationY(angle) * 
                 Mat4::scale(origScale.x, origScale.y, origScale.z);
        
        Vec3 translation, scale;
        Mat4 rotation;
        m.decompose(translation, rotation, scale);
        
        ASSERT_FLOAT_EQ(origTrans.x, translation.x);
        ASSERT_FLOAT_EQ(origTrans.y, translation.y);
        ASSERT_FLOAT_EQ(origTrans.z, translation.z);
        
        ASSERT_FLOAT_EQ(origScale.x, scale.x);
        ASSERT_FLOAT_EQ(origScale.y, scale.y);
        ASSERT_FLOAT_EQ(origScale.z, scale.z);
        
        ASSERT_TRUE(rotation.isOrthogonal());
    });

    TEST("Perspective Projection") {
        float fov = Math::toRadians(60);
        float aspect = 16.0f / 9.0f;
        float near = 0.1f;
        float far = 100.0f;
        
        Mat4 proj = Mat4::perspective(fov, aspect, near, far);
        
        // Test that a point at the center near plane maps correctly
        Vec4 nearPoint(0, 0, -near, 1);
        Vec4 projected = proj * nearPoint;
        Vec4 ndc = projected;  // After perspective divide would be (0,0,-1,1)
        
        // Test that it's a valid perspective matrix
        ASSERT_NE(0.0f, proj.at(2, 3));  // Should have perspective
        ASSERT_FLOAT_EQ(-1.0f, proj.at(3, 2));  // Standard perspective
    });

    TEST("Orthographic Projection") {
        Mat4 ortho = Mat4::orthographic(-10, 10, -10, 10, 0.1f, 100.0f);
        
        // Center point should map to origin
        Vec4 center(0, 0, -50, 1);
        Vec4 projected = ortho * center;
        ASSERT_FLOAT_EQ(0.0f, projected.x);
        ASSERT_FLOAT_EQ(0.0f, projected.y);
        
        // Should have no perspective (w stays 1)
        ASSERT_FLOAT_EQ(1.0f, projected.w);
    });

    TEST("LookAt Matrix") {
        Vec3 eye(10, 10, 10);
        Vec3 target(0, 0, 0);
        Vec3 up(0, 1, 0);
        
        Mat4 view = Mat4::lookAt(eye, target, up);
        
        // The view matrix should transform the eye position to origin
        Vec3 eyeTransformed = view.transformPoint(eye);
        TestFramework::getInstance().setEpsilon(1e-4f);
        ASSERT_FLOAT_EQ(0.0f, eyeTransformed.x);
        ASSERT_FLOAT_EQ(0.0f, eyeTransformed.y);
        ASSERT_FLOAT_EQ(0.0f, eyeTransformed.z);
        TestFramework::getInstance().setEpsilon(1e-5f);
    });

    TEST("Operators") {
        Mat4 m1 = Mat4::translation(1, 2, 3);
        Mat4 m2 = Mat4::translation(1, 2, 3);
        Mat4 m3 = Mat4::translation(1, 2, 4);
        
        ASSERT_TRUE(m1 == m2);
        ASSERT_FALSE(m1 == m3);
        ASSERT_FALSE(m1 != m2);
        ASSERT_TRUE(m1 != m3);
        
        // Test compound multiplication
        // m4 = scale(2) * translation(5,0,0)
        // This means: first translate by 5, then scale by 2
        // So point (1,0,0) becomes (1+5,0,0) = (6,0,0), then scaled to (12,0,0)
        Mat4 m4 = Mat4::scale(2);
        m4 *= Mat4::translation(5, 0, 0);
        Vec3 point(1, 0, 0);
        Vec3 result = m4.transformPoint(point);
        ASSERT_FLOAT_EQ(12.0f, result.x);  // (1+5)*2 = 12
    });

    TEST("Lerp") {
        Mat4 m1 = Mat4::identity();
        Mat4 m2 = Mat4::translation(10, 20, 30);
        
        Mat4 mid = Mat4::lerp(m1, m2, 0.5f);
        ASSERT_FLOAT_EQ(5.0f, mid.m[12]);
        ASSERT_FLOAT_EQ(10.0f, mid.m[13]);
        ASSERT_FLOAT_EQ(15.0f, mid.m[14]);
    });

    END_SUITE();
}