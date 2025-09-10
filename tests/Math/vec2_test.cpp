#include "../test_framework.h"
#include "Math/vec2.h"
#include "Math/math.h"
#include <sstream>

void testVec2() {
    TEST_SUITE("Vec2");

    TEST("Constructor Default") {
        Vec2 v;
        ASSERT_FLOAT_EQ(0.0f, v.x);
        ASSERT_FLOAT_EQ(0.0f, v.y);
    });

    TEST("Constructor With Values") {
        Vec2 v(3.0f, 4.0f);
        ASSERT_FLOAT_EQ(3.0f, v.x);
        ASSERT_FLOAT_EQ(4.0f, v.y);
    });

    TEST("Copy Constructor") {
        Vec2 v1(3.0f, 4.0f);
        Vec2 v2(v1);
        ASSERT_FLOAT_EQ(v1.x, v2.x);
        ASSERT_FLOAT_EQ(v1.y, v2.y);
    });

    TEST("Addition Operator") {
        Vec2 v1(1.0f, 2.0f);
        Vec2 v2(3.0f, 4.0f);
        Vec2 result = v1 + v2;
        ASSERT_FLOAT_EQ(4.0f, result.x);
        ASSERT_FLOAT_EQ(6.0f, result.y);
    });

    TEST("Subtraction Operator") {
        Vec2 v1(5.0f, 7.0f);
        Vec2 v2(2.0f, 3.0f);
        Vec2 result = v1 - v2;
        ASSERT_FLOAT_EQ(3.0f, result.x);
        ASSERT_FLOAT_EQ(4.0f, result.y);
    });

    TEST("Scalar Multiplication") {
        Vec2 v(2.0f, 3.0f);
        Vec2 result = v * 2.5f;
        ASSERT_FLOAT_EQ(5.0f, result.x);
        ASSERT_FLOAT_EQ(7.5f, result.y);
    });

    TEST("Scalar Division") {
        Vec2 v(10.0f, 20.0f);
        Vec2 result = v / 2.0f;
        ASSERT_FLOAT_EQ(5.0f, result.x);
        ASSERT_FLOAT_EQ(10.0f, result.y);
    });

    TEST("Negation Operator") {
        Vec2 v(3.0f, -4.0f);
        Vec2 result = -v;
        ASSERT_FLOAT_EQ(-3.0f, result.x);
        ASSERT_FLOAT_EQ(4.0f, result.y);
    });

    TEST("Compound Assignment Operators") {
        Vec2 v(1.0f, 2.0f);
        v += Vec2(3.0f, 4.0f);
        ASSERT_FLOAT_EQ(4.0f, v.x);
        ASSERT_FLOAT_EQ(6.0f, v.y);
        
        v -= Vec2(1.0f, 1.0f);
        ASSERT_FLOAT_EQ(3.0f, v.x);
        ASSERT_FLOAT_EQ(5.0f, v.y);
        
        v *= 2.0f;
        ASSERT_FLOAT_EQ(6.0f, v.x);
        ASSERT_FLOAT_EQ(10.0f, v.y);
        
        v /= 2.0f;
        ASSERT_FLOAT_EQ(3.0f, v.x);
        ASSERT_FLOAT_EQ(5.0f, v.y);
    });

    TEST("Dot Product") {
        Vec2 v1(3.0f, 4.0f);
        Vec2 v2(2.0f, 1.0f);
        float dot = v1.dot(v2);
        ASSERT_FLOAT_EQ(10.0f, dot);  // 3*2 + 4*1 = 10
    });

    TEST("Cross Product (2D)") {
        Vec2 v1(3.0f, 0.0f);
        Vec2 v2(0.0f, 3.0f);
        float cross = v1.cross(v2);
        ASSERT_FLOAT_EQ(9.0f, cross);  // 3*3 - 0*0 = 9
    });

    TEST("Length") {
        Vec2 v(3.0f, 4.0f);
        ASSERT_FLOAT_EQ(5.0f, v.length());  // sqrt(9 + 16) = 5
    });

    TEST("Length Squared") {
        Vec2 v(3.0f, 4.0f);
        ASSERT_FLOAT_EQ(25.0f, v.lengthSquared());  // 9 + 16 = 25
    });

    TEST("Normalize") {
        Vec2 v(3.0f, 4.0f);
        v.normalize();
        ASSERT_FLOAT_EQ(1.0f, v.length());
        ASSERT_FLOAT_EQ(0.6f, v.x);
        ASSERT_FLOAT_EQ(0.8f, v.y);
    });

    TEST("Normalized") {
        Vec2 v(3.0f, 4.0f);
        Vec2 n = v.normalized();
        ASSERT_FLOAT_EQ(3.0f, v.x);  // Original unchanged
        ASSERT_FLOAT_EQ(4.0f, v.y);
        ASSERT_FLOAT_EQ(1.0f, n.length());
        ASSERT_FLOAT_EQ(0.6f, n.x);
        ASSERT_FLOAT_EQ(0.8f, n.y);
    });

    TEST("Zero Vector Normalization") {
        Vec2 v(0.0f, 0.0f);
        Vec2 n = v.normalized();
        ASSERT_FLOAT_EQ(0.0f, n.x);
        ASSERT_FLOAT_EQ(0.0f, n.y);
    });

    TEST("Perpendicular") {
        Vec2 v(3.0f, 4.0f);
        Vec2 perp = v.perpendicular();
        ASSERT_FLOAT_EQ(-4.0f, perp.x);
        ASSERT_FLOAT_EQ(3.0f, perp.y);
        ASSERT_FLOAT_EQ(0.0f, v.dot(perp));  // Should be orthogonal
    });

    TEST("Lerp") {
        Vec2 v1(0.0f, 0.0f);
        Vec2 v2(10.0f, 20.0f);
        
        Vec2 mid = Vec2::lerp(v1, v2, 0.5f);
        ASSERT_FLOAT_EQ(5.0f, mid.x);
        ASSERT_FLOAT_EQ(10.0f, mid.y);
        
        Vec2 start = Vec2::lerp(v1, v2, 0.0f);
        ASSERT_FLOAT_EQ(v1.x, start.x);
        ASSERT_FLOAT_EQ(v1.y, start.y);
        
        Vec2 end = Vec2::lerp(v1, v2, 1.0f);
        ASSERT_FLOAT_EQ(v2.x, end.x);
        ASSERT_FLOAT_EQ(v2.y, end.y);
    });

    TEST("Distance") {
        Vec2 v1(0.0f, 0.0f);
        Vec2 v2(3.0f, 4.0f);
        ASSERT_FLOAT_EQ(5.0f, Vec2::distance(v1, v2));
    });

    TEST("Angle") {
        Vec2 v1(1.0f, 0.0f);
        Vec2 v2(0.0f, 1.0f);
        float angle = v1.angle(v2);
        ASSERT_FLOAT_EQ(Math::HALF_PI, angle);  // 90 degrees
    });

    TEST("Equality Operators") {
        Vec2 v1(1.0f, 2.0f);
        Vec2 v2(1.0f, 2.0f);
        Vec2 v3(1.0f, 2.1f);
        
        ASSERT_TRUE(v1 == v2);
        ASSERT_FALSE(v1 == v3);
        ASSERT_FALSE(v1 != v2);
        ASSERT_TRUE(v1 != v3);
    });

    TEST("Stream Output") {
        Vec2 v(3.14f, 2.71f);
        std::stringstream ss;
        ss << v;
        std::string output = ss.str();
        ASSERT_TRUE(output.find("3.14") != std::string::npos);
        ASSERT_TRUE(output.find("2.71") != std::string::npos);
    });

    END_SUITE();
}