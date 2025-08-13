#include "image_processing.h"
#include <opencv2/saliency.hpp>
#include <opencv2/ximgproc.hpp>

cv::Mat compute_saliency(const cv::Mat& image) {
    cv::Ptr<cv::saliency::Saliency> saliency = cv::saliency::StaticSaliencySpectralResidual::create();
    cv::Mat saliency_map;
    if (saliency->computeSaliency(image, saliency_map)) {
        // Convert to 8-bit for display or further processing
        saliency_map.convertTo(saliency_map, CV_8U, 255);
        return saliency_map;
    }
    // Return an empty matrix if saliency computation fails
    return cv::Mat();
}

cv::Mat compute_edge_map(const cv::Mat& image_gray) {
    cv::Mat edge_map;
    cv::Canny(image_gray, edge_map, 50, 150);
    return edge_map;
}

cv::Mat detect_features(const cv::Mat& image_gray) {
    // Note: The paths to the cascade files might need to be adjusted
    // depending on the system's OpenCV installation.
    // A robust solution would be to bundle them with the application or use cv::samples::findFile.
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load(cv::samples::findFile("haarcascades/haarcascade_frontalface_default.xml"))) {
        std::cerr << "Error: Could not load face cascade." << std::endl;
        return cv::Mat();
    }

    cv::CascadeClassifier eye_cascade;
    if (!eye_cascade.load(cv::samples::findFile("haarcascades/haarcascade_eye.xml"))) {
        std::cerr << "Error: Could not load eye cascade." << std::endl;
        return cv::Mat();
    }

    cv::Mat features_mask = cv::Mat::zeros(image_gray.size(), CV_8U);

    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(image_gray, faces, 1.1, 5, 0, cv::Size(30, 30));

    for (const auto& face_rect : faces) {
        cv::rectangle(features_mask, face_rect, cv::Scalar(255), -1);
        cv::Mat roi_gray = image_gray(face_rect);
        std::vector<cv::Rect> eyes;
        eye_cascade.detectMultiScale(roi_gray, eyes);
        for (const auto& eye_rect : eyes) {
            cv::Rect absolute_eye_rect = eye_rect + cv::Point(face_rect.x, face_rect.y);
            cv::rectangle(features_mask, absolute_eye_rect, cv::Scalar(255), -1);
        }
    }

    return features_mask;
}

cv::Mat segment_image(const cv::Mat& image, int num_segments) {
    // The Python code uses skimage.segmentation.slic.
    // Here, we use the OpenCV equivalent from the ximgproc module.
    cv::Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(image, cv::ximgproc::SLICO, 10, 10.0f);
    slic->iterate(10); // Number of iterations

    cv::Mat labels;
    slic->getLabels(labels);

    return labels;
}
