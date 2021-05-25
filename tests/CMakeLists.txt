cmake_minimum_required(VERSION 3.17)
project(bcg_library_test)

option(BCG_TORCH "Build Torch" ON)

add_executable(bcg_library_test)

target_sources(bcg_library_test PRIVATE
        bcg_map_test.cpp
        bcg_template_test.cpp
        bcg_test_base_grid.cpp
        bcg_test_dynamic_bitset.cpp
        bcg_test_file.cpp
        bcg_test_file_watcher.cpp
        bcg_test_stl_utils.cpp
        bcg_test_string_utils.cpp
        bcg_map_test.cpp
        bcg_test_property.cpp
        bcg_test_point_cloud.cpp
        bcg_test_point_cloudio.cpp
        bcg_test_graph.cpp
        bcg_test_mesh.cpp
        bcg_test_mesh_simplification.cpp
        bcg_test_meshio.cpp
        bcg_test_triangle.cpp
        bcg_test_sphere.cpp
        bcg_test_polynomial.cpp
        bcg_test_monomial_basis.cpp
        bcg_test_bernstein_basis.cpp
        bcg_test_occupancy_grid.cpp
        bcg_test_color_rgb.cpp
        )
if(BCG_TORCH)
    target_sources(bcg_library_test PRIVATE
            bcg_test_torch.cpp
            )

    target_link_libraries(bcg_library_test PUBLIC torch)
endif()

target_link_libraries(bcg_library_test PUBLIC bcg_graphics bcg_opengl gtest gtest_main)