if (BCG_OPENGL)
    add_definitions(-DBCG_OPENGL)
    find_package(OpenGL REQUIRED)

    add_library(bcg_opengl
            bcg_imgui.h bcg_imgui.cpp
            bcg_opengl.h bcg_opengl.cpp
            bcg_opengl_yocto.h bcg_opengl_yocto.cpp
            bcg_viewer.h
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

    target_include_directories(bcg_opengl PRIVATE ${CMAKE_SOURCE_DIR}/exts/glfw/include ./exts/imgui ./exts)
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
