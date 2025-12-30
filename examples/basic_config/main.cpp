#include <iostream>
#include <vector>
#include <string>
#include <vision-infra/config/ConfigManager.hpp>

using namespace vision_infra::config;

void PrintUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --server <address>      Server address (default: localhost)\n";
    std::cout << "  --port <port>           Server port (default: 8000)\n";
    std::cout << "  --protocol <protocol>   Protocol type (default: http)\n";
    std::cout << "  --model <name>          Model name\n";
    std::cout << "  --model-type <type>     Model type\n";
    std::cout << "  --batch-size <size>     Batch size (default: 1)\n";
    std::cout << "  --threads <count>       Number of threads (default: 1)\n";
    std::cout << "  --source <path>         Input source path\n";
    std::cout << "  --labels <file>         Labels file path\n";
    std::cout << "  --confidence <value>    Confidence threshold (default: 0.5)\n";
    std::cout << "  --nms <value>           NMS threshold (default: 0.4)\n";
    std::cout << "  --verbose               Enable verbose output\n";
    std::cout << "  --log-level <level>     Log level (trace, debug, info, warn, error, fatal)\n";
    std::cout << "  --log-file <file>       Log file path\n";
    std::cout << "  --help                  Show this help message\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program_name << " --model my_model --source input.jpg --batch-size 4 --verbose\n";
}

int main(int argc, char* argv[]) {
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            PrintUsage(argv[0]);
            return 0;
        }
    }
    
    std::cout << "=== Vision Infrastructure Configuration Example ===\n\n";
    
    try {
        // Create configuration manager
        auto config_manager = ConfigManager::Create();
        if (!config_manager) {
            std::cerr << "Error: Failed to create configuration manager\n";
            return 1;
        }
        
        std::cout << "1. Loading configuration from command line arguments...\n";
        
        // Load configuration from command line
        std::unique_ptr<InferenceConfig> config;
        try {
            config = config_manager->LoadFromCommandLine(argc, const_cast<const char**>(argv));
        } catch (const std::exception& e) {
            std::cerr << "Error parsing command line: " << e.what() << "\n";
            std::cerr << "Use --help for usage information.\n";
            return 1;
        }
        
        if (!config) {
            std::cout << "No command line arguments provided, creating default configuration...\n";
            config = config_manager->CreateDefault();
        }
        
        if (!config) {
            std::cerr << "Error: Failed to create configuration\n";
            return 1;
        }
        
        std::cout << "2. Configuration loaded successfully!\n\n";
        
        // Print current configuration
        std::cout << "3. Current Configuration:\n";
        std::cout << "   ========================\n";
        config_manager->PrintConfig(*config);
        std::cout << "\n";
        
        // Validate configuration
        std::cout << "4. Validating configuration...\n";
        bool is_valid = config_manager->ValidateConfig(*config);
        
        if (is_valid) {
            std::cout << "   ✓ Configuration is valid!\n\n";
        } else {
            std::cout << "   ✗ Configuration validation failed:\n";
            std::string errors = config_manager->GetValidationErrors(*config);
            std::cout << "   " << errors << "\n\n";
        }
        
        // Demonstrate configuration modification
        std::cout << "5. Demonstrating configuration modifications...\n";
        
        // Create a modified copy for demonstration
        auto modified_config = config_manager->CreateDefault();
        
        // Apply some example modifications
        modified_config->SetServerAddress("192.168.1.100");
        modified_config->SetPort(9000);
        modified_config->SetModelName("example_model");
        modified_config->SetModelType("onnx");
        modified_config->SetBatchSize(8);
        modified_config->SetNumThreads(4);
        modified_config->SetSource("/path/to/input/data");
        modified_config->SetConfidenceThreshold(0.8f);
        modified_config->SetNmsThreshold(0.6f);
        modified_config->SetVerbose(true);
        modified_config->SetLogLevel("debug");
        
        // Set custom parameters
        modified_config->SetCustomParam("custom_option", "custom_value");
        modified_config->SetCustomParam("optimization_level", "high");
        
        std::cout << "   Modified Configuration:\n";
        std::cout << "   ======================\n";
        config_manager->PrintConfig(*modified_config);
        std::cout << "\n";
        
        // Show custom parameters
        auto custom_param = modified_config->GetCustomParam("custom_option");
        if (custom_param) {
            std::cout << "   Custom parameter 'custom_option': " << *custom_param << "\n";
        }
        
        auto optimization = modified_config->GetCustomParam("optimization_level");
        if (optimization) {
            std::cout << "   Custom parameter 'optimization_level': " << *optimization << "\n";
        }
        
        auto missing_param = modified_config->GetCustomParam("non_existent");
        if (!missing_param) {
            std::cout << "   Custom parameter 'non_existent': not found\n";
        }
        std::cout << "\n";
        
        // Demonstrate configuration merging
        std::cout << "6. Demonstrating configuration merging...\n";
        auto merged_config = config_manager->Merge(*config, *modified_config);
        
        if (merged_config) {
            std::cout << "   Merged Configuration:\n";
            std::cout << "   ===================\n";
            config_manager->PrintConfig(*merged_config);
            std::cout << "\n";
        }
        
        // Show environment variable loading
        std::cout << "7. Environment variable configuration...\n";
        auto env_config = config_manager->LoadFromEnvironment();
        if (env_config) {
            std::cout << "   Environment-based configuration loaded\n";
        } else {
            std::cout << "   No environment configuration found\n";
        }
        std::cout << "   Note: Set environment variables like VISION_INFRA_MODEL_NAME, \n";
        std::cout << "         VISION_INFRA_BATCH_SIZE, etc. to configure via environment\n\n";
        
        std::cout << "8. Configuration Summary:\n";
        std::cout << "   ======================\n";
        std::cout << "   • Configuration manager created successfully\n";
        std::cout << "   • Command-line arguments processed\n";
        std::cout << "   • Configuration validation performed\n";
        std::cout << "   • Custom parameters demonstrated\n";
        std::cout << "   • Configuration merging demonstrated\n";
        std::cout << "   • Environment variable loading attempted\n\n";
        
        std::cout << "✓ Configuration example completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}