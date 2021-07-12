add_subdirectory(bcg_library)
add_subdirectory(${CMAKE_SOURCE_DIR}/spike/graphics ${CMAKE_CURRENT_BINARY_DIR}/spike/graphics)


if (BCG_OPENGL)
    add_subdirectory(bcg_opengl)
    add_subdirectory(${CMAKE_SOURCE_DIR}/spike/opengl ${CMAKE_CURRENT_BINARY_DIR}/spike/opengl)
endif ()

