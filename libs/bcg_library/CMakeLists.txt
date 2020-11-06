
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
        math/vector/bcg_math_vector_angle.h math/vector/bcg_math_vector_cos.h math/vector/bcg_math_vector_sin.h
        geometry/aligned_box/bcg_aligned_box.h geometry/aligned_box/bcg_aligned_child_hierarchy.h
        geometry/bcg_property.h
        geometry/bcg_property_map_eigen.h
        geometry/bcg_point_cloud.h geometry/bcg_point_cloud.cpp geometry/bcg_point_cloudio.h geometry/bcg_point_cloudio.cpp
        geometry/bcg_graph.h geometry/bcg_graph.cpp
        geometry/mesh/bcg_mesh.h geometry/mesh/bcg_mesh.cpp geometry/mesh/bcg_meshio.h geometry/mesh/bcg_meshio.cpp geometry/mesh/bcg_mesh_face_area_vector.h geometry/mesh/bcg_mesh_face_normals.h geometry/mesh/bcg_mesh_vertex_normals.h
        geometry/mesh/bcg_mesh_factory.h geometry/mesh/bcg_mesh_factory.cpp
        geometry/triangle/bcg_triangle.h geometry/triangle/bcg_barycentric_coordinates.h geometry/triangle/bcg_triangle_centers.h geometry/triangle/bcg_triangle_distance.h geometry/triangle/bcg_triangle_metric.h
        geometry/sphere/bcg_sphere.h geometry/sphere/bcg_sphere_fit.h geometry/sphere/bcg_sphere_sampling_fibunacci.h
        ${RPLY_SOURCE_DIRS}/rply.c
        )

set_target_properties(bcg_graphics PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(bcg_graphics PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC ${RPLY_INCLUDE_DIRS})
#target_include_directories(bcg_graphics PUBLIC . ./geometry)
#target_include_directories(bcg_graphics PUBLIC . ./math)
target_include_directories(bcg_graphics PUBLIC . ./geometry)

target_link_libraries(bcg_graphics ${OPENGL_gl_LIBRARY} tbb)

if (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_compile_options(bcg_graphics PUBLIC -Wall -Wpedantic -Wextra -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    target_link_libraries(bcg_graphics Threads::Threads)
endif (UNIX AND NOT APPLE)

# warning flags
if (APPLE)
    target_compile_options(bcg_graphics PUBLIC -Wall -Wpedantic -Wextra -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
endif (APPLE)
if (MSVC)
    target_compile_options(bcg_graphics PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267)
    target_compile_options(bcg_graphics PUBLIC /EHsc)
endif (MSVC)