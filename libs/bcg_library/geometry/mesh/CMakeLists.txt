find_package(rply REQUIRED)

target_sources(bcg_graphics PRIVATE
        bcg_mesh.h bcg_mesh.cpp
        bcg_mesh_boundary.h bcg_mesh_boundary.cpp
        bcg_mesh_connected_components.h bcg_mesh_connected_components.cpp
        bcg_mesh_covmesh.h bcg_mesh_covmesh.cpp
        bcg_mesh_curvature_taubin.h bcg_mesh_curvature_taubin.cpp
        bcg_mesh_dec.h bcg_mesh_dec.cpp
        bcg_mesh_edge_cotan.h bcg_mesh_edge_cotan.cpp
        bcg_mesh_edge_dihedral_angle.h bcg_mesh_edge_dihedral_angle.cpp
        bcg_mesh_edge_fujiwara.h bcg_mesh_edge_fujiwara.cpp
        bcg_mesh_edge_normals.h bcg_mesh_edge_normals.cpp
        bcg_mesh_face_area_vector.h bcg_mesh_face_area_vector.cpp
        bcg_mesh_face_areas.h bcg_mesh_face_areas.cpp
        bcg_mesh_face_centers.h bcg_mesh_face_centers.cpp
        bcg_mesh_face_gradient.h bcg_mesh_face_gradient.cpp
        bcg_mesh_face_normals.h bcg_mesh_face_normals.cpp
        bcg_mesh_factory.h bcg_mesh_factory.cpp
        bcg_mesh_features.h bcg_mesh_features.cpp
        bcg_mesh_laplacian.h bcg_mesh_laplacian.cpp
        bcg_mesh_normal_filtering_robust_statistics.h bcg_mesh_normal_filtering_robust_statistics.cpp
        bcg_mesh_remeshing.h bcg_mesh_remeshing.cpp
        bcg_mesh_simplification.h bcg_mesh_simplification.cpp
        bcg_mesh_smoothing.h bcg_mesh_smoothing.cpp
        bcg_mesh_statistics.h bcg_mesh_statistics.cpp
        bcg_mesh_subdivision.h bcg_mesh_subdivision.cpp
        bcg_mesh_surface_area.h bcg_mesh_surface_area.cpp
        bcg_mesh_triangle_area_from_metric.h bcg_mesh_triangle_area_from_metric.cpp
        bcg_mesh_triangle_cotan_weight.h bcg_mesh_triangle_cotan_weight.cpp
        bcg_mesh_vertex_area_barycentric.h bcg_mesh_vertex_area_barycentric.cpp
        bcg_mesh_vertex_area_voronoi.h bcg_mesh_vertex_area_voronoi.cpp
        bcg_mesh_vertex_convex_concave.h bcg_mesh_vertex_convex_concave.cpp
        bcg_mesh_vertex_cotan.h bcg_mesh_vertex_cotan.cpp
        bcg_mesh_vertex_fujiwara.h
        bcg_mesh_vertex_normals.h bcg_mesh_vertex_normals.cpp
        bcg_mesh_vertex_valences.h bcg_mesh_vertex_valences.cpp
        bcg_meshio.h bcg_meshio.cpp
        ${RPLY_SOURCE_DIRS}/rply.c)

target_include_directories(bcg_graphics PUBLIC ${RPLY_INCLUDE_DIRS})