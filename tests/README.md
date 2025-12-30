# Vision-Infra Testing Guide

This directory contains comprehensive unit tests and end-to-end tests for the vision-infra library using Google Test and Google Mock frameworks.

## Test Structure

```
tests/
├── unit/               # Unit tests for individual components
│   ├── test_logger.cpp
│   ├── test_config_manager.cpp
│   ├── test_vision_utils.cpp
│   └── test_file_system.cpp
└── e2e/                # End-to-end integration tests
    └── test_integration.cpp
```

## Dependencies

- **Google Test (GTest)** - Testing framework
- **Google Mock (GMock)** - Mocking framework  
- **OpenCV** - Required for vision utilities tests
- **C++20** - Language standard

## Building and Running Tests

### Prerequisites

1. Install Google Test and Google Mock:

```bash
# Ubuntu/Debian
sudo apt-get install libgtest-dev libgmock-dev

# macOS with Homebrew
brew install googletest

# Or build from source
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

2. Make sure OpenCV is installed (required by the main library).

### Building Tests

1. Configure the build with testing enabled:

```bash
mkdir build && cd build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug ..
```

2. Build the project including tests:

```bash
make -j$(nproc)
```

### Running Tests

#### Run All Tests
```bash
# Using CTest
ctest --verbose

# Or using the convenience target
make run_all_tests
```

#### Run Unit Tests Only
```bash
# Using CTest with pattern matching
ctest --verbose -R "unit"

# Or using the convenience target
make run_unit_tests

# Or run the executable directly
./tests/unit/unit_tests
```

#### Run End-to-End Tests Only
```bash
# Using CTest with pattern matching
ctest --verbose -R "e2e"

# Or using the convenience target  
make run_e2e_tests

# Or run the executable directly
./tests/e2e/e2e_tests
```

#### Run Specific Tests
```bash
# Run tests matching a pattern
ctest --verbose -R "Logger"

# Run specific test executable with GTest filters
./tests/unit/unit_tests --gtest_filter="LoggerTest.*"
./tests/unit/unit_tests --gtest_filter="*ConfigManager*"
```

### Test Configuration Options

#### Debug vs Release
```bash
# Debug build (default for testing)
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release ..
```

#### Enable Sanitizers (Recommended for Development)
```bash
cmake -DBUILD_TESTING=ON -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug ..
```

#### Enable All Warnings
```bash
cmake -DBUILD_TESTING=ON -DENABLE_WARNINGS=ON ..
```

## Test Categories

### Unit Tests (`tests/unit/`)

#### Logger Tests (`test_logger.cpp`)
- Tests the logging interface and implementations
- Mock logger demonstrations
- LogLevel functionality
- LoggerManager operations
- **Key Features Tested:**
  - Basic logging operations
  - Log level filtering
  - Mock expectations and sequences
  - Logger manager singleton behavior

#### Configuration Manager Tests (`test_config_manager.cpp`) 
- Tests configuration loading, validation, and management
- Mock interfaces for ConfigLoader, ConfigValidator, ConfigSerializer
- **Key Features Tested:**
  - Configuration loading from various sources
  - Configuration validation with success/failure cases
  - Configuration merging
  - Custom dependency injection
  - Default implementations

#### Vision Utils Tests (`test_vision_utils.cpp`)
- Tests utility classes for computer vision operations
- **Components Tested:**
  - `StringUtils`: String manipulation and parsing
  - `InputParser`: ML-specific input parsing
  - `DrawingUtils`: OpenCV drawing operations
  - `ImageUtils`: Image preprocessing utilities
  - `PerformanceUtils`: Timing and FPS monitoring
  - `MemoryUtils`: Memory usage analysis

#### File System Tests (`test_file_system.cpp`)
- Tests file system operations with both mocks and real implementations
- **Key Features Tested:**
  - Mock file system interface demonstrations
  - Real file I/O operations
  - Directory operations
  - Path manipulations
  - Performance characteristics

### End-to-End Tests (`tests/e2e/`)

#### Integration Tests (`test_integration.cpp`)
- Tests complete workflows using real components
- **Workflows Tested:**
  - Complete processing pipeline (config → logging → image processing)
  - Configuration file loading workflow
  - Batch image processing with performance monitoring
  - Error handling and edge cases
  - String utilities integration

## Google Mock Examples

The tests demonstrate various Google Mock features:

### Basic Mocking
```cpp
class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, Log, (LogLevel level, const std::string& message), (override));
    MOCK_METHOD(void, SetLevel, (LogLevel level), (override));
    MOCK_METHOD(LogLevel, GetLevel, (), (const, override));
    MOCK_METHOD(void, Flush, (), (override));
};
```

### Expectations and Actions
```cpp
EXPECT_CALL(*mock_logger_, SetLevel(LogLevel::DEBUG))
    .Times(1);

