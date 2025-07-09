#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cmath>
#include <chrono>

// ANSI color codes for better output
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RESET   "\033[0m"

class TestFramework {
public:
    struct TestResult {
        std::string testName;
        bool passed;
        std::string errorMessage;
        double executionTime;
    };

    struct TestSuite {
        std::string name;
        std::vector<TestResult> results;
        int passed = 0;
        int failed = 0;
    };

private:
    static TestFramework* instance;
    std::vector<TestSuite> suites;
    TestSuite* currentSuite = nullptr;
    bool verbose = false;
    float epsilon = 1e-5f;

    TestFramework() = default;

public:
    static TestFramework& getInstance() {
        if (!instance) {
            instance = new TestFramework();
        }
        return *instance;
    }

    void setVerbose(bool v) { verbose = v; }
    void setEpsilon(float e) { epsilon = e; }

    void beginSuite(const std::string& suiteName) {
        suites.emplace_back();
        currentSuite = &suites.back();
        currentSuite->name = suiteName;
        
        std::cout << "\n" << COLOR_BLUE << "=== Test Suite: " << suiteName << " ===" << COLOR_RESET << "\n";
    }

    void endSuite() {
        if (!currentSuite) return;
        
        std::cout << "\nSuite Summary: ";
        if (currentSuite->failed == 0) {
            std::cout << COLOR_GREEN << "ALL TESTS PASSED";
        } else {
            std::cout << COLOR_RED << currentSuite->failed << " FAILED";
        }
        std::cout << " (" << currentSuite->passed << "/" 
                  << (currentSuite->passed + currentSuite->failed) << ")" << COLOR_RESET << "\n";
        
        currentSuite = nullptr;
    }

    template<typename Func>
    void test(const std::string& testName, Func testFunc) {
        if (!currentSuite) {
            std::cerr << "Error: No test suite active. Call beginSuite() first.\n";
            return;
        }

        TestResult result;
        result.testName = testName;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            testFunc();
            result.passed = true;
            currentSuite->passed++;
            
            if (verbose) {
                std::cout << COLOR_GREEN << "✓ " << COLOR_RESET << testName;
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
            currentSuite->failed++;
            
            std::cout << COLOR_RED << "✗ " << COLOR_RESET << testName;
            std::cout << "\n  " << COLOR_RED << "Error: " << e.what() << COLOR_RESET;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        if (verbose || !result.passed) {
            std::cout << " (" << result.executionTime << " ms)\n";
        }
        
        currentSuite->results.push_back(result);
    }

    void printSummary() {
        std::cout << "\n" << COLOR_BLUE << "=== OVERALL TEST SUMMARY ===" << COLOR_RESET << "\n";
        
        int totalPassed = 0;
        int totalFailed = 0;
        
        for (const auto& suite : suites) {
            totalPassed += suite.passed;
            totalFailed += suite.failed;
            
            std::cout << "\n" << suite.name << ": ";
            if (suite.failed == 0) {
                std::cout << COLOR_GREEN << "PASSED";
            } else {
                std::cout << COLOR_RED << "FAILED";
            }
            std::cout << " (" << suite.passed << "/" << (suite.passed + suite.failed) << ")" << COLOR_RESET;
            
            if (suite.failed > 0) {
                std::cout << "\n  Failed tests:";
                for (const auto& result : suite.results) {
                    if (!result.passed) {
                        std::cout << "\n    - " << result.testName;
                        if (!result.errorMessage.empty()) {
                            std::cout << ": " << result.errorMessage;
                        }
                    }
                }
            }
        }
        
        std::cout << "\n\nTotal: ";
        if (totalFailed == 0) {
            std::cout << COLOR_GREEN << "ALL TESTS PASSED";
        } else {
            std::cout << COLOR_RED << totalFailed << " FAILED";
        }
        std::cout << " (" << totalPassed << "/" << (totalPassed + totalFailed) << ")" << COLOR_RESET << "\n\n";
    }

    // Assertion helpers
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error(message.empty() ? "Assertion failed" : message);
        }
    }

    void assertFalse(bool condition, const std::string& message = "") {
        assertTrue(!condition, message);
    }

    template<typename T>
    void assertEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected != actual) {
            std::stringstream ss;
            ss << "Expected: " << expected << ", Actual: " << actual;
            if (!message.empty()) ss << " - " << message;
            throw std::runtime_error(ss.str());
        }
    }

    template<typename T>
    void assertNotEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected == actual) {
            std::stringstream ss;
            ss << "Expected values to be different, but both were: " << actual;
            if (!message.empty()) ss << " - " << message;
            throw std::runtime_error(ss.str());
        }
    }

    void assertFloatEqual(float expected, float actual, const std::string& message = "") {
        if (std::abs(expected - actual) > epsilon) {
            std::stringstream ss;
            ss << "Expected: " << expected << ", Actual: " << actual 
               << " (difference: " << std::abs(expected - actual) << ", epsilon: " << epsilon << ")";
            if (!message.empty()) ss << " - " << message;
            throw std::runtime_error(ss.str());
        }
    }

    template<typename T>
    void assertThrows(std::function<void()> func, const std::string& message = "") {
        bool threw = false;
        try {
            func();
        } catch (const T&) {
            threw = true;
        } catch (...) {
            throw std::runtime_error("Wrong exception type thrown");
        }
        
        if (!threw) {
            throw std::runtime_error(message.empty() ? "Expected exception was not thrown" : message);
        }
    }

    void assertNoThrow(std::function<void()> func, const std::string& message = "") {
        try {
            func();
        } catch (const std::exception& e) {
            std::stringstream ss;
            ss << "Unexpected exception: " << e.what();
            if (!message.empty()) ss << " - " << message;
            throw std::runtime_error(ss.str());
        } catch (...) {
            throw std::runtime_error("Unexpected exception thrown");
        }
    }
};

inline TestFramework* TestFramework::instance = nullptr;

// Convenience macros
#define TEST_SUITE(name) TestFramework::getInstance().beginSuite(name)
#define END_SUITE() TestFramework::getInstance().endSuite()
#define TEST(name) TestFramework::getInstance().test(name, [&]()
#define ASSERT_TRUE(cond) TestFramework::getInstance().assertTrue(cond, #cond)
#define ASSERT_FALSE(cond) TestFramework::getInstance().assertFalse(cond, #cond)
#define ASSERT_EQ(expected, actual) TestFramework::getInstance().assertEqual(expected, actual)
#define ASSERT_NE(expected, actual) TestFramework::getInstance().assertNotEqual(expected, actual)
#define ASSERT_FLOAT_EQ(expected, actual) TestFramework::getInstance().assertFloatEqual(expected, actual)
#define ASSERT_THROWS(type, func) TestFramework::getInstance().assertThrows<type>(func)
#define ASSERT_NO_THROW(func) TestFramework::getInstance().assertNoThrow(func)
#define RUN_ALL_TESTS() TestFramework::getInstance().printSummary()