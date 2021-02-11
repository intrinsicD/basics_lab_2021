//
// Created by alex on 04.02.21.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_KERNEL_H
#define BCG_GRAPHICS_BCG_MATRIX_KERNEL_H

#include <random>
#include <iostream>
#include "bcg_matrix_pairwise_distances.h"
#include "Spectra/SymEigsSolver.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "geometry/grid/bcg_occupancy_grid.h"
#include "geometry/sampling/bcg_sampling_grid.h"

namespace bcg {

template<typename T>
inline Matrix<T, -1, -1>
compute_gaussian_kernel(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B, T normalizer /*=2*beta*beta*/) {
    return (-pairwise_distance_squared(A, B) / normalizer).array().exp();
}

template<typename T>
Matrix<T, -1, -1>
compute_laplace_kernel(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B, T normalizer /*=beta*/) {
    return (-(pairwise_distance(A, B) / normalizer)).array().exp();
}

template<typename T>
Matrix<T, -1, -1>
compute_rational_quadric_kernel(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B, T normalizer /*=beta*beta*/) {
    Matrix<T, -1, -1> D = pairwise_distance_squared(A, B);
    return 1.0 - D.array() / (D.array() + normalizer);
}

template<typename T>
Matrix<T, -1, -1> compute_inverse_multiquadric_kernel(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B,
                                                      T normalizer /*=beta*beta*/) {
    Matrix<T, -1, -1> D = pairwise_distance_squared(A, B);
    return 1.0 / (D.array() + normalizer).sqrt();
}

enum class KernelType {
    gaussian,
    laplace,
    rational_quadric,
    inverse_multiquadric,
    __last__
};

inline std::vector<std::string> kernel_type_names() {
    std::vector<std::string> names(static_cast<int>(KernelType::__last__));
    names[static_cast<int>(KernelType::gaussian)] = "gaussian";
    names[static_cast<int>(KernelType::laplace)] = "laplace";
    names[static_cast<int>(KernelType::rational_quadric)] = "rational_quadric";
    names[static_cast<int>(KernelType::inverse_multiquadric)] = "inverse_multiquadric";
    return names;
}

enum class SamplingType {
    uniform,
    grid_first,
    grid_last,
    grid_closest,
    grid_mean,
    __last__
};

inline std::vector<std::string> sampling_type_names() {
    std::vector<std::string> names(static_cast<int>(SamplingType::__last__));
    names[static_cast<int>(SamplingType::uniform)] = "uniform";
    names[static_cast<int>(SamplingType::grid_first)] = "grid_first";
    names[static_cast<int>(SamplingType::grid_last)] = "grid_last";
    names[static_cast<int>(SamplingType::grid_closest)] = "grid_closest";
    names[static_cast<int>(SamplingType::grid_mean)] = "grid_mean";
    return names;
}

template<typename T>
struct kernel_matrix {
    KernelType kernel_type;

    SamplingType sampling_type;

    bool use_eigen_decomposition = false;
    bool use_nyström_approximation = false;
    T two_sigma_squared;
    Matrix<T, -1, -1> K_AV, K_VV_INV, K_BV, VV, Evecs;
    Vector<T, -1> Evals;
    std::vector<size_t> sampled_indices;

    Matrix<T, -1, -1> compute_kernel(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
        switch (kernel_type) {
            case KernelType::gaussian: {
                return compute_gaussian_kernel(A, B, two_sigma_squared);
            }
            case KernelType::laplace: {
                return compute_laplace_kernel(A, B, std::sqrt(two_sigma_squared / 2));
            }
            case KernelType::rational_quadric: {
                return compute_rational_quadric_kernel(A, B, two_sigma_squared / 2);
            }
            case KernelType::inverse_multiquadric: {
                return compute_inverse_multiquadric_kernel(A, B, two_sigma_squared / 2);
            }
            case KernelType::__last__: {
                return Matrix<T, -1, -1>();
            }
        }
    }

    void sample(size_t num_samples, const Matrix<T, -1, -1> &A) {
        switch (sampling_type) {
            case SamplingType::uniform: {
                num_samples = std::min<size_t>(num_samples, A.rows());

                std::vector<size_t> indices_union(A.rows());
                std::iota(indices_union.begin(), indices_union.end(), 0);

                std::sample(indices_union.begin(), indices_union.end(), std::back_inserter(sampled_indices),
                            num_samples, std::mt19937{std::random_device{}()});

                VV.resize(num_samples, A.cols());

                for (size_t i = 0; i < num_samples; ++i) {
                    VV.row(i) = A.row(sampled_indices[i]);
                }
            }
            case SamplingType::grid_first: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                sample_first_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                sampled_indices = grid.get_occupied_samples_indices();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::grid_last: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                sample_last_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                sampled_indices = grid.get_occupied_samples_indices();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::grid_closest: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                sample_closest_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                sampled_indices = grid.get_occupied_samples_indices();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::grid_mean: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                sample_mean_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::__last__: {
                break;
            }
        }
    }

