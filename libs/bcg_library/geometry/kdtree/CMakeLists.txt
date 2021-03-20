#find_package(nanoflann REQUIRED PATHS ${CMAKE_SOURCE_DIR}/external/nanoflann)
find_package(nanoflann REQUIRED)
target_sources(bcg_graphics PRIVATE
        bcg_kdtree.h
        bcg_triangle_kdtree.h bcg_triangle_kdtree.cpp
        )


target_include_directories(bcg_graphics PUBLIC "${NANOFLANN_INCLUDE_DIRS}")
#target_link_libraries(bcg_graphics PUBLIC nanoflann)
