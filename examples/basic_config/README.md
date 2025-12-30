# Basic Configuration Example

This example demonstrates how to use the vision-infra configuration system for command-line parsing and configuration management.

## Features Demonstrated

- **Command-line argument parsing**
- **Configuration validation**
- **Default configuration creation**
- **Configuration merging**
- **Custom parameters**
- **Environment variable loading**
- **Configuration printing**

## Building and Running

```bash
# Build the example
cd build
make basic_config_example

# Run with default settings
./examples/basic_config/basic_config_example

# Run with custom parameters
./examples/basic_config/basic_config_example \
    --model my_model \
    --server 192.168.1.100 \
    --port 9000 \
    --batch-size 8 \
    --threads 4 \
    --verbose

# Show help
./examples/basic_config/basic_config_example --help
```

## Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--server <address>` | Server address | localhost |
| `--port <port>` | Server port | 8000 |
| `--protocol <protocol>` | Protocol type | http |
| `--model <name>` | Model name | (empty) |
| `--model-type <type>` | Model type | (empty) |
| `--batch-size <size>` | Batch size | 1 |
| `--threads <count>` | Number of threads | 1 |
| `--source <path>` | Input source path | (empty) |
| `--labels <file>` | Labels file path | (empty) |
| `--confidence <value>` | Confidence threshold | 0.5 |
| `--nms <value>` | NMS threshold | 0.4 |
| `--verbose` | Enable verbose output | false |
| `--log-level <level>` | Log level | info |
| `--log-file <file>` | Log file path | (empty) |

## Environment Variables

You can also configure the application using environment variables:

```bash
export VISION_INFRA_MODEL_NAME="my_model"
export VISION_INFRA_BATCH_SIZE="8"
export VISION_INFRA_SERVER_ADDRESS="192.168.1.100"
export VISION_INFRA_LOG_LEVEL="debug"

./examples/basic_config/basic_config_example
```

## Example Output

```
=== Vision Infrastructure Configuration Example ===

1. Loading configuration from command line arguments...
2. Configuration loaded successfully!

3. Current Configuration:
   ========================
Configuration:
  Server: 192.168.1.100:9000 (http)
  Model: my_model ()
  Source: /path/to/input/data
  Labels: 
  Batch Size: 8
  Show Frame: false
  Write Frame: true
  Confidence Threshold: 0.8
  NMS Threshold: 0.6
  Verbose: true
  Shared Memory Type: none
  Log Level: debug

4. Validating configuration...
   ✓ Configuration is valid!

5. Demonstrating configuration modifications...
   [... configuration details ...]

✓ Configuration example completed successfully!
```

## Key Concepts

### Configuration Manager

```cpp
auto config_manager = ConfigManager::Create();
auto config = config_manager->LoadFromCommandLine(argc, argv);
```

### Validation

```cpp
bool is_valid = config_manager->ValidateConfig(*config);
if (!is_valid) {
    std::string errors = config_manager->GetValidationErrors(*config);
    // Handle validation errors
}
```

### Custom Parameters

```cpp
config->SetCustomParam("optimization_level", "high");
auto value = config->GetCustomParam("optimization_level");
```

### Configuration Merging

```cpp
auto merged = config_manager->Merge(*base_config, *override_config);
```