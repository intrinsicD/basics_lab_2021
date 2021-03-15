//
// Created by alex on 22.01.21.
//

#include "bcg_kernel_functions.h"

namespace bcg{

std::vector<std::string> kernel_function_names(){
    std::vector<std::string> names(static_cast<int>(KernelFunctionType::__last__));
    names[static_cast<int>(KernelFunctionType ::linear)] = "linear";
    names[static_cast<int>(KernelFunctionType ::polynomial)] = "polynomial";
    names[static_cast<int>(KernelFunctionType ::gaussian)] = "gaussian";
    names[static_cast<int>(KernelFunctionType ::exponential)] = "exponential";
    names[static_cast<int>(KernelFunctionType ::laplacian)] = "laplacian";
    names[static_cast<int>(KernelFunctionType ::anova)] = "anova";
    return names;
}

bcg_scalar_t linear_kernel(bcg_scalar_t x){
    return x;
}

bcg_scalar_t polynomial_kernel(bcg_scalar_t x, int degree, bcg_scalar_t constant){
    return std::pow(x + constant, degree);
}

bcg_scalar_t gaussian_kernel(bcg_scalar_t x, bcg_scalar_t sigma_squared){
    return exponential_kernel(x * x, sigma_squared);
}

bcg_scalar_t exponential_kernel(bcg_scalar_t x, bcg_scalar_t sigma_squared){
    return std::exp(-std::abs(x) / (2 * sigma_squared));
}

bcg_scalar_t laplacian_kernel(bcg_scalar_t x, bcg_scalar_t sigma){
    return std::exp(-std::abs(x) / sigma);
}

bcg_scalar_t anova_kernel(bcg_scalar_t x, bcg_scalar_t y, int n, bcg_scalar_t sigma, int degree){
    bcg_scalar_t value = 0;
    for(size_t k = 0; k < n; ++k){
        value += std::pow(std::exp(-sigma * std::pow(x - y, 2)), degree);
        x *= x;
        y *= y;
    }
    return value;
}

}