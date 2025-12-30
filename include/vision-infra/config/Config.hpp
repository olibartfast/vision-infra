#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <cstdint>

namespace vision_infra {
namespace config {

/**
 * Base configuration class for inference applications
 */
class InferenceConfig {
public:
    InferenceConfig() = default;
    virtual ~InferenceConfig() = default;
    
    // Disable copy construction and assignment for safety
    InferenceConfig(const InferenceConfig&) = delete;
    InferenceConfig& operator=(const InferenceConfig&) = delete;
    
    // Enable move construction and assignment
    InferenceConfig(InferenceConfig&&) = default;
    InferenceConfig& operator=(InferenceConfig&&) = default;
    
    // Server configuration getters/setters
    const std::string& GetServerAddress() const noexcept { return server_address_; }
    void SetServerAddress(const std::string& address) { server_address_ = address; }
    
    int GetPort() const noexcept { return port_; }
    void SetPort(int port) { port_ = port; }
    
    const std::string& GetProtocol() const noexcept { return protocol_; }
    void SetProtocol(const std::string& protocol) { protocol_ = protocol; }
    
    bool GetVerbose() const noexcept { return verbose_; }
    void SetVerbose(bool verbose) noexcept { verbose_ = verbose; }
    
    // Model configuration getters/setters
    const std::string& GetModelName() const noexcept { return model_name_; }
    void SetModelName(const std::string& name) { model_name_ = name; }
    
    const std::string& GetModelVersion() const noexcept { return model_version_; }
    void SetModelVersion(const std::string& version) { model_version_ = version; }
    
    const std::string& GetModelType() const noexcept { return model_type_; }
    void SetModelType(const std::string& type) { model_type_ = type; }
    
    const std::vector<std::vector<int64_t>>& GetInputSizes() const noexcept { return input_sizes_; }
    void SetInputSizes(const std::vector<std::vector<int64_t>>& sizes) { input_sizes_ = sizes; }
    
    // Input/Output configuration
    const std::string& GetSource() const noexcept { return source_; }
    void SetSource(const std::string& source) { source_ = source; }
    
    const std::string& GetLabelsFile() const noexcept { return labels_file_; }
    void SetLabelsFile(const std::string& file) { labels_file_ = file; }
    
    int GetBatchSize() const noexcept { return batch_size_; }
    void SetBatchSize(int size) { batch_size_ = size; }
    
    // Processing configuration
    bool GetShowFrame() const noexcept { return show_frame_; }
    void SetShowFrame(bool show) noexcept { show_frame_ = show; }
    
    bool GetWriteFrame() const noexcept { return write_frame_; }
    void SetWriteFrame(bool write) noexcept { write_frame_ = write; }
    
    float GetConfidenceThreshold() const noexcept { return confidence_threshold_; }
    void SetConfidenceThreshold(float threshold) noexcept { confidence_threshold_ = threshold; }
    
    float GetNmsThreshold() const noexcept { return nms_threshold_; }
    void SetNmsThreshold(float threshold) noexcept { nms_threshold_ = threshold; }
    
    // Performance configuration
    int GetNumThreads() const noexcept { return num_threads_; }
    void SetNumThreads(int threads) noexcept { num_threads_ = threads; }
    
    bool GetEnableAsync() const noexcept { return enable_async_; }
    void SetEnableAsync(bool async) noexcept { enable_async_ = async; }
    
    // Shared memory configuration
    const std::string& GetSharedMemoryType() const noexcept { return shared_memory_type_; }
    void SetSharedMemoryType(const std::string& type) { shared_memory_type_ = type; }
    
    int GetCudaDeviceId() const noexcept { return cuda_device_id_; }
    void SetCudaDeviceId(int device_id) noexcept { cuda_device_id_ = device_id; }
    
    // Logging configuration
    const std::string& GetLogLevel() const noexcept { return log_level_; }
    void SetLogLevel(const std::string& level) { log_level_ = level; }
    
    const std::string& GetLogFile() const noexcept { return log_file_; }
    void SetLogFile(const std::string& file) { log_file_ = file; }
    
    // Custom parameters
    void SetCustomParam(const std::string& key, const std::string& value);
    std::optional<std::string> GetCustomParam(const std::string& key) const;
    
    // Validation methods
    virtual bool IsValid() const;
    virtual std::string GetValidationErrors() const;
    
    // Helper methods
    bool IsModelNameAPath() const;

private:
    // Server configuration
    std::string server_address_{"localhost"};
    int port_{8000};
    std::string protocol_{"http"};
    bool verbose_{false};
    
    // Model configuration
    std::string model_name_;
    std::string model_version_;
    std::string model_type_;
    std::vector<std::vector<int64_t>> input_sizes_;
    
    // Input/Output configuration
    std::string source_;
    std::string labels_file_;
    int batch_size_{1};
    
    // Processing configuration
    bool show_frame_{false};
    bool write_frame_{true};
    float confidence_threshold_{0.5f};
    float nms_threshold_{0.4f};
    
    // Performance configuration
    int num_threads_{1};
    bool enable_async_{false};
    
    // Shared memory configuration
    std::string shared_memory_type_{"none"};
    int cuda_device_id_{0};
    
    // Logging configuration
    std::string log_level_{"info"};
    std::string log_file_;
    
    // Additional custom parameters
    std::unordered_map<std::string, std::string> custom_params_;
};

} // namespace config
} // namespace vision_infra