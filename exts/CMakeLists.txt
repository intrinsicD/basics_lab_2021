if (BCG_APPS)

    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
    add_subdirectory(glfw)

endif (BCG_APPS)

if(BCG_TESTS)
    add_subdirectory(googletest)
    include_directories(googletest/googletest/include)
endif(BCG_TESTS)
