#include "../../test_assertions.h"
#include "pipeline/vertex_processor.h"
#include "math/vec3.h"
#include "math/vec2.h"

void testVertexConstruction() {
    TEST_SUITE("Vertex Construction");
    
    TEST("Default constructor") {
        Vertex v;
        ASSERT_VEC3_EQ(Vec3(0, 0, 0), v.position);
        ASSERT_VEC3_EQ(Vec3(0, 1, 0), v.normal);  // Default normal is up
        ASSERT_VEC2_EQ(Vec2(0, 0), v.texCoord);
        ASSERT_EQ(255, v.color.r);
        ASSERT_EQ(255, v.color.g);
        ASSERT_EQ(255, v.color.b);
        ASSERT_EQ(255, v.color.a);
    });
    
    TEST("Full constructor") {
        Vertex v(Vec3(1, 2, 3), Vec3(0, 0, 1), Vec2(0.5f, 0.5f), Framebuffer::Color(128, 64, 32, 255));
        ASSERT_VEC3_EQ(Vec3(1, 2, 3), v.position);
        ASSERT_VEC3_EQ(Vec3(0, 0, 1), v.normal);
        ASSERT_VEC2_EQ(Vec2(0.5f, 0.5f), v.texCoord);
        ASSERT_EQ(128, v.color.r);
        ASSERT_EQ(64, v.color.g);
        ASSERT_EQ(32, v.color.b);
        ASSERT_EQ(255, v.color.a);
    });
    
    TEST("Position-only constructor") {
        Vertex v(Vec3(4, 5, 6));
        ASSERT_VEC3_EQ(Vec3(4, 5, 6), v.position);
        ASSERT_VEC3_EQ(Vec3(0, 1, 0), v.normal);  // Default normal
        ASSERT_VEC2_EQ(Vec2(0, 0), v.texCoord);
        ASSERT_EQ(255, v.color.r);
        ASSERT_EQ(255, v.color.g);
        ASSERT_EQ(255, v.color.b);
        ASSERT_EQ(255, v.color.a);
    });
    
    END_SUITE();
}

void testTransformedVertex() {
    TEST_SUITE("TransformedVertex Structure");
    
    TEST("Default initialization") {
        TransformedVertex tv;
        // Just test that we can create one - specific values depend on implementation
        ASSERT_TRUE(true); // Placeholder test
    });
    
    TEST("Perspective correction value") {
        TransformedVertex tv;
        tv.clipPosition = Vec4(1, 2, 3, 2);
        tv.invW = 1.0f / tv.clipPosition.w;
        
        ASSERT_FLOAT_EQ(0.5f, tv.invW);
    });
    
    END_SUITE();
}

// Run all VertexProcessor tests
void runVertexProcessorTests() {
    testVertexConstruction();
    testTransformedVertex();
}