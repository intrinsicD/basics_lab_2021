if (BCG_OPENGL)
    add_definitions(-DBCG_OPENGL)
    find_package(OpenGL REQUIRED)

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
            renderers/test_renderer/bcg_material_test.h renderers/test_renderer/bcg_test_renderer.h renderers/test_renderer/bcg_test_renderer.cpp renderers/test_renderer/bcg_events_test_renderer.h
            exts/imgui/imgui.cpp
            exts/imgui/imgui_draw.cpp
            exts/imgui/imgui_widgets.cpp
            exts/imgui/examples/imgui_impl_glfw.cpp
            exts/imgui/examples/imgui_impl_opengl3.cpp
            exts/glad/glad.c)

    target_include_directories(bcg_opengl PRIVATE .)
    target_include_directories(bcg_opengl PUBLIC ${CMAKE_SOURCE_DIR}/libs)
    target_compile_definitions(bcg_opengl PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
    set_target_properties(bcg_opengl PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

    target_include_directories(bcg_opengl PUBLIC ${CMAKE_SOURCE_DIR}/exts/glfw/include)
    target_include_directories(bcg_opengl PUBLIC ./exts/imgui)
    target_include_directories(bcg_opengl PUBLIC ./exts)
    target_include_directories(bcg_opengl PUBLIC ${CMAKE_SOURCE_DIR}/exts/entt/single_include)
    target_link_libraries(bcg_opengl glfw ${OPENGL_gl_LIBRARY} bcg_graphics)

    if (UNIX AND NOT APPLE)
        target_link_libraries(bcg_opengl dl)
    endif (UNIX AND NOT APPLE)
endif (BCG_OPENGL)

# warning flags
if (APPLE)
    target_compile_options(bcg_opengl PUBLIC -Wall -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion)
endif (APPLE)
if (MSVC)
    target_compile_options(bcg_opengl PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267)
    target_compile_options(bcg_opengl PUBLIC /EHsc)
endif (MSVC)
