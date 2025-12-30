#include <gtest/gtest.h>
#include <vision-infra/config/ConfigManager.hpp>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <fstream>

using namespace vision_infra;

class SimpleIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directory for test files
        temp_dir_ = std::filesystem::temp_directory_path() / "vision_infra_simple_test";
        std::filesystem::create_directories(temp_dir_);
        
        // Create test image
        test_image_ = cv::Mat::zeros(100, 100, CV_8UC3);
        cv::rectangle(test_image_, cv::Point(20, 20), cv::Point(80, 80), cv::Scalar(0, 255, 0), -1);
    }
    
    void TearDown() override {
        // Clean up temporary directory
        std::filesystem::remove_all(temp_dir_);
    }
    
    std::filesystem::path temp_dir_;
    cv::Mat test_image_;
};

// Test basic configuration workflow
TEST_F(SimpleIntegrationTest, ConfigurationWorkflow) {
    // 1. Create configuration manager
    auto config_manager = config::ConfigManager::Create();
    ASSERT_NE(config_manager, nullptr);
    
    // 2. Create default configuration
    auto config = config_manager->CreateDefault();
    ASSERT_NE(config, nullptr);
    
    // 3. Test configuration values
    EXPECT_EQ(config->GetServerAddress(), "localhost");
    EXPECT_EQ(config->GetPort(), 8000);
    EXPECT_EQ(config->GetBatchSize(), 1);
    EXPECT_EQ(config->GetNumThreads(), 1);
    
    // 4. Modify configuration
    config->SetServerAddress("127.0.0.1");
    config->SetPort(9000);
    config->SetBatchSize(4);
    
    // 5. Verify modifications
    EXPECT_EQ(config->GetServerAddress(), "127.0.0.1");
    EXPECT_EQ(config->GetPort(), 9000);
    EXPECT_EQ(config->GetBatchSize(), 4);
    
    // 6. Test validation
    bool is_valid = config_manager->ValidateConfig(*config);
    EXPECT_TRUE(is_valid);
}

// Test basic image processing
TEST_F(SimpleIntegrationTest, ImageProcessingWorkflow) {
    // 1. Save test image
    auto image_path = temp_dir_ / "test_image.jpg";
    bool saved = cv::imwrite(image_path.string(), test_image_);
    EXPECT_TRUE(saved);
    EXPECT_TRUE(std::filesystem::exists(image_path));
    
    // 2. Load image back
    cv::Mat loaded_image = cv::imread(image_path.string());
    EXPECT_FALSE(loaded_image.empty());
    EXPECT_EQ(loaded_image.size(), test_image_.size());
    
    // 3. Basic image operations
    cv::Mat resized;
    cv::resize(loaded_image, resized, cv::Size(200, 200));
    EXPECT_EQ(resized.size(), cv::Size(200, 200));
    
    // 4. Save processed image
    auto processed_path = temp_dir_ / "processed_image.jpg";
    bool processed_saved = cv::imwrite(processed_path.string(), resized);
    EXPECT_TRUE(processed_saved);
    EXPECT_TRUE(std::filesystem::exists(processed_path));
}

// Test file system operations
TEST_F(SimpleIntegrationTest, FileSystemOperations) {
    // 1. Test directory exists
    EXPECT_TRUE(std::filesystem::exists(temp_dir_));
    EXPECT_TRUE(std::filesystem::is_directory(temp_dir_));
    
    // 2. Create a test file
    auto test_file = temp_dir_ / "test.txt";
    std::ofstream file(test_file);
    file << "Hello, World!";
    file.close();
    
    EXPECT_TRUE(std::filesystem::exists(test_file));
    EXPECT_TRUE(std::filesystem::is_regular_file(test_file));
    
    // 3. Read file content
    std::ifstream read_file(test_file);
    std::string content((std::istreambuf_iterator<char>(read_file)),
                       std::istreambuf_iterator<char>());
    read_file.close();
    
    EXPECT_EQ(content, "Hello, World!");
    
    // 4. Test file size
    auto file_size = std::filesystem::file_size(test_file);
    EXPECT_EQ(file_size, 13); // "Hello, World!" is 13 characters
    
    // 5. Remove file
    EXPECT_TRUE(std::filesystem::remove(test_file));
    EXPECT_FALSE(std::filesystem::exists(test_file));
}