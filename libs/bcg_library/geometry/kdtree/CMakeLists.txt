find_package(nanoflann REQUIRED PATHS ${CMAKE_SOURCE_DIR}/external/nanoflann)

target_sources(bcg_graphics PRIVATE
        bcg_kdtree.h
        bcg_triangle_kdtree.h bcg_triangle_kdtree.cpp)

target_link_libraries(bcg_graphics PUBLIC nanoflann::nanoflann)