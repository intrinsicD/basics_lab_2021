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
#include "geometry/sampling/bcg_sampling_octree.h"

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

template<typename T>
struct kernel_matrix {
    enum class KernelFunction {
        gaussian,
        laplace,
        rational_quadric,
        inverse_multiquadric
    } kernel_function;

    occupancy_grid grid;
    sampling_octree samplingOctree;

    bool use_eigen_decomposition = false;
    bool use_nyström_approximation = false;
    T normalizer;
    Matrix<T, -1, -1> K_AV, K_VV_INV, K_BV, VV, Evecs;
    Vector<T, -1> Evals;

    Matrix<T, -1, -1> compute_kernel(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
        switch (kernel_function) {
            case KernelFunction::gaussian: {
                return compute_gaussian_kernel(A, B, normalizer);
            }
            case KernelFunction::laplace: {
                return compute_laplace_kernel(A, B, normalizer);
            }
            case KernelFunction::rational_quadric: {
                return compute_rational_quadric_kernel(A, B, normalizer);
            }
            case KernelFunction::inverse_multiquadric: {
                return compute_inverse_multiquadric_kernel(A, B, normalizer);
            }
        }
    }

    void sample_uniform_union(size_t num_samples, const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
        /*std::vector<size_t> sampled_union;   // or reserve space for N elements up front
        std::vector<size_t> indices_union(A.rows() + B.rows());
        std::iota(indices_union.begin(), indices_union.end(), 0);   // or reserve space for N elements up front

        std::sample(indices_union.begin(), indices_union.end(), std::back_inserter(sampled_union),
                    num_samples, std::mt19937{std::random_device{}()});

        VV.resize(num_samples, A.cols());

        for (size_t i = 0; i < num_samples; ++i) {
            if (sampled_union[i] < A.rows()) {
                VV.row(i) = A.row(sampled_union[i]);
            } else {
                VV.row(i) = B.row(sampled_union[i] - A.rows());
            }
        }

        grid.clear();
        grid.dims = {num_samples, num_samples, num_samples};
        grid.aabb = aligned_box3();
        for (size_t i = 0; i < A.rows(); ++i) {
            grid.aabb.grow(A.row(i).template cast<bcg_scalar_t>());
        }
        for (size_t i = 0; i < B.rows(); ++i) {
            grid.aabb.grow(B.row(i).template cast<bcg_scalar_t>());
        }*/

/*        VV.resize(num_samples * num_samples * num_samples, A.cols());
        size_t counter = 0;
        traverse(grid, [this, &counter](size_t i, occupancy_grid & grid){
            VV.row(counter) = grid.idx_to_voxel_center(i);
            ++counter;
            return true;
        });*/


        /*std::vector<Vector<T, -1>> V;
        for(size_t i = 0; i < A.rows(); ++i){
            if(grid.is_occupied_point(A.row(i).template cast<bcg_scalar_t >())){

            }else{
                V.push_back(A.row(i));
                grid.mark_occupied_point(A.row(i).template cast<bcg_scalar_t>());
            }
        }
        for(size_t i = 0; i < B.rows(); ++i){
            if(grid.is_occupied_point(B.row(i).template cast<bcg_scalar_t>())){

            }else{
                V.push_back(B.row(i));
                grid.mark_occupied_point(B.row(i).template cast<bcg_scalar_t>());
            }
        }

        VV.resize(V.size(), A.cols());
        for(size_t i = 0; i < VV.rows(); ++i){
            VV.row(i) = V[i];
        }*/

        samplingOctree.sampling_type = sampling_octree::SamplingType::closest;
        samplingOctree.aabb = aligned_box3();

        vertex_container vertices;
        auto points = vertices.template get_or_add<VectorS<3>, 3>("v_samples");
        for (size_t i = 0; i < A.rows(); ++i) {
            samplingOctree.aabb.grow(A.row(i).template cast<bcg_scalar_t>());
            points.vector().push_back(A.row(i).template cast<bcg_scalar_t>());
        }
        for (size_t i = 0; i < B.rows(); ++i) {
            samplingOctree.aabb.grow(B.row(i).template cast<bcg_scalar_t>());
            points.vector().push_back(B.row(i).template cast<bcg_scalar_t>());
        }

        samplingOctree.build(sampling_octree::SamplingType::closest, points, 1, 10);

        auto VF = samplingOctree.get_samples(num_samples);
        VV.resize(VF.size(), A.cols());
        for(size_t i = 0; i < VV.rows(); ++i){
            VV.row(i) = VF[i].cast<T>();
        }
        std::cout << VV << std::endl;


        /*for (size_t i = 0; i < A.rows(); ++i) {
            grid.mark_occupied_point(A.row(i).template cast<bcg_scalar_t>());
        }
        for (size_t i = 0; i < B.rows(); ++i) {
            grid.mark_occupied_point(B.row(i).template cast<bcg_scalar_t>());

        }
        size_t counter = 0;
        VV.resize(grid.num_occupied_voxel(), A.cols());
        traverse(grid, [this, &counter](size_t i, occupancy_grid &grid) {
            if (grid.is_occupied_idx(i)) {
                VV.row(counter) = grid.idx_to_voxel_center(i).template cast<T>();
                ++counter;
            }
            return true;
        });*/
    }

    void sample_uniform(size_t num_samples, const Matrix<T, -1, -1> &A) {
        std::vector<size_t> sampled_union;   // or reserve space for N elements up front
        std::vector<size_t> indices_union(A.rows());
        std::iota(indices_union.begin(), indices_union.end(), 0);   // or reserve space for N elements up front

        std::sample(indices_union.begin(), indices_union.end(), std::back_inserter(sampled_union),
                    num_samples, std::mt19937{std::random_device{}()});

        VV.resize(num_samples, A.cols());

        for (size_t i = 0; i < num_samples; ++i) {
            VV.row(i) = A.row(sampled_union[i]);
        }
    }

    T approximation_error(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
        return (compute_kernel(A, B) - K_AV * K_VV_INV * K_BV.transpose()).norm();
    }

    void compute_nyström_approximation(const Matrix<T, -1, -1> &A, int num_samples) {
        use_nyström_approximation = true;
        sample_uniform(num_samples, A);
        K_AV = compute_kernel(A, VV);
        K_VV_INV = compute_kernel(VV, VV).inverse();
        K_BV = K_AV;
    }

    void compute_nyström_approximation(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B, int num_samples) {
        use_nyström_approximation = true;
        sample_uniform_union(num_samples, A, B);
        K_AV = compute_kernel(A, VV);
        K_VV_INV = compute_kernel(VV, VV).inverse();
        K_BV = compute_kernel(B, VV);
    }

    void compute_nyström_eigen_approximation(const Matrix<T, -1, -1> &A, int num_samples, int num_evals) {
        use_nyström_approximation = true;
        sample_uniform(num_samples, A);
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
