#ifndef BRUSH_ENGINE_H
#define BRUSH_ENGINE_H

#include <cstdint>
#include <vector>

// A simple struct to represent a 2D point.
struct Point {
    int x;
    int y;
};

/**
 * @brief Renders a single, textured brush stroke onto a canvas.
 *
 * This function simulates a brush stroke by stamping a brush texture along a given path.
 * It handles blending the brush color with the canvas, respecting opacity,
 * and simulates pressure sensitivity and brush rotation.
 *
 * @param canvas_data A pointer to the raw pixel data of the canvas (RGB format).
 * @param canvas_width The width of the canvas in pixels.
 * @param canvas_height The height of the canvas in pixels.
 * @param path A vector of Point structs representing the path of the stroke.
 * @param angles A vector of angles (in degrees) for each point in the path, for rotation.
 * @param brush_texture_data A pointer to the raw pixel data of the brush texture (grayscale).
 * @param brush_texture_size The width and height of the (square) brush texture.
 * @param color The RGB color of the brush stroke.
 * @param stroke_size The base size of the brush stroke.
 * @param opacity A global opacity multiplier for the stroke.
 */
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
    float opacity
);

#endif // BRUSH_ENGINE_H
