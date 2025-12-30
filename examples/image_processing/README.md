# Image Processing Example

This example demonstrates the computer vision utilities provided by vision-infra, including image processing, drawing operations, performance monitoring, and memory management.

## Features Demonstrated

- **String utilities** for text processing
- **Input parsing** for ML model configurations
- **Image processing** operations (resize, crop, normalize)
- **Drawing utilities** for annotations
- **Performance monitoring** (timing, FPS measurement)
- **Memory calculations** for images and tensors

## Building and Running

```bash
# Build the example
cd build
make image_processing_example

# Run the demo
./examples/image_processing/image_processing_example
```

## Outputs

The demo creates sample images in a temporary directory showing:

1. **Original test image** with geometric shapes
2. **Resized images** with aspect ratio preservation
3. **Cropped images** with different dimensions
4. **Normalized images** using ImageNet parameters
5. **Channel order conversions** (HWC ↔ CHW)
6. **Annotated images** with bounding boxes, labels, polygons, and keypoints

## Key Components

### String Utilities

```cpp
// String manipulation
auto files = StringUtils::Split("img1.jpg,img2.png,img3.tiff", ',');
auto joined = StringUtils::Join({"person", "car", "bike"}, " | ");
auto trimmed = StringUtils::Trim("  text with spaces  ");
auto lower = StringUtils::ToLower("UPPERCASE TEXT");

// Prefix/suffix checking
bool is_image = StringUtils::EndsWith(filename, ".jpg");
bool is_model = StringUtils::StartsWith(filename, "model_");
```

### Input Parsing for ML

```cpp
// Parse model input sizes
auto sizes = InputParser::ParseInputSizes("1,3,224,224;1,3,256,256");

// Parse normalization parameters
auto mean = InputParser::ParseFloatList("0.485,0.456,0.406");
auto std_dev = InputParser::ParseFloatList("0.229,0.224,0.225");

// Parse class information
auto class_ids = InputParser::ParseIntList("1,2,5,10,15,20");
auto class_names = InputParser::ParseStringList("person,car,bicycle,dog");
```

### Image Processing

```cpp
// Resize with aspect ratio preservation
cv::Mat resized = ImageUtils::ResizeKeepAspectRatio(image, cv::Size(224, 224));

// Center crop
cv::Mat cropped = ImageUtils::CenterCrop(image, cv::Size(200, 200));

// Normalize for ML models (ImageNet style)
std::vector<float> mean = {0.485f, 0.456f, 0.406f};
std::vector<float> std = {0.229f, 0.224f, 0.225f};
cv::Mat normalized = ImageUtils::Normalize(image, mean, std);

// Channel order conversion
cv::Mat chw = ImageUtils::HwcToChw(image);  // For PyTorch/ONNX
cv::Mat hwc = ImageUtils::ChwToHwc(chw);    // Back to OpenCV format
```

### Drawing and Annotations

```cpp
// Generate random colors
auto colors = DrawingUtils::GenerateRandomColors(10, 42);

// Draw bounding boxes
DrawingUtils::DrawBoundingBox(image, cv::Rect(x, y, w, h), color, thickness);
DrawingUtils::DrawBoundingBox(image, x, y, width, height, color, thickness);

// Draw labels with confidence scores
DrawingUtils::DrawLabel(image, "Person 95%", 0.95f, x, y, color);

// Draw polygons (segmentation masks)
std::vector<cv::Point> polygon = {{100,100}, {150,80}, {200,120}};
DrawingUtils::DrawPolygon(image, polygon, color, thickness);
DrawingUtils::DrawFilledPolygon(image, polygon, color);

// Draw keypoints (pose estimation)
std::vector<cv::Point2f> keypoints = {{x1,y1}, {x2,y2}, {x3,y3}};
DrawingUtils::DrawKeypoints(image, keypoints, color, radius);

// Get text dimensions
cv::Size text_size = DrawingUtils::GetTextSize("Hello", font, scale, thickness);
```

### Performance Monitoring

```cpp
// Time operations
PerformanceUtils::Timer timer;
timer.Start();
// ... do work ...
timer.Stop();
double elapsed_ms = timer.GetElapsedMs();

// Measure FPS
PerformanceUtils::FPSCounter fps_counter(30);  // 30-frame window
for (int frame = 0; frame < num_frames; ++frame) {
    fps_counter.Update();
    // ... process frame ...
    double current_fps = fps_counter.GetCurrentFPS();
    double average_fps = fps_counter.GetAverageFPS();
}
```

