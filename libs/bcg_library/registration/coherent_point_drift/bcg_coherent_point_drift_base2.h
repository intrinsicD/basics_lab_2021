//
// Created by alex on 10.02.21.
//

#ifndef BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_BASE2_H
#define BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_BASE2_H

#include "geometry/point_cloud/bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"
#include "math/bcg_linalg.h"
#include "math/matrix/bcg_matrix_kernel.h"

namespace bcg {

enum class SoftmatchingType {
    full,
    full_FGT,
    parallel,
    nystroem,
    nystroem_FGT,
    kdtree,
    __last__
};

inline std::vector<std::string> softmatching_type_names(){
    std::vector<std::string> names(static_cast<int>(SoftmatchingType::__last__));
    names[static_cast<int>(SoftmatchingType::full)] = "full";
    names[static_cast<int>(SoftmatchingType::full_FGT)] = "full_FGT";
    names[static_cast<int>(SoftmatchingType::parallel)] = "parallel";
    names[static_cast<int>(SoftmatchingType::nystroem)] = "nystroem";
    names[static_cast<int>(SoftmatchingType::nystroem_FGT)] = "nystroem_FGT";
    names[static_cast<int>(SoftmatchingType::kdtree)] = "kdtree";
    return names;
}

struct coherent_point_drift_base2 {
    property<bcg_scalar_t, 1> P1, PT1;
    property<VectorS<3>, 3> source_positions, target_positions, PX;
    bcg_scalar_t N_P, sigma_squared, kdtree_sigma_threshold = 0.02, omega = 0.5;
    kdtree_property<bcg_scalar_t> target_kdtree;
    Transform *source_model;
    Transform *target_model;
    size_t M, N, D;
    int num_samples;
    bool debug_output = false, initialized = false;
    using kernel_precision = double;
    kernel_matrix<kernel_precision> kernel_P;
    size_t parallel_grain_size = 1024;
    std::vector<float> likelihood;
    MatrixS<-1, -1> X, Y;

    SoftmatchingType softmatching_type;

    virtual void init(vertex_container *source_vertices,
                      Transform &source_model,
                      vertex_container *target_vertices,
                      Transform &target_model);

    void reset();

    virtual void update_P();

    virtual void compute_step() = 0;
    Transform backup_source_model;
protected:
    Transform backup_target_model;

    virtual void update_P_full();

    virtual void update_P_FGT();

    virtual void update_P_parallel(size_t parallel_grain_size);

    virtual void update_P_nystroem();

    virtual void update_P_nystroem_FGT();

    virtual void update_P_kdtree(size_t parallel_grain_size);

    virtual MatrixS<-1, 3> transform_source();

    virtual VectorS<3> transform_source(size_t idx);

    virtual MatrixS<-1, 3> transform_target();

    virtual VectorS<3> transform_target(size_t idx);
};

struct coherent_point_drift_rigid2 : public coherent_point_drift_base2 {
    bcg_scalar_t s;
    VectorS<3> t;
    MatrixS<3, 3> R;

    void init(vertex_container *source_vertices, Transform &source_model, vertex_container *target_vertices,
              Transform &target_model) override;

    void compute_step() override;
};

struct coherent_point_drift_affine2 : public coherent_point_drift_base2 {
    VectorS<3> t;
    MatrixS<3, 3> B;

    void init(vertex_container *source_vertices, Transform &source_model, vertex_container *target_vertices,
              Transform &target_model) override;

    void compute_step() override;
};

enum class CoherenceType{
    full,
    eigen,
    __last__
};

inline std::vector<std::string> coherence_type_names(){
    std::vector<std::string> names(static_cast<int>(CoherenceType::__last__));
    names[static_cast<int>(CoherenceType::full)] = "full";
    names[static_cast<int>(CoherenceType::eigen)] = "eigen";
    return names;
}

struct coherent_point_drift_nonrigid2 : public coherent_point_drift_base2 {
    CoherenceType coherence_type;
    int num_evals;
    Matrix<kernel_precision, -1, -1> G;
    kernel_matrix<kernel_precision> kernel_G;
    property<VectorS<3>, 3> deformation, deformed;
    bcg_scalar_t beta = 2, lambda = 2;
    bcg_scalar_t error;

    void init(vertex_container *source_vertices, Transform &source_model, vertex_container *target_vertices,
              Transform &target_model) override;

    void accept_deformation();

    void compute_step() override;

    MatrixS<-1, 3> transform_source() override;

    VectorS<3> transform_source(size_t idx) override;
};

struct coherent_point_drift_nonrigid3 : public coherent_point_drift_base2 {
    CoherenceType coherence_type;
    int num_evals;
    Matrix<kernel_precision, -1, -1> G;
    kernel_matrix<kernel_precision> kernel_G;
    property<VectorS<3>, 3> deformation, deformed;
    bcg_scalar_t beta = 2, lambda = 2;
    bcg_scalar_t error;
    bcg_scalar_t s;
    VectorS<3> t;
    MatrixS<3, 3> R;

    void init(vertex_container *source_vertices, Transform &source_model, vertex_container *target_vertices,
              Transform &target_model) override;

    void accept_deformation();

    void compute_step() override;

    MatrixS<-1, 3> transform_source() override;

    VectorS<3> transform_source(size_t idx) override;
};

struct coherent_point_drift_bayes2 : public coherent_point_drift_base2 {
    CoherenceType coherence_type;
    bcg_scalar_t s;
    VectorS<3> t;
    MatrixS<3, 3> R;
    property<bcg_scalar_t, 1> alpha, p_out;
    property<VectorS<3>, 3> deformation, deformed, residual;
    int num_evals = 30;
    MatrixS<-1, -1> Sigma;
    MatrixS<-1, -1> G_inv;
    kernel_matrix<double> kernel_G;

    bcg_scalar_t beta = 2, gamma = 2, kappa = 1, lambda = 2;

    void init(vertex_container *source_vertices, Transform &source_model, vertex_container *target_vertices,
              Transform &target_model) override;

    bcg_scalar_t compute_target_volume() const;

    void compute_step() override;

private:
    void update_P_full() override;

    void update_P_FGT() override;

    void update_P_parallel(size_t parallel_grain_size) override;

    void update_P_nystroem() override;

    void update_P_nystroem_FGT() override;

    void update_P_kdtree(size_t parallel_grain_size) override;

    MatrixS<-1, 3> transform_inverse(const MatrixS<-1, 3> &residual);

    MatrixS<-1, 3> transform_source() override;

    VectorS<3> transform_source(size_t idx) override;
};

}

#endif //BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_BASE2_H
