#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace vision_infra {
namespace utils {

/**
 * String utilities
 */
class StringUtils {
public:
    static std::vector<std::string> Split(const std::string& str, char delimiter);
    static std::vector<std::string> Split(const std::string& str, const std::string& delimiter);
    static std::string Join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string ToLower(const std::string& str);
    static std::string ToUpper(const std::string& str);
    static std::string Trim(const std::string& str);
    static bool StartsWith(const std::string& str, const std::string& prefix);
    static bool EndsWith(const std::string& str, const std::string& suffix);
    static std::string Replace(const std::string& str, const std::string& from, const std::string& to);
};

/**
 * Input parsing utilities for ML applications
 */
class InputParser {
public:
    static std::vector<std::vector<int64_t>> ParseInputSizes(const std::string& input);
    static std::vector<float> ParseFloatList(const std::string& input, char delimiter = ',');
    static std::vector<int> ParseIntList(const std::string& input, char delimiter = ',');
    static std::vector<std::string> ParseStringList(const std::string& input, char delimiter = ',');
};

/**
 * Computer vision drawing utilities
 */
class DrawingUtils {
public:
    static std::vector<cv::Scalar> GenerateRandomColors(size_t count, unsigned int seed = 42);
    static void DrawLabel(cv::Mat& image, const std::string& label, float confidence, 
                         int x, int y, const cv::Scalar& color = cv::Scalar(0, 255, 0),
                         int font = cv::FONT_HERSHEY_SIMPLEX, double font_scale = 0.5, int thickness = 1);
    static void DrawBoundingBox(cv::Mat& image, int x, int y, int width, int height,
                               const cv::Scalar& color = cv::Scalar(0, 255, 0), int thickness = 2);
    static void DrawBoundingBox(cv::Mat& image, const cv::Rect& rect,
                               const cv::Scalar& color = cv::Scalar(0, 255, 0), int thickness = 2);
    static void DrawPolygon(cv::Mat& image, const std::vector<cv::Point>& points,
                           const cv::Scalar& color = cv::Scalar(0, 255, 0), int thickness = 2);
    static void DrawFilledPolygon(cv::Mat& image, const std::vector<cv::Point>& points,
                                 const cv::Scalar& color = cv::Scalar(0, 255, 0));
    static void DrawKeypoints(cv::Mat& image, const std::vector<cv::Point2f>& keypoints,
                             const cv::Scalar& color = cv::Scalar(0, 255, 0), int radius = 3);
    static cv::Size GetTextSize(const std::string& text, int font = cv::FONT_HERSHEY_SIMPLEX,
                               double font_scale = 0.5, int thickness = 1);
};

/**
 * Image preprocessing utilities
 */
class ImageUtils {
public:
    static cv::Mat ResizeKeepAspectRatio(const cv::Mat& image, const cv::Size& target_size,
                                        const cv::Scalar& fill_color = cv::Scalar(114, 114, 114));
    static cv::Mat CenterCrop(const cv::Mat& image, const cv::Size& crop_size);
    static cv::Mat Normalize(const cv::Mat& image, const std::vector<float>& mean,
                            const std::vector<float>& std);
    static std::vector<cv::Mat> BatchImages(const std::vector<cv::Mat>& images);
    static cv::Mat HwcToChw(const cv::Mat& image);
    static cv::Mat ChwToHwc(const cv::Mat& image);
};

/**
 * Performance monitoring utilities
 */
class PerformanceUtils {
public:
    class Timer {
    public:
        Timer();
        void Start();
        void Stop();
        void Reset();
        double GetElapsedMs() const;
        double GetElapsedSeconds() const;
        
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
        std::chrono::high_resolution_clock::time_point end_time_;
        bool is_running_{false};
    };
    
    class FPSCounter {
    public:
        explicit FPSCounter(size_t window_size = 30);
        void Update();
        double GetCurrentFPS() const;
        double GetAverageFPS() const;
        void Reset();
        
    private:
        std::vector<std::chrono::high_resolution_clock::time_point> timestamps_;
        size_t window_size_;
        size_t current_index_{0};
        bool buffer_full_{false};
    };
};

/**
 * Memory management utilities
 */
class MemoryUtils {
public:
    static size_t GetImageMemorySize(const cv::Mat& image);
    static size_t GetTensorMemorySize(const std::vector<int64_t>& shape, size_t element_size);
    static std::string FormatBytes(size_t bytes);
    static size_t GetSystemMemoryUsage();
    static size_t GetProcessMemoryUsage();
};

} // namespace utils
} // namespace vision_infra