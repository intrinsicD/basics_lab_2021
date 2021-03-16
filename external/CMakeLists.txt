set(BUILD_TESTING OFF CACHE BOOL "Enable creation of Eigen tests.") # You don't want this.
add_subdirectory(eigen)

add_subdirectory(spectra)
add_subdirectory(entt)

add_subdirectory(nanoflann)

if(BCG_OPENGL)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_INSTALL OFF)
    add_subdirectory(glfw)
endif(BCG_OPENGL)

if(BCG_TESTS)
    add_subdirectory(googletest)
    include_directories(googletest/googletest/include)
endif(BCG_TESTS)
