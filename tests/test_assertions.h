#pragma once

#include "test_framework.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"
#include <chrono>

// Extended assertions for math types
#define ASSERT_VEC2_EQ(expected, actual) \
    do { \
        auto e = expected; \
        auto a = actual; \
        if (!(e.x == a.x && e.y == a.y)) { \
            std::stringstream ss; \
            ss << "Expected Vec2(" << e.x << ", " << e.y << "), "; \
            ss << "Actual Vec2(" << a.x << ", " << a.y << ")"; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_VEC2_NEAR(expected, actual, epsilon) \
    do { \
        auto e = expected; \
        auto a = actual; \
        float eps = epsilon; \
        if (std::abs(e.x - a.x) > eps || std::abs(e.y - a.y) > eps) { \
            std::stringstream ss; \
            ss << "Expected Vec2(" << e.x << ", " << e.y << "), "; \
            ss << "Actual Vec2(" << a.x << ", " << a.y << "), "; \
            ss << "Epsilon: " << eps; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_VEC3_EQ(expected, actual) \
    do { \
        auto e = expected; \
        auto a = actual; \
        if (!(e.x == a.x && e.y == a.y && e.z == a.z)) { \
            std::stringstream ss; \
            ss << "Expected Vec3(" << e.x << ", " << e.y << ", " << e.z << "), "; \
            ss << "Actual Vec3(" << a.x << ", " << a.y << ", " << a.z << ")"; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_VEC3_NEAR(expected, actual, epsilon) \
    do { \
        auto e = expected; \
        auto a = actual; \
        float eps = epsilon; \
        if (std::abs(e.x - a.x) > eps || std::abs(e.y - a.y) > eps || std::abs(e.z - a.z) > eps) { \
            std::stringstream ss; \
            ss << "Expected Vec3(" << e.x << ", " << e.y << ", " << e.z << "), "; \
            ss << "Actual Vec3(" << a.x << ", " << a.y << ", " << a.z << "), "; \
            ss << "Epsilon: " << eps; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_VEC4_EQ(expected, actual) \
    do { \
        auto e = expected; \
        auto a = actual; \
        if (!(e.x == a.x && e.y == a.y && e.z == a.z && e.w == a.w)) { \
            std::stringstream ss; \
            ss << "Expected Vec4(" << e.x << ", " << e.y << ", " << e.z << ", " << e.w << "), "; \
            ss << "Actual Vec4(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")"; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_VEC4_NEAR(expected, actual, epsilon) \
    do { \
        auto e = expected; \
        auto a = actual; \
        float eps = epsilon; \
        if (std::abs(e.x - a.x) > eps || std::abs(e.y - a.y) > eps || \
            std::abs(e.z - a.z) > eps || std::abs(e.w - a.w) > eps) { \
            std::stringstream ss; \
            ss << "Expected Vec4(" << e.x << ", " << e.y << ", " << e.z << ", " << e.w << "), "; \
            ss << "Actual Vec4(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << "), "; \
            ss << "Epsilon: " << eps; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_MAT4_NEAR(expected, actual, epsilon) \
    do { \
        auto e = expected; \
        auto a = actual; \
        float eps = epsilon; \
        bool equal = true; \
        for (int i = 0; i < 16; ++i) { \
            if (std::abs(e.m[i] - a.m[i]) > eps) { \
                equal = false; \
                break; \
            } \
        } \
        if (!equal) { \
            std::stringstream ss; \
            ss << "Matrix mismatch (epsilon: " << eps << ")"; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_IN_RANGE(value, min, max) \
    do { \
        auto __v = value; \
        auto __mn = min; \
        auto __mx = max; \
        if (__v < __mn || __v > __mx) { \
            std::stringstream ss; \
            ss << "Value " << __v << " not in range [" << __mn << ", " << __mx << "]"; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

// Performance testing utilities
class PerformanceTimer {
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsedMs() {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(endTime - startTime).count();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

#define BENCHMARK(name, iterations, code) \
    do { \
        PerformanceTimer timer; \
        timer.start(); \
        for (int i = 0; i < iterations; ++i) { \
            code; \
        } \
        double elapsed = timer.elapsedMs(); \
        double perIteration = elapsed / iterations; \
        std::cout << "  Benchmark [" << name << "]: " \
                  << elapsed << " ms total, " \
                  << perIteration << " ms/iteration" \
                  << " (" << iterations << " iterations)" << std::endl; \
    } while(0)

#define ASSERT_PERFORMANCE(code, maxTimeMs) \
    do { \
        PerformanceTimer timer; \
        timer.start(); \
        code; \
        double elapsed = timer.elapsedMs(); \
        if (elapsed > maxTimeMs) { \
            std::stringstream ss; \
            ss << "Performance assertion failed: " << elapsed << " ms > " << maxTimeMs << " ms"; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)