    void sample(size_t num_samples, const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
        switch (sampling_type) {
            case SamplingType::uniform: {
                num_samples = std::min<size_t>(num_samples, A.rows());
                std::vector<size_t> indices_union(A.rows() + B.rows());
                std::iota(indices_union.begin(), indices_union.end(), 0);

                std::sample(indices_union.begin(), indices_union.end(), std::back_inserter(sampled_indices),
                            num_samples, std::mt19937{std::random_device{}()});

                VV.resize(num_samples, A.cols());

                for (size_t i = 0; i < num_samples; ++i) {
                    if (sampled_indices[i] < A.rows()) {
                        VV.row(i) = A.row(sampled_indices[i]);
                    } else {
                        VV.row(i) = B.row(sampled_indices[i] - A.rows());
                    }
                }
            }
            case SamplingType::grid_first: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows() + B.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                for (size_t i = 0; i < B.rows(); ++i) {
                    aabb.grow(B.row(i).template cast<bcg_scalar_t>());
                    Union[i + A.rows()] = B.row(i).template cast<bcg_scalar_t>();
                }
                sample_first_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::grid_last: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows() + B.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                for (size_t i = 0; i < B.rows(); ++i) {
                    aabb.grow(B.row(i).template cast<bcg_scalar_t>());
                    Union[i + A.rows()] = B.row(i).template cast<bcg_scalar_t>();
                }
                sample_last_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::grid_closest: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows() + B.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                for (size_t i = 0; i < B.rows(); ++i) {
                    aabb.grow(B.row(i).template cast<bcg_scalar_t>());
                    Union[i + A.rows()] = B.row(i).template cast<bcg_scalar_t>();
                }
                sample_closest_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::grid_mean: {
                aligned_box3 aabb;
                std::vector<VectorS<3>> Union(A.rows() + B.rows());
                for (size_t i = 0; i < A.rows(); ++i) {
                    aabb.grow(A.row(i).template cast<bcg_scalar_t>());
                    Union[i] = A.row(i).template cast<bcg_scalar_t>();
                }
                for (size_t i = 0; i < B.rows(); ++i) {
                    aabb.grow(B.row(i).template cast<bcg_scalar_t>());
                    Union[i + A.rows()] = B.row(i).template cast<bcg_scalar_t>();
                }
                sample_mean_grid grid({num_samples, num_samples, num_samples}, aabb);
                grid.build(Union);

                auto V = grid.get_occupied_sample_points();
                VV.resize(V.size(), A.cols());
                for (size_t i = 0; i < V.size(); ++i) {
                    VV.row(i) = V[i].template cast<T>();
                }
            }
            case SamplingType::__last__ : {
                break;
            }
        }
    }

    T approximation_error(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
        return (compute_kernel(A, B) - K_AV * K_VV_INV * K_BV.transpose()).norm();
    }

    void compute_nyström_approximation(const Matrix<T, -1, -1> &A, int num_samples) {
        use_nyström_approximation = true;
        sample(num_samples, A);
        K_AV = compute_kernel(A, VV);
        K_VV_INV = compute_kernel(VV, VV).inverse();
        K_BV = K_AV;
    }

    void compute_nyström_approximation(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B, int num_samples) {
        use_nyström_approximation = true;
        sample(num_samples, A, B);
        K_AV = compute_kernel(A, VV);
        K_VV_INV = compute_kernel(VV, VV).inverse();
        K_BV = compute_kernel(B, VV);
    }

    void compute_nyström_eigen_approximation(const Matrix<T, -1, -1> &A, int num_samples, int num_evals) {
        use_nyström_approximation = true;
        sample(num_samples, A);
        K_AV = compute_kernel(A, VV);
        Matrix<T, -1, -1> K_VV = compute_kernel(VV, VV);

        int ncv = std::min<int>(A.rows(), 2 * num_evals);
        Spectra::DenseSymMatProd<T> op(K_VV);
        Spectra::SymEigsSolver<T, Spectra::LARGEST_MAGN, Spectra::DenseSymMatProd<T> > eigs(&op, num_evals, ncv);
        eigs.init();
        int nconv = eigs.compute();
        if (eigs.info() == Spectra::SUCCESSFUL) {
            use_eigen_decomposition = true;
            Evals = eigs.eigenvalues();
            Evecs = eigs.eigenvectors();
            std::cout << "#Eigenvalues converged:" << nconv << std::endl;
        } else {
            use_eigen_decomposition = false;
            std::cout << "#Eigenvalues not converged:" << nconv << std::endl;
            K_VV_INV = K_VV.inverse();
        }
        K_BV = K_AV;
    }


    void compute_eigen_approximation(const Matrix<T, -1, -1> &A, int num_evals) {
        use_nyström_approximation = false;
        Matrix<T, -1, -1> K_VV = compute_kernel(A, A);
        int ncv = std::min<int>(A.rows(), 2 * num_evals);
        Spectra::DenseSymMatProd<T> op(K_VV);
        Spectra::SymEigsSolver<T, Spectra::LARGEST_MAGN, Spectra::DenseSymMatProd<T> > eigs(&op, num_evals, ncv);
        eigs.init();
        int nconv = eigs.compute();
        if (eigs.info() == Spectra::SUCCESSFUL) {
            use_eigen_decomposition = true;
            Evals = eigs.eigenvalues();
            Evecs = eigs.eigenvectors();
            std::cout << "#Eigenvalues converged:" << nconv << std::endl;
        } else {
            use_eigen_decomposition = false;
            std::cout << "#Eigenvalues not converged:" << nconv << std::endl;
        }
    }


};

}

#endif //BCG_GRAPHICS_BCG_MATRIX_KERNEL_H
