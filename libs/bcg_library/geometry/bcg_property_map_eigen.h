//
// Created by alex on 10.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PROPERTY_MAP_EIGEN_H
#define BCG_GRAPHICS_BCG_PROPERTY_MAP_EIGEN_H

#include "../exts/eigen/Eigen/Core"
#include "bcg_property.h"
#include "bcg_library/math/bcg_linalg.h"

namespace bcg {

template<int L, typename T, qualifier Q>
constexpr bool is_glm_type_f(const typename glm::vec<L, T, Q> *) {
    return true;
}

constexpr bool is_glm_type_f(const void *) {
    return false;
}

template<typename T>
constexpr bool is_glm_type = is_glm_type_f((T *) (nullptr));

template<typename Derived>
constexpr bool is_eigen_type_f(const Eigen::EigenBase<Derived> *) {
    return true;
}

constexpr bool is_eigen_type_f(const void *) {
    return false;
}

template<typename T>
constexpr bool is_eigen_type = is_eigen_type_f((T *) (nullptr));

template<typename T>
constexpr bool is_fundamental_type = !is_eigen_type<T> && !is_glm_type<T>;

template<typename T>
inline Eigen::Map<Eigen::Matrix<T, -1, -1>, 0, Eigen::Stride<-1, -1>> Map(base_property *p) {
    return Eigen::Map<Eigen::Matrix<T, -1, -1>, 0, Eigen::Stride<-1, -1>>((T *) p->void_ptr(), p->size(), p->dims(),
                                                                          Eigen::Stride<-1, -1>(1, p->dims()));
}

template<typename T>
inline Eigen::Map<const Eigen::Matrix<T, -1, -1>, 0, Eigen::Stride<-1, -1>> MapConst(const base_property *p) {
    return Eigen::Map<const Eigen::Matrix<T, -1, -1>, 0, Eigen::Stride<-1, -1>>((const T *) p->void_ptr(), p->size(),
                                                                                p->dims(),
                                                                                Eigen::Stride<-1, -1>(1, p->dims()));
}

template<typename T, int N>
inline std::enable_if_t<is_eigen_type<T>, Eigen::Map<Eigen::Matrix<typename T::Scalar, -1, N>, 0, Eigen::Stride<-1, N>>>
Map(property <T, N> p) {
    return Eigen::Map<Eigen::Matrix<typename T::Scalar, -1, N>, 0, Eigen::Stride<-1, N>>(&p[0][0], p.size(),
                                                                                         p[0].size(),
                                                                                         Eigen::Stride<-1, N>(1,
                                                                                                              p[0].size()));
}

template<typename T, int N>
inline std::enable_if_t<is_eigen_type<T>, Eigen::Map<const Eigen::Matrix<typename T::Scalar, -1, N>, 0, Eigen::Stride<-1, N>>>
MapConst(const property <T, N> p) {
    return Eigen::Map<const Eigen::Matrix<typename T::Scalar, -1, N>, 0, Eigen::Stride<-1, N>>(&p[0][0], p.size(),
                                                                                               p[0].size(),
                                                                                               Eigen::Stride<-1, N>(1,
                                                                                                                    p[0].size()));
}


template<typename T, int N>
inline std::enable_if_t<is_glm_type<T>, Eigen::Map<Eigen::Matrix<typename T::value_type, -1, N>, 0, Eigen::Stride<-1, N>>>
Map(property <T, N> p) {
    return Eigen::Map<Eigen::Matrix<typename T::value_type, -1, N>, 0, Eigen::Stride<-1, N>>(glm::value_ptr(p[0]), p.size(), N,
                                                                                             Eigen::Stride<-1, N>(1,
                                                                                                                  N));
}

template<typename T, int N>
inline std::enable_if_t<is_glm_type<T>, Eigen::Map<const Eigen::Matrix<typename T::value_type, -1, N>, 0, Eigen::Stride<-1, N>>>
MapConst(const property <T, N> p) {
    return Eigen::Map<const Eigen::Matrix<typename T::value_type, -1, N>, 0, Eigen::Stride<-1, N>>(glm::value_ptr(p[0]), p.size(),
                                                                                                   N,
                                                                                                   Eigen::Stride<-1, N>(
                                                                                                           1,
                                                                                                           N));
}

template<typename T, int N>
inline std::enable_if_t<is_fundamental_type<T>, Eigen::Map<Eigen::Matrix<T, -1, N>, 0, Eigen::Stride<-1, N>>>
Map(property <T, N> p) {
    return Eigen::Map<Eigen::Matrix<T, -1, N>, 0, Eigen::Stride<-1, N>>(&p[0], p.size(), N, Eigen::Stride<-1, N>(1, N));
}

template<typename T, int N>
inline std::enable_if_t<is_fundamental_type<T>, Eigen::Map<const Eigen::Matrix<T, -1, N>, 0, Eigen::Stride<-1, N>>>
MapConst(property <T, N> p) {
    return Eigen::Map<const Eigen::Matrix<T, -1, N>, 0, Eigen::Stride<-1, N>>(&p[0], p.size(), N,
                                                                              Eigen::Stride<-1, N>(1, N));
}

inline Eigen::Matrix<bool, -1, 1> Map(property<bool, 1> p) {
    Eigen::Matrix<bool, -1, 1> m(p.size(), 1);
    for (size_t i = 0; i < p.size(); ++i) {
        m(i, 0) = p[i];
    }
    return m;
}

inline Eigen::Matrix<bool, -1, 1> MapConst(property<bool, 1> p) {
    Eigen::Matrix<bool, -1, 1> m(p.size(), 1);
    for (size_t i = 0; i < p.size(); ++i) {
        m(i, 0) = p[i];
    }
    return m;
}

}

#endif //BCG_GRAPHICS_BCG_PROPERTY_MAP_EIGEN_H