### Memory Management

```cpp
// Calculate image memory usage
size_t image_memory = MemoryUtils::GetImageMemorySize(image);

// Calculate tensor memory usage
std::vector<int64_t> shape = {1, 3, 224, 224};
size_t tensor_memory = MemoryUtils::GetTensorMemorySize(shape, sizeof(float));

// Format memory sizes
std::string formatted = MemoryUtils::FormatBytes(1024 * 1024);  // "1.00 MB"

// System memory information
size_t system_memory = MemoryUtils::GetSystemMemoryUsage();
size_t process_memory = MemoryUtils::GetProcessMemoryUsage();
```

## Example Output

```
=== Vision Infrastructure Image Processing Demo ===

=== String Utilities Demo ===

1. String splitting and joining:
Original string: image1.jpg,image2.png,image3.tiff,image4.bmp
Split into 4 parts:
   [0] image1.jpg
   [1] image2.png
   [2] image3.tiff
   [3] image4.bmp
Joined labels: person | car | bicycle | dog | cat

=== Image Utilities Demo ===

1. Creating and processing test images:
Created test image: 480x360

2. Resize keeping aspect ratio:
   Resized to 640x480 -> actual: 640x480
   Resized to 800x600 -> actual: 800x600
   Resized to 224x224 -> actual: 224x168
   Resized to 416x416 -> actual: 416x312

=== Drawing Utilities Demo ===

1. Generating random colors:
Generated 10 random colors
   Color 0: RGB(114, 119, 132)
   Color 1: RGB(103, 191, 231)
   ...

=== Performance Utilities Demo ===

1. Timing operations:
   Image processing completed in 15.2 ms (0.0152 seconds)

2. FPS measurement:
   Frame 10: Current FPS = 55.2, Average FPS = 58.1
   Frame 20: Current FPS = 52.8, Average FPS = 56.4
   ...
```

## Common Use Cases

### ML Model Preprocessing Pipeline

```cpp
// Load and prepare image for inference
cv::Mat image = cv::imread("input.jpg");

// Resize to model input size
cv::Mat resized = ImageUtils::ResizeKeepAspectRatio(image, cv::Size(224, 224));

// Normalize using ImageNet parameters
cv::Mat normalized = ImageUtils::Normalize(resized, {0.485,0.456,0.406}, {0.229,0.224,0.225});

// Convert to CHW format for ONNX/PyTorch
cv::Mat chw = ImageUtils::HwcToChw(normalized);
```

### Object Detection Visualization

```cpp
// Draw detection results
auto colors = DrawingUtils::GenerateRandomColors(num_classes);
for (const auto& detection : detections) {
    cv::Scalar color = colors[detection.class_id];
    
    // Draw bounding box
    DrawingUtils::DrawBoundingBox(image, detection.bbox, color);
    
    // Draw label with confidence
    std::string label = class_names[detection.class_id] + " " + 
                       std::to_string(int(detection.confidence * 100)) + "%";
    DrawingUtils::DrawLabel(image, label, detection.confidence,
                           detection.bbox.x, detection.bbox.y - 5, color);
}
```

### Performance Benchmarking

```cpp
PerformanceUtils::Timer timer;
PerformanceUtils::FPSCounter fps(100);

for (int i = 0; i < num_iterations; ++i) {
    timer.Start();
    
    // Run inference
    auto result = model.inference(input);
    
    timer.Stop();
    fps.Update();
    
    if (i % 100 == 0) {
        std::cout << "Iteration " << i 
                  << ": " << timer.GetElapsedMs() << "ms, "
                  << "FPS: " << fps.GetAverageFPS() << std::endl;
    }
}
```

## Memory Optimization

```cpp
// Calculate memory requirements before allocation
std::vector<int64_t> batch_shape = {batch_size, 3, 224, 224};
size_t required_memory = MemoryUtils::GetTensorMemorySize(batch_shape, sizeof(float));

std::cout << "Batch processing requires: " 
          << MemoryUtils::FormatBytes(required_memory) << std::endl;

// Monitor memory usage during processing
size_t initial_memory = MemoryUtils::GetProcessMemoryUsage();
// ... process data ...
size_t final_memory = MemoryUtils::GetProcessMemoryUsage();
size_t memory_increase = final_memory - initial_memory;

std::cout << "Processing used additional: " 
          << MemoryUtils::FormatBytes(memory_increase) << std::endl;
```