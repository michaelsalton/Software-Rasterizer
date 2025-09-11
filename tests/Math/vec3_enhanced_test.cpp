#include "../test_framework.h"
#include "../test_assertions.h"
#include "math/vec3.h"
#include <limits>

void testVec3Enhanced() {
    TEST_SUITE("Vec3 Enhanced Tests");

    TEST("Vector comparison with custom assertions") {
        Vec3 v1(1.0f, 2.0f, 3.0f);
        Vec3 v2(1.0f, 2.0f, 3.0f);
        Vec3 v3(1.1f, 2.0f, 3.0f);
        
        // Using enhanced assertions
        ASSERT_VEC3_EQ(v1, v2);
        ASSERT_VEC3_NEAR(v1, v3, 0.2f);
    });

    TEST("Performance benchmark example") {
        Vec3 v1(1.0f, 2.0f, 3.0f);
        Vec3 v2(4.0f, 5.0f, 6.0f);
        
        BENCHMARK("Vec3 dot product", 100000, {
            float result = v1.dot(v2);
            (void)result; // Avoid unused variable warning
        });
        
        BENCHMARK("Vec3 cross product", 100000, {
            Vec3 result = v1.cross(v2);
            (void)result;
        });
    });

    TEST("Range validation") {
        Vec3 v(0.5f, 0.7f, 0.9f);
        
        // All components should be in [0, 1] range
        ASSERT_IN_RANGE(v.x, 0.0f, 1.0f);
        ASSERT_IN_RANGE(v.y, 0.0f, 1.0f);
        ASSERT_IN_RANGE(v.z, 0.0f, 1.0f);
        
        // Length should be less than sqrt(3)
        ASSERT_IN_RANGE(v.length(), 0.0f, 1.8f);
    });

    TEST("Performance assertion example") {
        Vec3 v1(1.0f, 2.0f, 3.0f);
        Vec3 v2(4.0f, 5.0f, 6.0f);
        
        // This operation should complete very quickly
        ASSERT_PERFORMANCE({
            for (int i = 0; i < 1000; ++i) {
                Vec3 result = v1 + v2;
                (void)result;
            }
        }, 1.0); // Should complete in less than 1ms
    });

    END_SUITE();
}