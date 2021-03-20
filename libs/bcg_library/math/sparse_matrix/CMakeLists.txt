target_link_libraries(bcg_graphics PUBLIC Eigen3::Eigen)
target_sources(bcg_graphics PRIVATE
        bcg_sparse_matrix.h
        bcg_sparse_check_finite.h bcg_sparse_check_finite.cpp
        bcg_sparse_check_hermetian.h bcg_sparse_check_hermetian.cpp
        bcg_sparse_check_symmetric.h bcg_sparse_check_symmetric.cpp
        bcg_sparse_horizontal_stack.h bcg_sparse_horizontal_stack.cpp
        bcg_sparse_vertical_stack.h bcg_sparse_vertical_stack.cpp)
