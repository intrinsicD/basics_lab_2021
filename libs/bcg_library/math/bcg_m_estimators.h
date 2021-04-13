//
// Created by alex on 13.04.21.
//

#ifndef BCG_GRAPHICS_BCG_M_ESTIMATORS_H
#define BCG_GRAPHICS_BCG_M_ESTIMATORS_H

#include "bcg_math_common.h"

namespace bcg {
/*  M-Estimators
 *  (from paper: Surface Denoising based on Normal Filtering in a Robust Statistics Framework (2020)):
 *
 *      Error functions rho(x) or rho(x, sigma)
 *      Influence functions psi(x) = rho'(x) or psi(x, sigma) = rho'(x, sigma)
 *      weighting functions w(x) = psi(x) / x or w(x, sigma) = psi(x, sigma) / x
 *      where x is some distance measure ans sigma a user parameter
 *          - Tipp: pick sigma values depending on noise level, 0.1 - 0.4 * average_edge_length
 * */

inline bcg_scalar_t l2_norm(bcg_scalar_t x) {
    return x * x;
}

inline bcg_scalar_t truncated_l2_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < std::sqrt(sigma) ? l2_norm(x) : sigma);
}

inline bcg_scalar_t l1_norm(bcg_scalar_t x) {
    return std::abs(x);
}

inline bcg_scalar_t truncated_l1_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < sigma ? l1_norm(x) : sigma);
}

inline bcg_scalar_t huber_minimax_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < sigma ? (l2_norm(x) / (2.0 * sigma) + sigma / 2.0) : std::abs(x));
}

inline bcg_scalar_t lorentzian_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return std::log(1.0 + (x * x / (sigma * sigma)) / 2.0);
}

inline bcg_scalar_t gaussian_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return 1.0 - std::exp(-x * x / (sigma * sigma));
}

inline bcg_scalar_t tuckeys_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    if(std::abs(x) < sigma){
        bcg_scalar_t x_2 = x * x;
        bcg_scalar_t x_4 = x_2 * x_2;
        bcg_scalar_t x_6 = x_4 * x_2;
        bcg_scalar_t sigma_2 = sigma * sigma;
        bcg_scalar_t sigma_4 = sigma_2 * sigma_2;
        bcg_scalar_t sigma_6 = sigma_4 * sigma_2;
        return x_2 / sigma_2 - x_4 / sigma_4 + x_6 / (2.0 * sigma_6);
    }
    return 1.0 / 3.0;
}

namespace influence_function{
//error function derivative
inline bcg_scalar_t l2_norm(bcg_scalar_t x) {
    return x / 2.0;
}

inline bcg_scalar_t truncated_l2_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < std::sqrt(sigma) ? influence_function::l2_norm(x) : 0.0);
}

inline bcg_scalar_t l1_norm(bcg_scalar_t ) {
    return 1.0;
}

inline bcg_scalar_t truncated_l1_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < sigma ? l1_norm(x) : 0.0);
}

inline bcg_scalar_t huber_minimax_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < sigma ? x / sigma : 1.0);
}

inline bcg_scalar_t lorentzian_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return x / (sigma * sigma + x * x / 2.0);
}

inline bcg_scalar_t gaussian_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    bcg_scalar_t sigma_2 = sigma * sigma;
    return 2.0 * x * std::exp(-x * x / (sigma_2)) / sigma_2 ;
}

inline bcg_scalar_t tuckeys_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    if(std::abs(x) < sigma){
        bcg_scalar_t x_2 = x * x;
        bcg_scalar_t x_3 = x_2 * x;
        bcg_scalar_t x_5 = x_3 * x_2;
        bcg_scalar_t sigma_2 = sigma * sigma;
        bcg_scalar_t sigma_4 = sigma_2 * sigma_2;
        bcg_scalar_t sigma_6 = sigma_4 * sigma_2;
        return 2.0 * x / sigma_2 - 4.0 * x_3 / sigma_4 + 2.0 * x_5 / sigma_6;
    }
    return 0.0;
}

}

namespace weighting_function{
// influence function divided by x
inline bcg_scalar_t l2_norm(bcg_scalar_t) {
    return 0.5;
}

inline bcg_scalar_t truncated_l2_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < std::sqrt(sigma) ? influence_function::l2_norm(x) : 0.0);
}

inline bcg_scalar_t l1_norm(bcg_scalar_t x) {
    return 1.0 / x;
}

inline bcg_scalar_t truncated_l1_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < sigma ? l1_norm(x) : 0.0);
}

inline bcg_scalar_t huber_minimax_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return (std::abs(x) < sigma ? 1.0 / sigma : 1.0 / x);
}

inline bcg_scalar_t lorentzian_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    return 1.0 / (sigma * sigma + x * x / 2.0);
}

inline bcg_scalar_t gaussian_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    bcg_scalar_t sigma_2 = sigma * sigma;
    return 2.0 * std::exp(-x * x / (sigma_2)) / sigma_2 ;
}

inline bcg_scalar_t tuckeys_norm(bcg_scalar_t x, bcg_scalar_t sigma) {
    if(std::abs(x) < sigma){
        bcg_scalar_t x_2 = x * x;
        bcg_scalar_t x_4 = x_2 * x_2;
        bcg_scalar_t sigma_2 = sigma * sigma;
        bcg_scalar_t sigma_4 = sigma_2 * sigma_2;
        bcg_scalar_t sigma_6 = sigma_4 * sigma_2;
        return 2.0 / sigma_2 - 4.0 * x_2 / sigma_4 + 2.0 * x_4 / sigma_6;
    }
    return 0.0;
}
}

}

#endif //BCG_GRAPHICS_BCG_M_ESTIMATORS_H
