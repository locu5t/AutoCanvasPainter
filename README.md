# AutoCanvasPainter (C++ Version)

This project is a standalone C++ application that automatically simulates the painting of an input image, creating a piece of art that mimics human brush strokes and painting styles.

This version is a complete rewrite of the original Python-based painter, designed for higher performance and enhanced visual realism.

## Features

-   **Standalone C++ Application:** No Python dependency for running the final application.
-   **High-Performance Painting:** Core logic written in C++ for speed.
-   **Advanced Brush Engine:** The brush engine simulates pressure sensitivity and brush rotation for more realistic and dynamic strokes.
-   **Image Analysis:** Uses OpenCV for advanced image analysis, including saliency detection, edge maps, feature detection, and image segmentation.
-   **Command-Line Interface:** A simple command-line application to process images.

## Prerequisites

To build this application, you will need:

-   **A modern C++ Compiler:** Supporting C++17 (e.g., GCC, Clang, MSVC).
-   **CMake (version 3.18+):** For building the project.
-   **OpenCV (version 4.x):** You need to have the full OpenCV library installed on your system, including the `saliency`, `ximgproc`, and `objdetect` modules (often found in `opencv-contrib`).

## How to Build

1.  **Clone the repository and create a build directory:**
    ```bash
    # git clone ... (or download the source)
    mkdir build
    cd build
    ```

2.  **Run CMake to configure the project:**
    CMake will try to find the OpenCV installation on your system.
    ```bash
    cmake ..
    ```

3.  **Compile the application:**
    ```bash
    cmake --build .
    ```
    On Windows, you might need to specify the configuration, e.g., `cmake --build . --config Release`.

After the build is complete, you will find the executable `AutoCanvasPainter` (or `AutoCanvasPainter.exe` on Windows) inside the `build` directory.

## How to Run

The application is run from the command line, with the path to the input image as an argument.

From inside the `build` directory, run:
```bash
./AutoCanvasPainter ../path/to/your/image.jpg
```

The output will be saved as `painting_output.png` in the `build` directory.

---
*This project was developed with assistance from a cutting-edge AI software engineer.*
