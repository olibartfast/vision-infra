#include "vision-infra/core/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <mutex>

namespace vision_infra {
namespace core {

// Logger::Impl (PIMPL implementation)
class Logger::Impl {
public:
    std::string name_;
    LogLevel current_level_{LogLevel::INFO};
    std::ofstream file_stream_;
    bool console_enabled_{true};
    bool timestamp_enabled_{true};
    std::string pattern_{"[{timestamp}] [{level}] [{name}] {message}"};
    std::mutex log_mutex_;
    
    std::string FormatMessage(LogLevel level, const std::string& message) {
        std::string formatted = pattern_;
        
        // Replace timestamp
        if (timestamp_enabled_) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            
            size_t pos = formatted.find("{timestamp}");
            if (pos != std::string::npos) {
                formatted.replace(pos, 11, ss.str());
            }
        }
        
        // Replace level
        size_t pos = formatted.find("{level}");
        if (pos != std::string::npos) {
            formatted.replace(pos, 7, LoggerManager::LogLevelToString(level));
        }
        
        // Replace name
        pos = formatted.find("{name}");
        if (pos != std::string::npos) {
            formatted.replace(pos, 6, name_);
        }
        
        // Replace message
        pos = formatted.find("{message}");
        if (pos != std::string::npos) {
            formatted.replace(pos, 9, message);
        }
        
        return formatted;
    }
};

// Logger implementation
Logger::Logger(const std::string& name) : pImpl_(std::make_unique<Impl>()) {
    pImpl_->name_ = name.empty() ? "default" : name;
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (level < pImpl_->current_level_) return;
    
    std::lock_guard<std::mutex> lock(pImpl_->log_mutex_);
    
    std::string formatted = pImpl_->FormatMessage(level, message);
    
    // Console output
    if (pImpl_->console_enabled_) {
        if (level >= LogLevel::ERROR) {
            std::cerr << formatted << std::endl;
        } else {
            std::cout << formatted << std::endl;
        }
    }
    
    // File output
    if (pImpl_->file_stream_.is_open()) {
        pImpl_->file_stream_ << formatted << std::endl;
    }
}

void Logger::SetLevel(LogLevel level) {
    pImpl_->current_level_ = level;
}

LogLevel Logger::GetLevel() const {
    return pImpl_->current_level_;
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(pImpl_->log_mutex_);
    if (pImpl_->console_enabled_) {
        std::cout.flush();
        std::cerr.flush();
    }
    if (pImpl_->file_stream_.is_open()) {
        pImpl_->file_stream_.flush();
    }
}

void Logger::SetOutputFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl_->log_mutex_);
    if (pImpl_->file_stream_.is_open()) {
        pImpl_->file_stream_.close();
    }
    
    if (!filename.empty()) {
        pImpl_->file_stream_.open(filename, std::ios::app);
    }
}

void Logger::EnableConsoleOutput(bool enable) {
    pImpl_->console_enabled_ = enable;
}

void Logger::EnableTimestamp(bool enable) {
    pImpl_->timestamp_enabled_ = enable;
}

void Logger::SetPattern(const std::string& pattern) {
    pImpl_->pattern_ = pattern;
}

void Logger::Trace(const std::string& message) {
    Log(LogLevel::TRACE, message);
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void Logger::Warn(const std::string& message) {
    Log(LogLevel::WARN, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::ERROR, message);
}

void Logger::Fatal(const std::string& message) {
    Log(LogLevel::FATAL, message);
}

// LoggerManager implementation
class LoggerManagerImpl {
public:
    std::unordered_map<std::string, std::shared_ptr<ILogger>> loggers_;
    std::shared_ptr<ILogger> default_logger_;
    std::mutex manager_mutex_;
    LogLevel global_level_{LogLevel::INFO};
    
    LoggerManagerImpl() {
        default_logger_ = std::make_shared<Logger>("default");
    }
};

static LoggerManagerImpl& GetManagerImpl() {
    static LoggerManagerImpl instance;
    return instance;
}

std::shared_ptr<ILogger> LoggerManager::GetLogger(const std::string& name) {
    auto& impl = GetManagerImpl();
    std::lock_guard<std::mutex> lock(impl.manager_mutex_);
    
    if (name.empty() || name == "default") {
        return impl.default_logger_;
    }
    
    auto it = impl.loggers_.find(name);
    if (it != impl.loggers_.end()) {
        return it->second;
    }
    
    auto logger = std::make_shared<Logger>(name);
    logger->SetLevel(impl.global_level_);
    impl.loggers_[name] = logger;
    return logger;
}

void LoggerManager::SetDefaultLogger(std::shared_ptr<ILogger> logger) {
    auto& impl = GetManagerImpl();
    std::lock_guard<std::mutex> lock(impl.manager_mutex_);
    impl.default_logger_ = logger;
}

void LoggerManager::SetGlobalLevel(LogLevel level) {
    auto& impl = GetManagerImpl();
    std::lock_guard<std::mutex> lock(impl.manager_mutex_);
    impl.global_level_ = level;
    
    // Update all existing loggers
    if (impl.default_logger_) {
        impl.default_logger_->SetLevel(level);
    }
    for (auto& [name, logger] : impl.loggers_) {
        logger->SetLevel(level);
    }
}

LogLevel LoggerManager::ParseLogLevel(const std::string& level) {
    std::string lower_level = level;
    std::transform(lower_level.begin(), lower_level.end(), lower_level.begin(), ::tolower);
    
    if (lower_level == "trace") return LogLevel::TRACE;
    if (lower_level == "debug") return LogLevel::DEBUG;
    if (lower_level == "info") return LogLevel::INFO;
    if (lower_level == "warn" || lower_level == "warning") return LogLevel::WARN;
    if (lower_level == "error") return LogLevel::ERROR;
    if (lower_level == "fatal") return LogLevel::FATAL;
    
    return LogLevel::INFO; // Default
}

std::string LoggerManager::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

} // namespace core
} // namespace vision_infra