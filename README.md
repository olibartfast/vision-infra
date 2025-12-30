# vision-infra

A C++ library providing infrastructure components for computer vision and machine learning applications.

## Overview

vision-infra is designed to be a reusable foundation for building inference applications across different ML frameworks and backends. It provides common functionality that most CV/ML applications need:

- **Configuration Management**: Command-line parsing, environment variables, file-based config
- **Logging**: Structured logging with multiple outputs and levels  
- **File System**: Cross-platform file operations with abstractions for testing
- **Vision Utilities**: Image processing, drawing, performance monitoring, and parsing helpers

## Features

### Configuration (`vision_infra::config`)
- Command-line argument parsing using OpenCV's CommandLineParser
- Environment variable loading with sensible defaults
- Validation and error reporting
- Custom parameter support for extensibility
- Multiple configuration source merging

### Core Infrastructure (`vision_infra::core`)
- Thread-safe logging with multiple outputs (console, file)
- Configurable log levels, patterns, and formatting
- File system abstraction for cross-platform compatibility
- Support for image, video, and model file detection

### Vision Utilities (`vision_infra::utils`)
- String manipulation and parsing utilities
- Input parsing for ML tensor shapes and parameters
- Computer vision drawing functions (bounding boxes, labels, polygons)
- Image preprocessing (resize, crop, normalize, format conversion)
- Performance monitoring (timers, FPS counters)
- Memory usage utilities

## Quick Start

### Installation

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

### Basic Usage

```cpp
#include <vision-infra/vision-infra.hpp>

using namespace vision_infra;

int main(int argc, const char* argv[]) {
    // Load configuration from command line
    auto config = config::ConfigManager::loadFromCommandLine(argc, argv);
    if (!config) return 1;
    
    // Set up logging
    auto logger = core::LoggerManager::getLogger("myapp");
    logger->setLevel(core::LoggerManager::parseLogLevel(config->log_level));
    
    // Use file system utilities
    auto fs = core::FileSystemUtils::getDefault();
    if (core::FileSystemUtils::isImageFile(config->source)) {
        logger->info("Processing image file: " + config->source);
    }
    
    // Parse input dimensions
    if (!config->input_sizes.empty()) {
        logger->info("Input shapes configured");
        for (const auto& shape : config->input_sizes) {
            logger->info("Shape: " + utils::StringUtils::join(
                std::vector<std::string>(shape.begin(), shape.end()), ","));
        }
    }
    
    return 0;
}
```

## CMake Integration

### Using find_package

```cmake
find_package(vision-infra REQUIRED)
target_link_libraries(your_target vision-infra::vision-infra)
```

### Using FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
  vision-infra
  GIT_REPOSITORY https://github.com/your-org/vision-infra.git
  GIT_TAG main
)
FetchContent_MakeAvailable(vision-infra)

target_link_libraries(your_target vision-infra::vision-infra)
```

## Dependencies

- **OpenCV** (>= 4.0): Core image processing and command line parsing
- **C++20**: Modern C++ features
- **CMake** (>= 3.20): Build system

## Examples

See the `examples/` directory for complete usage examples:

- `basic_config`: Command-line parsing and configuration
- `logging_demo`: Logging setup and usage
- `image_processing`: Computer vision utilities
- `file_operations`: File system operations

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## License

MIT License - see LICENSE file for details.

## Related Projects

- [vision-core](https://github.com/olibartfast/vision-core): Model preprocessing and postprocessing
- [tritonic](https://github.com/olibartfast/tritonic): Triton Inference Server C++ client