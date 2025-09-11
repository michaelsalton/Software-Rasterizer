#include "../test_framework.h"
#include "math/vec4.h"
#include "math/vec3.h"

void testVec4() {
    TEST_SUITE("Vec4");

    TEST("Constructor Default") {
        Vec4 v;
        ASSERT_FLOAT_EQ(0.0f, v.x);
        ASSERT_FLOAT_EQ(0.0f, v.y);
        ASSERT_FLOAT_EQ(0.0f, v.z);
        ASSERT_FLOAT_EQ(0.0f, v.w);
    });

    TEST("Constructor With Values") {
        Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
        ASSERT_FLOAT_EQ(1.0f, v.x);
        ASSERT_FLOAT_EQ(2.0f, v.y);
        ASSERT_FLOAT_EQ(3.0f, v.z);
        ASSERT_FLOAT_EQ(4.0f, v.w);
    });

    TEST("Constructor From Vec3") {
        Vec3 v3(1.0f, 2.0f, 3.0f);
        Vec4 v4(v3, 1.0f);
        ASSERT_FLOAT_EQ(1.0f, v4.x);
        ASSERT_FLOAT_EQ(2.0f, v4.y);
        ASSERT_FLOAT_EQ(3.0f, v4.z);
        ASSERT_FLOAT_EQ(1.0f, v4.w);
    });

    TEST("Arithmetic Operations") {
        Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
        
        Vec4 sum = v1 + v2;
        ASSERT_FLOAT_EQ(6.0f, sum.x);
        ASSERT_FLOAT_EQ(8.0f, sum.y);
        ASSERT_FLOAT_EQ(10.0f, sum.z);
        ASSERT_FLOAT_EQ(12.0f, sum.w);
        
        Vec4 diff = v2 - v1;
        ASSERT_FLOAT_EQ(4.0f, diff.x);
        ASSERT_FLOAT_EQ(4.0f, diff.y);
        ASSERT_FLOAT_EQ(4.0f, diff.z);
        ASSERT_FLOAT_EQ(4.0f, diff.w);
        
        Vec4 scaled = v1 * 2.0f;
        ASSERT_FLOAT_EQ(2.0f, scaled.x);
        ASSERT_FLOAT_EQ(4.0f, scaled.y);
        ASSERT_FLOAT_EQ(6.0f, scaled.z);
        ASSERT_FLOAT_EQ(8.0f, scaled.w);
    });

    TEST("Dot Product") {
        Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
        float dot = v1.dot(v2);
        ASSERT_FLOAT_EQ(70.0f, dot);  // 1*5 + 2*6 + 3*7 + 4*8 = 70
    });

    TEST("Length and Normalization") {
        Vec4 v(1.0f, 2.0f, 2.0f, 0.0f);
        ASSERT_FLOAT_EQ(3.0f, v.length());
        ASSERT_FLOAT_EQ(9.0f, v.lengthSquared());
        
        Vec4 n = v.normalized();
        ASSERT_FLOAT_EQ(1.0f, n.length());
        TestFramework::getInstance().setEpsilon(1e-4f);
        ASSERT_FLOAT_EQ(1.0f/3.0f, n.x);
        ASSERT_FLOAT_EQ(2.0f/3.0f, n.y);
        ASSERT_FLOAT_EQ(2.0f/3.0f, n.z);
        ASSERT_FLOAT_EQ(0.0f, n.w);
        TestFramework::getInstance().setEpsilon(1e-5f);
    });

    TEST("Homogeneous Coordinate Conversion") {
        Vec3 v3(6.0f, 8.0f, 10.0f);
        Vec4 v4(v3, 2.0f);
        
        Vec3 result = v4.toVec3();
        ASSERT_FLOAT_EQ(3.0f, result.x);  // 6/2
        ASSERT_FLOAT_EQ(4.0f, result.y);  // 8/2
        ASSERT_FLOAT_EQ(5.0f, result.z);  // 10/2
        
        // Test with w = 0 (direction vector)
        Vec4 direction(1.0f, 2.0f, 3.0f, 0.0f);
        Vec3 dirResult = direction.toVec3();
        ASSERT_FLOAT_EQ(1.0f, dirResult.x);
        ASSERT_FLOAT_EQ(2.0f, dirResult.y);
        ASSERT_FLOAT_EQ(3.0f, dirResult.z);
    });

    TEST("XYZ Extraction") {
        Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
        Vec3 xyz = v.xyz();
        ASSERT_FLOAT_EQ(1.0f, xyz.x);
        ASSERT_FLOAT_EQ(2.0f, xyz.y);
        ASSERT_FLOAT_EQ(3.0f, xyz.z);
        // w component should be ignored
    });

    TEST("Lerp") {
        Vec4 v1(0.0f, 0.0f, 0.0f, 0.0f);
        Vec4 v2(10.0f, 20.0f, 30.0f, 40.0f);
        
        Vec4 mid = Vec4::lerp(v1, v2, 0.5f);
        ASSERT_FLOAT_EQ(5.0f, mid.x);
        ASSERT_FLOAT_EQ(10.0f, mid.y);
        ASSERT_FLOAT_EQ(15.0f, mid.z);
        ASSERT_FLOAT_EQ(20.0f, mid.w);
    });

    TEST("Compound Assignment") {
        Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
        
        v += Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        ASSERT_FLOAT_EQ(2.0f, v.x);
        ASSERT_FLOAT_EQ(3.0f, v.y);
        ASSERT_FLOAT_EQ(4.0f, v.z);
        ASSERT_FLOAT_EQ(5.0f, v.w);
        
        v *= 2.0f;
        ASSERT_FLOAT_EQ(4.0f, v.x);
        ASSERT_FLOAT_EQ(6.0f, v.y);
        ASSERT_FLOAT_EQ(8.0f, v.z);
        ASSERT_FLOAT_EQ(10.0f, v.w);
    });

    TEST("Equality Operators") {
        Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 v2(1.0f, 2.0f, 3.0f, 4.0f);
        Vec4 v3(1.0f, 2.0f, 3.0f, 4.1f);
        
        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 == v3);
        ASSERT_FALSE(v1 != v2);
        ASSERT_TRUE(v1 != v3);
    });

    TEST("Special Cases") {
        // Test very small w value (near zero)
        Vec4 v(10.0f, 20.0f, 30.0f, 0.0001f);
        Vec3 result = v.toVec3();
        // Should produce very large values
        ASSERT_TRUE(result.x > 10000.0f);
        ASSERT_TRUE(result.y > 10000.0f);
        ASSERT_TRUE(result.z > 10000.0f);
        
        // Test negative w
        Vec4 vNeg(10.0f, 20.0f, 30.0f, -2.0f);
        Vec3 resultNeg = vNeg.toVec3();
        ASSERT_FLOAT_EQ(-5.0f, resultNeg.x);
        ASSERT_FLOAT_EQ(-10.0f, resultNeg.y);
        ASSERT_FLOAT_EQ(-15.0f, resultNeg.z);
    });

    END_SUITE();
}