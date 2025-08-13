cmake_minimum_required(VERSION 3.18)
project(AutoCanvasPainter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV and its required components
# The user will need to have OpenCV installed on their system.
find_package(OpenCV REQUIRED COMPONENTS
    core
    highgui
    imgproc
    imgcodecs
    saliency
    ximgproc
    objdetect # For face detection
)

# Add the executable target with all source files
add_executable(AutoCanvasPainter
    main.cpp
    brush_engine.cpp
    image_processing.cpp
    painter.cpp
)

# Link against OpenCV libraries
target_link_libraries(AutoCanvasPainter PRIVATE ${OpenCV_LIBS})

# Add include directories for OpenCV
target_include_directories(AutoCanvasPainter PRIVATE ${OpenCV_INCLUDE_DIRS})
