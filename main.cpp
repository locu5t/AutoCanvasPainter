#include <iostream>
#include <opencv2/opencv.hpp>
#include "painter.h"

int main(int argc, char** argv) {
    // Check for command-line arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_image_path>" << std::endl;
        return 1;
    }

    // Load the input image
    std::string image_path = argv[1];
    cv::Mat input_image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (input_image.empty()) {
        std::cerr << "Error: Could not load image from path: " << image_path << std::endl;
        return 1;
    }

    std::cout << "Successfully loaded image: " << image_path << std::endl;

    try {
        // Create a Painter instance
        Painter painter(input_image);

        // Run the painting process
        painter.paint();

        // Get the final canvas
        cv::Mat final_canvas = painter.get_canvas();

        // Save the output image
        std::string output_filename = "painting_output.png";
        if (cv::imwrite(output_filename, final_canvas)) {
            std::cout << "Successfully saved painting to " << output_filename << std::endl;
        } else {
            std::cerr << "Error: Could not save the output image." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred during the painting process: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
