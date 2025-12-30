#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <vision-infra/core/Logger.hpp>
#include <opencv2/opencv.hpp>

using namespace vision_infra::core;

void DemonstrateBasicFileOperations() {
    std::cout << "=== Basic File Operations Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("file_ops");
    
    // Create a temporary directory for our demo
    auto temp_dir = std::filesystem::temp_directory_path() / "vision_infra_file_demo";
    std::filesystem::create_directories(temp_dir);
    
    logger->Info("Created temporary directory: " + temp_dir.string());
    
    std::cout << "1. Directory operations:\n";
    
    // Create subdirectories
    std::vector<std::string> subdirs = {"images", "models", "logs", "config", "output"};
    
    for (const auto& subdir : subdirs) {
        auto subdir_path = temp_dir / subdir;
        bool created = std::filesystem::create_directory(subdir_path);
        std::cout << "   " << (created ? "✓" : "×") << " Created directory: " << subdir << "\n";
    }
    
    std::cout << "\n2. File creation and writing:\n";
    
    // Create different types of files
    std::vector<std::pair<std::string, std::string>> files_to_create = {
        {"config/settings.txt", "# Configuration file\nlog_level=INFO\noutput_dir=/tmp\n"},
        {"logs/application.log", "[INFO] Application started\n[DEBUG] Loading configuration\n"},
        {"models/model_info.txt", "Model: YOLO v5\nInput: 640x640\nClasses: 80\n"},
        {"README.md", "# Vision Infrastructure Demo\n\nThis is a demo project.\n"}
    };
    
    for (const auto& [file_path, content] : files_to_create) {
        auto full_path = temp_dir / file_path;
        
        std::ofstream file(full_path);
        if (file.is_open()) {
            file << content;
            file.close();
            
            auto file_size = std::filesystem::file_size(full_path);
            std::cout << "   ✓ Created " << file_path << " (" << file_size << " bytes)\n";
        } else {
            std::cout << "   × Failed to create " << file_path << "\n";
        }
    }
    
    std::cout << "\n3. File reading and content verification:\n";
    
    for (const auto& [file_path, expected_content] : files_to_create) {
        auto full_path = temp_dir / file_path;
        
        std::ifstream file(full_path);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            
            bool content_matches = (content == expected_content);
            std::cout << "   " << (content_matches ? "✓" : "×") 
                      << " Verified " << file_path << " content\n";
                      
            if (!content_matches) {
                std::cout << "     Expected: " << expected_content.substr(0, 50) << "...\n";
                std::cout << "     Got: " << content.substr(0, 50) << "...\n";
            }
        } else {
            std::cout << "   × Failed to read " << file_path << "\n";
        }
    }
    
    std::cout << "\n4. Directory listing and traversal:\n";
    
    std::cout << "   Directory structure:\n";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(temp_dir)) {
        auto relative_path = std::filesystem::relative(entry.path(), temp_dir);
        
        if (entry.is_directory()) {
            std::cout << "   📁 " << relative_path.string() << "/\n";
        } else if (entry.is_regular_file()) {
            auto size = std::filesystem::file_size(entry.path());
            std::cout << "   📄 " << relative_path.string() << " (" << size << " bytes)\n";
        }
    }
    
    std::cout << "\n5. File operations (copy, move, delete):\n";
    
    // Copy a file
    auto source_file = temp_dir / "README.md";
    auto backup_file = temp_dir / "README_backup.md";
    
    try {
        std::filesystem::copy_file(source_file, backup_file);
        std::cout << "   ✓ Copied README.md to README_backup.md\n";
    } catch (const std::exception& e) {
        std::cout << "   × Copy failed: " << e.what() << "\n";
    }
    
    // Move a file
    auto log_source = temp_dir / "logs" / "application.log";
    auto log_dest = temp_dir / "application_moved.log";
    
    try {
        std::filesystem::rename(log_source, log_dest);
        std::cout << "   ✓ Moved application.log to root directory\n";
    } catch (const std::exception& e) {
        std::cout << "   × Move failed: " << e.what() << "\n";
    }
    
    // Delete a file
    try {
        bool deleted = std::filesystem::remove(backup_file);
        std::cout << "   " << (deleted ? "✓" : "×") << " Deleted README_backup.md\n";
    } catch (const std::exception& e) {
        std::cout << "   × Delete failed: " << e.what() << "\n";
    }
    
    logger->Info("Basic file operations completed");
}

