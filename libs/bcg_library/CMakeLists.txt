
find_package(Eigen3 REQUIRED)
find_package(rply REQUIRED)
find_package(entt REQUIRED)
find_package(tbb REQUIRED)
find_package(spectra REQUIRED)
find_package(nanoflann REQUIRED)

add_library(bcg_graphics
        color/bcg_colors.h color/bcg_colors.cpp
        utils/bcg_file.h utils/bcg_file.cpp
        utils/bcg_dynamic_bitset.h
        utils/bcg_file_watcher.h
        utils/bcg_logger.h
        utils/bcg_path.h utils/bcg_path.cpp
        utils/bcg_stl_utils.h
        utils/bcg_string_utils.h utils/bcg_string_utils.cpp
        math/bcg_math_common.h math/bcg_linalg.h
        math/bcg_polynomial.h math/bcg_polynomial.cpp
        math/bcg_monomial_basis.h math/bcg_monomial_basis.cpp
        math/bcg_factorial.h math/bcg_factorial.cpp
        math/bcg_binomial_coefficient.h math/bcg_binomial_coefficient.cpp
        math/bcg_bernstein_basis.h math/bcg_bernstein_basis.cpp
        math/vector/bcg_math_vector_angle.h math/vector/bcg_math_vector_cos.h math/vector/bcg_math_vector_sin.h
        geometry/aligned_box/bcg_aligned_box.h geometry/aligned_box/bcg_aligned_child_hierarchy.h
        geometry/bcg_property.h
        geometry/bcg_property_map_eigen.h
        geometry/point_cloud/bcg_point_cloud.h geometry/point_cloud/bcg_point_cloud.cpp geometry/point_cloud/bcg_point_cloudio.h geometry/point_cloud/bcg_point_cloudio.cpp
        geometry/graph/bcg_graph.h geometry/graph/bcg_graph.cpp
        geometry/graph/bcg_graph_edge_centers.h geometry/graph/bcg_graph_edge_centers.cpp
        geometry/mesh/bcg_mesh.h geometry/mesh/bcg_mesh.cpp
        geometry/mesh/bcg_meshio.h geometry/mesh/bcg_meshio.cpp
        geometry/mesh/bcg_mesh_face_area_vector.h geometry/mesh/bcg_mesh_face_area_vector.cpp
        geometry/mesh/bcg_mesh_face_normals.h geometry/mesh/bcg_mesh_face_normals.cpp
        geometry/mesh/bcg_mesh_face_centers.h geometry/mesh/bcg_mesh_face_centers.cpp
        geometry/mesh/bcg_mesh_vertex_normals.h geometry/mesh/bcg_mesh_vertex_normals.cpp
        geometry/mesh/bcg_mesh_factory.h geometry/mesh/bcg_mesh_factory.cpp
        geometry/mesh/bcg_mesh_edge_dihedral_angle.h geometry/mesh/bcg_mesh_edge_dihedral_angle.cpp
        geometry/mesh/bcg_mesh_boundary.h geometry/mesh/bcg_mesh_boundary.cpp
        geometry/mesh/bcg_mesh_vertex_convex_concave.h geometry/mesh/bcg_mesh_vertex_convex_concave.cpp
        geometry/mesh/bcg_mesh_features.h geometry/mesh/bcg_mesh_features.cpp
        geometry/mesh/bcg_mesh_subdivision.h geometry/mesh/bcg_mesh_subdivision.cpp
        geometry/mesh/bcg_mesh_connected_components.h geometry/mesh/bcg_mesh_connected_components.cpp
        geometry/curve/bcg_curve.h geometry/curve/bcg_curve.cpp
        geometry/curve/bcg_curve_bezier.h geometry/curve/bcg_curve_bezier.cpp
        geometry/triangle/bcg_triangle.h geometry/triangle/bcg_barycentric_coordinates.h geometry/triangle/bcg_triangle_centers.h geometry/triangle/bcg_triangle_distance.h geometry/triangle/bcg_triangle_metric.h
        geometry/sphere/bcg_sphere.h geometry/sphere/bcg_sphere_fit.h geometry/sphere/bcg_sphere_sampling_fibunacci.h
        geometry/distance_query/bcg_distance_segment_point.h
        geometry/distance_query/bcg_distance_aligned_box_point.h
        geometry/distance_query/bcg_distance_triangle_point.h
        ${RPLY_SOURCE_DIRS}/rply.c
        )

target_compile_features(bcg_graphics PRIVATE cxx_std_17)
set_target_properties(bcg_graphics PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(bcg_graphics PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC ${RPLY_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC ${NANOFLANN_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC . ./geometry)

target_link_libraries(bcg_graphics ${OPENGL_gl_LIBRARY} tbb stdc++fs)

if (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wno-shorten-64-to-32 -Wno-c++20-extensions -Wno-deprecated-copy -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wpedantic -Wextra -Wno-missing-field-initializers -g -ggdb -fprofile-instr-generate -fcoverage-mapping -fstandalone-debug -O0)
    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
    target_link_libraries(bcg_graphics Threads::Threads)
endif (UNIX AND NOT APPLE)

# warning flags
if (APPLE)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wno-deprecated -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
endif (APPLE)
if (MSVC)
    target_compile_options(bcg_graphics PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267 /wd4838)
    target_compile_options(bcg_graphics PUBLIC /EHsc)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

endif (MSVC)
