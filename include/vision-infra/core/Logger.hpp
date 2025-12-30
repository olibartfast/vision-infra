#pragma once

#include <string>
#include <memory>
#include <sstream>

namespace vision_infra {
namespace core {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

/**
 * Interface for logging implementations
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void Log(LogLevel level, const std::string& message) = 0;
    virtual void SetLevel(LogLevel level) = 0;
    virtual LogLevel GetLevel() const = 0;
    virtual void Flush() = 0;
};

/**
 * Logger implementation with file and console output
 */
class Logger : public ILogger {
public:
    explicit Logger(const std::string& name = "");
    ~Logger() override = default;
    
    void Log(LogLevel level, const std::string& message) override;
    void SetLevel(LogLevel level) override;
    LogLevel GetLevel() const override;
    void Flush() override;
    
    // Configuration
    void SetOutputFile(const std::string& filename);
    void EnableConsoleOutput(bool enable = true);
    void EnableTimestamp(bool enable = true);
    void SetPattern(const std::string& pattern);
    
    // Convenience methods
    void Trace(const std::string& message);
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warn(const std::string& message);
    void Error(const std::string& message);
    void Fatal(const std::string& message);
    
    // Template methods for formatted logging
    template<typename... Args>
    void Trace(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void Debug(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void Info(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void Warn(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void Error(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void Fatal(const std::string& format, Args&&... args);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

/**
 * Global logger management
 */
class LoggerManager {
public:
    static std::shared_ptr<ILogger> GetLogger(const std::string& name = "default");
    static void SetDefaultLogger(std::shared_ptr<ILogger> logger);
    static void SetGlobalLevel(LogLevel level);
    static LogLevel ParseLogLevel(const std::string& level);
    static std::string LogLevelToString(LogLevel level);
};

// Convenience macros
#define LOG_TRACE(msg) vision_infra::core::LoggerManager::GetLogger()->Trace(msg)
#define LOG_DEBUG(msg) vision_infra::core::LoggerManager::GetLogger()->Debug(msg)
#define LOG_INFO(msg) vision_infra::core::LoggerManager::GetLogger()->Info(msg)
#define LOG_WARN(msg) vision_infra::core::LoggerManager::GetLogger()->Warn(msg)
#define LOG_ERROR(msg) vision_infra::core::LoggerManager::GetLogger()->Error(msg)
#define LOG_FATAL(msg) vision_infra::core::LoggerManager::GetLogger()->Fatal(msg)

} // namespace core
} // namespace vision_infra