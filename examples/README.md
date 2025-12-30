# Vision-Infra Examples

This directory contains comprehensive examples demonstrating the usage of the vision-infra library for computer vision and machine learning applications.

## Available Examples

### 📁 [basic_config](basic_config/)
**Command-line parsing and configuration management**

Demonstrates how to:
- Parse command-line arguments
- Load configuration from files and environment variables
- Validate configuration settings
- Merge configurations from multiple sources
- Use custom parameters
- Print and manage configuration

```bash
./examples/basic_config/basic_config_example --model yolov5 --batch-size 8 --verbose
```

### 📁 [logging_demo](logging_demo/)
**Logging setup and usage patterns**

Demonstrates how to:
- Use different log levels (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
- Configure loggers with timestamps and custom patterns
- Output logs to files and console
- Manage multiple named loggers
- Parse log levels from strings
- Use thread-safe logging

```bash
./examples/logging_demo/logging_demo_example
```

### 📁 [image_processing](image_processing/)
**Computer vision utilities and operations**

Demonstrates how to:
- Process images with OpenCV utilities
- Resize images while keeping aspect ratio
- Crop and normalize images for ML models
- Draw bounding boxes, labels, and annotations
- Generate random colors for visualizations
- Parse input configurations for ML models
- Monitor performance and memory usage

```bash
./examples/image_processing/image_processing_example
```

### 📁 [file_operations](file_operations/)
**File system operations and management**

Demonstrates how to:
- Perform basic file operations (create, read, write, copy, move, delete)
- Handle different image file formats
- Manage configuration files (JSON, INI, YAML)
- Implement log file rotation strategies
- Monitor file system usage and permissions
- Organize datasets and model files

```bash
./examples/file_operations/file_operations_example
```

## Building Examples

### Prerequisites

Make sure you have the required dependencies:
- **CMake 3.25+**
- **C++20 compatible compiler**
- **OpenCV 4.x** (for image processing examples)

### Build Configuration

```bash
# Configure with examples enabled
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release ..

# Build all examples
make examples

# Or build individual examples
make basic_config_example
make logging_demo_example
make image_processing_example
make file_operations_example
```

### Build Options

```bash
# Release build (optimized)
cmake -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release ..

# Debug build (with debugging symbols)
cmake -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug ..

# Build with sanitizers (for development)
cmake -DBUILD_EXAMPLES=ON -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug ..

# Build with all warnings
cmake -DBUILD_EXAMPLES=ON -DENABLE_WARNINGS=ON ..
```

## Running Examples

After building, the examples will be located in:
```
build/examples/
├── basic_config/basic_config_example
├── logging_demo/logging_demo_example
├── image_processing/image_processing_example
└── file_operations/file_operations_example
```

### Run All Examples

```bash
# Run each example in sequence
cd build

echo "=== Basic Config Example ==="
./examples/basic_config/basic_config_example --help

echo -e "\n=== Logging Demo ==="
./examples/logging_demo/logging_demo_example

echo -e "\n=== Image Processing ==="
./examples/image_processing/image_processing_example

echo -e "\n=== File Operations ==="
./examples/file_operations/file_operations_example
```

### Example with Custom Parameters

```bash
# Configuration example with custom settings
./examples/basic_config/basic_config_example \
    --server 192.168.1.100 \
    --port 9000 \
    --model my_detection_model \
    --model-type onnx \
    --batch-size 16 \
    --threads 8 \
    --source /path/to/images \
    --confidence 0.7 \
    --verbose
```

## Integration Examples

### ML Pipeline Example

Combining multiple utilities for a complete ML pipeline:

```cpp
#include <vision-infra/vision-infra.hpp>

int main(int argc, char* argv[]) {
    // 1. Configuration management
    auto config_manager = vision_infra::config::ConfigManager::Create();
    auto config = config_manager->LoadFromCommandLine(argc, argv);
    
    // 2. Logging setup
    auto logger = vision_infra::core::LoggerManager::GetLogger("pipeline");
    logger->SetLevel(vision_infra::core::LogLevel::INFO);
    
    // 3. Image processing
    cv::Mat image = cv::imread(config->GetSource());
    auto resized = vision_infra::utils::ImageUtils::ResizeKeepAspectRatio(
        image, cv::Size(640, 640));
    
    // 4. Performance monitoring
    vision_infra::utils::PerformanceUtils::Timer timer;
    timer.Start();
    
    // ... ML inference here ...
    
    timer.Stop();
    logger->Info("Processing completed in " + 
                std::to_string(timer.GetElapsedMs()) + "ms");
    
    // 5. Result visualization
    auto colors = vision_infra::utils::DrawingUtils::GenerateRandomColors(10);
    // ... draw detections ...
    
    return 0;
}
```

### Computer Vision Application

```cpp
// Load and preprocess images
std::vector<cv::Mat> images;
for (const auto& file : image_files) {
    cv::Mat img = cv::imread(file);
    
    // Resize and normalize
    auto resized = ImageUtils::ResizeKeepAspectRatio(img, cv::Size(224, 224));
    auto normalized = ImageUtils::Normalize(resized, imagenet_mean, imagenet_std);
    
    images.push_back(normalized);
}

// Process batch
auto batch_tensor = ImageUtils::BatchImages(images);

// Monitor performance
PerformanceUtils::FPSCounter fps_counter(100);
for (const auto& image : images) {
    fps_counter.Update();
    
    // ... process image ...
    
    if (frame_count % 100 == 0) {
        logger->Info("Average FPS: " + std::to_string(fps_counter.GetAverageFPS()));
    }
}
```

## Output Examples

### Basic Config Output

```
=== Vision Infrastructure Configuration Example ===

1. Loading configuration from command line arguments...
2. Configuration loaded successfully!

3. Current Configuration:
   ========================
Configuration:
  Server: 192.168.1.100:9000 (http)
  Model: my_detection_model (onnx)
  Source: /path/to/images
  Batch Size: 16
  Threads: 8
  Confidence Threshold: 0.7
  Verbose: true

4. Validating configuration...
   ✓ Configuration is valid!

✓ Configuration example completed successfully!
```

### Image Processing Output

```
=== Vision Infrastructure Image Processing Demo ===

=== String Utilities Demo ===
1. String splitting and joining:
Original string: image1.jpg,image2.png,image3.tiff,image4.bmp
Split into 4 parts:
   [0] image1.jpg
   [1] image2.png

=== Image Utilities Demo ===
1. Creating and processing test images:
Created test image: 480x360

2. Resize keeping aspect ratio:
   Resized to 640x480 -> actual: 640x480
   Resized to 224x224 -> actual: 224x168

Output images saved to: /tmp/vision_infra_image_demo
✓ Image processing demo completed successfully!
```

## Error Handling Examples

Each example includes comprehensive error handling:

```cpp
try {
    auto config = config_manager->LoadFromCommandLine(argc, argv);
    if (!config) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    bool is_valid = config_manager->ValidateConfig(*config);
    if (!is_valid) {
        std::string errors = config_manager->GetValidationErrors(*config);
        std::cerr << "Configuration validation failed: " << errors << std::endl;
        return 1;
    }
    
    // ... continue with application logic ...
    
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

## Best Practices Demonstrated

### 1. **RAII Resource Management**
- Automatic cleanup of temporary files and directories
- Proper file handle management
- Exception-safe resource handling

### 2. **Error Handling**
- Comprehensive exception catching
- Graceful degradation when optional features fail
- Clear error messages and logging

### 3. **Performance Monitoring**
- Timing critical operations
- Memory usage tracking
- FPS measurement for real-time applications

### 4. **Logging Strategy**
- Appropriate log levels for different message types
- Structured logging with timestamps
- Multiple logger instances for different components

### 5. **Configuration Management**
- Validation of all configuration parameters
- Support for multiple configuration sources
- Default values and environment variable overrides

## Troubleshooting

### Common Build Issues

**OpenCV not found:**
```bash
# Install OpenCV development packages
sudo apt-get install libopencv-dev

# Or specify OpenCV location
cmake -DOpenCV_DIR=/path/to/opencv -DBUILD_EXAMPLES=ON ..
```

**C++20 features not available:**
```bash
# Ensure you have a recent compiler
gcc --version  # Should be 8.0 or later
clang --version  # Should be 10.0 or later

# Or specify compiler explicitly
cmake -DCMAKE_CXX_COMPILER=g++-10 -DBUILD_EXAMPLES=ON ..
```

### Runtime Issues

**Missing shared libraries:**
```bash
# Check library dependencies
ldd ./examples/image_processing/image_processing_example

# Update library path if needed
export LD_LIBRARY_PATH=/path/to/opencv/lib:$LD_LIBRARY_PATH
```

**Permission errors:**
- Ensure write permissions for temporary directories
- Check that output directories are accessible
- Verify file system space availability

## Contributing

When adding new examples:

1. **Follow the existing structure**: Create subdirectory with `main.cpp`, `CMakeLists.txt`, and `README.md`
2. **Include comprehensive documentation**: Explain what the example demonstrates
3. **Add proper error handling**: Don't assume inputs are always valid
4. **Use temporary directories**: Clean up after yourself
5. **Update this README**: Add your example to the list above

## Further Reading

- [Vision-Infra API Documentation](../docs/)
- [Configuration Reference](../docs/configuration.md)
- [Logging Guide](../docs/logging.md)
- [Performance Optimization](../docs/performance.md)