add_executable(bcg_viewer main.cpp)

set_target_properties(bcg_viewer PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(bcg_viewer PRIVATE ${CMAKE_SOURCE_DIR}/libs)
target_link_libraries(bcg_viewer bcg_opengl bcg_graphics)