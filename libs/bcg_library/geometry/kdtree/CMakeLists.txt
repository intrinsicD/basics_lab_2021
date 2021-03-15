find_package(nanoflann REQUIRED)

target_sources(bcg_graphics PRIVATE
        bcg_kdtree.h
        bcg_triangle_kdtree.h bcg_triangle_kdtree.cpp)

target_link_libraries(bcg_graphics PUBLIC nanoflann::nanoflann)