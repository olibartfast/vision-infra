# Logging Demo Example

This example demonstrates the vision-infra logging system, including different log levels, logger configuration, and usage patterns.

## Features Demonstrated

- **Different log levels** (TRACE, DEBUG, INFO, WARN, ERROR, FATAL)
- **Log level filtering**
- **Logger configuration** (timestamps, patterns, file output)
- **LoggerManager** for managing multiple loggers
- **Formatted logging** for structured output
- **Thread-safe logging** from multiple threads
- **File output** and log file management

## Building and Running

```bash
# Build the example
cd build
make logging_demo_example

# Run the demo
./examples/logging_demo/logging_demo_example
```

## Key Features

### Log Levels

The logging system supports six log levels in order of severity:

1. **TRACE** - Very detailed debugging information
2. **DEBUG** - General debugging information  
3. **INFO** - General informational messages
4. **WARN** - Warning messages for noteworthy events
5. **ERROR** - Error messages for recoverable failures
6. **FATAL** - Critical system failures

### Logger Configuration

```cpp
Logger logger("my_logger");

// Set log level (filters out lower-priority messages)
logger.SetLevel(LogLevel::INFO);

// Enable timestamps in log messages
logger.EnableTimestamp(true);

// Set custom output pattern
logger.SetPattern("[%l] %m");

// Redirect output to file
logger.SetOutputFile("/path/to/logfile.log");

// Ensure all messages are written
logger.Flush();
```

### LoggerManager Usage

```cpp
// Get default logger
auto logger = LoggerManager::GetLogger();

// Get named loggers (same name returns same instance)
auto app_logger = LoggerManager::GetLogger("application");
auto net_logger = LoggerManager::GetLogger("network");

// Set global log level for all loggers
LoggerManager::SetGlobalLevel(LogLevel::WARN);

// Parse log level from string
LogLevel level = LoggerManager::ParseLogLevel("DEBUG");
std::string level_str = LoggerManager::LogLevelToString(LogLevel::INFO);
```

### Basic Logging

```cpp
Logger logger("example");

// Direct logging methods
logger.Trace("Detailed debugging info");
logger.Debug("General debugging");
logger.Info("Informational message");
logger.Warn("Warning message");
logger.Error("Error occurred");
logger.Fatal("Critical failure");

// Generic logging method
logger.Log(LogLevel::INFO, "Custom message");
```

## Example Output

```
=== Vision Infrastructure Logging Demo ===

=== Basic Logging Demo ===

1. Demonstrating different log levels:
[TRACE] This is a trace message - very detailed debugging info
[DEBUG] This is a debug message - general debugging info
[INFO] This is an info message - general information
[WARN] This is a warning message - something noteworthy happened
[ERROR] This is an error message - something went wrong but recoverable
[FATAL] This is a fatal message - critical system failure

2. Testing log level filtering:
   Log level set to WARN - only WARN, ERROR, FATAL should appear:
[WARN] This warning message SHOULD appear
[ERROR] This error message SHOULD appear

=== Logger Configuration Demo ===

1. Testing logger configuration options:
[INFO] Logger configured with timestamps and console output
[DEBUG] Custom pattern applied - should show level and message

2. Testing file output:
[WARN] This message should appear in both console and file
[ERROR] This error message is also logged to file
   ✓ Log file created: /tmp/vision_infra_demo.log (size: 156 bytes)

=== Logger Manager Demo ===

1. Testing LoggerManager functionality:
[INFO] Message from default logger
   ✓ LoggerManager correctly returns same instance for same name

2. Demonstrating multiple logger usage:
[INFO] Application starting up
[DEBUG] Connecting to server...
[INFO] Database connection established

✓ Logging demo completed successfully!
```

## Use Cases

### Application Monitoring

```cpp
auto app_logger = LoggerManager::GetLogger("application");
app_logger->Info("Application started successfully");
app_logger->Warn("High memory usage detected: 85%");
app_logger->Error("Database connection failed, retrying...");
```

### Performance Logging

```cpp
auto perf_logger = LoggerManager::GetLogger("performance");
auto start = std::chrono::high_resolution_clock::now();

// ... do work ...

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
perf_logger->Info("Operation completed in " + std::to_string(duration.count()) + "ms");
```

### Error Tracking

```cpp
auto error_logger = LoggerManager::GetLogger("errors");
try {
    // risky operation
} catch (const std::exception& e) {
    error_logger->Error("Operation failed: " + std::string(e.what()));
}
```

## Thread Safety

The logging system is designed to be thread-safe:

```cpp
// Multiple threads can safely log concurrently
std::thread t1([logger]() { 
    logger->Info("Message from thread 1"); 
});
std::thread t2([logger]() { 
    logger->Info("Message from thread 2"); 
});
```

## Best Practices

1. **Use appropriate log levels** - Don't use ERROR for informational messages
2. **Use named loggers** for different components/modules
3. **Set appropriate log levels** for production vs development
4. **Include context** in log messages (timestamps, user IDs, etc.)
5. **Flush logs** before application exit for file output
6. **Rotate log files** in production to manage disk space