# File Operations Example

This example demonstrates file system operations commonly used in computer vision applications, including file I/O, image file handling, configuration management, and log file operations.

## Features Demonstrated

- **Basic file operations** (create, read, write, copy, move, delete)
- **Directory management** and traversal
- **Image file operations** and format conversions
- **Configuration file handling** (JSON, INI, YAML)
- **Log file management** and rotation
- **File system monitoring** and space analysis
- **File permissions** and access control

## Building and Running

```bash
# Build the example
cd build
make file_operations_example

# Run the demo
./examples/file_operations/file_operations_example
```

## Key Operations

### Basic File Operations

```cpp
#include <filesystem>
#include <fstream>

// Create directories
std::filesystem::create_directories("/path/to/dir");
std::filesystem::create_directory("/path/to/single/dir");

// Write files
std::ofstream file("/path/to/file.txt");
file << "Content here";
file.close();

// Read files
std::ifstream file("/path/to/file.txt");
std::string content((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
file.close();

// File operations
std::filesystem::copy_file(source, dest);
std::filesystem::rename(old_path, new_path);
std::filesystem::remove(file_path);
std::filesystem::remove_all(directory_path);

// File information
size_t size = std::filesystem::file_size(file_path);
auto modified = std::filesystem::last_write_time(file_path);
bool exists = std::filesystem::exists(file_path);
```

### Directory Traversal

```cpp
// List directory contents
for (const auto& entry : std::filesystem::directory_iterator("/path")) {
    if (entry.is_directory()) {
        std::cout << "Directory: " << entry.path() << std::endl;
    } else if (entry.is_regular_file()) {
        std::cout << "File: " << entry.path() 
                  << " (" << std::filesystem::file_size(entry) << " bytes)" 
                  << std::endl;
    }
}

// Recursive traversal
for (const auto& entry : std::filesystem::recursive_directory_iterator("/path")) {
    auto relative = std::filesystem::relative(entry.path(), "/path");
    std::cout << relative << std::endl;
}
```

### Image File Operations

```cpp
#include <opencv2/opencv.hpp>

// Save images in different formats
cv::Mat image = cv::imread("input.jpg");

// High quality JPEG
std::vector<int> jpeg_params = {cv::IMWRITE_JPEG_QUALITY, 95};
cv::imwrite("output_hq.jpg", image, jpeg_params);

// PNG with compression
std::vector<int> png_params = {cv::IMWRITE_PNG_COMPRESSION, 9};
cv::imwrite("output.png", image, png_params);

// TIFF uncompressed
std::vector<int> tiff_params = {cv::IMWRITE_TIFF_COMPRESSION, 1};
cv::imwrite("output.tiff", image, tiff_params);

// Analyze image files
cv::Mat loaded = cv::imread("image.jpg", cv::IMREAD_UNCHANGED);
std::cout << "Size: " << loaded.cols << "x" << loaded.rows << std::endl;
std::cout << "Channels: " << loaded.channels() << std::endl;
std::cout << "Type: " << loaded.type() << std::endl;
```

### Configuration File Management

#### JSON Configuration

```cpp
// Write JSON config
std::ofstream json_file("config.json");
json_file << "{\n"
          << "  \"model\": {\n"
          << "    \"name\": \"yolov5s\",\n"
          << "    \"input_size\": [640, 640],\n"
          << "    \"confidence_threshold\": 0.5\n"
          << "  }\n"
          << "}\n";
json_file.close();
```

#### INI Configuration

```cpp
// Write INI config
std::ofstream ini_file("config.ini");
ini_file << "[Application]\n"
         << "name=VisionApp\n"
         << "version=1.0\n\n"
         << "[Logging]\n"
         << "level=INFO\n"
         << "file=app.log\n";
ini_file.close();

// Simple INI parser
std::ifstream file("config.ini");
std::string line, current_section;
std::map<std::string, std::map<std::string, std::string>> config;

while (std::getline(file, line)) {
    if (line[0] == '[' && line.back() == ']') {
        current_section = line.substr(1, line.length() - 2);
    } else {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            config[current_section][key] = value;
        }
    }
}
```

### Log File Management

```cpp
#include <chrono>
#include <iomanip>

// Create timestamped log entries
auto now = std::chrono::system_clock::now();
auto time_t = std::chrono::system_clock::to_time_t(now);

std::ofstream log_file("application.log");
log_file << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") 
         << "] INFO: Application started" << std::endl;

// Log rotation - delete old files
auto cutoff_time = std::chrono::file_clock::now() - std::chrono::hours(24 * 7); // 1 week

for (const auto& entry : std::filesystem::directory_iterator("logs/")) {
    if (entry.is_regular_file()) {
        auto modified = std::filesystem::last_write_time(entry);
        if (modified < cutoff_time) {
            std::filesystem::remove(entry.path());
        }
    }
}
```

### File System Monitoring

```cpp
// Calculate directory space usage
size_t calculate_directory_size(const std::filesystem::path& dir) {
    size_t total_size = 0;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            total_size += std::filesystem::file_size(entry);
        }
    }
    
    return total_size;
}

// Find largest files
std::vector<std::pair<std::filesystem::path, size_t>> get_largest_files(
    const std::filesystem::path& dir, int count = 10) {
    
    std::vector<std::pair<std::filesystem::path, size_t>> files;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            files.emplace_back(entry.path(), std::filesystem::file_size(entry));
        }
    }
    
    std::partial_sort(files.begin(), files.begin() + std::min(count, (int)files.size()), 
                     files.end(), [](const auto& a, const auto& b) {
                         return a.second > b.second;
                     });
    
    if (files.size() > count) {
        files.resize(count);
    }
    
    return files;
}

// Check permissions
auto status = std::filesystem::status(path);
auto perms = status.permissions();

bool can_read = (perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none;
bool can_write = (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
bool can_execute = (perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
```

