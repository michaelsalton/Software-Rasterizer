#include <iostream>
#include <cstdlib>
#include "TestFramework.h"

// Forward declarations for test functions
void testVec2();
void testVec3();
void testVec4();
void testMat4();

int main(int argc, char* argv[]) {
    std::cout << "Running Software Rasterizer Test Suite\n";
    std::cout << "=====================================\n";
    
    bool verbose = false;
    
    // Check for verbose flag
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-v" || std::string(argv[i]) == "--verbose") {
            verbose = true;
            break;
        }
    }
    
    if (verbose) {
        std::cout << "Verbose mode enabled\n";
        TestFramework::getInstance().setVerbose(true);
    }
    
    // Run all test suites
    testVec2();
    testVec3();
    testVec4();
    testMat4();
    
    // You can add more test suites here as you create them:
    // testRenderer();
    // testEntity();
    // etc.
    
    // Print overall summary
    RUN_ALL_TESTS();
    
    return 0;
}