#include "vision-infra/config/ConfigManager.hpp"
#include "vision-infra/utils/VisionUtils.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace vision_infra {
namespace config {

// PIMPL implementation for ConfigManager
class ConfigManager::Impl {
public:
    std::unique_ptr<IConfigLoader> loader_;
    std::unique_ptr<IConfigValidator> validator_;
    std::unordered_map<std::string, std::unique_ptr<IConfigSerializer>> serializers_;
    
    Impl(std::unique_ptr<IConfigLoader> loader, std::unique_ptr<IConfigValidator> validator)
        : loader_(std::move(loader)), validator_(std::move(validator)) {
        if (!loader_) {
            loader_ = std::make_unique<DefaultConfigLoader>();
        }
        if (!validator_) {
            validator_ = std::make_unique<DefaultConfigValidator>();
        }
    }
};

// ConfigManager implementation
ConfigManager::ConfigManager(std::unique_ptr<IConfigLoader> loader, std::unique_ptr<IConfigValidator> validator)
    : pImpl_(std::make_unique<Impl>(std::move(loader), std::move(validator))) {}

ConfigManager::~ConfigManager() = default;

ConfigManager::ConfigManager(ConfigManager&&) noexcept = default;
ConfigManager& ConfigManager::operator=(ConfigManager&&) noexcept = default;

std::unique_ptr<InferenceConfig> ConfigManager::LoadFromCommandLine(int argc, const char* argv[]) {
    return pImpl_->loader_->LoadFromCommandLine(argc, argv);
}

std::unique_ptr<InferenceConfig> ConfigManager::CreateFromArguments(const std::vector<std::string>& args) {
    std::vector<const char*> argv;
    argv.push_back("program");
    for (const auto& arg : args) {
        argv.push_back(arg.c_str());
    }
    return LoadFromCommandLine(static_cast<int>(argv.size()), argv.data());
}

std::unique_ptr<InferenceConfig> ConfigManager::LoadFromEnvironment() {
    return pImpl_->loader_->LoadFromEnvironment();
}

std::unique_ptr<InferenceConfig> ConfigManager::LoadFromFile(const std::string& filename) {
    return pImpl_->loader_->LoadFromFile(filename);
}

std::unique_ptr<InferenceConfig> ConfigManager::CreateDefault() {
    return pImpl_->loader_->CreateDefault();
}

void ConfigManager::PrintConfig(const InferenceConfig& config) const {
    std::cout << "Configuration:\n";
    std::cout << "  Server: " << config.GetServerAddress() << ":" << config.GetPort() << " (" << config.GetProtocol() << ")\n";
    std::cout << "  Model: " << config.GetModelName() << " (" << config.GetModelType() << ")\n";
    std::cout << "  Source: " << config.GetSource() << "\n";
    std::cout << "  Labels: " << config.GetLabelsFile() << "\n";
    std::cout << "  Batch Size: " << config.GetBatchSize() << "\n";
    std::cout << "  Show Frame: " << (config.GetShowFrame() ? "true" : "false") << "\n";
    std::cout << "  Write Frame: " << (config.GetWriteFrame() ? "true" : "false") << "\n";
    std::cout << "  Confidence Threshold: " << config.GetConfidenceThreshold() << "\n";
    std::cout << "  NMS Threshold: " << config.GetNmsThreshold() << "\n";
    std::cout << "  Verbose: " << (config.GetVerbose() ? "true" : "false") << "\n";
    std::cout << "  Shared Memory Type: " << config.GetSharedMemoryType() << "\n";
    if (config.GetSharedMemoryType() == "cuda") {
        std::cout << "  CUDA Device ID: " << config.GetCudaDeviceId() << "\n";
    }
    std::cout << "  Log Level: " << config.GetLogLevel() << "\n";
    if (!config.GetLogFile().empty()) {
        std::cout << "  Log File: " << config.GetLogFile() << "\n";
    }
}

bool ConfigManager::ValidateConfig(const InferenceConfig& config) const {
    return pImpl_->validator_->Validate(config);
}

std::string ConfigManager::GetValidationErrors(const InferenceConfig& config) const {
    return pImpl_->validator_->GetValidationErrors(config);
}

std::unique_ptr<InferenceConfig> ConfigManager::Merge(const InferenceConfig& base, const InferenceConfig& override) const {
    auto merged = std::make_unique<InferenceConfig>();
    
    // Copy base values
    merged->SetServerAddress(base.GetServerAddress());
    merged->SetPort(base.GetPort());
    merged->SetProtocol(base.GetProtocol());
    merged->SetModelName(base.GetModelName());
    merged->SetModelType(base.GetModelType());
    merged->SetSource(base.GetSource());
    merged->SetLabelsFile(base.GetLabelsFile());
    merged->SetBatchSize(base.GetBatchSize());
    merged->SetShowFrame(base.GetShowFrame());
    merged->SetWriteFrame(base.GetWriteFrame());
    merged->SetConfidenceThreshold(base.GetConfidenceThreshold());
    merged->SetNmsThreshold(base.GetNmsThreshold());
    merged->SetVerbose(base.GetVerbose());
    merged->SetSharedMemoryType(base.GetSharedMemoryType());
    merged->SetCudaDeviceId(base.GetCudaDeviceId());
    merged->SetLogLevel(base.GetLogLevel());
    merged->SetLogFile(base.GetLogFile());
    
    // Override with non-default values
    if (!override.GetServerAddress().empty() && override.GetServerAddress() != "localhost") {
        merged->SetServerAddress(override.GetServerAddress());
    }
    if (override.GetPort() != 8000) merged->SetPort(override.GetPort());
    if (!override.GetProtocol().empty() && override.GetProtocol() != "http") {
        merged->SetProtocol(override.GetProtocol());
    }
    if (!override.GetModelName().empty()) merged->SetModelName(override.GetModelName());
    if (!override.GetModelType().empty()) merged->SetModelType(override.GetModelType());
    if (!override.GetSource().empty()) merged->SetSource(override.GetSource());
    if (!override.GetLabelsFile().empty()) merged->SetLabelsFile(override.GetLabelsFile());
    if (override.GetBatchSize() != 1) merged->SetBatchSize(override.GetBatchSize());
    
    // For boolean values, always take override
    merged->SetShowFrame(override.GetShowFrame());
    merged->SetWriteFrame(override.GetWriteFrame());
    merged->SetVerbose(override.GetVerbose());
    
    return merged;
}

void ConfigManager::RegisterSerializer(const std::string& extension, std::unique_ptr<IConfigSerializer> serializer) {
    pImpl_->serializers_[extension] = std::move(serializer);
}

std::unique_ptr<ConfigManager> ConfigManager::CreateDefault() {
    return std::make_unique<ConfigManager>(nullptr, nullptr);
}

// DefaultConfigLoader implementation
std::unique_ptr<InferenceConfig> DefaultConfigLoader::LoadFromCommandLine(int argc, const char* argv[]) {
    auto config = std::make_unique<InferenceConfig>();
    
    const cv::String keys =
        "{help h usage ? |      | show this help message}"
        "{source s       |      | path to input image/video file}"
        "{model_type mt  |      | type of model (yolov5, yolov8, etc.)}"
        "{model m        |      | model name on inference server}"
        "{labelsFile lf  |      | path to labels file}"
        "{protocol p     |http  | protocol to use (http or grpc)}"
        "{serverAddress sa |localhost | inference server address}"
        "{port pt        |8000  | inference server port}"
        "{input_sizes is |      | input sizes for dynamic axes (format: 'c,h,w;c,h,w')}"
        "{batch_size bs  |1     | batch size}"
        "{show_frame sf  |false | show processed frames}"
        "{write_frame wf |true  | write processed frames to disk}"
        "{confidence_threshold ct |0.5 | confidence threshold}"
        "{nms_threshold nt |0.4 | NMS threshold}"
        "{verbose v      |false | verbose output}"
        "{shared_memory_type smt |none | shared memory type (none, system, cuda)}"
        "{cuda_device_id cdi |0  | CUDA device ID for CUDA shared memory}"
        "{log_level ll   |info  | log level (debug, info, warn, error)}"
        "{log_file lf    |      | log file path}";

    cv::CommandLineParser parser(argc, argv, keys);
    
    if (parser.has("help")) {
        parser.printMessage();
        return nullptr;
    }

    // Parse command line arguments
    config->SetSource(parser.get<cv::String>("source"));
    config->SetModelType(parser.get<cv::String>("model_type"));
    config->SetModelName(parser.get<cv::String>("model"));
    config->SetLabelsFile(parser.get<cv::String>("labelsFile"));
    config->SetProtocol(parser.get<cv::String>("protocol"));
    config->SetServerAddress(parser.get<cv::String>("serverAddress"));
    config->SetPort(parser.get<int>("port"));
    config->SetBatchSize(parser.get<int>("batch_size"));
    config->SetShowFrame(parser.get<bool>("show_frame"));
    config->SetWriteFrame(parser.get<bool>("write_frame"));
    config->SetConfidenceThreshold(parser.get<float>("confidence_threshold"));
    config->SetNmsThreshold(parser.get<float>("nms_threshold"));
    config->SetVerbose(parser.get<bool>("verbose"));
    config->SetSharedMemoryType(parser.get<cv::String>("shared_memory_type"));
    config->SetCudaDeviceId(parser.get<int>("cuda_device_id"));
    config->SetLogLevel(parser.get<cv::String>("log_level"));
    config->SetLogFile(parser.get<cv::String>("log_file"));

    // Parse input sizes if provided
    if (parser.has("input_sizes")) {
        std::string input_sizes_str = parser.get<cv::String>("input_sizes");
        config->SetInputSizes(ParseInputSizes(input_sizes_str));
    }

    return config;
}

std::unique_ptr<InferenceConfig> DefaultConfigLoader::LoadFromEnvironment() {
    auto config = std::make_unique<InferenceConfig>();
    
    config->SetServerAddress(GetEnvVar("INFERENCE_SERVER_ADDRESS", "localhost"));
    config->SetPort(std::stoi(GetEnvVar("INFERENCE_SERVER_PORT", "8000")));
    config->SetProtocol(GetEnvVar("INFERENCE_PROTOCOL", "http"));
    config->SetModelName(GetEnvVar("INFERENCE_MODEL_NAME", ""));
    config->SetModelType(GetEnvVar("INFERENCE_MODEL_TYPE", ""));
    config->SetSource(GetEnvVar("INFERENCE_SOURCE", ""));
    config->SetLabelsFile(GetEnvVar("INFERENCE_LABELS_FILE", ""));
    config->SetBatchSize(std::stoi(GetEnvVar("INFERENCE_BATCH_SIZE", "1")));
    config->SetShowFrame(GetEnvVar("INFERENCE_SHOW_FRAME", "false") == "true");
    config->SetWriteFrame(GetEnvVar("INFERENCE_WRITE_FRAME", "true") == "true");
    config->SetConfidenceThreshold(std::stof(GetEnvVar("INFERENCE_CONFIDENCE_THRESHOLD", "0.5")));
    config->SetNmsThreshold(std::stof(GetEnvVar("INFERENCE_NMS_THRESHOLD", "0.4")));
    config->SetVerbose(GetEnvVar("INFERENCE_VERBOSE", "false") == "true");
    config->SetSharedMemoryType(GetEnvVar("INFERENCE_SHARED_MEMORY_TYPE", "none"));
    config->SetCudaDeviceId(std::stoi(GetEnvVar("INFERENCE_CUDA_DEVICE_ID", "0")));
    config->SetLogLevel(GetEnvVar("INFERENCE_LOG_LEVEL", "info"));
    config->SetLogFile(GetEnvVar("INFERENCE_LOG_FILE", ""));
    
    std::string input_sizes_env = GetEnvVar("INFERENCE_INPUT_SIZES", "");
    if (!input_sizes_env.empty()) {
        config->SetInputSizes(ParseInputSizes(input_sizes_env));
    }

    return config;
}

std::unique_ptr<InferenceConfig> ConfigManager::loadFromJsonFile(const std::string& filename) {
    // TODO: Implement JSON parsing (requires JSON library like nlohmann::json)
    throw std::runtime_error("JSON config loading not yet implemented");
}

std::unique_ptr<InferenceConfig> ConfigManager::loadFromYamlFile(const std::string& filename) {
    // TODO: Implement YAML parsing (requires YAML library like yaml-cpp)
    throw std::runtime_error("YAML config loading not yet implemented");
}

std::unique_ptr<InferenceConfig> ConfigManager::createDefault() {
    return std::make_unique<InferenceConfig>();
}

void ConfigManager::saveToJsonFile(const InferenceConfig& config, const std::string& filename) {
    // TODO: Implement JSON saving
    throw std::runtime_error("JSON config saving not yet implemented");
}

void ConfigManager::saveToYamlFile(const InferenceConfig& config, const std::string& filename) {
    // TODO: Implement YAML saving  
    throw std::runtime_error("YAML config saving not yet implemented");
}

void ConfigManager::printConfig(const InferenceConfig& config) {
    std::cout << "Configuration:\n";
    std::cout << "  Server: " << config.server_address << ":" << config.port << " (" << config.protocol << ")\n";
    std::cout << "  Model: " << config.model_name << " (" << config.model_type << ")\n";
    std::cout << "  Source: " << config.source << "\n";
    std::cout << "  Labels: " << config.labels_file << "\n";
    std::cout << "  Batch Size: " << config.batch_size << "\n";
    std::cout << "  Show Frame: " << (config.show_frame ? "true" : "false") << "\n";
    std::cout << "  Write Frame: " << (config.write_frame ? "true" : "false") << "\n";
    std::cout << "  Confidence Threshold: " << config.confidence_threshold << "\n";
    std::cout << "  NMS Threshold: " << config.nms_threshold << "\n";
    std::cout << "  Verbose: " << (config.verbose ? "true" : "false") << "\n";
    std::cout << "  Shared Memory Type: " << config.shared_memory_type << "\n";
    if (config.shared_memory_type == "cuda") {
        std::cout << "  CUDA Device ID: " << config.cuda_device_id << "\n";
    }
    std::cout << "  Log Level: " << config.log_level << "\n";
    if (!config.log_file.empty()) {
        std::cout << "  Log File: " << config.log_file << "\n";
    }
}

std::unique_ptr<InferenceConfig> ConfigManager::merge(const InferenceConfig& base, const InferenceConfig& override) {
    auto merged = std::make_unique<InferenceConfig>(base);
    
    // Override non-empty values
    if (!override.server_address.empty()) merged->server_address = override.server_address;
    if (override.port != 8000) merged->port = override.port;
    if (!override.protocol.empty()) merged->protocol = override.protocol;
    if (!override.model_name.empty()) merged->model_name = override.model_name;
    if (!override.model_type.empty()) merged->model_type = override.model_type;
    if (!override.source.empty()) merged->source = override.source;
    if (!override.labels_file.empty()) merged->labels_file = override.labels_file;
    if (override.batch_size != 1) merged->batch_size = override.batch_size;
    
    // For boolean values, always take override
    merged->show_frame = override.show_frame;
    merged->write_frame = override.write_frame;
    merged->verbose = override.verbose;
    
    // For thresholds
    if (override.confidence_threshold != 0.5f) merged->confidence_threshold = override.confidence_threshold;
    if (override.nms_threshold != 0.4f) merged->nms_threshold = override.nms_threshold;
    
    // Merge custom parameters
    for (const auto& [key, value] : override.custom_params) {
        merged->setCustomParam(key, value);
    }
    
    return merged;
}

void ConfigManager::registerLoader(const std::string& extension, ConfigLoader loader) {
    // TODO: Implement custom loader registration
    static std::unordered_map<std::string, ConfigLoader> loaders;
    loaders[extension] = loader;
}

std::vector<std::vector<int64_t>> ConfigManager::parseInputSizes(const std::string& input) {
    return vision_infra::utils::InputParser::parseInputSizes(input);
}

std::string ConfigManager::getEnvVar(const std::string& name, const std::string& default_value) {
    const char* value = std::getenv(name.c_str());
    return value ? value : default_value;
}

void ConfigManager::printUsage() {
    std::cout << "Usage: program [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -s, --source=PATH       Input image/video file path\n";
    std::cout << "  -mt, --model_type=TYPE  Model type (yolov5, yolov8, etc.)\n";
    std::cout << "  -m, --model=NAME        Model name on inference server\n";
    // ... add more options
}

} // namespace config
} // namespace vision_infra