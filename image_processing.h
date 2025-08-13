#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>

// Computes the saliency map of an image.
cv::Mat compute_saliency(const cv::Mat& image);

// Computes the edge map of a grayscale image using the Canny edge detector.
cv::Mat compute_edge_map(const cv::Mat& image_gray);

// Detects features (faces and eyes) in a grayscale image.
// Returns a binary mask where features are detected.
cv::Mat detect_features(const cv::Mat& image_gray);

// Segments an image using the SLIC superpixel algorithm.
// Returns a matrix of segment labels.
cv::Mat segment_image(const cv::Mat& image, int num_segments = 500);

#endif // IMAGE_PROCESSING_H