void DemonstrateImageFileOperations() {
    std::cout << "\n=== Image File Operations Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("image_files");
    
    auto base_dir = std::filesystem::temp_directory_path() / "vision_infra_file_demo";
    auto images_dir = base_dir / "images";
    
    std::cout << "1. Creating sample images:\n";
    
    // Create different types of images
    std::vector<std::tuple<std::string, cv::Size, int>> image_configs = {
        {"sample_rgb.jpg", cv::Size(640, 480), CV_8UC3},
        {"sample_grayscale.png", cv::Size(400, 300), CV_8UC1},
        {"sample_large.tiff", cv::Size(1920, 1080), CV_8UC3},
        {"sample_small.bmp", cv::Size(128, 128), CV_8UC3}
    };
    
    for (const auto& [filename, size, type] : image_configs) {
        cv::Mat image;
        
        if (type == CV_8UC1) {
            // Grayscale pattern
            image = cv::Mat::zeros(size, type);
            for (int y = 0; y < image.rows; ++y) {
                for (int x = 0; x < image.cols; ++x) {
                    image.at<uchar>(y, x) = static_cast<uchar>((x + y) % 256);
                }
            }
        } else {
            // Color pattern
            image = cv::Mat::zeros(size, type);
            for (int y = 0; y < image.rows; ++y) {
                for (int x = 0; x < image.cols; ++x) {
                    image.at<cv::Vec3b>(y, x) = cv::Vec3b(
                        static_cast<uchar>((x * 255) / image.cols),
                        static_cast<uchar>((y * 255) / image.rows),
                        static_cast<uchar>((x + y) * 128 / (image.cols + image.rows))
                    );
                }
            }
        }
        
        // Add some geometric shapes
        if (type == CV_8UC3) {
            cv::rectangle(image, cv::Point(20, 20), cv::Point(80, 60), cv::Scalar(255, 0, 0), 2);
            cv::circle(image, cv::Point(size.width - 50, 50), 30, cv::Scalar(0, 255, 0), -1);
        } else {
            cv::rectangle(image, cv::Point(20, 20), cv::Point(80, 60), cv::Scalar(255), 2);
            cv::circle(image, cv::Point(size.width - 50, 50), 30, cv::Scalar(128), -1);
        }
        
        auto image_path = images_dir / filename;
        bool saved = cv::imwrite(image_path.string(), image);
        
        if (saved) {
            auto file_size = std::filesystem::file_size(image_path);
            std::cout << "   ✓ Created " << filename << " (" 
                      << size.width << "x" << size.height << ", " 
                      << file_size << " bytes)\n";
        } else {
            std::cout << "   × Failed to create " << filename << "\n";
        }
    }
    
    std::cout << "\n2. Image file analysis:\n";
    
    for (const auto& entry : std::filesystem::directory_iterator(images_dir)) {
        if (!entry.is_regular_file()) continue;
        
        auto filename = entry.path().filename().string();
        auto file_size = std::filesystem::file_size(entry.path());
        
        // Try to load and analyze the image
        cv::Mat loaded_image = cv::imread(entry.path().string(), cv::IMREAD_UNCHANGED);
        
        if (!loaded_image.empty()) {
            std::string type_str;
            switch (loaded_image.type()) {
                case CV_8UC1: type_str = "Grayscale"; break;
                case CV_8UC3: type_str = "RGB"; break;
                case CV_8UC4: type_str = "RGBA"; break;
                default: type_str = "Other"; break;
            }
            
            std::cout << "   📸 " << filename << ":\n";
            std::cout << "       Size: " << loaded_image.cols << "x" << loaded_image.rows << "\n";
            std::cout << "       Type: " << type_str << " (" << loaded_image.channels() << " channels)\n";
            std::cout << "       File size: " << file_size << " bytes\n";
            std::cout << "       Compression ratio: " << std::fixed << std::setprecision(1)
                      << static_cast<double>(loaded_image.total() * loaded_image.elemSize()) / file_size << "x\n";
        } else {
            std::cout << "   × Failed to load " << filename << "\n";
        }
    }
    
    std::cout << "\n3. Image format conversion:\n";
    
    // Load the RGB image and convert to different formats
    auto source_image_path = images_dir / "sample_rgb.jpg";
    cv::Mat source_image = cv::imread(source_image_path.string());
    
    if (!source_image.empty()) {
        std::vector<std::tuple<std::string, std::string, std::vector<int>>> conversions = {
            {"png", "sample_rgb_converted.png", {cv::IMWRITE_PNG_COMPRESSION, 9}},
            {"tiff", "sample_rgb_converted.tiff", {cv::IMWRITE_TIFF_COMPRESSION, 1}},
            {"bmp", "sample_rgb_converted.bmp", {}},
            {"jpg_high", "sample_rgb_quality95.jpg", {cv::IMWRITE_JPEG_QUALITY, 95}},
            {"jpg_low", "sample_rgb_quality30.jpg", {cv::IMWRITE_JPEG_QUALITY, 30}}
        };
        
        for (const auto& [format_name, filename, params] : conversions) {
            auto output_path = images_dir / filename;
            bool saved = cv::imwrite(output_path.string(), source_image, params);
            
            if (saved) {
                auto file_size = std::filesystem::file_size(output_path);
                std::cout << "   ✓ Converted to " << format_name << ": " << filename 
                          << " (" << file_size << " bytes)\n";
            } else {
                std::cout << "   × Failed to convert to " << format_name << "\n";
            }
        }
    }
    
    logger->Info("Image file operations completed");
}

