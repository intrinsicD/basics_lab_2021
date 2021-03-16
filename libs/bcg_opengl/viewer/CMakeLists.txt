find_package(glad REQUIRED)
find_package(imgui REQUIRED)

target_sources(bcg_opengl PRIVATE
        bcg_camera.h bcg_camera.cpp
        bcg_entity_hierarchy.h
        bcg_entity_info.h
        bcg_imgui.h bcg_imgui.cpp
        bcg_opengl.h bcg_opengl.cpp
        bcg_projection_matrix.h bcg_projection_matrix.cpp
        bcg_selection.h
        bcg_viewer.h bcg_viewer.cpp
        bcg_viewer_state.h bcg_viewer_state.cpp
        ${GLAD_SOURCE_DIRS}/glad.c
        ${IMGUI_SOURCES})

target_include_directories(bcg_opengl PUBLIC ${GLAD_INCLUDE_DIRS})
target_include_directories(bcg_opengl PUBLIC ${IMGUI_INCLUDE_DIRS})
target_compile_definitions(bcg_opengl PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLAD)