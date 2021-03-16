if (BCG_OPENGL)
    find_package(OpenGL REQUIRED)

    add_library(bcg_opengl STATIC)

    target_include_directories(bcg_opengl PRIVATE .)
    target_include_directories(bcg_opengl PUBLIC ${CMAKE_SOURCE_DIR}/libs)

    target_link_libraries(bcg_opengl PUBLIC glfw3 bcg_graphics EnTT)

    add_subdirectory(events)
    add_subdirectory(systems)
    add_subdirectory(guis)
    add_subdirectory(renderers)
    add_subdirectory(viewer)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
        message("CMAKE_CXX_COMPILER_VERSION " ${CMAKE_CXX_COMPILER_VERSION})
        set_target_properties(bcg_opengl PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
    endif()

    target_compile_features(bcg_opengl PRIVATE cxx_std_17)

    #target_link_libraries(bcg_opengl PUBLIC glfw3 ${OPENGL_gl_LIBRARY} bcg_graphics tbb)


    if (UNIX AND NOT APPLE)
        target_link_libraries(bcg_opengl PUBLIC dl X11 stdc++fs)
        #target_compile_options(bcg_opengl PUBLIC -Wall -Wno-shorten-64-to-32 -Wno-c++20-extensions -Wno-deprecated-copy -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
        target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
        target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
    endif (UNIX AND NOT APPLE)
    # warning flags
    if (APPLE)
        #target_compile_options(bcg_opengl PUBLIC -Wall -Wno-shorten-64-to-32 -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
        target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
        target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
    endif (APPLE)
    if (MSVC)
        target_compile_options(bcg_opengl PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267 /wd4838)
        target_compile_options(bcg_opengl PUBLIC /EHsc)
        target_compile_definitions(bcg_opengl PUBLIC NOMINMAX)
    endif (MSVC)
endif (BCG_OPENGL)

