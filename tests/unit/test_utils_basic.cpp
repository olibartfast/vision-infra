#include <gtest/gtest.h>
#include <vision-infra/utils/VisionUtils.hpp>
#include <opencv2/opencv.hpp>

using namespace vision_infra::utils;

// Test StringUtils functionality
class StringUtilsBasicTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(StringUtilsBasicTest, SplitByCharDelimiter) {
    std::string input = "a,b,c,d";
    auto result = StringUtils::Split(input, ',');
    
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
    EXPECT_EQ(result[3], "d");
}

TEST_F(StringUtilsBasicTest, SplitByStringDelimiter) {
    std::string input = "apple::banana::cherry";
    auto result = StringUtils::Split(input, "::");
    
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "banana");
    EXPECT_EQ(result[2], "cherry");
}

TEST_F(StringUtilsBasicTest, JoinStrings) {
    std::vector<std::string> strings = {"apple", "banana", "cherry"};
    auto result = StringUtils::Join(strings, ", ");
    
    EXPECT_EQ(result, "apple, banana, cherry");
}

TEST_F(StringUtilsBasicTest, ToLowerCase) {
    std::string input = "Hello World!";
    auto result = StringUtils::ToLower(input);
    
    EXPECT_EQ(result, "hello world!");
}

TEST_F(StringUtilsBasicTest, ToUpperCase) {
    std::string input = "Hello World!";
    auto result = StringUtils::ToUpper(input);
    
    EXPECT_EQ(result, "HELLO WORLD!");
}

TEST_F(StringUtilsBasicTest, TrimWhitespace) {
    std::string input = "  hello world  ";
    auto result = StringUtils::Trim(input);
    
    EXPECT_EQ(result, "hello world");
}

TEST_F(StringUtilsBasicTest, StartsWith) {
    std::string text = "hello world";
    
    EXPECT_TRUE(StringUtils::StartsWith(text, "hello"));
    EXPECT_TRUE(StringUtils::StartsWith(text, ""));
    EXPECT_FALSE(StringUtils::StartsWith(text, "world"));
}

TEST_F(StringUtilsBasicTest, EndsWith) {
    std::string text = "hello world";
    
    EXPECT_TRUE(StringUtils::EndsWith(text, "world"));
    EXPECT_TRUE(StringUtils::EndsWith(text, ""));
    EXPECT_FALSE(StringUtils::EndsWith(text, "hello"));
}

// Test InputParser functionality
class InputParserBasicTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(InputParserBasicTest, ParseFloatList) {
    std::string input = "1.0,2.5,3.14,4.2";
    auto result = InputParser::ParseFloatList(input);
    
    ASSERT_EQ(result.size(), 4);
    EXPECT_FLOAT_EQ(result[0], 1.0f);
    EXPECT_FLOAT_EQ(result[1], 2.5f);
    EXPECT_FLOAT_EQ(result[2], 3.14f);
    EXPECT_FLOAT_EQ(result[3], 4.2f);
}

TEST_F(InputParserBasicTest, ParseIntList) {
    std::string input = "1,2,3,4,5";
    auto result = InputParser::ParseIntList(input);
    
    ASSERT_EQ(result.size(), 5);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(result[i], static_cast<int>(i) + 1);
    }
}

TEST_F(InputParserBasicTest, ParseStringList) {
    std::string input = "apple,banana,cherry";
    auto result = InputParser::ParseStringList(input);
    
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "banana");
    EXPECT_EQ(result[2], "cherry");
}

// Test DrawingUtils functionality
class DrawingUtilsBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_image_ = cv::Mat::zeros(480, 640, CV_8UC3);
    }
    
    cv::Mat test_image_;
};

TEST_F(DrawingUtilsBasicTest, GenerateRandomColors) {
    auto colors = DrawingUtils::GenerateRandomColors(5);
    
    EXPECT_EQ(colors.size(), 5);
    
    // Check that colors are valid (each channel 0-255)
    for (const auto& color : colors) {
        EXPECT_GE(color[0], 0);
        EXPECT_LE(color[0], 255);
        EXPECT_GE(color[1], 0);
        EXPECT_LE(color[1], 255);
        EXPECT_GE(color[2], 0);
        EXPECT_LE(color[2], 255);
    }
}

TEST_F(DrawingUtilsBasicTest, DrawBoundingBox) {
    cv::Mat original = test_image_.clone();
    
    DrawingUtils::DrawBoundingBox(test_image_, 10, 10, 100, 50);
    
    // Convert to single channel for comparison
    cv::Mat original_gray, modified_gray;
    cv::cvtColor(original, original_gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(test_image_, modified_gray, cv::COLOR_BGR2GRAY);
    
    // Image should be modified (difference should be non-zero)
    cv::Mat diff;
    cv::absdiff(original_gray, modified_gray, diff);
    EXPECT_GT(cv::countNonZero(diff), 0);
}

TEST_F(DrawingUtilsBasicTest, GetTextSize) {
    auto size = DrawingUtils::GetTextSize("Test");
    
    EXPECT_GT(size.width, 0);
    EXPECT_GT(size.height, 0);
}

// Test ImageUtils functionality  
class ImageUtilsBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_image_ = cv::Mat::ones(100, 100, CV_8UC3) * 128; // Gray image
    }
    
    cv::Mat test_image_;
};

TEST_F(ImageUtilsBasicTest, ResizeKeepAspectRatio) {
    cv::Size target_size(200, 150);
    auto resized = ImageUtils::ResizeKeepAspectRatio(test_image_, target_size);
    
    EXPECT_EQ(resized.size(), target_size);
    EXPECT_EQ(resized.type(), test_image_.type());
}

TEST_F(ImageUtilsBasicTest, CenterCrop) {
    cv::Size crop_size(50, 50);
    auto cropped = ImageUtils::CenterCrop(test_image_, crop_size);
    
    EXPECT_EQ(cropped.size(), crop_size);
    EXPECT_EQ(cropped.type(), test_image_.type());
}

// Test MemoryUtils functionality
class MemoryUtilsBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_image_ = cv::Mat::zeros(100, 100, CV_8UC3);
    }
    
    cv::Mat test_image_;
};

TEST_F(MemoryUtilsBasicTest, GetImageMemorySize) {
    size_t memory_size = MemoryUtils::GetImageMemorySize(test_image_);
    
    // 100x100x3 bytes = 30,000 bytes
    EXPECT_EQ(memory_size, 30000);
}

TEST_F(MemoryUtilsBasicTest, GetTensorMemorySize) {
    std::vector<int64_t> shape = {1, 3, 224, 224};
    size_t element_size = sizeof(float);
    
    size_t memory_size = MemoryUtils::GetTensorMemorySize(shape, element_size);
    
    // 1*3*224*224*4 bytes = 602,112 bytes
    EXPECT_EQ(memory_size, 602112);
}

TEST_F(MemoryUtilsBasicTest, FormatBytes) {
    EXPECT_EQ(MemoryUtils::FormatBytes(1024), "1.00 KB");
    EXPECT_EQ(MemoryUtils::FormatBytes(1024 * 1024), "1.00 MB");
    EXPECT_EQ(MemoryUtils::FormatBytes(1024 * 1024 * 1024), "1.00 GB");
}