#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <vision-infra/utils/VisionUtils.hpp>
#include <vision-infra/core/Logger.hpp>
#include <opencv2/opencv.hpp>

using namespace vision_infra::utils;
using namespace vision_infra::core;

// Create a sample test image
cv::Mat CreateTestImage(const cv::Size& size) {
    cv::Mat image = cv::Mat::zeros(size, CV_8UC3);
    
    // Create a colorful test pattern
    // Background gradient
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(
                static_cast<uchar>(x * 255 / image.cols),
                static_cast<uchar>(y * 255 / image.rows),
                128
            );
        }
    }
    
    // Add some geometric shapes
    cv::rectangle(image, cv::Point(50, 50), cv::Point(150, 120), cv::Scalar(0, 255, 0), 3);
    cv::circle(image, cv::Point(250, 180), 40, cv::Scalar(255, 0, 0), -1);
    cv::rectangle(image, cv::Point(180, 250), cv::Point(280, 320), cv::Scalar(0, 0, 255), -1);
    cv::ellipse(image, cv::Point(400, 150), cv::Size(60, 30), 45, 0, 360, cv::Scalar(255, 255, 0), 2);
    
    return image;
}

void DemonstrateStringUtilities() {
    std::cout << "=== String Utilities Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("string_utils");
    
    std::cout << "1. String splitting and joining:\n";
    
    // Demonstrate string splitting
    std::string file_list = "image1.jpg,image2.png,image3.tiff,image4.bmp";
    auto files = StringUtils::Split(file_list, ',');
    
    logger->Info("Original string: " + file_list);
    logger->Info("Split into " + std::to_string(files.size()) + " parts:");
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << "   [" << i << "] " << files[i] << "\n";
    }
    
    // Demonstrate string joining
    std::vector<std::string> labels = {"person", "car", "bicycle", "dog", "cat"};
    std::string joined = StringUtils::Join(labels, " | ");
    logger->Info("Joined labels: " + joined);
    
    std::cout << "\n2. String transformations:\n";
    
    std::string mixed_case = "  Mixed Case String With Spaces  ";
    std::cout << "   Original: '" << mixed_case << "'\n";
    std::cout << "   Trimmed: '" << StringUtils::Trim(mixed_case) << "'\n";
    std::cout << "   Lower: '" << StringUtils::ToLower(mixed_case) << "'\n";
    std::cout << "   Upper: '" << StringUtils::ToUpper(mixed_case) << "'\n";
    
    std::cout << "\n3. String prefix/suffix checking:\n";
    
    std::string filename = "model_weights.onnx";
    bool is_onnx = StringUtils::EndsWith(filename, ".onnx");
    bool starts_model = StringUtils::StartsWith(filename, "model");
    
    std::cout << "   File: " << filename << "\n";
    std::cout << "   Is ONNX file: " << (is_onnx ? "Yes" : "No") << "\n";
    std::cout << "   Starts with 'model': " << (starts_model ? "Yes" : "No") << "\n";
    
    std::cout << "\n4. String replacement:\n";
    
    std::string path = "/old/path/to/file.txt";
    std::string new_path = StringUtils::Replace(path, "/old/", "/new/");
    std::cout << "   Original path: " << path << "\n";
    std::cout << "   Updated path: " << new_path << "\n";
    
    std::cout << "\n";
}

