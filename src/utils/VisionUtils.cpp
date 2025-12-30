#include "vision-infra/utils/VisionUtils.hpp"
#include <algorithm>
#include <random>
#include <sstream>
#include <set>
#include <chrono>
#include <filesystem>

namespace vision_infra {
namespace utils {

// StringUtils implementation
std::vector<std::string> StringUtils::Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> StringUtils::Split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(str.substr(start));
    
    return tokens;
}

std::string StringUtils::Join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";
    
    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += delimiter + strings[i];
    }
    return result;
}

std::string StringUtils::ToLower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return lower_str;
}

std::string StringUtils::ToUpper(const std::string& str) {
    std::string upper_str = str;
    std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), 
                   [](unsigned char c) { return std::toupper(c); });
    return upper_str;
}

std::string StringUtils::Trim(const std::string& str) {
    const char* whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

bool StringUtils::StartsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

bool StringUtils::EndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.substr(str.size() - suffix.size()) == suffix;
}

std::string StringUtils::Replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t start = 0;
    while ((start = result.find(from, start)) != std::string::npos) {
        result.replace(start, from.length(), to);
        start += to.length();
    }
    return result;
}

// InputParser implementation
std::vector<std::vector<int64_t>> InputParser::ParseInputSizes(const std::string& input) {
    std::vector<std::vector<int64_t>> sizes;
    std::vector<std::string> inputs = StringUtils::Split(input, ';');

    for (const auto& input_str : inputs) {
        std::vector<std::string> dims = StringUtils::Split(input_str, ',');
        std::vector<int64_t> dimensions;
        for (const auto& dim : dims) {
            dimensions.push_back(std::stoll(StringUtils::Trim(dim)));
        }
        sizes.push_back(dimensions);
    }

    return sizes;
}

std::vector<float> InputParser::ParseFloatList(const std::string& input, char delimiter) {
    std::vector<float> values;
    std::vector<std::string> tokens = StringUtils::Split(input, delimiter);
    
    for (const auto& token : tokens) {
        values.push_back(std::stof(StringUtils::Trim(token)));
    }
    return values;
}

std::vector<int> InputParser::ParseIntList(const std::string& input, char delimiter) {
    std::vector<int> values;
    std::vector<std::string> tokens = StringUtils::Split(input, delimiter);
    
    for (const auto& token : tokens) {
        values.push_back(std::stoi(StringUtils::Trim(token)));
    }
    return values;
}

std::vector<std::string> InputParser::ParseStringList(const std::string& input, char delimiter) {
    std::vector<std::string> values = StringUtils::Split(input, delimiter);
    
    // Trim each value
    for (auto& value : values) {
        value = StringUtils::Trim(value);
    }
    return values;
}

// DrawingUtils implementation
std::vector<cv::Scalar> DrawingUtils::GenerateRandomColors(size_t count, unsigned int seed) {
    std::vector<cv::Scalar> colors;
    colors.reserve(count);

    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist(0, 255);

    for (size_t i = 0; i < count; ++i) {
        cv::Scalar color(dist(gen), dist(gen), dist(gen));
        colors.push_back(color);
    }

    return colors;
}

void DrawingUtils::DrawLabel(cv::Mat& image, const std::string& label, float confidence, 
                            int x, int y, const cv::Scalar& color, int font, double font_scale, int thickness) {
    std::string confidenceStr = std::to_string(confidence).substr(0, 4);
    std::string display_text = label + ": " + confidenceStr;
    
    int baseline;
    cv::Size label_size = cv::getTextSize(display_text, font, font_scale, thickness, &baseline);
    y = std::max(y, label_size.height);

    cv::Point tlc(x, y);
    cv::Point brc(x + label_size.width, y + label_size.height + baseline);

    cv::rectangle(image, tlc, brc, cv::Scalar(255, 0, 255), cv::FILLED);
    cv::putText(image, display_text, cv::Point(x, y + label_size.height), 
                font, font_scale, cv::Scalar(0, 255, 255), thickness);
}

void DrawingUtils::DrawBoundingBox(cv::Mat& image, int x, int y, int width, int height,
                                  const cv::Scalar& color, int thickness) {
    cv::rectangle(image, cv::Point(x, y), cv::Point(x + width, y + height), color, thickness);
}

void DrawingUtils::DrawBoundingBox(cv::Mat& image, const cv::Rect& rect,
                                  const cv::Scalar& color, int thickness) {
    cv::rectangle(image, rect, color, thickness);
}

void DrawingUtils::DrawPolygon(cv::Mat& image, const std::vector<cv::Point>& points,
                              const cv::Scalar& color, int thickness) {
    if (points.size() < 2) return;
    
    for (size_t i = 0; i < points.size(); ++i) {
        cv::Point pt1 = points[i];
        cv::Point pt2 = points[(i + 1) % points.size()];
        cv::line(image, pt1, pt2, color, thickness);
    }
}

void DrawingUtils::DrawFilledPolygon(cv::Mat& image, const std::vector<cv::Point>& points,
                                    const cv::Scalar& color) {
    if (points.size() < 3) return;
    
    std::vector<cv::Point> pts = points;
    cv::fillPoly(image, pts, color);
}

void DrawingUtils::DrawKeypoints(cv::Mat& image, const std::vector<cv::Point2f>& keypoints,
                                const cv::Scalar& color, int radius) {
    for (const auto& point : keypoints) {
        cv::circle(image, cv::Point(static_cast<int>(point.x), static_cast<int>(point.y)), 
                  radius, color, -1);
    }
}

