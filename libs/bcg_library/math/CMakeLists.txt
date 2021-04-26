target_sources(bcg_graphics PRIVATE
        bcg_bernstein_basis.h bcg_bernstein_basis.cpp
        bcg_binomial_coefficient.h bcg_binomial_coefficient.cpp
        bcg_factorial.h bcg_factorial.cpp
        bcg_integration.h bcg_integration.cpp
        bcg_kernel_functions.h bcg_kernel_functions.cpp
        bcg_linalg.h
        bcg_math_common.h
        bcg_math_safe_tigonometric_functions.h
        bcg_monomial_basis.h
        bcg_monomial_basis.cpp
        bcg_pca.h
        bcg_polynomial.h bcg_polynomial.cpp
        bcg_robust_pca.h
        bcg_vertex_classify_curvature.h bcg_vertex_classify_curvature.cpp)



add_subdirectory(vector)
add_subdirectory(statistics)
add_subdirectory(sparse_matrix)
add_subdirectory(rotations)
add_subdirectory(matrix)
add_subdirectory(laplacian)