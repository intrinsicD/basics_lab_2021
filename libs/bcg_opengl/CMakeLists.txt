if (BCG_OPENGL)
    add_library(bcg_opengl STATIC)

    target_include_directories(bcg_opengl PRIVATE .)
    target_include_directories(bcg_opengl PUBLIC ${CMAKE_SOURCE_DIR}/libs)

    target_link_libraries(bcg_opengl PUBLIC bcg_graphics EnTT)

    add_subdirectory(events)
    add_subdirectory(systems)
    add_subdirectory(guis)
    add_subdirectory(renderers)
    add_subdirectory(viewer)
    add_subdirectory(orthodontic)
    add_subdirectory(components)

    target_compile_options(bcg_opengl PUBLIC "-ftime-trace")

    #target_link_libraries(bcg_opengl PUBLIC glfw3 ${OPENGL_gl_LIBRARY} bcg_graphics tbb)

    if (UNIX AND NOT APPLE)
        target_link_libraries(bcg_opengl PUBLIC dl X11)
    endif (UNIX AND NOT APPLE)
endif (BCG_OPENGL)

