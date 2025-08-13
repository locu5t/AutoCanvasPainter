cmake_minimum_required(VERSION 3.18)
project(CBrushEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# Find pybind11
execute_process(
  COMMAND ${Python_EXECUTABLE} -m pybind11 --cmakedir
  OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(pybind11_DIR ${PYBIND11_CMAKE_DIR})
find_package(pybind11 REQUIRED)

# Create the Python module
pybind11_add_module(c_brush_engine MODULE
    brush_engine.cpp
    bindings.cpp
)
