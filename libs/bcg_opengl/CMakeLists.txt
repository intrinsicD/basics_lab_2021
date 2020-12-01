if (BCG_OPENGL)
    add_definitions(-DBCG_OPENGL)
    find_package(OpenGL REQUIRED)
    find_package(glad REQUIRED)
    find_package(entt REQUIRED)
    find_package(imgui REQUIRED)
    find_package(rply REQUIRED)
    find_package(nanoflann REQUIRED)

    add_library(bcg_opengl
            bcg_imgui.h bcg_imgui.cpp
            bcg_opengl.h bcg_opengl.cpp
            bcg_camera.h bcg_camera.cpp bcg_projection_matrix.h bcg_projection_matrix.cpp
            bcg_viewer.h bcg_viewer.cpp
            renderers/bcg_material.h
            bcg_viewer_state.h bcg_viewer_state.cpp
            guis/bcg_gui_viewer_state.h guis/bcg_gui_viewer_state.cpp
            guis/bcg_gui_viewer_colors.h guis/bcg_gui_viewer_colors.cpp
            guis/bcg_gui_viewer_window.h guis/bcg_gui_viewer_window.cpp
            guis/bcg_gui_viewer_keyboard.h guis/bcg_gui_viewer_keyboard.cpp
            guis/bcg_gui_viewer_mouse.h guis/bcg_gui_viewer_mouse.cpp
            guis/bcg_gui_viewer_time.h guis/bcg_gui_viewer_time.cpp
            guis/bcg_gui_viewer_gui.h guis/bcg_gui_viewer_gui.cpp
            guis/bcg_gui_viewer_picker.h guis/bcg_gui_viewer_picker.cpp
            guis/bcg_gui_viewer_camera.h guis/bcg_gui_viewer_camera.cpp
            guis/bcg_gui_viewer_curve.h guis/bcg_gui_viewer_curve.cpp
            guis/bcg_gui_vectorfields.h guis/bcg_gui_vectorfields.cpp
            guis/bcg_gui_transform.h guis/bcg_gui_transform.cpp
            guis/bcg_gui_rendering_options.h guis/bcg_gui_rendering_options.cpp
            guis/bcg_gui_property_selector.h guis/bcg_gui_property_selector.cpp
            guis/bcg_gui_property_container_selector.h guis/bcg_gui_property_container_selector.cpp
            guis/bcg_gui_point_cloud.h guis/bcg_gui_point_cloud.cpp
            guis/bcg_gui_point_cloud_graph_builder.h guis/bcg_gui_point_cloud_graph_builder.cpp
            guis/bcg_gui_point_cloud_vertex_pca.h guis/bcg_gui_point_cloud_vertex_pca.cpp
            guis/bcg_gui_point_cloud_curvature_taubin.h guis/bcg_gui_point_cloud_curvature_taubin.cpp
            guis/bcg_gui_graph_vertex_pca.h guis/bcg_gui_graph_vertex_pca.cpp
            guis/bcg_gui_ogl_shape.h guis/bcg_gui_ogl_shape.cpp
            guis/bcg_gui_ogl_buffer.h guis/bcg_gui_ogl_buffer.cpp
            guis/bcg_gui_mesh.h guis/bcg_gui_mesh.cpp
            guis/bcg_gui_mesh_face_normals.h guis/bcg_gui_mesh_face_normals.cpp
            guis/bcg_gui_mesh_vertex_normals.h guis/bcg_gui_mesh_vertex_normals.cpp
            guis/bcg_gui_mesh_boundary.h guis/bcg_gui_mesh_boundary.cpp
            guis/bcg_gui_materials.h guis/bcg_gui_materials.cpp
            guis/bcg_gui_material_vectorfield.h guis/bcg_gui_material_vectorfield.cpp
            guis/bcg_gui_material_points.h guis/bcg_gui_material_points.cpp
            guis/bcg_gui_material_mesh.h guis/bcg_gui_material_mesh.cpp
            guis/bcg_gui_material_graph.h guis/bcg_gui_material_graph.cpp
            guis/bcg_gui_material_curve.h guis/bcg_gui_material_curve.cpp
            guis/bcg_gui_graph.h guis/bcg_gui_graph.cpp
            guis/bcg_gui_entity_info.h guis/bcg_gui_entity_info.cpp
            guis/bcg_gui_aligned_box3.h guis/bcg_gui_aligned_box3.cpp
            guis/bcg_gui_entity_hierarchy.h guis/bcg_gui_entity_hierarchy.cpp
            guis/bcg_gui_mesh_dihedral_angle.h guis/bcg_gui_mesh_dihedral_angle.cpp
            guis/bcg_gui_mesh_vertex_convex_concave.h guis/bcg_gui_mesh_vertex_convex_concave.cpp
            guis/bcg_gui_mesh_features.h guis/bcg_gui_mesh_features.cpp
            guis/bcg_gui_mesh_subdivision.h guis/bcg_gui_mesh_subdivision.cpp
            guis/bcg_gui_mesh_laplacian.h guis/bcg_gui_mesh_laplacian.cpp
            guis/bcg_gui_marching_cubes.h guis/bcg_gui_marching_cubes.cpp
            guis/bcg_gui_mesh_curvature_taubin.h guis/bcg_gui_mesh_curvature_taubin.cpp
            guis/bcg_gui_mesh_simplification.h guis/bcg_gui_mesh_simplification.cpp
            guis/bcg_gui_mesh_remeshing.h guis/bcg_gui_mesh_remeshing.cpp
            guis/bcg_gui_mesh_statistics.h guis/bcg_gui_mesh_statistics.cpp
            events/bcg_events.h
            systems/bcg_systems.h
            systems/bcg_camera_system.h systems/bcg_camera_system.cpp
            systems/bcg_graph_system.h systems/bcg_graph_system.cpp
            systems/bcg_mesh_system.h systems/bcg_mesh_system.cpp
            systems/bcg_picker_system.h systems/bcg_picker_system.cpp
            systems/bcg_point_cloud_system.h systems/bcg_point_cloud_system.cpp
            systems/bcg_transform_system.h systems/bcg_transform_system.cpp
            systems/bcg_gpu_system.h systems/bcg_gpu_system.cpp
            systems/bcg_mouse_system.h systems/bcg_mouse_system.cpp
            systems/bcg_keyboard_system.h systems/bcg_keyboard_system.cpp
            systems/bcg_aligned_box_system.h systems/bcg_aligned_box_system.cpp
            systems/bcg_loading_system.h systems/bcg_loading_system.cpp
            systems/bcg_bezier_curve_system.h systems/bcg_bezier_curve_system.cpp
            systems/bcg_hierarchy_system.h systems/bcg_hierarchy_system.cpp
            systems/bcg_spatial_index_system.h systems/bcg_spatial_index_system.cpp
            renderers/bcg_renderer.h renderers/bcg_renderer.cpp
            renderers/bcg_render_system.h renderers/bcg_render_system.cpp
            renderers/points_renderer/bcg_material_points.h renderers/points_renderer/bcg_points_renderer.h renderers/points_renderer/bcg_points_renderer.cpp renderers/points_renderer/bcg_events_points_renderer.h
            renderers/mesh_renderer/bcg_material_mesh.h renderers/mesh_renderer/bcg_mesh_renderer.h renderers/mesh_renderer/bcg_mesh_renderer.cpp renderers/mesh_renderer/bcg_events_mesh_renderer.h
            renderers/picking_renderer/bcg_material_picking.h renderers/picking_renderer/bcg_picking_renderer.h renderers/picking_renderer/bcg_picking_renderer.cpp renderers/picking_renderer/bcg_events_picking_renderer.h
            renderers/graph_renderer/bcg_material_graph.h renderers/graph_renderer/bcg_graph_renderer.h renderers/graph_renderer/bcg_graph_renderer.cpp renderers/graph_renderer/bcg_events_graph_renderer.h
            renderers/vectorfield_renderer/bcg_material_vectorfield.h renderers/vectorfield_renderer/bcg_vectorfield_renderer.h renderers/vectorfield_renderer/bcg_vectorfield_renderer.cpp renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h
            renderers/curve_renderer/bcg_material_curve.h renderers/curve_renderer/bcg_curve_renderer.h renderers/curve_renderer/bcg_curve_renderer.cpp renderers/curve_renderer/bcg_events_curve_renderer.h
            ${GLAD_SOURCE_DIRS}/glad.c
            ${IMGUI_SOURCES}
            )

    target_include_directories(bcg_opengl PRIVATE .)
    target_include_directories(bcg_opengl PUBLIC ${CMAKE_SOURCE_DIR}/libs)
    target_include_directories(bcg_opengl PUBLIC ${IMGUI_INCLUDE_DIRS})
    target_include_directories(bcg_opengl PUBLIC ${GLAD_INCLUDE_DIRS})
    target_include_directories(bcg_opengl PUBLIC ${ENTT_INCLUDE_DIRS})
    target_include_directories(bcg_opengl PUBLIC ${GLFW_INCLUDE_DIRS})
    target_include_directories(bcg_opengl PUBLIC ${NANOFLANN_INCLUDE_DIRS})
    target_compile_definitions(bcg_opengl PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
    set_target_properties(bcg_opengl PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
    target_compile_features(bcg_opengl PRIVATE cxx_std_17)

    target_link_libraries(bcg_opengl glfw ${OPENGL_gl_LIBRARY} bcg_graphics tbb stdc++fs)

    if (UNIX AND NOT APPLE)
        target_link_libraries(bcg_opengl dl)
        #target_compile_options(bcg_opengl PUBLIC -Wall -Wno-shorten-64-to-32 -Wno-c++20-extensions -Wno-deprecated-copy -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
        target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
        target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
    endif (UNIX AND NOT APPLE)
endif (BCG_OPENGL)

# warning flags
if (APPLE)
    #target_compile_options(bcg_opengl PUBLIC -Wall -Wno-shorten-64-to-32 -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
endif (APPLE)
if (MSVC)
    target_compile_options(bcg_opengl PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267 /wd4838)
    target_compile_options(bcg_opengl PUBLIC /EHsc)
endif (MSVC)
