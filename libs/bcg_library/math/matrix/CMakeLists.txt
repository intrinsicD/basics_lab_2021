target_link_libraries(bcg_graphics PUBLIC Eigen3::Eigen)
target_link_libraries(bcg_graphics PUBLIC Spectra)

target_sources(bcg_graphics PRIVATE
        bcg_matrix.h
        bcg_matrix_anti_symmetric_part.h
        bcg_matrix_check_finite.h
        bcg_matrix_covariance.h
        bcg_matrix_elementwise_median.h
        bcg_matrix_horizontal_stack.h
        bcg_matrix_kernel.h
        bcg_matrix_map_eigen.h
        bcg_matrix_pairwise_distances.h
        bcg_matrix_vertical_stack.h)
