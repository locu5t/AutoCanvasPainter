#include "brush_engine.h"
#include <algorithm> // For std::min and std::max
#include <cmath>     // For std::sin and trigonometry functions

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
    const std::vector<float>& angles,
    const uint8_t* brush_texture_data,
    int brush_texture_size,
    const uint8_t color[3],
    int stroke_size,
    float opacity) {

    if (path.empty() || stroke_size <= 0 || opacity <= 0.0f || path.size() != angles.size()) {
        return;
    }

    for (size_t i = 0; i < path.size(); ++i) {
        const auto& point = path[i];
        float angle_rad = angles[i] * (3.1415926535f / 180.0f);

        // Pressure Sensitivity Simulation (Tapering)
        float t = (path.size() <= 1) ? 1.0f : static_cast<float>(i) / (path.size() - 1.0f);
        float taper_factor = std::sin(t * 3.1415926535f);

        int current_stroke_size = static_cast<int>(stroke_size * taper_factor);
        if (current_stroke_size < 1) continue;
        float current_opacity = opacity * taper_factor;
        int half_stroke_size = current_stroke_size / 2;

        // Bounding box for the brush stamp
        int start_x = std::max(0, point.x - half_stroke_size);
        int end_x = std::min(canvas_width, point.x + half_stroke_size);
        int start_y = std::max(0, point.y - half_stroke_size);
        int end_y = std::min(canvas_height, point.y + half_stroke_size);

        float cos_angle = std::cos(-angle_rad);
        float sin_angle = std::sin(-angle_rad);

        for (int y = start_y; y < end_y; ++y) {
            for (int x = start_x; x < end_x; ++x) {
                // Inverse transformation: from canvas coordinates to brush texture coordinates
                float translated_x = x - point.x;
                float translated_y = y - point.y;
                float rotated_x = translated_x * cos_angle - translated_y * sin_angle;
                float rotated_y = translated_x * sin_angle + translated_y * cos_angle;
                float tex_x_f = (rotated_x / current_stroke_size + 0.5f) * brush_texture_size;
                float tex_y_f = (rotated_y / current_stroke_size + 0.5f) * brush_texture_size;

                int tex_x = static_cast<int>(tex_x_f);
                int tex_y = static_cast<int>(tex_y_f);

                if (tex_x >= 0 && tex_x < brush_texture_size && tex_y >= 0 && tex_y < brush_texture_size) {
                    // Get the alpha from the brush texture
                    uint8_t texture_alpha_byte = brush_texture_data[tex_y * brush_texture_size + tex_x];
                    float texture_alpha = texture_alpha_byte / 255.0f;

                    // Combine with global opacity and taper
                    float final_alpha = texture_alpha * current_opacity;

                    if (final_alpha > 0) {
                        uint8_t* canvas_pixel = &canvas_data[(y * canvas_width + x) * 3];
                        blend_pixel(canvas_pixel, color, final_alpha);
                    }
                }
            }
        }
    }
}
