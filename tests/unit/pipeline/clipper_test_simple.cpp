#include "../../test_assertions.h"
#include "pipeline/clipper.h"
#include "pipeline/vertex_processor.h"
#include <vector>

void testClipperBasics() {
    TEST_SUITE("Clipper Basic Tests");
    
    TEST("ViewFrustum initialization") {
        ViewFrustum frustum;
        frustum.InitializeClipSpace();
        
        // Should have 6 planes
        ASSERT_EQ(6, static_cast<int>(frustum.planes.size()));
    });
    
    TEST("ClipPlane distance calculation") {
        ClipPlane plane(Vec4(0, 0, 1, 0), 5.0f);  // z = -5 plane
        
        Vec4 pointInFront(0, 0, 0, 1);     // z = 0, should be at distance 5
        Vec4 pointBehind(0, 0, -10, 1);    // z = -10, should be at distance -5
        
        ASSERT_FLOAT_EQ(5.0f, plane.Distance(pointInFront));
        ASSERT_FLOAT_EQ(-5.0f, plane.Distance(pointBehind));
        
        ASSERT_TRUE(plane.IsInside(pointInFront));
        ASSERT_FALSE(plane.IsInside(pointBehind));
    });
    
    TEST("Triangle outside detection") {
        // Create three vertices all behind the near plane (z < 0 in clip space)
        TransformedVertex v0, v1, v2;
        v0.clipPosition = Vec4(0, 0, -2, 1);  // Behind near plane
        v1.clipPosition = Vec4(1, 0, -2, 1);  // Behind near plane
        v2.clipPosition = Vec4(0, 1, -2, 1);  // Behind near plane
        
        bool isOutside = Clipper::IsTriangleOutsideFrustum(v0, v1, v2);
        
        // Should be detected as outside
        ASSERT_TRUE(isOutside);
    });
    
    END_SUITE();
}

// Run all Clipper tests
void runClipperTests() {
    testClipperBasics();
}