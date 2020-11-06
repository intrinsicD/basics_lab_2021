**Setup:**
- to start run bash **script/init.sh**
- add **glad/glad.{c, h}** opengl 4.5 to *libs/bcg_opengl/external/*
- change in *external/spectral/CMakeLists.txt* all occurrences of
    **CMAKE_SOURCE_DIR** to **CMAKE_CURRENT_SOURCE_DIR**
  	and find_package(**Eigen3 NO_MODULE REQUIRED**) to find_package(**eigen3 REQUIRED**)