## Example Output

```
=== Vision Infrastructure File Operations Demo ===

=== Basic File Operations Demo ===

1. Directory operations:
   ✓ Created directory: images
   ✓ Created directory: models
   ✓ Created directory: logs
   ✓ Created directory: config
   ✓ Created directory: output

2. File creation and writing:
   ✓ Created config/settings.txt (42 bytes)
   ✓ Created logs/application.log (45 bytes)
   ✓ Created models/model_info.txt (38 bytes)
   ✓ Created README.md (47 bytes)

3. Directory structure:
   📁 config/
   📄 config/settings.txt (42 bytes)
   📁 images/
   📁 logs/
   📄 logs/application.log (45 bytes)
   📁 models/
   📄 models/model_info.txt (38 bytes)
   📄 README.md (47 bytes)

=== Image File Operations Demo ===

1. Creating sample images:
   ✓ Created sample_rgb.jpg (640x480, 25847 bytes)
   ✓ Created sample_grayscale.png (400x300, 15234 bytes)
   ✓ Created sample_large.tiff (1920x1080, 185632 bytes)

2. Image file analysis:
   📸 sample_rgb.jpg:
       Size: 640x480
       Type: RGB (3 channels)
       File size: 25847 bytes
       Compression ratio: 35.5x

=== File System Monitoring Demo ===

1. Directory space analysis:
   Total directories: 5
   Total files: 12
   Total size: 387456 bytes (378.38 KB)

2. File type distribution:
           .jpg:    89234 bytes (23.0%)
           .png:    78456 bytes (20.2%)
          .tiff:   185632 bytes (47.9%)
           .log:     2134 bytes (0.6%)
    (no extension):    32000 bytes (8.3%)
```

## Common Use Cases

### Dataset Management

```cpp
// Organize image dataset
std::filesystem::path dataset_dir = "dataset";
std::vector<std::string> classes = {"cats", "dogs", "birds"};

for (const auto& class_name : classes) {
    auto class_dir = dataset_dir / class_name;
    std::filesystem::create_directories(class_dir / "train");
    std::filesystem::create_directories(class_dir / "val");
    std::filesystem::create_directories(class_dir / "test");
}

// Move images to appropriate directories
for (const auto& entry : std::filesystem::directory_iterator("raw_images")) {
    if (entry.path().extension() == ".jpg") {
        auto filename = entry.path().filename().string();
        
        // Extract class from filename (assuming format: class_001.jpg)
        auto class_name = filename.substr(0, filename.find('_'));
        
        auto dest_path = dataset_dir / class_name / "train" / filename;
        std::filesystem::copy_file(entry.path(), dest_path);
    }
}
```

### Model and Checkpoint Management

```cpp
// Save model checkpoints with versioning
void save_checkpoint(const std::string& model_data, int epoch) {
    auto checkpoints_dir = std::filesystem::path("checkpoints");
    std::filesystem::create_directories(checkpoints_dir);
    
    auto checkpoint_file = checkpoints_dir / ("epoch_" + std::to_string(epoch) + ".ckpt");
    
    std::ofstream file(checkpoint_file, std::ios::binary);
    file.write(model_data.data(), model_data.size());
    file.close();
    
    // Keep only the last 5 checkpoints
    std::vector<std::filesystem::path> checkpoints;
    for (const auto& entry : std::filesystem::directory_iterator(checkpoints_dir)) {
        if (entry.path().extension() == ".ckpt") {
            checkpoints.push_back(entry.path());
        }
    }
    
    if (checkpoints.size() > 5) {
        std::sort(checkpoints.begin(), checkpoints.end());
        for (size_t i = 0; i < checkpoints.size() - 5; ++i) {
            std::filesystem::remove(checkpoints[i]);
        }
    }
}
```

### Configuration Validation

```cpp
bool validate_config_files() {
    std::vector<std::string> required_configs = {
        "config/model.json",
        "config/training.ini", 
        "config/dataset.yaml"
    };
    
    for (const auto& config_file : required_configs) {
        if (!std::filesystem::exists(config_file)) {
            std::cerr << "Missing required config: " << config_file << std::endl;
            return false;
        }
        
        // Check file is not empty
        if (std::filesystem::file_size(config_file) == 0) {
            std::cerr << "Empty config file: " << config_file << std::endl;
            return false;
        }
    }
    
    return true;
}
```

### Log Analysis

```cpp
// Analyze error patterns in log files
void analyze_log_files(const std::filesystem::path& log_dir) {
    std::map<std::string, int> error_counts;
    
    for (const auto& entry : std::filesystem::directory_iterator(log_dir)) {
        if (entry.path().extension() == ".log") {
            std::ifstream file(entry.path());
            std::string line;
            
            while (std::getline(file, line)) {
                if (line.find("ERROR") != std::string::npos) {
                    // Extract error type (simplified)
                    size_t pos = line.find("ERROR:");
                    if (pos != std::string::npos) {
                        std::string error_msg = line.substr(pos + 6);
                        error_counts[error_msg]++;
                    }
                }
            }
        }
    }
    
    // Report most common errors
    std::vector<std::pair<std::string, int>> sorted_errors(
        error_counts.begin(), error_counts.end());
    std::sort(sorted_errors.begin(), sorted_errors.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::cout << "Top errors:\n";
    for (size_t i = 0; i < std::min(5UL, sorted_errors.size()); ++i) {
        std::cout << sorted_errors[i].second << "x: " 
                  << sorted_errors[i].first << std::endl;
    }
}