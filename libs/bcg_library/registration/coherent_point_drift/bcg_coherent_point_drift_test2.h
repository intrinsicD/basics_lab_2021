//
// Created by alex on 04.02.21.
//

#ifndef BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_TEST2_H
#define BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_TEST2_H

namespace bcg{

struct coherent_point_drift_test2{
    bool use_nyström_approximation_P;
    bool use_kdtree_P;
    bool use_nyström_approximation_G;
    bool use_eigen_decomposition_G;

    double sigma_kdtree_threshold;
    int num_evecs;
    int num_samples_P;
    int num_samples_G;
    int rnd_seed;

    double omega;
    double lambda;
    double kappa;
    double gamma;
    double beta;

    double sigma_squared, s;
    Matrix<double, -1, -1> X, Y, T, V, U, G, Sigma, R, PX, residual, Evecs;
    Vector<double, -1> t, alpha, P1, PT1, Evals;

    double kernel_inverse_multi_quadric(double beta, const Vector<double, -1> &a, const Vector<double, -1> &b);
    double kernel_rational_quadric(double beta, const Vector<double, -1> &a, const Vector<double, -1> &b);
    double kernel_laplace(double beta, const Vector<double, -1> &a, const Vector<double, -1> &b);
    double kernel_gaussian(double beta, const Vector<double, -1> &a, const Vector<double, -1> &b);

    void initialize(const Matrix<double, -1, -1> &X, const Matrix<double, -1, -1> &Y);

    void update_correspondences();
    void update_deformation();
    void update_similarity_transform();
    void compute_step();

    void compute_nyström_P();
    void compute_kdtree_P();
    void compute_nyström_G();
    void compute_eigendecomposition_G();
};

}

#endif //BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_TEST2_H