void DemonstrateInputParsing() {
    std::cout << "=== Input Parsing Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("input_parser");
    
    std::cout << "1. Parsing input sizes for ML models:\n";
    
    std::string input_sizes_str = "1,3,224,224;1,3,256,256;1,1,512,512";
    auto input_sizes = InputParser::ParseInputSizes(input_sizes_str);
    
    logger->Info("Input sizes string: " + input_sizes_str);
    logger->Info("Parsed " + std::to_string(input_sizes.size()) + " input size configurations:");
    
    for (size_t i = 0; i < input_sizes.size(); ++i) {
        std::cout << "   Input " << i << ": [";
        for (size_t j = 0; j < input_sizes[i].size(); ++j) {
            std::cout << input_sizes[i][j];
            if (j < input_sizes[i].size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    
    std::cout << "\n2. Parsing numeric lists:\n";
    
    // Parse float list (e.g., normalization parameters)
    std::string mean_str = "0.485,0.456,0.406";
    std::string std_str = "0.229,0.224,0.225";
    
    auto mean_values = InputParser::ParseFloatList(mean_str);
    auto std_values = InputParser::ParseFloatList(std_str);
    
    std::cout << "   Mean values: [";
    for (size_t i = 0; i < mean_values.size(); ++i) {
        std::cout << mean_values[i];
        if (i < mean_values.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    std::cout << "   Std values: [";
    for (size_t i = 0; i < std_values.size(); ++i) {
        std::cout << std_values[i];
        if (i < std_values.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    // Parse integer list (e.g., class IDs)
    std::string class_ids_str = "1,2,5,10,15,20";
    auto class_ids = InputParser::ParseIntList(class_ids_str);
    
    std::cout << "   Class IDs: [";
    for (size_t i = 0; i < class_ids.size(); ++i) {
        std::cout << class_ids[i];
        if (i < class_ids.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    std::cout << "\n3. Parsing string lists:\n";
    
    std::string class_names_str = "background,person,bicycle,car,motorcycle,airplane,bus";
    auto class_names = InputParser::ParseStringList(class_names_str);
    
    std::cout << "   Class names (" << class_names.size() << " total):\n";
    for (size_t i = 0; i < class_names.size(); ++i) {
        std::cout << "     " << i << ": " << class_names[i] << "\n";
    }
    
    std::cout << "\n";
}

void DemonstrateImageUtilities(const std::filesystem::path& output_dir) {
    std::cout << "=== Image Utilities Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("image_utils");
    
    std::cout << "1. Creating and processing test images:\n";
    
    // Create test image
    cv::Mat original_image = CreateTestImage(cv::Size(480, 360));
    logger->Info("Created test image: " + std::to_string(original_image.cols) + "x" + 
                std::to_string(original_image.rows));
    
    // Save original
    auto original_path = output_dir / "01_original.jpg";
    cv::imwrite(original_path.string(), original_image);
    logger->Info("Saved original image: " + original_path.string());
    
    std::cout << "\n2. Resize keeping aspect ratio:\n";
    
    std::vector<cv::Size> target_sizes = {
        cv::Size(640, 480),   // 4:3 ratio
        cv::Size(800, 600),   // 4:3 ratio  
        cv::Size(1024, 768),  // 4:3 ratio
        cv::Size(224, 224),   // Square (common ML input)
        cv::Size(416, 416)    // Square (YOLO input)
    };
    
    for (size_t i = 0; i < target_sizes.size(); ++i) {
        auto resized = ImageUtils::ResizeKeepAspectRatio(original_image, target_sizes[i]);
        
        auto resized_path = output_dir / ("02_resized_" + std::to_string(target_sizes[i].width) + 
                                         "x" + std::to_string(target_sizes[i].height) + ".jpg");
        cv::imwrite(resized_path.string(), resized);
        
        std::cout << "   Resized to " << target_sizes[i].width << "x" << target_sizes[i].height 
                  << " -> actual: " << resized.cols << "x" << resized.rows << "\n";
    }
    
    std::cout << "\n3. Center cropping:\n";
    
    std::vector<cv::Size> crop_sizes = {
        cv::Size(200, 200),
        cv::Size(300, 200),
        cv::Size(150, 300)
    };
    
    for (size_t i = 0; i < crop_sizes.size(); ++i) {
        auto cropped = ImageUtils::CenterCrop(original_image, crop_sizes[i]);
        
        auto crop_path = output_dir / ("03_cropped_" + std::to_string(crop_sizes[i].width) + 
                                      "x" + std::to_string(crop_sizes[i].height) + ".jpg");
        cv::imwrite(crop_path.string(), cropped);
        
        std::cout << "   Center cropped to " << crop_sizes[i].width << "x" << crop_sizes[i].height << "\n";
    }
    
    std::cout << "\n4. Image normalization (for ML):\n";
    
    // Convert to float and normalize
    cv::Mat float_image;
    original_image.convertTo(float_image, CV_32F, 1.0/255.0);
    
    std::vector<float> imagenet_mean = {0.485f, 0.456f, 0.406f};
    std::vector<float> imagenet_std = {0.229f, 0.224f, 0.225f};
    
    auto normalized = ImageUtils::Normalize(float_image, imagenet_mean, imagenet_std);
    
    // Convert back to uint8 for visualization (denormalize)
    cv::Mat denormalized;
    for (int c = 0; c < 3; ++c) {
        std::vector<cv::Mat> channels;
        cv::split(normalized, channels);
        
        // Reverse normalization: pixel = (normalized * std) + mean
        channels[c] = (channels[c] * imagenet_std[c]) + imagenet_mean[c];
        cv::merge(channels, denormalized);
    }
    denormalized.convertTo(denormalized, CV_8U, 255.0);
    
    auto normalized_path = output_dir / "04_normalized.jpg";
    cv::imwrite(normalized_path.string(), denormalized);
    
    std::cout << "   Applied ImageNet normalization (mean=[0.485,0.456,0.406], std=[0.229,0.224,0.225])\n";
    
    std::cout << "\n5. Channel order conversion:\n";
    
    // Demonstrate HWC to CHW conversion (common for ML frameworks)
    auto chw_image = ImageUtils::HwcToChw(original_image);
    auto hwc_back = ImageUtils::ChwToHwc(chw_image);
    
    auto channel_path = output_dir / "05_channel_converted.jpg";
    cv::imwrite(channel_path.string(), hwc_back);
    
    std::cout << "   Original (HWC): " << original_image.rows << "x" << original_image.cols 
              << "x" << original_image.channels() << "\n";
    std::cout << "   Converted to CHW and back to HWC\n";
    std::cout << "   Result matches original: " << (cv::countNonZero(original_image != hwc_back) == 0 ? "Yes" : "No") << "\n";
    
    std::cout << "\n";
}

void DemonstrateDrawingUtilities(const std::filesystem::path& output_dir) {
    std::cout << "=== Drawing Utilities Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("drawing_utils");
    
    // Create a clean canvas
    cv::Mat canvas = cv::Mat::zeros(600, 800, CV_8UC3);
    canvas.setTo(cv::Scalar(50, 50, 50)); // Dark gray background
    
    std::cout << "1. Generating random colors:\n";
    
    auto colors = DrawingUtils::GenerateRandomColors(10, 42); // Fixed seed for consistency
    logger->Info("Generated " + std::to_string(colors.size()) + " random colors");
    
    for (size_t i = 0; i < colors.size(); ++i) {
        std::cout << "   Color " << i << ": RGB(" 
                  << static_cast<int>(colors[i][2]) << ", "  // OpenCV uses BGR
                  << static_cast<int>(colors[i][1]) << ", "
                  << static_cast<int>(colors[i][0]) << ")\n";
    }
    
    std::cout << "\n2. Drawing bounding boxes:\n";
    
    std::vector<cv::Rect> bboxes = {
        cv::Rect(50, 50, 120, 80),
        cv::Rect(200, 100, 150, 100),
        cv::Rect(400, 80, 100, 120),
        cv::Rect(550, 150, 180, 90)
    };
    
    std::vector<std::string> labels = {"Person", "Car", "Bicycle", "Dog"};
    std::vector<float> confidences = {0.95f, 0.87f, 0.76f, 0.82f};
    
    for (size_t i = 0; i < bboxes.size() && i < colors.size(); ++i) {
        // Draw bounding box
        DrawingUtils::DrawBoundingBox(canvas, bboxes[i], colors[i], 2);
        
        // Draw label with confidence
        std::string label_text = labels[i] + " " + std::to_string(static_cast<int>(confidences[i] * 100)) + "%";
        DrawingUtils::DrawLabel(canvas, label_text, confidences[i], 
                               bboxes[i].x, bboxes[i].y - 5, colors[i]);
        
        std::cout << "   " << labels[i] << " at (" << bboxes[i].x << ", " << bboxes[i].y 
                  << ") size " << bboxes[i].width << "x" << bboxes[i].height 
                  << " confidence: " << confidences[i] << "\n";
    }
    
    std::cout << "\n3. Drawing polygons and keypoints:\n";
    
    // Draw a polygon (e.g., segmentation mask)
    std::vector<cv::Point> polygon = {
        cv::Point(100, 300),
        cv::Point(150, 280),
        cv::Point(200, 320),
        cv::Point(180, 380),
        cv::Point(120, 390),
        cv::Point(80, 350)
    };
    
    DrawingUtils::DrawPolygon(canvas, polygon, cv::Scalar(0, 255, 255), 3);
    DrawingUtils::DrawFilledPolygon(canvas, polygon, cv::Scalar(0, 255, 255) * 0.3); // Semi-transparent
    
    // Draw keypoints (e.g., pose estimation)
    std::vector<cv::Point2f> keypoints = {
        cv::Point2f(350, 320), // head
        cv::Point2f(350, 350), // neck
        cv::Point2f(330, 370), // left shoulder
        cv::Point2f(370, 370), // right shoulder
        cv::Point2f(320, 400), // left elbow
        cv::Point2f(380, 400), // right elbow
        cv::Point2f(350, 420), // waist
        cv::Point2f(340, 460), // left hip
        cv::Point2f(360, 460), // right hip
        cv::Point2f(335, 500), // left knee
        cv::Point2f(365, 500)  // right knee
    };
    
    DrawingUtils::DrawKeypoints(canvas, keypoints, cv::Scalar(255, 0, 255), 4);
    
    std::cout << "   Drew polygon with " << polygon.size() << " points\n";
    std::cout << "   Drew " << keypoints.size() << " keypoints\n";
    
    std::cout << "\n4. Text rendering:\n";
    
    std::string title = "Computer Vision Demo";
    auto text_size = DrawingUtils::GetTextSize(title, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2);
    
    // Center the title
    int x = (canvas.cols - text_size.width) / 2;
    cv::putText(canvas, title, cv::Point(x, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    
    std::cout << "   Title text size: " << text_size.width << "x" << text_size.height << "\n";
    std::cout << "   Positioned at: (" << x << ", 30)\n";
    
    // Save the annotated canvas
    auto canvas_path = output_dir / "06_drawing_demo.jpg";
    cv::imwrite(canvas_path.string(), canvas);
    logger->Info("Saved drawing demo: " + canvas_path.string());
    
    std::cout << "\n";
}

void DemonstratePerformanceUtils() {
    std::cout << "=== Performance Utilities Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("performance");
    
    std::cout << "1. Timing operations:\n";
    
    PerformanceUtils::Timer timer;
    
    // Time image processing operation
    cv::Mat test_image = CreateTestImage(cv::Size(1920, 1080));
    
    timer.Start();
    
    // Simulate some processing
    cv::Mat blurred;
    cv::GaussianBlur(test_image, blurred, cv::Size(15, 15), 0);
    
    cv::Mat edges;
    cv::Canny(blurred, edges, 100, 200);
    
    timer.Stop();
    
    double elapsed_ms = timer.GetElapsedMs();
    double elapsed_sec = timer.GetElapsedSeconds();
    
    std::cout << "   Image processing completed in " << elapsed_ms << " ms (" 
              << elapsed_sec << " seconds)\n";
    
    logger->Info("Processing time: " + std::to_string(elapsed_ms) + " ms");
    
    std::cout << "\n2. FPS measurement:\n";
    
    PerformanceUtils::FPSCounter fps_counter(30); // 30-frame window
    
    // Simulate video processing
    for (int frame = 0; frame < 50; ++frame) {
        fps_counter.Update();
        
        // Simulate frame processing
        cv::Mat frame_image = CreateTestImage(cv::Size(640, 480));
        cv::Mat processed;
        cv::resize(frame_image, processed, cv::Size(224, 224));
        
        // Simulate variable processing time
        int delay = 16 + (frame % 5) * 2; // 16-24ms (targeting ~45-60 FPS)
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        
        if (frame % 10 == 9) { // Report every 10 frames
            double current_fps = fps_counter.GetCurrentFPS();
            double average_fps = fps_counter.GetAverageFPS();
            std::cout << "   Frame " << (frame + 1) << ": Current FPS = " << std::fixed 
                      << std::setprecision(1) << current_fps 
                      << ", Average FPS = " << average_fps << "\n";
        }
    }
    
    std::cout << "\n3. Timing multiple operations:\n";
    
    timer.Reset();
    
    // Time different operations
    std::vector<std::string> operations = {
        "Image creation",
        "Gaussian blur",
        "Edge detection", 
        "Resize operation",
        "Color conversion"
    };
    
    cv::Mat work_image = CreateTestImage(cv::Size(800, 600));
    
    for (size_t i = 0; i < operations.size(); ++i) {
        timer.Start();
        
        switch (i) {
            case 0: // Image creation
                work_image = CreateTestImage(cv::Size(800, 600));
                break;
            case 1: // Gaussian blur
                cv::GaussianBlur(work_image, work_image, cv::Size(21, 21), 0);
                break;
            case 2: // Edge detection
                {
                    cv::Mat gray, edges;
                    cv::cvtColor(work_image, gray, cv::COLOR_BGR2GRAY);
                    cv::Canny(gray, edges, 50, 150);
                }
                break;
            case 3: // Resize operation
                cv::resize(work_image, work_image, cv::Size(400, 300));
                break;
            case 4: // Color conversion
                cv::cvtColor(work_image, work_image, cv::COLOR_BGR2HSV);
                break;
        }
        
        timer.Stop();
        
        std::cout << "   " << operations[i] << ": " << timer.GetElapsedMs() << " ms\n";
    }
    
    std::cout << "\n";
}

void DemonstrateMemoryUtils() {
    std::cout << "=== Memory Utilities Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("memory");
    
    std::cout << "1. Image memory calculations:\n";
    
    std::vector<cv::Size> image_sizes = {
        cv::Size(224, 224),    // Small (ML input)
        cv::Size(640, 480),    // VGA
        cv::Size(1920, 1080),  // Full HD
        cv::Size(3840, 2160)   // 4K
    };
    
    for (const auto& size : image_sizes) {
        cv::Mat image(size, CV_8UC3);
        size_t memory_bytes = MemoryUtils::GetImageMemorySize(image);
        std::string formatted = MemoryUtils::FormatBytes(memory_bytes);
        
        std::cout << "   " << size.width << "x" << size.height << " RGB image: " 
                  << memory_bytes << " bytes (" << formatted << ")\n";
    }
    
    std::cout << "\n2. Tensor memory calculations:\n";
    
    std::vector<std::vector<int64_t>> tensor_shapes = {
        {1, 3, 224, 224},      // Single image batch (float32)
        {8, 3, 416, 416},      // 8-image batch
        {1, 1000},             // Classification output
        {1, 25200, 85},        // YOLO detection output
        {16, 512, 512, 3}      // Batch of feature maps
    };
    
    for (const auto& shape : tensor_shapes) {
        size_t memory_float32 = MemoryUtils::GetTensorMemorySize(shape, sizeof(float));
        size_t memory_float16 = MemoryUtils::GetTensorMemorySize(shape, sizeof(uint16_t));
        
        std::cout << "   Shape [";
        for (size_t i = 0; i < shape.size(); ++i) {
            std::cout << shape[i];
            if (i < shape.size() - 1) std::cout << ", ";
        }
        std::cout << "]:\n";
        std::cout << "     Float32: " << MemoryUtils::FormatBytes(memory_float32) << "\n";
        std::cout << "     Float16: " << MemoryUtils::FormatBytes(memory_float16) << "\n";
    }
    
    std::cout << "\n3. System memory information:\n";
    
    try {
        size_t system_memory = MemoryUtils::GetSystemMemoryUsage();
        size_t process_memory = MemoryUtils::GetProcessMemoryUsage();
        
        std::cout << "   System memory usage: " << MemoryUtils::FormatBytes(system_memory) << "\n";
        std::cout << "   Process memory usage: " << MemoryUtils::FormatBytes(process_memory) << "\n";
        
        double memory_percentage = (static_cast<double>(process_memory) / system_memory) * 100.0;
        std::cout << "   Process memory percentage: " << std::fixed << std::setprecision(2) 
                  << memory_percentage << "%\n";
        
    } catch (const std::exception& e) {
        logger->Warn("Could not retrieve memory information: " + std::string(e.what()));
    }
    
    std::cout << "\n4. Memory formatting examples:\n";
    
    std::vector<size_t> memory_values = {
        512,                    // 512 bytes
        1024,                   // 1 KB
        1024 * 1024,           // 1 MB
        1024 * 1024 * 1024,    // 1 GB
        1536 * 1024 * 1024,    // 1.5 GB
        5ULL * 1024 * 1024 * 1024  // 5 GB
    };
    
    for (size_t value : memory_values) {
        std::string formatted = MemoryUtils::FormatBytes(value);
        std::cout << "   " << value << " bytes = " << formatted << "\n";
    }
    
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::cout << "=== Vision Infrastructure Image Processing Demo ===\n\n";
    
    try {
        // Create output directory
        auto output_dir = std::filesystem::temp_directory_path() / "vision_infra_image_demo";
        std::filesystem::create_directories(output_dir);
        
        auto logger = LoggerManager::GetLogger("main");
        logger->SetLevel(LogLevel::INFO);
        logger->Info("Demo started, output directory: " + output_dir.string());
        
        // Run all demonstrations
        DemonstrateStringUtilities();
        DemonstrateInputParsing();
        DemonstrateImageUtilities(output_dir);
        DemonstrateDrawingUtilities(output_dir);
        DemonstratePerformanceUtils();
        DemonstrateMemoryUtils();
        
        std::cout << "=== Summary ===\n";
        std::cout << "✓ String utilities demonstrated\n";
        std::cout << "✓ Input parsing for ML applications shown\n";
        std::cout << "✓ Image processing utilities tested\n";
        std::cout << "✓ Drawing and annotation capabilities shown\n";
        std::cout << "✓ Performance measurement tools demonstrated\n";
        std::cout << "✓ Memory calculation utilities tested\n\n";
        
        std::cout << "Output images saved to: " << output_dir << "\n";
        std::cout << "Image processing demo completed successfully!\n";
        
        logger->Info("Demo completed successfully");
        
    } catch (const std::exception& e) {
        std::cerr << "Error during image processing demo: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}