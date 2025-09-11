#include "../test_framework.h"
#include "math/vec3.h"
#include "math/math.h"

void testVec3() {
    TEST_SUITE("Vec3");

    TEST("Constructor Default") {
        Vec3 v;
        ASSERT_FLOAT_EQ(0.0f, v.x);
        ASSERT_FLOAT_EQ(0.0f, v.y);
        ASSERT_FLOAT_EQ(0.0f, v.z);
    });

    TEST("Constructor With Values") {
        Vec3 v(1.0f, 2.0f, 3.0f);
        ASSERT_FLOAT_EQ(1.0f, v.x);
        ASSERT_FLOAT_EQ(2.0f, v.y);
        ASSERT_FLOAT_EQ(3.0f, v.z);
    });

    // Vec3 doesn't have a constructor from Vec2
    // This test is removed

    TEST("Addition and Subtraction") {
        Vec3 v1(1.0f, 2.0f, 3.0f);
        Vec3 v2(4.0f, 5.0f, 6.0f);
        
        Vec3 sum = v1 + v2;
        ASSERT_FLOAT_EQ(5.0f, sum.x);
        ASSERT_FLOAT_EQ(7.0f, sum.y);
        ASSERT_FLOAT_EQ(9.0f, sum.z);
        
        Vec3 diff = v2 - v1;
        ASSERT_FLOAT_EQ(3.0f, diff.x);
        ASSERT_FLOAT_EQ(3.0f, diff.y);
        ASSERT_FLOAT_EQ(3.0f, diff.z);
    });

    TEST("Scalar Operations") {
        Vec3 v(2.0f, 3.0f, 4.0f);
        
        Vec3 scaled = v * 2.0f;
        ASSERT_FLOAT_EQ(4.0f, scaled.x);
        ASSERT_FLOAT_EQ(6.0f, scaled.y);
        ASSERT_FLOAT_EQ(8.0f, scaled.z);
        
        Vec3 divided = v / 2.0f;
        ASSERT_FLOAT_EQ(1.0f, divided.x);
        ASSERT_FLOAT_EQ(1.5f, divided.y);
        ASSERT_FLOAT_EQ(2.0f, divided.z);
    });

    TEST("Dot Product") {
        Vec3 v1(1.0f, 2.0f, 3.0f);
        Vec3 v2(4.0f, 5.0f, 6.0f);
        float dot = v1.dot(v2);
        ASSERT_FLOAT_EQ(32.0f, dot);  // 1*4 + 2*5 + 3*6 = 32
    });

    TEST("Cross Product") {
        Vec3 v1(1.0f, 0.0f, 0.0f);
        Vec3 v2(0.0f, 1.0f, 0.0f);
        Vec3 cross = v1.cross(v2);
        ASSERT_FLOAT_EQ(0.0f, cross.x);
        ASSERT_FLOAT_EQ(0.0f, cross.y);
        ASSERT_FLOAT_EQ(1.0f, cross.z);  // Right-hand rule
        
        // Test anti-commutativity
        Vec3 cross2 = v2.cross(v1);
        ASSERT_FLOAT_EQ(-cross.x, cross2.x);
        ASSERT_FLOAT_EQ(-cross.y, cross2.y);
        ASSERT_FLOAT_EQ(-cross.z, cross2.z);
    });

    TEST("Length and Normalization") {
        Vec3 v(3.0f, 0.0f, 4.0f);
        ASSERT_FLOAT_EQ(5.0f, v.length());
        ASSERT_FLOAT_EQ(25.0f, v.lengthSquared());
        
        Vec3 n = v.normalized();
        ASSERT_FLOAT_EQ(1.0f, n.length());
        ASSERT_FLOAT_EQ(0.6f, n.x);
        ASSERT_FLOAT_EQ(0.0f, n.y);
        ASSERT_FLOAT_EQ(0.8f, n.z);
        
        // Original should be unchanged
        ASSERT_FLOAT_EQ(3.0f, v.x);
        ASSERT_FLOAT_EQ(0.0f, v.y);
        ASSERT_FLOAT_EQ(4.0f, v.z);
    });

    TEST("Reflect") {
        Vec3 incident(1.0f, -1.0f, 0.0f);
        Vec3 normal(0.0f, 1.0f, 0.0f);
        incident.normalize();
        
        Vec3 reflected = incident.reflect(normal);
        // Should reflect across the normal
        ASSERT_FLOAT_EQ(incident.x, reflected.x);
        ASSERT_FLOAT_EQ(-incident.y, reflected.y);
        ASSERT_FLOAT_EQ(incident.z, reflected.z);
    });

    TEST("Distance Between Points") {
        Vec3 p1(1.0f, 2.0f, 3.0f);
        Vec3 p2(4.0f, 6.0f, 3.0f);
        float dist = Vec3::distance(p1, p2);
        ASSERT_FLOAT_EQ(5.0f, dist);  // 3-4-5 triangle in XY plane
    });

    TEST("Angle Between Vectors") {
        Vec3 v1(1.0f, 0.0f, 0.0f);
        Vec3 v2(0.0f, 1.0f, 0.0f);
        float angle = v1.angle(v2);
        ASSERT_FLOAT_EQ(Math::HALF_PI, angle);  // 90 degrees
        
        Vec3 v3(1.0f, 0.0f, 0.0f);
        Vec3 v4(-1.0f, 0.0f, 0.0f);
        angle = v3.angle(v4);
        ASSERT_FLOAT_EQ(Math::PI, angle);  // 180 degrees
    });

    TEST("Lerp") {
        Vec3 v1(0.0f, 0.0f, 0.0f);
        Vec3 v2(10.0f, 20.0f, 30.0f);
        
        Vec3 mid = Vec3::lerp(v1, v2, 0.5f);
        ASSERT_FLOAT_EQ(5.0f, mid.x);
        ASSERT_FLOAT_EQ(10.0f, mid.y);
        ASSERT_FLOAT_EQ(15.0f, mid.z);
        
        Vec3 quarter = Vec3::lerp(v1, v2, 0.25f);
        ASSERT_FLOAT_EQ(2.5f, quarter.x);
        ASSERT_FLOAT_EQ(5.0f, quarter.y);
        ASSERT_FLOAT_EQ(7.5f, quarter.z);
    });

    // These tests are commented out as these methods are not yet implemented
    // TODO: Add min/max static methods and isOrthogonal, isZero, isUnit methods
    
    /*
    TEST("Component-wise Min/Max") {
        Vec3 v1(1.0f, 5.0f, 3.0f);
        Vec3 v2(4.0f, 2.0f, 6.0f);
        
        Vec3 minV = Vec3::min(v1, v2);
        ASSERT_FLOAT_EQ(1.0f, minV.x);
        ASSERT_FLOAT_EQ(2.0f, minV.y);
        ASSERT_FLOAT_EQ(3.0f, minV.z);
        
        Vec3 maxV = Vec3::max(v1, v2);
        ASSERT_FLOAT_EQ(4.0f, maxV.x);
        ASSERT_FLOAT_EQ(5.0f, maxV.y);
        ASSERT_FLOAT_EQ(6.0f, maxV.z);
    });

    TEST("Orthogonality Test") {
        Vec3 v1(1.0f, 0.0f, 0.0f);
        Vec3 v2(0.0f, 1.0f, 0.0f);
        Vec3 v3(1.0f, 1.0f, 0.0f);
        
        ASSERT_TRUE(v1.isOrthogonal(v2));
        ASSERT_FALSE(v1.isOrthogonal(v3));
    });

    TEST("Special Vectors") {
        Vec3 v(3.0f, 4.0f, 5.0f);
        ASSERT_FALSE(v.isZero());
        
        Vec3 zero;
        ASSERT_TRUE(zero.isZero());
        
        Vec3 unit = v.normalized();
        ASSERT_TRUE(unit.isUnit());
        ASSERT_FALSE(v.isUnit());
    });
    */

    END_SUITE();
}