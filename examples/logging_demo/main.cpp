#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <vision-infra/core/Logger.hpp>

using namespace vision_infra::core;

void DemonstrateBasicLogging() {
    std::cout << "=== Basic Logging Demo ===\n\n";
    
    // Create a named logger using LoggerManager
    auto logger = LoggerManager::GetLogger("demo_logger");
    
    std::cout << "1. Demonstrating different log levels:\n";
    
    // Test all log levels
    logger->SetLevel(LogLevel::TRACE);
    logger->Log(LogLevel::TRACE, "This is a trace message - very detailed debugging info");
    logger->Log(LogLevel::DEBUG, "This is a debug message - general debugging info");
    logger->Log(LogLevel::INFO, "This is an info message - general information");
    logger->Log(LogLevel::WARN, "This is a warning message - something noteworthy happened");
    logger->Log(LogLevel::ERROR, "This is an error message - something went wrong but recoverable");
    logger->Log(LogLevel::FATAL, "This is a fatal message - critical system failure");
    
    std::cout << "\n2. Testing log level filtering:\n";
    
    // Set higher log level to filter out trace/debug
    logger->SetLevel(LogLevel::WARN);
    std::cout << "   Log level set to WARN - only WARN, ERROR, FATAL should appear:\n";
    
    logger->Log(LogLevel::TRACE, "This trace message should NOT appear");
    logger->Log(LogLevel::DEBUG, "This debug message should NOT appear");
    logger->Log(LogLevel::INFO, "This info message should NOT appear");
    logger->Log(LogLevel::WARN, "This warning message SHOULD appear");
    logger->Log(LogLevel::ERROR, "This error message SHOULD appear");
    
    std::cout << "\n";
}

void DemonstrateLoggerConfiguration() {
    std::cout << "=== Logger Configuration Demo ===\n\n";
    
    auto logger_ptr = LoggerManager::GetLogger("config_demo");
    // Cast to Logger to access configuration methods
    auto logger = std::static_pointer_cast<Logger>(logger_ptr);
    
    std::cout << "1. Testing logger configuration options:\n";
    
    // Configure logger appearance
    logger->SetLevel(LogLevel::DEBUG);
    logger->EnableTimestamp(true);
    logger->EnableConsoleOutput(true);
    
    logger->Log(LogLevel::INFO, "Logger configured with timestamps and console output");
    
    // Test custom pattern (if supported)
    logger->SetPattern("[%l] %m");
    logger->Log(LogLevel::DEBUG, "Custom pattern applied - should show level and message");
    
    // Create temporary log file
    auto temp_dir = std::filesystem::temp_directory_path();
    auto log_file = temp_dir / "vision_infra_demo.log";
    
    logger->SetOutputFile(log_file.string());
    logger->Log(LogLevel::INFO, "Log output redirected to file: " + log_file.string());
    
    std::cout << "\n2. Testing file output:\n";
    logger->Log(LogLevel::WARN, "This message should appear in both console and file");
    logger->Log(LogLevel::ERROR, "This error message is also logged to file");
    
    // Flush to ensure everything is written
    logger->Flush();
    
    // Check if file was created and has content
    if (std::filesystem::exists(log_file)) {
        auto file_size = std::filesystem::file_size(log_file);
        std::cout << "   ✓ Log file created: " << log_file.string() << " (size: " << file_size << " bytes)\n";
        
        // Optionally show file contents (first few lines)
        std::ifstream file(log_file);
        std::string line;
        int line_count = 0;
        std::cout << "   Log file contents (first 3 lines):\n";
        while (std::getline(file, line) && line_count < 3) {
            std::cout << "     " << line << "\n";
            ++line_count;
        }
        file.close();
        
        // Clean up
        std::filesystem::remove(log_file);
        std::cout << "   Log file cleaned up\n";
    } else {
        std::cout << "   ⚠ Log file was not created\n";
    }
    
    std::cout << "\n";
}

