**Setup:**
- to start run bash **script/init.sh**
- go to the website **https://glad.dav1d.de/** and download the glad files
  - Language **C/C++**
  - use specification **OpenGL**
  - Profile **Core**
  - API gl **Version 4.6** if possible, but at least **Version 4.1**
  - Options mark **Generate a loader**
- add the **glad.{c, h}** files to *external/glad*
- change in *external/spectral/CMakeLists.txt* all occurrences of
    **CMAKE_SOURCE_DIR** to **CMAKE_CURRENT_SOURCE_DIR**
  	and find_package(**Eigen3 NO_MODULE REQUIRED**) to find_package(**eigen3 REQUIRED**)

**Usage**
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