EXPECT_CALL(*mock_logger_, GetLevel())
    .WillOnce(Return(LogLevel::DEBUG));

EXPECT_CALL(*mock_logger_, LoadFromCommandLine(_, _))
    .WillOnce(Invoke([this](int /*argc*/, const char* /*argv*/[]) {
        return std::make_unique<InferenceConfig>(*test_config_);
    }));
```

### Sequence Testing
```cpp
InSequence seq;
EXPECT_CALL(*mock_logger_, SetLevel(LogLevel::TRACE));
EXPECT_CALL(*mock_logger_, Log(LogLevel::TRACE, "First message"));
EXPECT_CALL(*mock_logger_, Log(LogLevel::DEBUG, "Second message"));
```

### Matchers
```cpp
EXPECT_THAT(result, ElementsAre("a", "b", "c", "d"));
EXPECT_THAT(files, UnorderedElementsAre("file1.txt", "file2.dat"));
EXPECT_THAT(errors, HasSubstr("model_path"));
```

## Best Practices

### Test Organization
1. **One test class per component** - Each header file gets its own test file
2. **Separate unit and integration tests** - Unit tests focus on isolated components
3. **Use descriptive test names** - `TEST_F(LoggerTest, ValidationFailure)` vs `TEST_F(LoggerTest, Test1)`

### Mock Usage
1. **Mock external dependencies** - File systems, network calls, hardware interfaces
2. **Test behavior, not implementation** - Focus on what the code does, not how
3. **Use strict mocks for critical interfaces** - Catch unexpected calls
4. **Prefer real objects for simple value objects** - Don't mock everything

### Test Data Management
1. **Use temporary directories** - Clean up after tests
2. **Create realistic test data** - Use actual image files, valid configurations
3. **Test edge cases** - Empty inputs, invalid parameters, resource exhaustion

## Troubleshooting

### Common Build Issues

**GTest not found:**
```bash
# Make sure development packages are installed
sudo apt-get install libgtest-dev googletest

# Or set CMAKE_PREFIX_PATH
cmake -DCMAKE_PREFIX_PATH=/usr/local -DBUILD_TESTING=ON ..
```

**OpenCV not found:**
```bash
# Install OpenCV development packages
sudo apt-get install libopencv-dev

# Or specify OpenCV location
cmake -DOpenCV_DIR=/path/to/opencv -DBUILD_TESTING=ON ..
```

**Link errors:**
```bash
# Clean and rebuild
make clean
make -j$(nproc)
```

### Common Test Failures

**Temporary file access issues:**
- Check filesystem permissions
- Ensure `/tmp` is writable
- Tests clean up temporary files in teardown

**Mock expectation failures:**
- Verify all EXPECT_CALL statements match actual calls
- Check parameter matching (exact values vs matchers)
- Ensure proper test setup and teardown

**Performance test flakiness:**
- Performance tests use reasonable thresholds
- Consider system load when running tests
- Use relative rather than absolute timing checks

## Adding New Tests

### Unit Test Template
```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vision-infra/your_component.hpp>

class YourComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
    }
    
    void TearDown() override {
        // Clean up
    }
    
    // Test data members
};

TEST_F(YourComponentTest, BasicFunctionality) {
    // Test implementation
    EXPECT_TRUE(true);
}
```

### Integration Test Template
```cpp
TEST_F(IntegrationTest, YourWorkflowTest) {
    logger_->Info("Starting your workflow test");
    
    // Set up test scenario
    // Execute workflow
    // Verify results
    
    logger_->Info("Your workflow test completed");
}
```

## Continuous Integration

For CI/CD systems, use:

```bash
# Configure with all safety checks
cmake -DBUILD_TESTING=ON -DENABLE_SANITIZERS=ON -DENABLE_WARNINGS=ON -DCMAKE_BUILD_TYPE=Debug ..

# Build with parallel jobs
make -j$(nproc)

# Run tests with JUnit XML output
ctest --verbose --output-junit results.xml
```