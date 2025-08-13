#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "brush_engine.h"

namespace py = pybind11;

void py_render_stroke(
    py::array_t<uint8_t, py::array::c_style | py::array::forcecast> canvas,
    const std::vector<Point>& path,
    py::array_t<uint8_t, py::array::c_style | py::array::forcecast> brush_texture,
    std::tuple<uint8_t, uint8_t, uint8_t> color,
    int stroke_size,
    float opacity) {

    // Get pointers to the raw data of the numpy arrays
    py::buffer_info canvas_buf = canvas.request();
    uint8_t* canvas_ptr = static_cast<uint8_t*>(canvas_buf.ptr);
    int canvas_height = canvas_buf.shape[0];
    int canvas_width = canvas_buf.shape[1];

    py::buffer_info brush_buf = brush_texture.request();
    uint8_t* brush_ptr = static_cast<uint8_t*>(brush_buf.ptr);
    int brush_size = brush_buf.shape[0]; // Assuming square brush

    // Convert color tuple to array
    uint8_t c_color[3] = {std::get<0>(color), std::get<1>(color), std::get<2>(color)};

    // Call the actual C++ function
    render_stroke(
        canvas_ptr,
        canvas_width,
        canvas_height,
        path,
        brush_ptr,
        brush_size,
        c_color,
        stroke_size,
        opacity
    );
}

PYBIND11_MODULE(c_brush_engine, m) {
    m.doc() = "C++ brush engine for realistic brush strokes";

    // Define the Point struct for Python
    py::class_<Point>(m, "Point")
        .def(py::init<int, int>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y);

    // Define the binding for the render_stroke function
    m.def("render_stroke", &py_render_stroke, "Renders a stroke to a canvas",
          py::arg("canvas"),
          py::arg("path"),
          py::arg("brush_texture"),
          py::arg("color"),
          py::arg("stroke_size"),
          py::arg("opacity"));
}
