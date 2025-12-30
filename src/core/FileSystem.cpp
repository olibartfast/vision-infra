#include "vision-infra/core/FileSystem.hpp"
#include <filesystem>
#include <fstream>
#include <set>
#include <algorithm>

namespace vision_infra {
namespace core {

// FileSystem implementation
bool FileSystem::Exists(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::exists(path, ec);
}

bool FileSystem::IsFile(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::is_regular_file(path, ec);
}

bool FileSystem::IsDirectory(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::is_directory(path, ec);
}

bool FileSystem::CreateDirectory(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::create_directory(path, ec);
}

bool FileSystem::CreateDirectories(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::create_directories(path, ec);
}

bool FileSystem::Remove(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::remove(path, ec);
}

bool FileSystem::RemoveAll(const std::string& path) const {
    std::error_code ec;
    return std::filesystem::remove_all(path, ec) > 0;
}

std::optional<std::string> FileSystem::ReadFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

bool FileSystem::WriteFile(const std::string& path, const std::string& content) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

std::vector<std::string> FileSystem::ListFiles(const std::string& directory) const {
    std::vector<std::string> files;
    std::error_code ec;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
        if (!ec && entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }
    
    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::string> FileSystem::ListDirectories(const std::string& directory) const {
    std::vector<std::string> directories;
    std::error_code ec;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
        if (!ec && entry.is_directory()) {
            directories.push_back(entry.path().filename().string());
        }
    }
    
    std::sort(directories.begin(), directories.end());
    return directories;
}

std::optional<size_t> FileSystem::GetFileSize(const std::string& path) const {
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    if (ec) {
        return std::nullopt;
    }
    return size;
}

std::optional<std::string> FileSystem::GetFileExtension(const std::string& path) const {
    std::filesystem::path p(path);
    std::string ext = p.extension().string();
    if (ext.empty()) {
        return std::nullopt;
    }
    return ext;
}

std::string FileSystem::GetFileName(const std::string& path) const {
    return std::filesystem::path(path).filename().string();
}

std::string FileSystem::GetDirectoryName(const std::string& path) const {
    return std::filesystem::path(path).parent_path().string();
}

std::string FileSystem::JoinPath(const std::string& left, const std::string& right) const {
    return (std::filesystem::path(left) / std::filesystem::path(right)).string();
}

std::string FileSystem::GetAbsolutePath(const std::string& path) const {
    std::error_code ec;
    auto abs_path = std::filesystem::absolute(path, ec);
    if (ec) {
        return path; // Return original path if conversion fails
    }
    return abs_path.string();
}

std::string FileSystem::GetCurrentWorkingDirectory() const {
    std::error_code ec;
    auto cwd = std::filesystem::current_path(ec);
    if (ec) {
        return "."; // Return current directory symbol if fails
    }
    return cwd.string();
}

// FileSystemUtils implementation
std::shared_ptr<IFileSystem> FileSystemUtils::default_file_system_ = std::make_shared<FileSystem>();

std::shared_ptr<IFileSystem> FileSystemUtils::GetDefault() {
    return default_file_system_;
}

void FileSystemUtils::SetDefault(std::shared_ptr<IFileSystem> fs) {
    default_file_system_ = fs;
}

bool FileSystemUtils::IsImageFile(const std::string& filename) {
    static const std::set<std::string> image_extensions = {
        ".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff", ".tif", ".webp", 
        ".ico", ".ppm", ".pgm", ".pbm", ".sr", ".ras", ".jp2"
    };
    
    auto fs = GetDefault();
    auto ext = fs->GetFileExtension(filename);
    if (!ext.has_value()) {
        return false;
    }
    
    std::string extension = ext.value();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return image_extensions.find(extension) != image_extensions.end();
}

bool FileSystemUtils::IsVideoFile(const std::string& filename) {
    static const std::set<std::string> video_extensions = {
        ".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm", ".m4v", 
        ".3gp", ".3g2", ".mxf", ".roq", ".nsv", ".f4v", ".f4p", ".f4a", ".f4b"
    };
    
    auto fs = GetDefault();
    auto ext = fs->GetFileExtension(filename);
    if (!ext.has_value()) {
        return false;
    }
    
    std::string extension = ext.value();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return video_extensions.find(extension) != video_extensions.end();
}

bool FileSystemUtils::IsModelFile(const std::string& filename) {
    static const std::set<std::string> model_extensions = {
        ".onnx", ".pb", ".trt", ".engine", ".plan", ".pth", ".pt", ".h5", 
        ".savedmodel", ".tflite", ".mlmodel", ".bin", ".caffemodel", ".prototxt"
    };
    
    auto fs = GetDefault();
    auto ext = fs->GetFileExtension(filename);
    if (!ext.has_value()) {
        return false;
    }
    
    std::string extension = ext.value();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return model_extensions.find(extension) != model_extensions.end();
}

std::vector<std::string> FileSystemUtils::GetSupportedImageExtensions() {
    return {".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff", ".tif", ".webp", 
            ".ico", ".ppm", ".pgm", ".pbm", ".sr", ".ras", ".jp2"};
}

std::vector<std::string> FileSystemUtils::GetSupportedVideoExtensions() {
    return {".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm", ".m4v", 
            ".3gp", ".3g2", ".mxf", ".roq", ".nsv", ".f4v", ".f4p", ".f4a", ".f4b"};
}

std::vector<std::string> FileSystemUtils::GetSupportedModelExtensions() {
    return {".onnx", ".pb", ".trt", ".engine", ".plan", ".pth", ".pt", ".h5", 
            ".savedmodel", ".tflite", ".mlmodel", ".bin", ".caffemodel", ".prototxt"};
}

} // namespace core
} // namespace vision_infra