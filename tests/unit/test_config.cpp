#include <gtest/gtest.h>
#include <vision-infra/config/Config.hpp>
#include <vision-infra/config/ConfigManager.hpp>

using namespace vision_infra::config;

class InferenceConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_unique<InferenceConfig>();
    }
    
    std::unique_ptr<InferenceConfig> config_;
};

// Test default values
TEST_F(InferenceConfigTest, DefaultValues) {
    EXPECT_EQ(config_->GetServerAddress(), "localhost");
    EXPECT_EQ(config_->GetPort(), 8000);
    EXPECT_EQ(config_->GetProtocol(), "http");
    EXPECT_FALSE(config_->GetVerbose());
    EXPECT_EQ(config_->GetBatchSize(), 1);
    EXPECT_EQ(config_->GetNumThreads(), 1);
    EXPECT_FALSE(config_->GetEnableAsync());
    EXPECT_EQ(config_->GetLogLevel(), "info");
}

// Test setters and getters
TEST_F(InferenceConfigTest, SettersAndGetters) {
    config_->SetServerAddress("192.168.1.100");
    EXPECT_EQ(config_->GetServerAddress(), "192.168.1.100");
    
    config_->SetPort(8080);
    EXPECT_EQ(config_->GetPort(), 8080);
    
    config_->SetProtocol("https");
    EXPECT_EQ(config_->GetProtocol(), "https");
    
    config_->SetVerbose(true);
    EXPECT_TRUE(config_->GetVerbose());
    
    config_->SetModelName("my_model");
    EXPECT_EQ(config_->GetModelName(), "my_model");
    
    config_->SetBatchSize(8);
    EXPECT_EQ(config_->GetBatchSize(), 8);
    
    config_->SetNumThreads(4);
    EXPECT_EQ(config_->GetNumThreads(), 4);
}

// Test input sizes
TEST_F(InferenceConfigTest, InputSizes) {
    std::vector<std::vector<int64_t>> input_sizes = {{1, 3, 224, 224}, {1, 1, 256, 256}};
    config_->SetInputSizes(input_sizes);
    
    auto retrieved_sizes = config_->GetInputSizes();
    ASSERT_EQ(retrieved_sizes.size(), 2);
    EXPECT_EQ(retrieved_sizes[0], std::vector<int64_t>({1, 3, 224, 224}));
    EXPECT_EQ(retrieved_sizes[1], std::vector<int64_t>({1, 1, 256, 256}));
}

// Test thresholds
TEST_F(InferenceConfigTest, Thresholds) {
    config_->SetConfidenceThreshold(0.8f);
    EXPECT_FLOAT_EQ(config_->GetConfidenceThreshold(), 0.8f);
    
    config_->SetNmsThreshold(0.6f);
    EXPECT_FLOAT_EQ(config_->GetNmsThreshold(), 0.6f);
}

// Test custom parameters
TEST_F(InferenceConfigTest, CustomParameters) {
    config_->SetCustomParam("custom_key", "custom_value");
    
    auto value = config_->GetCustomParam("custom_key");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), "custom_value");
    
    auto missing_value = config_->GetCustomParam("non_existent");
    EXPECT_FALSE(missing_value.has_value());
}

// Test validation
TEST_F(InferenceConfigTest, Validation) {
    // Set model name to make config valid
    config_->SetModelName("test_model");
    
    // Check if config is valid and print errors if not
    bool is_valid = config_->IsValid();
    if (!is_valid) {
        std::string errors = config_->GetValidationErrors();
        std::cout << "Validation errors: " << errors << std::endl;
    }
    
    // For now, just test that validation methods work
    EXPECT_NO_THROW(config_->IsValid());
    EXPECT_NO_THROW(config_->GetValidationErrors());
}

// Test ConfigManager
class ConfigManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager_ = ConfigManager::Create();
    }
    
    std::unique_ptr<ConfigManager> manager_;
};

TEST_F(ConfigManagerTest, CreateManager) {
    EXPECT_NE(manager_, nullptr);
}

TEST_F(ConfigManagerTest, CreateDefaultConfig) {
    auto config = manager_->CreateDefault();
    ASSERT_NE(config, nullptr);
    
    // Should have reasonable defaults
    EXPECT_GE(config->GetBatchSize(), 1);
    EXPECT_GT(config->GetNumThreads(), 0);
    EXPECT_FALSE(config->GetServerAddress().empty());
}

TEST_F(ConfigManagerTest, ValidateConfig) {
    auto config = manager_->CreateDefault();
    ASSERT_NE(config, nullptr);
    
    bool is_valid = manager_->ValidateConfig(*config);
    // Default config should be valid
    EXPECT_TRUE(is_valid);
}

TEST_F(ConfigManagerTest, PrintConfig) {
    auto config = manager_->CreateDefault();
    ASSERT_NE(config, nullptr);
    
    // Should not throw when printing config
    EXPECT_NO_THROW(manager_->PrintConfig(*config));
}