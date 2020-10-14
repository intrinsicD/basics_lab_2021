cmake_minimum_required(VERSION 3.17)
project(bcg_library_test)

link_directories(${CMAKE_BINARY_DIR})

set(TEST_SOURCES
        bcg_template_test.cpp
        bcg_test_dynamic_bitset.cpp
        bcg_test_file.cpp
        bcg_test_file_watcher.cpp
        bcg_stl_utils.cpp
        bcg_glm_vector_test.cpp
        bcg_test_property.cpp
        )
add_executable(bcg_library_test ${TEST_SOURCES})
set_target_properties(bcg_library_test PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_link_libraries(bcg_library_test gtest gtest_main bcg_graphics bcg_opengl)