void DemonstrateConfigurationFileHandling() {
    std::cout << "\n=== Configuration File Handling Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("config_files");
    
    auto base_dir = std::filesystem::temp_directory_path() / "vision_infra_file_demo";
    auto config_dir = base_dir / "config";
    
    std::cout << "1. Creating configuration files:\n";
    
    // Create different types of configuration files
    
    // JSON-style config
    {
        auto json_config = config_dir / "model_config.json";
        std::ofstream file(json_config);
        file << "{\n"
             << "  \"model\": {\n"
             << "    \"name\": \"yolov5s\",\n"
             << "    \"input_size\": [640, 640],\n"
             << "    \"num_classes\": 80,\n"
             << "    \"confidence_threshold\": 0.5,\n"
             << "    \"nms_threshold\": 0.4\n"
             << "  },\n"
             << "  \"preprocessing\": {\n"
             << "    \"normalize\": true,\n"
             << "    \"mean\": [0.485, 0.456, 0.406],\n"
             << "    \"std\": [0.229, 0.224, 0.225]\n"
             << "  }\n"
             << "}\n";
        file.close();
        
        auto size = std::filesystem::file_size(json_config);
        std::cout << "   ✓ Created JSON config: " << json_config.filename() 
                  << " (" << size << " bytes)\n";
    }
    
    // INI-style config
    {
        auto ini_config = config_dir / "application.ini";
        std::ofstream file(ini_config);
        file << "[Application]\n"
             << "name=VisionInfra\n"
             << "version=1.0.0\n"
             << "debug=true\n\n"
             << "[Logging]\n"
             << "level=INFO\n"
             << "file=application.log\n"
             << "console=true\n\n"
             << "[Processing]\n"
             << "threads=4\n"
             << "batch_size=8\n"
             << "timeout=30000\n";
        file.close();
        
        auto size = std::filesystem::file_size(ini_config);
        std::cout << "   ✓ Created INI config: " << ini_config.filename() 
                  << " (" << size << " bytes)\n";
    }
    
    // YAML-style config
    {
        auto yaml_config = config_dir / "pipeline.yaml";
        std::ofstream file(yaml_config);
        file << "pipeline:\n"
             << "  name: object_detection\n"
             << "  stages:\n"
             << "    - name: preprocessing\n"
             << "      operations:\n"
             << "        - resize: [640, 640]\n"
             << "        - normalize:\n"
             << "            mean: [0.485, 0.456, 0.406]\n"
             << "            std: [0.229, 0.224, 0.225]\n"
             << "    - name: inference\n"
             << "      model: models/yolov5s.onnx\n"
             << "      device: cuda\n"
             << "    - name: postprocessing\n"
             << "      nms_threshold: 0.4\n"
             << "      confidence_threshold: 0.5\n";
        file.close();
        
        auto size = std::filesystem::file_size(yaml_config);
        std::cout << "   ✓ Created YAML config: " << yaml_config.filename() 
                  << " (" << size << " bytes)\n";
    }
    
    std::cout << "\n2. Reading and parsing configuration files:\n";
    
    // Simple INI parser demonstration
    auto ini_config = config_dir / "application.ini";
    std::ifstream ini_file(ini_config);
    if (ini_file.is_open()) {
        std::string line;
        std::string current_section;
        std::map<std::string, std::map<std::string, std::string>> ini_data;
        
        while (std::getline(ini_file, line)) {
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue; // Skip empty lines and comments
            }
            
            if (line[0] == '[' && line.back() == ']') {
                // Section header
                current_section = line.substr(1, line.length() - 2);
                ini_data[current_section] = {};
            } else {
                // Key-value pair
                size_t equals_pos = line.find('=');
                if (equals_pos != std::string::npos && !current_section.empty()) {
                    std::string key = line.substr(0, equals_pos);
                    std::string value = line.substr(equals_pos + 1);
                    ini_data[current_section][key] = value;
                }
            }
        }
        ini_file.close();
        
        std::cout << "   ✓ Parsed INI configuration:\n";
        for (const auto& [section, keys] : ini_data) {
            std::cout << "     [" << section << "]\n";
            for (const auto& [key, value] : keys) {
                std::cout << "       " << key << " = " << value << "\n";
            }
        }
    }
    
    std::cout << "\n3. Configuration file validation:\n";
    
    // Check for required configuration files
    std::vector<std::string> required_configs = {
        "model_config.json",
        "application.ini",
        "pipeline.yaml"
    };
    
    bool all_configs_present = true;
    for (const auto& config_file : required_configs) {
        auto config_path = config_dir / config_file;
        bool exists = std::filesystem::exists(config_path);
        
        std::cout << "   " << (exists ? "✓" : "×") << " " << config_file;
        if (exists) {
            auto size = std::filesystem::file_size(config_path);
            auto modified_time = std::filesystem::last_write_time(config_path);
            
            // Convert file time to system time (C++20 feature, may not be available)
            std::cout << " (" << size << " bytes)";
        }
        std::cout << "\n";
        
        if (!exists) all_configs_present = false;
    }
    
    std::cout << "\n   Configuration validation: " 
              << (all_configs_present ? "PASSED" : "FAILED") << "\n";
    
    logger->Info("Configuration file handling completed");
}

