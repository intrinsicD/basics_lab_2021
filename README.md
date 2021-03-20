# Setup

Clone this repository by running `git clone --recurse-submodules https://github.com/intrinsicD/basics_lab.git`.

In *external/spectra/CMakeLists.txt* the following modifications have to be performed:
  - change all occurences of `CMAKE_SOURCE_DIR` to `CMAKE_CURRENT_SOURCE_DIR`.
  - ~~in line 23 replace `find_package(Eigen3 NO_MODULE REQUIRED)` with `find_package(eigen3 REQUIRED)`.~~
  - ~~in line 36 replace `target_link_libraries(Spectra INTERFACE Eigen3::Eigen)` with `target_link_libraries(Spectra INTERFACE Eigen3)`.~~

~~In *external/eigen/CMakeLists.txt* the following modifications have to be performed:~~
  - ~~comment out the `add_custom_target(uninstall ...)` command in lines 672 and 673.~~
~~In *external/nanoflann/CMakeLists.txt* the following modifications have to be performed:~~
  - ~~comment out the `add_custom_target(uninstall ...)` command in lines 151 and 152.~~

If glad does not work:
- go to the website **https://glad.dav1d.de/** and download the glad files
  - Language **C/C++**
  - use specification **OpenGL**
  - Profile **Core**
  - API gl **Version 4.6** if possible, but at least **Version 4.1**
  - Options mark **Generate a loader**
- add the **glad.{c, h}** files to *external/glad*
- add the **KHR** folder to *external/*


# Compilation

To compile the project create a *build* folder first.

### Linux/Mac

From the *build* folder run `cmake ..` to create a `Makefile`.

For std::filesystem support using clang10: `cmake -DCMAKE_C_COMPILER=clang-10 -DCMAKE_CXX_COMPILER=clang++-10 ..` or

For std::filesystem support using gcc10: `cmake -DCMAKE_C_COMPILER=gcc-10 -DCMAKE_CXX_COMPILER=gcc++-10 ..`

Then run `make` to compile the project.

If you want to build using multiple cpu cores in parallel run `make -j 5` for building with 5 cores for example.

### Windows

From the *build* folder run `cmake .. -A x64` to create a Visual Studio project solution.

Open the created project solution and compile it like a regular Visual Studio project.


### Troubleshooting

If the application can't find the renderers, change the path in `bin/bcg_viewer.config` to the correct path on your machine.  
Maybe you need to provide the full path to the renderers in `libs/bcg_opengl/renderers`, instead of the relative path from the binary.

# Usage

- Quit: Strg/Ctrl+Q
- Scale Points: Strg + Mouse wheel
- Zoom Camera: Mouse wheel
- Rotate Camera: Mouse left drag
- Move Camera in Plane: Mouse middle drag
- Move Camera Forward/Left/Backward/Right: W/A/S/D
- Reset Camera: R
- Left Click: Select
- Curve:
  - Left Click: Add point
  - Right Click: End point
  - Strg Press: Fix Camera
