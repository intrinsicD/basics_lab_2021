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
            bcg_camera.h bcg_camera.cpp
            bcg_viewer.h bcg_viewer.cpp
            bcg_viewer_state.h bcg_viewer_state.cpp
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
            renderers/bcg_renderer.h renderers/bcg_renderer.cpp
            renderers/bcg_render_system.h renderers/bcg_render_system.cpp
            renderers/points_renderer/bcg_material_points.h renderers/points_renderer/bcg_points_renderer.h renderers/points_renderer/bcg_points_renderer.cpp renderers/points_renderer/bcg_events_points_renderer.h
            renderers/mesh_renderer/bcg_material_mesh.h renderers/mesh_renderer/bcg_mesh_renderer.h renderers/mesh_renderer/bcg_mesh_renderer.cpp renderers/mesh_renderer/bcg_events_mesh_renderer.h
            renderers/picking_renderer/bcg_material_picking.h renderers/picking_renderer/bcg_picking_renderer.h renderers/picking_renderer/bcg_picking_renderer.cpp renderers/picking_renderer/bcg_events_picking_renderer.h
            renderers/graph_renderer/bcg_material_graph.h renderers/graph_renderer/bcg_graph_renderer.h renderers/graph_renderer/bcg_graph_renderer.cpp renderers/graph_renderer/bcg_events_graph_renderer.h
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

    target_link_libraries(bcg_opengl glfw ${OPENGL_gl_LIBRARY} bcg_graphics tbb)

    if (UNIX AND NOT APPLE)
        target_link_libraries(bcg_opengl dl)
        target_compile_options(bcg_opengl PUBLIC -Wall -Wno-shorten-64-to-32 -Wno-c++20-extensions -Wno-deprecated-copy -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    endif (UNIX AND NOT APPLE)
endif (BCG_OPENGL)

# warning flags
if (APPLE)
    target_compile_options(bcg_opengl PUBLIC -Wall -Wno-shorten-64-to-32 -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
endif (APPLE)
if (MSVC)
    target_compile_options(bcg_opengl PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267)
    target_compile_options(bcg_opengl PUBLIC /EHsc)
endif (MSVC)
