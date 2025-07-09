#include <iostream>
#include "../Math/Math.h"

void testVec2() {
    std::cout << "=== Vec2 Tests ===" << std::endl;
    
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 2.0f);
    
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "a + b = " << (a + b) << std::endl;
    std::cout << "a - b = " << (a - b) << std::endl;
    std::cout << "a * 2 = " << (a * 2.0f) << std::endl;
    std::cout << "a.dot(b) = " << a.dot(b) << std::endl;
    std::cout << "a.cross(b) = " << a.cross(b) << std::endl;
    std::cout << "a.length() = " << a.length() << std::endl;
    std::cout << "a.normalized() = " << a.normalized() << std::endl;
    std::cout << "a.perpendicular() = " << a.perpendicular() << std::endl;
    
    Vec2 rotated = Math::rotate(a, Math::PI / 4.0f);  // Rotate 45 degrees
    std::cout << "a rotated 45° = " << rotated << std::endl;
    std::cout << std::endl;
}

void testVec3() {
    std::cout << "=== Vec3 Tests ===" << std::endl;
    
    Vec3 a(1.0f, 2.0f, 3.0f);
    Vec3 b(4.0f, 5.0f, 6.0f);
    
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "a + b = " << (a + b) << std::endl;
    std::cout << "a - b = " << (a - b) << std::endl;
    std::cout << "a * 2 = " << (a * 2.0f) << std::endl;
    std::cout << "a.dot(b) = " << a.dot(b) << std::endl;
    std::cout << "a.cross(b) = " << a.cross(b) << std::endl;
    std::cout << "a.length() = " << a.length() << std::endl;
    std::cout << "a.normalized() = " << a.normalized() << std::endl;
    
    Vec3 normal(0.0f, 1.0f, 0.0f);
    Vec3 incident(1.0f, -1.0f, 0.0f);
    std::cout << "incident.reflect(normal) = " << incident.reflect(normal) << std::endl;
    
    Vec3 rotatedX = Math::rotateX(a, Math::PI / 4.0f);
    std::cout << "a rotated 45° around X = " << rotatedX << std::endl;
    std::cout << std::endl;
}

void testVec4() {
    std::cout << "=== Vec4 Tests ===" << std::endl;
    
    Vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4 b(5.0f, 6.0f, 7.0f, 8.0f);
    
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "a + b = " << (a + b) << std::endl;
    std::cout << "a - b = " << (a - b) << std::endl;
    std::cout << "a * 2 = " << (a * 2.0f) << std::endl;
    std::cout << "a.dot(b) = " << a.dot(b) << std::endl;
    std::cout << "a.length() = " << a.length() << std::endl;
    std::cout << "a.normalized() = " << a.normalized() << std::endl;
    
    // Homogeneous coordinate conversion
    Vec4 homogeneous(2.0f, 4.0f, 6.0f, 2.0f);
    std::cout << "homogeneous = " << homogeneous << std::endl;
    std::cout << "homogeneous.toVec3() = " << homogeneous.toVec3() << std::endl;
    std::cout << std::endl;
}


// Uncomment to run tests
/*
int main() {
    testVec2();
    testVec3();
    testVec4();
    return 0;
}
*/