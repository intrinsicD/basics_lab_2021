find_package(Eigen3 REQUIRED PATHS external/eigen)
target_link_libraries(bcg_graphics PUBLIC Eigen3::Eigen)

target_sources(bcg_graphics PRIVATE
        bcg_vector.h
        bcg_vector_angle.h
        bcg_vector_cos.h
        bcg_vector_sin.h
        bcg_vector_map_eigen.h
        bcg_vector_median_filter.h
        bcg_vector_median_filter_directional.h
        bcg_vector_median_filter_fuzzy.h)
