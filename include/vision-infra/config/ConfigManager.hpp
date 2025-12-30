#pragma once

#include "Config.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace vision_infra {
namespace config {

/**
 * Interface for configuration loading operations
 */
class IConfigLoader {
public:
    virtual ~IConfigLoader() = default;
    virtual std::unique_ptr<InferenceConfig> LoadFromCommandLine(int argc, const char* argv[]) = 0;
    virtual std::unique_ptr<InferenceConfig> LoadFromEnvironment() = 0;
    virtual std::unique_ptr<InferenceConfig> LoadFromFile(const std::string& filename) = 0;
    virtual std::unique_ptr<InferenceConfig> CreateDefault() = 0;
};

/**
 * Interface for configuration validation
 */
class IConfigValidator {
public:
    virtual ~IConfigValidator() = default;
    virtual bool Validate(const InferenceConfig& config) = 0;
    virtual std::string GetValidationErrors(const InferenceConfig& config) = 0;
};

/**
 * Interface for configuration serialization
 */
class IConfigSerializer {
public:
    virtual ~IConfigSerializer() = default;
    virtual void SaveToFile(const InferenceConfig& config, const std::string& filename) = 0;
    virtual std::unique_ptr<InferenceConfig> LoadFromFile(const std::string& filename) = 0;
};

/**
 * Configuration manager for loading and validating inference configurations
 */
class ConfigManager {
public:
    explicit ConfigManager(
        std::unique_ptr<IConfigLoader> loader = nullptr,
        std::unique_ptr<IConfigValidator> validator = nullptr
    );
    
    ~ConfigManager();
    
    // Disable copy operations
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Enable move operations
    ConfigManager(ConfigManager&&) noexcept;
    ConfigManager& operator=(ConfigManager&&) noexcept;
    
    /**
     * Load configuration from command line arguments
     */
    std::unique_ptr<InferenceConfig> LoadFromCommandLine(int argc, const char* argv[]);
    
    /**
     * Create configuration from argument vector (for testing)
     */
    std::unique_ptr<InferenceConfig> CreateFromArguments(const std::vector<std::string>& args);
    
    /**
     * Load configuration from environment variables
     */
    std::unique_ptr<InferenceConfig> LoadFromEnvironment();
    
    /**
     * Load configuration from file
     */
    std::unique_ptr<InferenceConfig> LoadFromFile(const std::string& filename);
    
    /**
     * Create default configuration
     */
    std::unique_ptr<InferenceConfig> CreateDefault();
    
    /**
     * Print configuration to stdout
     */
    void PrintConfig(const InferenceConfig& config) const;
    
    /**
     * Merge configurations (second config takes precedence)
     */
    std::unique_ptr<InferenceConfig> Merge(
        const InferenceConfig& base, 
        const InferenceConfig& override
    ) const;
    
    /**
     * Validate configuration
     */
    bool ValidateConfig(const InferenceConfig& config) const;
    
    /**
     * Get validation errors
     */
    std::string GetValidationErrors(const InferenceConfig& config) const;
    
    /**
     * Register custom serializer for specific file types
     */
    void RegisterSerializer(const std::string& extension, std::unique_ptr<IConfigSerializer> serializer);

    // Static factory methods for convenience
    static std::unique_ptr<ConfigManager> Create();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

/**
 * Default implementation of IConfigLoader
 */
class DefaultConfigLoader : public IConfigLoader {
public:
    DefaultConfigLoader() = default;
    ~DefaultConfigLoader() override = default;
    
    std::unique_ptr<InferenceConfig> LoadFromCommandLine(int argc, const char* argv[]) override;
    std::unique_ptr<InferenceConfig> LoadFromEnvironment() override;
    std::unique_ptr<InferenceConfig> LoadFromFile(const std::string& filename) override;
    std::unique_ptr<InferenceConfig> CreateDefault() override;

private:
    static std::vector<std::vector<int64_t>> ParseInputSizes(const std::string& input);
    static std::string GetEnvVar(const std::string& name, const std::string& default_value = "");
    static void PrintUsage();
};

/**
 * Default implementation of IConfigValidator
 */
class DefaultConfigValidator : public IConfigValidator {
public:
    DefaultConfigValidator() = default;
    ~DefaultConfigValidator() override = default;
    
    bool Validate(const InferenceConfig& config) override;
    std::string GetValidationErrors(const InferenceConfig& config) override;
};

} // namespace config
} // namespace vision_infra