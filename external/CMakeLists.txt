set(BUILD_TESTING OFF CACHE BOOL "Enable creation of Eigen tests.") # You don't want this.
add_subdirectory(eigen)

add_subdirectory(spectra)
add_subdirectory(entt)

if(BCG_OPENGL)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(glfw)
endif()

if(BCG_TESTS)
    add_subdirectory(googletest)
    include_directories(googletest/googletest/include)
endif()
