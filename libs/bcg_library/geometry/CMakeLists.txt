target_sources(bcg_graphics PRIVATE
        bcg_neighbors_query.h
        bcg_property.h
        bcg_property_eigen_trait.h
        bcg_property_map_eigen.h
        bcg_property_utils.h bcg_property_utils.cpp)

add_subdirectory(aligned_box)
add_subdirectory(correspondences)
add_subdirectory(curve)
add_subdirectory(distance_query)
add_subdirectory(graph)
add_subdirectory(grid)
add_subdirectory(intersect)
add_subdirectory(kdtree)
add_subdirectory(marching_cubes)
add_subdirectory(mesh)
add_subdirectory(octree)
add_subdirectory(parameterized_plane)
add_subdirectory(plane)
add_subdirectory(point_cloud)
add_subdirectory(quad)
add_subdirectory(quadric)
add_subdirectory(sampling)
add_subdirectory(segment)
add_subdirectory(sphere)
add_subdirectory(triangle)