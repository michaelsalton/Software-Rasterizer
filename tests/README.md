# Software Rasterizer Test Framework

## Overview
This test framework provides a simple and effective way to write and run unit tests for the Software Rasterizer project.

## Running Tests

### Run all tests:
```bash
make test
```

### Run tests with verbose output:
```bash
make test-verbose
```

### Clean test files:
```bash
make clean
```

## Writing Tests

### Basic Test Structure

```cpp
#include "../TestFramework.h"
#include "YourHeaderFile.h"

void testYourClass() {
    TEST_SUITE("YourClass");

    TEST("Test Name") {
        // Your test code here
        ASSERT_TRUE(condition);
        ASSERT_FLOAT_EQ(expected, actual);
    });

    TEST("Another Test") {
        // More test code
        ASSERT_FALSE(condition);
        ASSERT_EQ(expected, actual);
    });

    END_SUITE();
}
```

### Available Assertions

- `ASSERT_TRUE(condition)` - Assert condition is true
- `ASSERT_FALSE(condition)` - Assert condition is false
- `ASSERT_EQ(expected, actual)` - Assert values are equal
- `ASSERT_NE(expected, actual)` - Assert values are not equal
- `ASSERT_FLOAT_EQ(expected, actual)` - Assert floats are equal (with epsilon)
- `ASSERT_THROWS(ExceptionType, function)` - Assert function throws specific exception
- `ASSERT_NO_THROW(function)` - Assert function doesn't throw

### Adding New Test Files

1. Create your test file in the appropriate directory (e.g., `tests/Math/MyClassTest.cpp`)
2. Include the test function declaration in `RunAllTests.cpp`
3. Add the test file to the `TEST_SOURCES` variable in the Makefile
4. Call your test function in `main()` of `RunAllTests.cpp`

## Test Output

The framework provides colored output:
- Green checkmarks (✓) for passing tests
- Red X marks (✗) for failing tests
- Summary statistics for each test suite
- Overall summary at the end

## Features

- Simple macro-based API
- Execution time tracking
- Colored console output
- Verbose mode for detailed results
- Test suite organization
- Automatic test discovery within suites