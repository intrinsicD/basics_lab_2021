target_sources(bcg_opengl PRIVATE
        bcg_attribute.h bcg_material.h
        bcg_render_system.h bcg_render_system.cpp
        bcg_renderer.h bcg_renderer.cpp
        bcg_vectorfields.h)

add_subdirectory(curve_renderer)
add_subdirectory(deferred_renderer)
add_subdirectory(graph_renderer)
add_subdirectory(mesh_renderer)
add_subdirectory(picking_renderer)
add_subdirectory(points_renderer)
add_subdirectory(vectorfield_renderer)
add_subdirectory(point_rendering)