void DemonstrateLogFileManagement() {
    std::cout << "\n=== Log File Management Demo ===\n\n";
    
    auto base_dir = std::filesystem::temp_directory_path() / "vision_infra_file_demo";
    auto logs_dir = base_dir / "logs";
    
    // Recreate logs directory since we moved the file earlier
    std::filesystem::create_directories(logs_dir);
    
    std::cout << "1. Creating multiple log files:\n";
    
    // Simulate application logs over several days
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::vector<std::pair<std::string, int>> log_configs = {
        {"application.log", 0},      // Today
        {"application_1.log", 1},    // Yesterday  
        {"application_2.log", 2},    // 2 days ago
        {"error.log", 0},            // Today
        {"debug.log", 0},            // Today
        {"performance.log", 1}       // Yesterday
    };
    
    for (const auto& [filename, days_ago] : log_configs) {
        auto log_path = logs_dir / filename;
        std::ofstream log_file(log_path);
        
        if (log_file.is_open()) {
            // Generate sample log content
            auto file_time = now - std::chrono::hours(24 * days_ago);
            auto file_time_t = std::chrono::system_clock::to_time_t(file_time);
            
            log_file << "=== Log file started at " << std::put_time(std::localtime(&file_time_t), "%Y-%m-%d %H:%M:%S") << " ===\n";
            
            for (int i = 0; i < 20 + (days_ago * 5); ++i) {
                auto log_time = file_time + std::chrono::minutes(i * 5);
                auto log_time_t = std::chrono::system_clock::to_time_t(log_time);
                
                if (filename.find("error") != std::string::npos) {
                    log_file << "[" << std::put_time(std::localtime(&log_time_t), "%H:%M:%S") 
                             << "] ERROR: Sample error message " << (i + 1) << "\n";
                } else if (filename.find("debug") != std::string::npos) {
                    log_file << "[" << std::put_time(std::localtime(&log_time_t), "%H:%M:%S") 
                             << "] DEBUG: Debug information " << (i + 1) << "\n";
                } else if (filename.find("performance") != std::string::npos) {
                    log_file << "[" << std::put_time(std::localtime(&log_time_t), "%H:%M:%S") 
                             << "] PERF: Operation took " << (10 + i % 50) << "ms\n";
                } else {
                    log_file << "[" << std::put_time(std::localtime(&log_time_t), "%H:%M:%S") 
                             << "] INFO: Application event " << (i + 1) << "\n";
                }
            }
            
            log_file.close();
            
            // Set file modification time to simulate aging
            auto file_time_point = std::chrono::file_clock::from_sys(file_time);
            std::filesystem::last_write_time(log_path, file_time_point);
            
            auto file_size = std::filesystem::file_size(log_path);
            std::cout << "   ✓ Created " << filename << " (" << file_size << " bytes, " 
                      << days_ago << " days old)\n";
        }
    }
    
    std::cout << "\n2. Log file analysis:\n";
    
    size_t total_log_size = 0;
    int total_log_files = 0;
    
    for (const auto& entry : std::filesystem::directory_iterator(logs_dir)) {
        if (!entry.is_regular_file()) continue;
        
        auto filename = entry.path().filename().string();
        auto file_size = std::filesystem::file_size(entry.path());
        auto last_modified = std::filesystem::last_write_time(entry.path());
        
        total_log_size += file_size;
        total_log_files++;
        
        // Calculate file age
        auto now_file_clock = std::chrono::file_clock::now();
        auto file_age = now_file_clock - last_modified;
        auto age_hours = std::chrono::duration_cast<std::chrono::hours>(file_age).count();
        
        std::cout << "   📄 " << filename << ": " << file_size << " bytes, " 
                  << age_hours << "h old\n";
    }
    
    std::cout << "\n   Summary: " << total_log_files << " log files, " 
              << total_log_size << " bytes total\n";
    
    std::cout << "\n3. Log file cleanup simulation:\n";
    
    // Simulate log rotation - delete files older than 24 hours
    auto cutoff_time = std::chrono::file_clock::now() - std::chrono::hours(24);
    
    std::vector<std::filesystem::path> files_to_delete;
    for (const auto& entry : std::filesystem::directory_iterator(logs_dir)) {
        if (!entry.is_regular_file()) continue;
        
        auto last_modified = std::filesystem::last_write_time(entry.path());
        if (last_modified < cutoff_time) {
            files_to_delete.push_back(entry.path());
        }
    }
    
    std::cout << "   Found " << files_to_delete.size() << " files older than 24 hours:\n";
    
    for (const auto& file_path : files_to_delete) {
        auto file_size = std::filesystem::file_size(file_path);
        std::cout << "   🗑️  Would delete: " << file_path.filename() 
                  << " (" << file_size << " bytes)\n";
        
        // Actually delete the file (uncomment for real cleanup)
        // std::filesystem::remove(file_path);
    }
    
    auto logger = LoggerManager::GetLogger("log_mgmt");
    logger->Info("Log file management completed");
}

