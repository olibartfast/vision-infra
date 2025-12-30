#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace vision_infra {
namespace core {

/**
 * Interface for file system operations
 */
class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    
    virtual bool Exists(const std::string& path) const = 0;
    virtual bool IsFile(const std::string& path) const = 0;
    virtual bool IsDirectory(const std::string& path) const = 0;
    virtual bool CreateDirectory(const std::string& path) const = 0;
    virtual bool CreateDirectories(const std::string& path) const = 0;
    virtual bool Remove(const std::string& path) const = 0;
    virtual bool RemoveAll(const std::string& path) const = 0;
    virtual std::optional<std::string> ReadFile(const std::string& path) const = 0;
    virtual bool WriteFile(const std::string& path, const std::string& content) const = 0;
    virtual std::vector<std::string> ListFiles(const std::string& directory) const = 0;
    virtual std::vector<std::string> ListDirectories(const std::string& directory) const = 0;
    virtual std::optional<size_t> GetFileSize(const std::string& path) const = 0;
    virtual std::optional<std::string> GetFileExtension(const std::string& path) const = 0;
    virtual std::string GetFileName(const std::string& path) const = 0;
    virtual std::string GetDirectoryName(const std::string& path) const = 0;
    virtual std::string JoinPath(const std::string& left, const std::string& right) const = 0;
    virtual std::string GetAbsolutePath(const std::string& path) const = 0;
    virtual std::string GetCurrentWorkingDirectory() const = 0;
};

/**
 * Standard file system implementation
 */
class FileSystem : public IFileSystem {
public:
    FileSystem() = default;
    ~FileSystem() override = default;
    
    bool Exists(const std::string& path) const override;
    bool IsFile(const std::string& path) const override;
    bool IsDirectory(const std::string& path) const override;
    bool CreateDirectory(const std::string& path) const override;
    bool CreateDirectories(const std::string& path) const override;
    bool Remove(const std::string& path) const override;
    bool RemoveAll(const std::string& path) const override;
    std::optional<std::string> ReadFile(const std::string& path) const override;
    bool WriteFile(const std::string& path, const std::string& content) const override;
    std::vector<std::string> ListFiles(const std::string& directory) const override;
    std::vector<std::string> ListDirectories(const std::string& directory) const override;
    std::optional<size_t> GetFileSize(const std::string& path) const override;
    std::optional<std::string> GetFileExtension(const std::string& path) const override;
    std::string GetFileName(const std::string& path) const override;
    std::string GetDirectoryName(const std::string& path) const override;
    std::string JoinPath(const std::string& left, const std::string& right) const override;
    std::string GetAbsolutePath(const std::string& path) const override;
    std::string GetCurrentWorkingDirectory() const override;
};

/**
 * File system utilities
 */
class FileSystemUtils {
public:
    static std::shared_ptr<IFileSystem> GetDefault();
    static void SetDefault(std::shared_ptr<IFileSystem> fs);
    
    // Convenience functions
    static bool IsImageFile(const std::string& filename);
    static bool IsVideoFile(const std::string& filename);
    static bool IsModelFile(const std::string& filename);
    static std::vector<std::string> GetSupportedImageExtensions();
    static std::vector<std::string> GetSupportedVideoExtensions();
    static std::vector<std::string> GetSupportedModelExtensions();
    
private:
    static std::shared_ptr<IFileSystem> default_file_system_;
};

} // namespace core
} // namespace vision_infra