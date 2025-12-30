#include "vision-infra/config/Config.hpp"
#include <vector>
#include <string>

namespace vision_infra {
namespace config {

bool InferenceConfig::IsValid() const {
    return !GetServerAddress().empty() && 
           !GetModelName().empty() && 
           !GetModelType().empty() && 
           !GetSource().empty() &&
           GetPort() > 0 && GetPort() <= 65535 &&
           !IsModelNameAPath();
}

bool InferenceConfig::IsModelNameAPath() const {
    const auto& model_name = GetModelName();
    return model_name.find('/') != std::string::npos || 
           model_name.find('\\') != std::string::npos;
}

std::string InferenceConfig::GetValidationErrors() const {
    std::vector<std::string> errors;
    
    if (GetServerAddress().empty()) {
        errors.emplace_back("Server address is required");
    }
    if (GetModelName().empty()) {
        errors.emplace_back("Model name is required");
    }
    if (IsModelNameAPath()) {
        errors.emplace_back("Model name must not contain path separators (/ or \\). Use only the model repository name.");
    }
    if (GetModelType().empty()) {
        errors.emplace_back("Model type is required");
    }
    if (GetSource().empty()) {
        errors.emplace_back("Source is required");
    }
    if (GetPort() <= 0 || GetPort() > 65535) {
        errors.emplace_back("Port must be between 1 and 65535");
    }
    
    const auto& protocol = GetProtocol();
    if (protocol != "http" && protocol != "grpc") {
        errors.emplace_back("Protocol must be 'http' or 'grpc'");
    }
    
    std::string result;
    for (const auto& error : errors) {
        if (!result.empty()) {
            result += "; ";
        }
        result += error;
    }
    return result;
}

void InferenceConfig::SetCustomParam(const std::string& key, const std::string& value) {
    custom_params_[key] = value;
}

std::optional<std::string> InferenceConfig::GetCustomParam(const std::string& key) const {
    const auto it = custom_params_.find(key);
    if (it != custom_params_.end()) {
        return it->second;
    }
    return std::nullopt;
}

} // namespace config
} // namespace vision_infra