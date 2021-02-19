//
// Created by alex on 19.02.21.
//

#ifndef BCG_GRAPHICS_BCG_PROPERTY_EIGEN_TRAIT_H
#define BCG_GRAPHICS_BCG_PROPERTY_EIGEN_TRAIT_H

#include "Eigen/Core"

namespace bcg{

template<typename Derived>
constexpr bool is_eigen_type_f(const Eigen::EigenBase<Derived> *) {
    return true;
}

constexpr bool is_eigen_type_f(const void *) {
    return false;
}

template<typename T>
constexpr bool is_eigen_type = is_eigen_type_f((T *) (nullptr));

}

#endif //BCG_GRAPHICS_BCG_PROPERTY_EIGEN_TRAIT_H
