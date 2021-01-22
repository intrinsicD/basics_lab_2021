//
// Created by alex on 22.01.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_FUNCTIONS_H
#define BCG_GRAPHICS_BCG_KERNEL_FUNCTIONS_H

#include <vector>
#include <string>

namespace bcg {

enum class KernelFunctionType {
    linear,
    polynomial,
    gaussian,
    exponential,
    laplacian,
    anova,
    __last__
};

std::vector<std::string> kernel_function_names();

bcg_scalar_t linear_kernel(bcg_scalar_t x);

bcg_scalar_t polynomial_kernel(bcg_scalar_t x, int degree, bcg_scalar_t constant = 1);

bcg_scalar_t gaussian_kernel(bcg_scalar_t x, bcg_scalar_t sigma_squared);

bcg_scalar_t exponential_kernel(bcg_scalar_t x, bcg_scalar_t sigma_squared);

bcg_scalar_t laplacian_kernel(bcg_scalar_t x, bcg_scalar_t sigma);

bcg_scalar_t anova_kernel(bcg_scalar_t x, bcg_scalar_t y, int n, bcg_scalar_t sigma, int degree);


}

#endif //BCG_GRAPHICS_BCG_KERNEL_FUNCTIONS_H
