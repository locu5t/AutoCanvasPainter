#ifndef PAINTER_H
#define PAINTER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "image_processing.h"
#include "brush_engine.h"

// Struct to hold parameters for a painting pass
struct PaintingPass {
    int chunk_size;
    int brush_size;
    double opacity;
    // We can add other parameters like masks here if needed
};

class Painter {
public:
    // Constructor takes the reference image to be painted
    Painter(const cv::Mat& reference_image);

    // The main method that orchestrates the entire painting process
    void paint();

    // Returns the final painted canvas
    cv::Mat get_canvas() const;

private:
    cv::Mat canvas;
    cv::Mat reference_image_rgb;
    cv::Mat reference_image_gray;

    // Maps and data structures for painting logic
    cv::Mat saliency_map;
    cv::Mat edge_map;
    cv::Mat feature_mask;
    cv::Mat orientation_map;
    cv::Mat magnitude_map;
    cv::Mat segments;

    // A map from color to a list of segment IDs
    std::map<cv::Vec3b, std::vector<int>, decltype([](const cv::Vec3b& a, const cv::Vec3b& b) {
        if (a[0] != b[0]) return a[0] < b[0];
        if (a[1] != b[1]) return a[1] < b[1];
        return a[2] < b[2];
    })> color_tones;

    // Private helper methods to structure the painting process
    void initialize_maps();
    void analyze_image_tones();
    void run_painting_passes(const std::vector<PaintingPass>& passes);
    void run_detailed_touch_up_pass();
};

#endif // PAINTER_H
