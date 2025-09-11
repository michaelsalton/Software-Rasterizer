# Software Rasterizer Test Suite

## Overview
The test suite provides comprehensive testing for all components of the software rasterizer, from basic math operations to complex rendering pipelines.

## Running Tests

### Basic Usage
```bash
make test      # Run all tests
make test-verbose  # Run with detailed output
```

### Test Framework Features

#### Enhanced Assertions
The test framework now includes specialized assertions for graphics programming:

- **Vector/Matrix Assertions**: `ASSERT_VEC2_EQ`, `ASSERT_VEC3_EQ`, `ASSERT_VEC4_EQ`, `ASSERT_MAT4_NEAR`
- **Float Comparison**: `ASSERT_FLOAT_EQ` with epsilon tolerance
- **Range Validation**: `ASSERT_IN_RANGE(value, min, max)`
- **Performance Testing**: `ASSERT_PERFORMANCE(code, maxTimeMs)`

#### Benchmarking
```cpp
BENCHMARK("Operation name", iterations, {
    // Code to benchmark
});
```

## Test Structure

### Current Tests
- `tests/Math/` - Basic math tests for Vec2, Vec3, Vec4, Mat4
- `tests/Math/vec3_enhanced_test.cpp` - Example of enhanced testing features

### Planned Tests (see `docs/testing/test_specification.md`)
- Pipeline component tests (vertex processing, clipping, rasterization)
- Rendering validation tests
- Performance benchmarks
- Integration tests

## Example Enhanced Test

```cpp
TEST("Vector comparison with custom assertions") {
    Vec3 v1(1.0f, 2.0f, 3.0f);
    Vec3 v2(1.0f, 2.0f, 3.0f);
    
    ASSERT_VEC3_EQ(v1, v2);  // Exact equality
    ASSERT_VEC3_NEAR(v1, v2, 0.001f);  // Near equality with epsilon
});

TEST("Performance benchmark") {
    BENCHMARK("Vec3 operations", 100000, {
        Vec3 result = v1.cross(v2);
    });
});
```

## Test Coverage Goals

- Unit test coverage: >80%
- Critical path coverage: 100%
- Performance regression detection

## Contributing

When adding new tests:
1. Use the enhanced assertion macros where applicable
2. Include performance benchmarks for critical operations
3. Follow the naming convention: `test_<module>_<feature>.cpp`
4. Update this README with new test locations