
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
        utils/bcg_timer.h
        utils/bcg_string_utils.h utils/bcg_string_utils.cpp
        math/bcg_math_common.h math/bcg_linalg.h
        math/bcg_polynomial.h math/bcg_polynomial.cpp
        math/bcg_monomial_basis.h math/bcg_monomial_basis.cpp
        math/bcg_factorial.h math/bcg_factorial.cpp
        math/bcg_binomial_coefficient.h math/bcg_binomial_coefficient.cpp
        math/bcg_bernstein_basis.h math/bcg_bernstein_basis.cpp
        math/bcg_pca.h
        math/bcg_robust_pca.h
        math/statistics/bcg_gaussian.h
        math/bcg_vertex_classify_curvature.h math/bcg_vertex_classify_curvature.cpp
        math/laplacian/bcg_laplacian_matrix.h
        math/laplacian/bcg_laplacian_heat_diffusion.h
        math/laplacian/bcg_laplacian_smoothing.h
        math/matrix/bcg_matrix.h
        math/matrix/bcg_matrix_check_finite.h
        math/matrix/bcg_matrix_horizontal_stack.h
        math/matrix/bcg_matrix_vertical_stack.h
        math/matrix/bcg_matrix_anti_symmetric_part.h
        math/matrix/bcg_matrix_covariance.h
        math/matrix/bcg_matrix_kernel.h
        math/matrix/bcg_matrix_pairwise_distances.h
        math/vector/bcg_math_vector_angle.h math/vector/bcg_math_vector_cos.h math/vector/bcg_math_vector_sin.h math/vector/bcg_vector_median_filter.h math/vector/bcg_vector_median_filter_directional.h math/vector/bcg_vector_median_filter_fuzzy.h
        math/sparse_matrix/bcg_sparse_matrix.h
        math/sparse_matrix/bcg_sparse_check_symmetric.h math/sparse_matrix/bcg_sparse_check_symmetric.cpp
        math/sparse_matrix/bcg_sparse_check_hermetian.h math/sparse_matrix/bcg_sparse_check_hermetian.cpp
        math/sparse_matrix/bcg_sparse_check_finite.h math/sparse_matrix/bcg_sparse_check_finite.cpp
        math/sparse_matrix/bcg_sparse_vertical_stack.h math/sparse_matrix/bcg_sparse_vertical_stack.cpp
        math/sparse_matrix/bcg_sparse_horizontal_stack.h math/sparse_matrix/bcg_sparse_horizontal_stack.cpp
        math/statistics/bcg_statistics_running.h math/statistics/bcg_statistics_running.cpp
        math/laplacian/bcg_laplacian_matrix.h
        math/laplacian/bcg_laplacian_smoothing.h
        math/laplacian/bcg_laplacian_heat_diffusion.h
        math/laplacian/bcg_laplacian_harmonic_field.h
        math/rotations/bcg_rotation_chordal_mean.h math/rotations/bcg_rotation_chordal_mean.cpp
        math/rotations/bcg_rotation_geodesic_mean.h math/rotations/bcg_rotation_geodesic_mean.cpp
        math/rotations/bcg_rotation_geodesic_median.h math/rotations/bcg_rotation_geodesic_median.cpp
        math/rotations/bcg_rotation_quaternion_mean.h math/rotations/bcg_rotation_quaternion_mean.cpp
        math/rotations/bcg_rotation_quaternion_median.h math/rotations/bcg_rotation_quaternion_median.cpp
        math/rotations/bcg_rotation_cross_product_matrix.h
        math/rotations/bcg_rotation_robust_elementwise_median.h
        math/rotations/bcg_rotation_matrix_exponential.h
        math/rotations/bcg_rotation_matrix_logarithm.h
        math/rotations/bcg_rotation_optimal.h
        math/rotations/bcg_rotation_project_on_so.h
        math/rotations/bcg_rotation_random.h
        geometry/intersect/bcg_intersect_sphere_aabb.h
        geometry/aligned_box/bcg_aligned_box.h geometry/aligned_box/bcg_aligned_child_hierarchy.h geometry/aligned_box/bcg_aligned_box_contains_sphere.h
        geometry/plane/bcg_plane.h
        geometry/bcg_property.h
        geometry/bcg_property_map_eigen.h
        geometry/quadric/bcg_quadric.h geometry/quadric/bcg_quadric.cpp
        geometry/point_cloud/bcg_point_cloud.h geometry/point_cloud/bcg_point_cloud.cpp geometry/point_cloud/bcg_point_cloudio.h geometry/point_cloud/bcg_point_cloudio.cpp
        geometry/point_cloud/bcg_point_cloud_graph_builder.h geometry/point_cloud/bcg_point_cloud_graph_builder.cpp
        geometry/point_cloud/bcg_point_cloud_vertex_pca.h geometry/point_cloud/bcg_point_cloud_vertex_pca.cpp
        geometry/point_cloud/bcg_point_cloud_curvature_taubin.h geometry/point_cloud/bcg_point_cloud_curvature_taubin.cpp
        geometry/point_cloud/bcg_point_cloud_vertex_outlier_probability.h geometry/point_cloud/bcg_point_cloud_vertex_outlier_probability.cpp
        geometry/point_cloud/bcg_point_cloud_kernel_density_estimation.h geometry/point_cloud/bcg_point_cloud_kernel_density_estimation.cpp
        geometry/point_cloud/bcg_point_cloud_vertex_quadric.h geometry/point_cloud/bcg_point_cloud_vertex_quadric.cpp
        geometry/point_cloud/bcg_point_cloud_bilateral_filter.h geometry/point_cloud/bcg_point_cloud_bilateral_filter.cpp
        geometry/graph/bcg_graph.h geometry/graph/bcg_graph.cpp
        geometry/graph/bcg_graph_edge_centers.h geometry/graph/bcg_graph_edge_centers.cpp
        geometry/graph/bcg_graph_edge_lengths.h geometry/graph/bcg_graph_edge_lengths.cpp
        geometry/graph/bcg_graph_vertex_pca.h geometry/graph/bcg_graph_vertex_pca.cpp
        geometry/mesh/bcg_mesh.h geometry/mesh/bcg_mesh.cpp
        geometry/mesh/bcg_meshio.h geometry/mesh/bcg_meshio.cpp
        geometry/mesh/bcg_mesh_face_area_vector.h geometry/mesh/bcg_mesh_face_area_vector.cpp
        geometry/mesh/bcg_mesh_face_areas.h geometry/mesh/bcg_mesh_face_areas.cpp
        geometry/mesh/bcg_mesh_face_normals.h geometry/mesh/bcg_mesh_face_normals.cpp
        geometry/mesh/bcg_mesh_face_centers.h geometry/mesh/bcg_mesh_face_centers.cpp
        geometry/mesh/bcg_mesh_face_gradient.h geometry/mesh/bcg_mesh_face_gradient.cpp
        geometry/mesh/bcg_mesh_factory.h geometry/mesh/bcg_mesh_factory.cpp
        geometry/mesh/bcg_mesh_triangle_area_from_metric.h geometry/mesh/bcg_mesh_triangle_area_from_metric.cpp
        geometry/mesh/bcg_mesh_triangle_cotan_weight.h geometry/mesh/bcg_mesh_triangle_cotan_weight.cpp
        geometry/mesh/bcg_mesh_edge_dihedral_angle.h geometry/mesh/bcg_mesh_edge_dihedral_angle.cpp
        geometry/mesh/bcg_mesh_edge_cotan.h geometry/mesh/bcg_mesh_edge_cotan.cpp
        geometry/mesh/bcg_mesh_edge_normals.h geometry/mesh/bcg_mesh_edge_normals.cpp
        geometry/mesh/bcg_mesh_edge_fujiwara.h geometry/mesh/bcg_mesh_edge_fujiwara.cpp
        geometry/mesh/bcg_mesh_vertex_normals.h geometry/mesh/bcg_mesh_vertex_normals.cpp
        geometry/mesh/bcg_mesh_vertex_convex_concave.h geometry/mesh/bcg_mesh_vertex_convex_concave.cpp
        geometry/mesh/bcg_mesh_vertex_area_voronoi.h geometry/mesh/bcg_mesh_vertex_area_voronoi.cpp
        geometry/mesh/bcg_mesh_vertex_area_barycentric.h geometry/mesh/bcg_mesh_vertex_area_barycentric.cpp
        geometry/mesh/bcg_mesh_vertex_cotan.h geometry/mesh/bcg_mesh_vertex_cotan.cpp
        geometry/mesh/bcg_mesh_vertex_valences.h geometry/mesh/bcg_mesh_vertex_valences.cpp
        geometry/mesh/bcg_mesh_boundary.h geometry/mesh/bcg_mesh_boundary.cpp
        geometry/mesh/bcg_mesh_features.h geometry/mesh/bcg_mesh_features.cpp
        geometry/mesh/bcg_mesh_subdivision.h geometry/mesh/bcg_mesh_subdivision.cpp
        geometry/mesh/bcg_mesh_connected_components.h geometry/mesh/bcg_mesh_connected_components.cpp
        geometry/mesh/bcg_mesh_curvature_taubin.h geometry/mesh/bcg_mesh_curvature_taubin.cpp
        geometry/mesh/bcg_mesh_smoothing.h geometry/mesh/bcg_mesh_smoothing.cpp
        geometry/mesh/bcg_mesh_laplacian.h geometry/mesh/bcg_mesh_laplacian.cpp
        geometry/mesh/bcg_mesh_surface_area.h geometry/mesh/bcg_mesh_surface_area.cpp
        geometry/mesh/bcg_mesh_simplification.h geometry/mesh/bcg_mesh_simplification.cpp
        geometry/mesh/bcg_mesh_remeshing.h geometry/mesh/bcg_mesh_remeshing.cpp
        geometry/mesh/bcg_mesh_statistics.h geometry/mesh/bcg_mesh_statistics.cpp
        geometry/curve/bcg_curve.h geometry/curve/bcg_curve.cpp
        geometry/curve/bcg_curve_bezier.h geometry/curve/bcg_curve_bezier.cpp
        geometry/triangle/bcg_triangle.h geometry/triangle/bcg_barycentric_coordinates.h geometry/triangle/bcg_triangle_centers.h geometry/triangle/bcg_triangle_metric.h
        geometry/sphere/bcg_sphere.h geometry/sphere/bcg_sphere_fit.h geometry/sphere/bcg_sphere_sampling_fibunacci.h geometry/sphere/bcg_sphere_contains_aabb.h geometry/sphere/bcg_sphere_contains_point.h
        geometry/grid/bcg_base_grid.h geometry/grid/bcg_base_grid.cpp
        geometry/grid/bcg_voxel_grid.h geometry/grid/bcg_voxel_grid.cpp
        geometry/grid/bcg_occupancy_grid.h geometry/grid/bcg_occupancy_grid.cpp
        geometry/distance_query/bcg_distance_segment_point.h
        geometry/distance_query/bcg_distance_aligned_box_point.h
        geometry/distance_query/bcg_distance_triangle_point.h
        geometry/kdtree/bcg_kdtree.h
        geometry/kdtree/bcg_triangle_kdtree.h geometry/kdtree/bcg_triangle_kdtree.cpp
        geometry/octree/bcg_octree.h geometry/octree/bcg_octree.cpp
        geometry/sampling/bcg_sampling_octree.h geometry/sampling/bcg_sampling_octree.cpp
        geometry/sampling/bcg_sampling_locally_optimal_projection.h geometry/sampling/bcg_sampling_locally_optimal_projection.cpp
        geometry/sampling/bcg_sampling_grid.h geometry/sampling/bcg_sampling_grid.cpp
        geometry/marching_cubes/bcg_marching_cubes.h geometry/marching_cubes/bcg_marching_cubes.cpp
        geometry/correspondences/bcg_correspondences.h geometry/correspondences/bcg_correspondences.cpp
        registration/bcg_registration.h
        registration/rigid_idp/bcg_rigid_icp.h registration/rigid_idp/bcg_rigid_icp.cpp
        registration/coherent_point_drift/bcg_coherent_point_drift_base2.h registration/coherent_point_drift/bcg_coherent_point_drift_base2.cpp
        registration/coherent_point_drift/bcg_coherent_point_drift_base.h registration/coherent_point_drift/bcg_coherent_point_drift_base.cpp
        registration/coherent_point_drift/bcg_coherent_point_drift_rigid.h registration/coherent_point_drift/bcg_coherent_point_drift_rigid.cpp
        registration/coherent_point_drift/bcg_coherent_point_drift_affine.h registration/coherent_point_drift/bcg_coherent_point_drift_affine.cpp
        registration/coherent_point_drift/bcg_coherent_point_drift_nonrigid.h registration/coherent_point_drift/bcg_coherent_point_drift_nonrigid.cpp
        registration/coherent_point_drift/bcg_coherent_point_drift_bayes.h registration/coherent_point_drift/bcg_coherent_point_drift_bayes.cpp
        registration/coherent_point_drift/bcg_coherent_point_drift_test.h registration/coherent_point_drift/bcg_coherent_point_drift_test.cpp
        ${RPLY_SOURCE_DIRS}/rply.c
        )

target_compile_features(bcg_graphics PRIVATE cxx_std_17)
set_target_properties(bcg_graphics PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(bcg_graphics PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC ${RPLY_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC ${ENTT_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC ${NANOFLANN_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC ${SPECTRA_INCLUDE_DIRS})
target_include_directories(bcg_graphics PUBLIC . ./geometry)

target_link_libraries(bcg_graphics ${OPENGL_gl_LIBRARY} tbb)

if (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wno-shorten-64-to-32 -Wno-c++20-extensions -Wno-deprecated-copy -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wpedantic -Wextra -Wno-missing-field-initializers -g -ggdb -fprofile-instr-generate -fcoverage-mapping -fstandalone-debug -O0)
    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
    target_link_libraries(bcg_graphics Threads::Threads stdc++fs)
    #target_include_directories(bcg_graphics PUBLIC /usr/local/cuda-10.1/targets/x86_64-linux/include) // cuda support
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
    target_compile_definitions(bcg_graphics PUBLIC NOMINMAX)
endif (MSVC)
