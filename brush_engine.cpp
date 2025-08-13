#include "brush_engine.h"
#include <algorithm> // For std::min and std::max

// Helper function for linear interpolation
template<typename T>
T lerp(T a, T b, float t) {
    return static_cast<T>(a + (b - a) * t);
}

// Helper function for alpha blending
void blend_pixel(uint8_t* canvas_pixel, const uint8_t color[3], float alpha) {
    canvas_pixel[0] = lerp(canvas_pixel[0], color[0], alpha);
    canvas_pixel[1] = lerp(canvas_pixel[1], color[1], alpha);
    canvas_pixel[2] = lerp(canvas_pixel[2], color[2], alpha);
}

void render_stroke(
    uint8_t* canvas_data,
    int canvas_width,
    int canvas_height,
    const std::vector<Point>& path,
    const uint8_t* brush_texture_data,
    int brush_texture_size,
    const uint8_t color[3],
    int stroke_size,
    float opacity) {

    if (path.empty() || stroke_size <= 0 || opacity <= 0.0f) {
        return;
    }

    int half_stroke_size = stroke_size / 2;

    for (const auto& point : path) {
        int center_x = point.x;
        int center_y = point.y;

        // Determine the bounding box of the brush stamp on the canvas
        int start_x = std::max(0, center_x - half_stroke_size);
        int end_x = std::min(canvas_width, center_x + half_stroke_size);
        int start_y = std::max(0, center_y - half_stroke_size);
        int end_y = std::min(canvas_height, center_y + half_stroke_size);

        for (int y = start_y; y < end_y; ++y) {
            for (int x = start_x; x < end_x; ++x) {
                // Convert canvas coordinates to brush texture coordinates
                float u = static_cast<float>(x - (center_x - half_stroke_size)) / stroke_size;
                float v = static_cast<float>(y - (center_y - half_stroke_size)) / stroke_size;

                int tex_x = static_cast<int>(u * brush_texture_size);
                int tex_y = static_cast<int>(v * brush_texture_size);

                if (tex_x >= 0 && tex_x < brush_texture_size && tex_y >= 0 && tex_y < brush_texture_size) {
                    // Get the alpha from the brush texture
                    uint8_t texture_alpha_byte = brush_texture_data[tex_y * brush_texture_size + tex_x];
                    float texture_alpha = texture_alpha_byte / 255.0f;

                    // Combine with global opacity
                    float final_alpha = texture_alpha * opacity;

                    if (final_alpha > 0) {
                        // Get a pointer to the current pixel on the canvas
                        uint8_t* canvas_pixel = &canvas_data[(y * canvas_width + x) * 3];
                        blend_pixel(canvas_pixel, color, final_alpha);
                    }
                }
            }
        }
    }
}
