#include <iostream>
#include <cstdlib>
#include "test_framework.h"
#include "test_assertions.h"

// Forward declarations for existing test functions
void testVec2();
void testVec3();
void testVec4();
void testMat4();
void testVec3Enhanced();

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
    
    // Run existing basic tests
    std::cout << "\n" << COLOR_YELLOW << "=== Basic Math Tests ===" << COLOR_RESET << "\n";
    testVec2();
    testVec3();
    testVec4();
    testMat4();
    
    // Run enhanced tests
    std::cout << "\n" << COLOR_YELLOW << "=== Enhanced Tests ===" << COLOR_RESET << "\n";
    testVec3Enhanced();
    
    // Note about additional tests
    std::cout << "\n" << COLOR_YELLOW << "Note: See docs/testing/test_specification.md for the comprehensive test plan." << COLOR_RESET << "\n";
    
    // Print overall summary
    std::cout << "\n";
    RUN_ALL_TESTS();
    
    return 0;
}