cv::Size DrawingUtils::GetTextSize(const std::string& text, int font, double font_scale, int thickness) {
    int baseline;
    return cv::getTextSize(text, font, font_scale, thickness, &baseline);
}

// ImageUtils implementation
cv::Mat ImageUtils::ResizeKeepAspectRatio(const cv::Mat& image, const cv::Size& target_size,
                                         const cv::Scalar& fill_color) {
    double scale = std::min(static_cast<double>(target_size.width) / image.cols,
                           static_cast<double>(target_size.height) / image.rows);
    
    cv::Size new_size(static_cast<int>(image.cols * scale), 
                      static_cast<int>(image.rows * scale));
    
    cv::Mat resized;
    cv::resize(image, resized, new_size);
    
    cv::Mat result(target_size, image.type(), fill_color);
    
    int x_offset = (target_size.width - new_size.width) / 2;
    int y_offset = (target_size.height - new_size.height) / 2;
    
    cv::Rect roi(x_offset, y_offset, new_size.width, new_size.height);
    resized.copyTo(result(roi));
    
    return result;
}

cv::Mat ImageUtils::CenterCrop(const cv::Mat& image, const cv::Size& crop_size) {
    int x = (image.cols - crop_size.width) / 2;
    int y = (image.rows - crop_size.height) / 2;
    
    cv::Rect crop_rect(x, y, crop_size.width, crop_size.height);
    return image(crop_rect);
}

cv::Mat ImageUtils::Normalize(const cv::Mat& image, const std::vector<float>& mean,
                             const std::vector<float>& std) {
    cv::Mat normalized;
    image.convertTo(normalized, CV_32F, 1.0 / 255.0);
    
    std::vector<cv::Mat> channels;
    cv::split(normalized, channels);
    
    for (size_t i = 0; i < channels.size() && i < mean.size() && i < std.size(); ++i) {
        channels[i] = (channels[i] - mean[i]) / std[i];
    }
    
    cv::merge(channels, normalized);
    return normalized;
}

cv::Mat ImageUtils::HwcToChw(const cv::Mat& image) {
    // OpenCV stores images as HWC, this converts to CHW for some frameworks
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    
    cv::Mat result;
    cv::merge(channels, result);
    
    // Reshape to CHW format
    int dims[] = {static_cast<int>(channels.size()), image.rows, image.cols};
    result = result.reshape(1, 3, dims);
    
    return result;
}

cv::Mat ImageUtils::ChwToHwc(const cv::Mat& image) {
    // Convert CHW back to HWC format
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    
    cv::Mat result;
    cv::merge(channels, result);
    
    return result;
}

// PerformanceUtils::Timer implementation
PerformanceUtils::Timer::Timer() {
    Reset();
}

void PerformanceUtils::Timer::Start() {
    start_time_ = std::chrono::high_resolution_clock::now();
    is_running_ = true;
}

void PerformanceUtils::Timer::Stop() {
    if (is_running_) {
        end_time_ = std::chrono::high_resolution_clock::now();
        is_running_ = false;
    }
}

void PerformanceUtils::Timer::Reset() {
    start_time_ = std::chrono::high_resolution_clock::now();
    end_time_ = start_time_;
    is_running_ = false;
}

double PerformanceUtils::Timer::GetElapsedMs() const {
    auto end = is_running_ ? std::chrono::high_resolution_clock::now() : end_time_;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_);
    return duration.count() / 1000.0;
}

double PerformanceUtils::Timer::GetElapsedSeconds() const {
    return GetElapsedMs() / 1000.0;
}

// PerformanceUtils::FPSCounter implementation
PerformanceUtils::FPSCounter::FPSCounter(size_t window_size) 
    : window_size_(window_size), timestamps_(window_size) {}

void PerformanceUtils::FPSCounter::Update() {
    timestamps_[current_index_] = std::chrono::high_resolution_clock::now();
    current_index_ = (current_index_ + 1) % window_size_;
    
    if (!buffer_full_ && current_index_ == 0) {
        buffer_full_ = true;
    }
}

double PerformanceUtils::FPSCounter::GetCurrentFPS() const {
    if (!buffer_full_ && current_index_ < 2) return 0.0;
    
    size_t count = buffer_full_ ? window_size_ : current_index_;
    size_t prev_index = (current_index_ - 1 + window_size_) % window_size_;
    size_t start_index = (current_index_ - count + window_size_) % window_size_;
    
    auto duration = timestamps_[prev_index] - timestamps_[start_index];
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    return ms > 0 ? (count - 1) * 1000.0 / ms : 0.0;
}

double PerformanceUtils::FPSCounter::GetAverageFPS() const {
    return GetCurrentFPS(); // Same calculation for now
}

void PerformanceUtils::FPSCounter::Reset() {
    current_index_ = 0;
    buffer_full_ = false;
}

// MemoryUtils implementation
size_t MemoryUtils::GetImageMemorySize(const cv::Mat& image) {
    return image.total() * image.elemSize();
}

size_t MemoryUtils::GetTensorMemorySize(const std::vector<int64_t>& shape, size_t element_size) {
    size_t total_elements = 1;
    for (auto dim : shape) {
        total_elements *= dim;
    }
    return total_elements * element_size;
}

std::string MemoryUtils::FormatBytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = static_cast<double>(bytes);
    int unit_index = 0;
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unit_index]);
    return std::string(buffer);
}

size_t MemoryUtils::GetSystemMemoryUsage() {
    // Platform-specific implementation needed
    // This is a placeholder
    return 0;
}

size_t MemoryUtils::GetProcessMemoryUsage() {
    // Platform-specific implementation needed
    // This is a placeholder
    return 0;
}

} // namespace utils
} // namespace vision_infra