void DemonstrateLoggerManager() {
    std::cout << "=== Logger Manager Demo ===\n\n";
    
    std::cout << "1. Testing LoggerManager functionality:\n";
    
    // Get default logger
    auto default_logger = LoggerManager::GetLogger();
    default_logger->Log(LogLevel::INFO, "Message from default logger");
    
    // Get named loggers
    auto app_logger = LoggerManager::GetLogger("application");
    auto network_logger = LoggerManager::GetLogger("network");
    auto db_logger = LoggerManager::GetLogger("database");
    
    // Test that same name returns same instance
    auto app_logger2 = LoggerManager::GetLogger("application");
    if (app_logger == app_logger2) {
        std::cout << "   ✓ LoggerManager correctly returns same instance for same name\n";
    } else {
        std::cout << "   ✗ LoggerManager created different instances for same name\n";
    }
    
    std::cout << "\n2. Demonstrating multiple logger usage:\n";
    
    // Configure different loggers
    app_logger->Log(LogLevel::INFO, "Application starting up");
    network_logger->Log(LogLevel::DEBUG, "Connecting to server...");
    db_logger->Log(LogLevel::INFO, "Database connection established");
    
    // Test global log level setting
    std::cout << "\n3. Testing global log level:\n";
    LoggerManager::SetGlobalLevel(LogLevel::WARN);
    std::cout << "   Global log level set to WARN\n";
    
    app_logger->Log(LogLevel::DEBUG, "This debug message should be filtered");
    app_logger->Log(LogLevel::WARN, "This warning message should appear");
    network_logger->Log(LogLevel::ERROR, "Network error occurred");
    
    // Test log level parsing
    std::cout << "\n4. Testing log level parsing:\n";
    
    std::vector<std::string> level_strings = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "debug", "info"};
    
    for (const auto& level_str : level_strings) {
        try {
            LogLevel level = LoggerManager::ParseLogLevel(level_str);
            std::string parsed_str = LoggerManager::LogLevelToString(level);
            std::cout << "   '" << level_str << "' -> " << parsed_str << "\n";
        } catch (const std::exception& e) {
            std::cout << "   '" << level_str << "' -> Error: " << e.what() << "\n";
        }
    }
    
    std::cout << "\n";
}

void DemonstrateFormattedLogging() {
    std::cout << "=== Formatted Logging Demo ===\n\n";
    
    auto logger_ptr = LoggerManager::GetLogger("formatter");
    // Cast to Logger to access convenience methods
    auto logger = std::static_pointer_cast<Logger>(logger_ptr);
    logger->SetLevel(LogLevel::TRACE);
    
    std::cout << "1. Demonstrating formatted logging:\n";
    
    // Simulate application events with formatted messages
    int user_id = 12345;
    std::string username = "john_doe";
    double processing_time = 1.234;
    int frame_count = 150;
    
    logger->Log(LogLevel::INFO, "User login successful: ID=" + std::to_string(user_id) + ", Username=" + username);
    logger->Log(LogLevel::DEBUG, "Processing completed in " + std::to_string(processing_time) + " seconds");
    logger->Log(LogLevel::INFO, "Processed " + std::to_string(frame_count) + " frames");
    
    // Simulate error conditions
    std::cout << "\n2. Simulating error scenarios:\n";
    
    std::string filename = "missing_file.jpg";
    logger->Log(LogLevel::ERROR, "Failed to load image file: " + filename);
    
    int error_code = 404;
    logger->Log(LogLevel::ERROR, "HTTP request failed with code " + std::to_string(error_code));
    
    // Simulate performance logging
    std::cout << "\n3. Performance logging simulation:\n";
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    logger->Log(LogLevel::INFO, "Operation completed in " + std::to_string(duration.count()) + "ms");
    
    // Simulate memory usage logging
    size_t memory_usage = 1024 * 1024 * 150; // 150 MB
    double memory_mb = static_cast<double>(memory_usage) / (1024.0 * 1024.0);
    logger->Log(LogLevel::DEBUG, "Current memory usage: " + std::to_string(memory_mb) + " MB");
    
    std::cout << "\n";
}

void DemonstrateThreadSafetyLogging() {
    std::cout << "=== Thread Safety Demo ===\n\n";
    
    std::cout << "1. Testing concurrent logging from multiple threads:\n";
    
    auto logger = LoggerManager::GetLogger("thread_test");
    
    // Create multiple threads that log simultaneously
    std::vector<std::thread> threads;
    const int num_threads = 4;
    const int messages_per_thread = 5;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([logger, t, messages_per_thread]() {
            for (int i = 0; i < messages_per_thread; ++i) {
                std::string message = "Thread " + std::to_string(t) + " - Message " + std::to_string(i);
                logger->Log(LogLevel::INFO, message);
                
                // Small delay to increase chance of interleaving
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger->Log(LogLevel::INFO, "All threads completed successfully");
    
    std::cout << "   ✓ Concurrent logging completed\n";
    std::cout << "   Note: Check that log messages are not corrupted or interleaved improperly\n\n";
}

int main() {
    std::cout << "=== Vision Infrastructure Logging Demo ===\n\n";
    
    try {
        // Run all demonstrations
        DemonstrateBasicLogging();
        DemonstrateLoggerConfiguration();
        DemonstrateLoggerManager();
        DemonstrateFormattedLogging();
        DemonstrateThreadSafetyLogging();
        
        std::cout << "=== Summary ===\n";
        std::cout << "✓ Basic logging with different levels demonstrated\n";
        std::cout << "✓ Logger configuration (file output, patterns) tested\n";
        std::cout << "✓ LoggerManager functionality verified\n";
        std::cout << "✓ Formatted logging examples provided\n";
        std::cout << "✓ Thread safety tested with concurrent logging\n\n";
        
        std::cout << "Logging demo completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error during logging demo: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}