void DemonstrateFileSystemMonitoring() {
    std::cout << "\n=== File System Monitoring Demo ===\n\n";
    
    auto logger = LoggerManager::GetLogger("fs_monitor");
    
    auto base_dir = std::filesystem::temp_directory_path() / "vision_infra_file_demo";
    
    std::cout << "1. Directory space analysis:\n";
    
    // Calculate total space usage
    size_t total_size = 0;
    int file_count = 0;
    int dir_count = 0;
    
    std::map<std::string, size_t> size_by_extension;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(base_dir)) {
        if (entry.is_directory()) {
            dir_count++;
        } else if (entry.is_regular_file()) {
            file_count++;
            auto file_size = std::filesystem::file_size(entry.path());
            total_size += file_size;
            
            auto extension = entry.path().extension().string();
            if (extension.empty()) extension = "(no extension)";
            size_by_extension[extension] += file_size;
        }
    }
    
    std::cout << "   Total directories: " << dir_count << "\n";
    std::cout << "   Total files: " << file_count << "\n";
    std::cout << "   Total size: " << total_size << " bytes (" 
              << std::fixed << std::setprecision(2) << total_size / 1024.0 << " KB)\n";
    
    std::cout << "\n2. File type distribution:\n";
    for (const auto& [extension, size] : size_by_extension) {
        double percentage = static_cast<double>(size) / total_size * 100.0;
        std::cout << "   " << std::setw(15) << extension << ": " 
                  << std::setw(8) << size << " bytes (" 
                  << std::fixed << std::setprecision(1) << percentage << "%)\n";
    }
    
    std::cout << "\n3. Largest files:\n";
    
    std::vector<std::pair<std::filesystem::path, size_t>> file_sizes;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(base_dir)) {
        if (entry.is_regular_file()) {
            auto file_size = std::filesystem::file_size(entry.path());
            file_sizes.emplace_back(entry.path(), file_size);
        }
    }
    
    // Sort by size (largest first)
    std::sort(file_sizes.begin(), file_sizes.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    int files_to_show = std::min(5, static_cast<int>(file_sizes.size()));
    for (int i = 0; i < files_to_show; ++i) {
        auto relative_path = std::filesystem::relative(file_sizes[i].first, base_dir);
        std::cout << "   " << (i + 1) << ". " << relative_path.string() 
                  << " (" << file_sizes[i].second << " bytes)\n";
    }
    
    std::cout << "\n4. Directory permissions and access:\n";
    
    // Check directory permissions
    std::vector<std::string> dirs_to_check = {"images", "models", "logs", "config", "output"};
    
    for (const auto& dir_name : dirs_to_check) {
        auto dir_path = base_dir / dir_name;
        if (std::filesystem::exists(dir_path)) {
            auto perms = std::filesystem::status(dir_path).permissions();
            
            std::cout << "   " << dir_name << ": ";
            
            // Check basic permissions (simplified)
            if ((perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none) {
                std::cout << "R";
            } else {
                std::cout << "-";
            }
            
            if ((perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none) {
                std::cout << "W";
            } else {
                std::cout << "-";
            }
            
            if ((perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) {
                std::cout << "X";
            } else {
                std::cout << "-";
            }
            
            std::cout << " (owner permissions)\n";
        }
    }
    
    std::cout << "\n5. Cleanup demonstration:\n";
    
    // Clean up our demo directory
    std::cout << "   Cleaning up demo files...\n";
    
    try {
        auto removed_count = std::filesystem::remove_all(base_dir);
        std::cout << "   ✓ Removed " << removed_count << " items\n";
        std::cout << "   ✓ Demo directory cleaned up\n";
    } catch (const std::exception& e) {
        std::cout << "   × Cleanup failed: " << e.what() << "\n";
    }
    
    logger->Info("File system monitoring completed");
}

int main() {
    std::cout << "=== Vision Infrastructure File Operations Demo ===\n\n";
    
    try {
        auto main_logger = LoggerManager::GetLogger("main");
        main_logger->SetLevel(LogLevel::INFO);
        main_logger->Info("File operations demo started");
        
        // Run all demonstrations
        DemonstrateBasicFileOperations();
        DemonstrateImageFileOperations();
        DemonstrateConfigurationFileHandling();
        DemonstrateLogFileManagement();
        DemonstrateFileSystemMonitoring();
        
        std::cout << "\n=== Summary ===\n";
        std::cout << "✓ Basic file operations (create, read, write, copy, move, delete)\n";
        std::cout << "✓ Image file handling and format conversions\n";
        std::cout << "✓ Configuration file management (JSON, INI, YAML)\n";
        std::cout << "✓ Log file management and rotation strategies\n";
        std::cout << "✓ File system monitoring and space analysis\n\n";
        
        std::cout << "File operations demo completed successfully!\n";
        
        main_logger->Info("Demo completed successfully");
        
    } catch (const std::exception& e) {
        std::cerr << "Error during file operations demo: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}