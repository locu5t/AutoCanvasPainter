#include "painter.h"
#include <iostream>
#include <algorithm>
#include <vector>

// Helper function to compute points along a Bézier curve
std::vector<Point> bezier_curve(const std::vector<Point>& points, int num_steps) {
    std::vector<Point> curve;
    int n = points.size() - 1;
    if (n < 0) return curve;

    auto combinations = [](int n, int k) {
        if (k < 0 || k > n) {
            return 0;
        }
        if (k == 0 || k == n) {
            return 1;
        }
        if (k > n / 2) {
            k = n - k;
        }
        long long res = 1;
        for (int i = 1; i <= k; ++i) {
            res = res * (n - i + 1) / i;
        }
        return res;
    };

    for (int step = 0; step < num_steps; ++step) {
        float t = static_cast<float>(step) / (num_steps - 1);
        float x = 0, y = 0;
        for (int i = 0; i <= n; ++i) {
            float bernstein_poly = combinations(n, i) * std::pow(t, i) * std::pow(1 - t, n - i);
            x += points[i].x * bernstein_poly;
            y += points[i].y * bernstein_poly;
        }
        curve.push_back({static_cast<int>(x), static_cast<int>(y)});
    }
    return curve;
}


// Custom comparator for cv::Vec3b used in the map
bool compareVec3b(const cv::Vec3b& a, const cv::Vec3b& b) {
    if (a[0] != b[0]) return a[0] < b[0];
    if (a[1] != b[1]) return a[1] < b[1];
    return a[2] < b[2];
}

// Helper to get brightness of a color
float get_brightness(const cv::Vec3b& color) {
    // Using standard formula for luminance
    return 0.299f * color[2] + 0.587f * color[1] + 0.114f * color[0]; // BGR order
}


Painter::Painter(const cv::Mat& reference_image)
    : reference_image_rgb(reference_image),
      color_tones(compareVec3b)
{
    if (reference_image_rgb.channels() != 3) {
        throw std::runtime_error("Reference image must be a 3-channel color image.");
    }
    canvas = cv::Mat(reference_image.size(), CV_8UC3, cv::Scalar(255, 255, 255));
    cv::cvtColor(reference_image_rgb, reference_image_gray, cv::COLOR_BGR2GRAY);
    initialize_maps();
    analyze_image_tones();
}

void Painter::initialize_maps() {
    std::cout << "Initializing analysis maps..." << std::endl;
    saliency_map = compute_saliency(reference_image_rgb);
    edge_map = compute_edge_map(reference_image_gray);
    feature_mask = detect_features(reference_image_gray);
    segments = segment_image(reference_image_rgb);

    cv::Mat grad_x, grad_y;
    cv::Sobel(reference_image_gray, grad_x, CV_32F, 1, 0, 3);
    cv::Sobel(reference_image_gray, grad_y, CV_32F, 0, 1, 3);
    cv::cartToPolar(grad_x, grad_y, magnitude_map, orientation_map, true);
    cv::normalize(magnitude_map, magnitude_map, 0, 255, cv::NORM_MINMAX);
    std::cout << "Initialization complete." << std::endl;
}

void Painter::analyze_image_tones() {
    std::cout << "Analyzing image color tones..." << std::endl;
    double minVal, maxVal;
    cv::minMaxLoc(segments, &minVal, &maxVal);
    int num_segments_val = static_cast<int>(maxVal);

    for (int i = 0; i <= num_segments_val; ++i) {
        cv::Mat mask = (segments == i);
        if (cv::countNonZero(mask) > 0) {
            cv::Scalar avg_color_scalar = cv::mean(reference_image_rgb, mask);
            cv::Vec3b avg_color(static_cast<uchar>(avg_color_scalar[0]), static_cast<uchar>(avg_color_scalar[1]), static_cast<uchar>(avg_color_scalar[2]));
            color_tones[avg_color].push_back(i);
        }
    }
    std::cout << "Found " << color_tones.size() << " distinct color tones." << std::endl;
}

void Painter::paint() {
    std::cout << "Starting painting process..." << std::endl;
    std::vector<PaintingPass> passes = {
        {1000, 8, 0.7},
        {500, 4, 0.8},
        {200, 2, 0.9}
    };
    run_painting_passes(passes);
    run_detailed_touch_up_pass();
    std::cout << "Painting process complete." << std::endl;
}

void Painter::run_painting_passes(const std::vector<PaintingPass>& passes) {
    std::cout << "Running main painting passes..." << std::endl;

    // Create a vector of colors to sort by brightness
    std::vector<std::pair<cv::Vec3b, std::vector<int>>> sorted_tones;
    for (const auto& pair : color_tones) {
        sorted_tones.push_back(pair);
    }
    std::sort(sorted_tones.begin(), sorted_tones.end(), [](const auto& a, const auto& b) {
        return get_brightness(a.first) < get_brightness(b.first);
    });

    // Create a dummy brush texture for now
    cv::Mat brush_texture(64, 64, CV_8U, cv::Scalar(255));
    cv::circle(brush_texture, cv::Point(32, 32), 32, cv::Scalar(0), -1);
    brush_texture = 255 - brush_texture;


    for (const auto& pass : passes) {
        std::cout << "  - Pass with brush size: " << pass.brush_size << std::endl;
        for (const auto& color_pair : sorted_tones) {
            cv::Vec3b color = color_pair.first;
            uint8_t stroke_color[3] = {color[2], color[1], color[0]}; // RGB for brush engine

            for (int seg_id : color_pair.second) {
                cv::Mat mask = (segments == seg_id);
                std::vector<cv::Point> points;
                cv::findNonZero(mask, points);
                if (points.empty()) continue;

                std::random_shuffle(points.begin(), points.end());

                for (size_t i = 0; i < points.size(); i += pass.chunk_size) {
                    size_t end = std::min(i + pass.chunk_size, points.size());
                    for (size_t j = i; j < end; ++j) {
                        cv::Point start_pos = points[j];
                        float angle_deg = orientation_map.at<float>(start_pos.y, start_pos.x);
                        float length = std::max(5, pass.brush_size);

                        Point end_p;
                        end_p.x = start_pos.x + static_cast<int>(length * std::cos(angle_deg * CV_PI / 180.0));
                        end_p.y = start_pos.y + static_cast<int>(length * std::sin(angle_deg * CV_PI / 180.0));

                        // Clamp to image bounds
                        end_p.x = std::max(0, std::min(canvas.cols - 1, end_p.x));
                        end_p.y = std::max(0, std::min(canvas.rows - 1, end_p.y));

                        std::vector<Point> stroke_path = {{start_pos.x, start_pos.y}, end_p};
                        std::vector<float> angles = {angle_deg, angle_deg};

                        render_stroke(
                            canvas.data, canvas.cols, canvas.rows,
                            stroke_path,
                            angles,
                            brush_texture.data, brush_texture.cols,
                            stroke_color,
                            pass.brush_size,
                            pass.opacity
                        );
                    }
                }
            }
        }
    }
}

void Painter::run_detailed_touch_up_pass() {
    std::cout << "Running detailed touch-up pass (placeholder)..." << std::endl;
    // TODO: Implement the more detailed touch-up logic from the Python script.
}

cv::Mat Painter::get_canvas() const {
    return canvas;
}
