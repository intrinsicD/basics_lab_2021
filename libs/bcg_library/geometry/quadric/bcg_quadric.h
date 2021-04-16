//
// Created by alex on 03.12.20.
//

#ifndef BCG_GRAPHICS_BCG_QUADRIC_H
#define BCG_GRAPHICS_BCG_QUADRIC_H

#include "math/bcg_linalg.h"

namespace bcg {

struct quadric {
private:
    bcg_scalar_t m_A00, m_A01, m_A02, m_A11, m_A12, m_A22, m_b0, m_b1, m_b2, m_c;
public:
    quadric();

    [[nodiscard]] static quadric from_coefficients(const MatrixS<3, 3> &A, const VectorS<3> &b, bcg_scalar_t c);

    [[nodiscard]] static quadric from_coefficients(bcg_scalar_t A00, bcg_scalar_t A01, bcg_scalar_t A02, bcg_scalar_t A11, bcg_scalar_t A12,
                           bcg_scalar_t A22, bcg_scalar_t b0, bcg_scalar_t b1,
                           bcg_scalar_t b2, bcg_scalar_t c);

    [[nodiscard]] static quadric point_quadric(const VectorS<3> &p);

    [[nodiscard]] static quadric plane_quadric(const VectorS<3> &p, const VectorS<3> &n);

    [[nodiscard]] static quadric probabilistic_plane_quadric(const VectorS<3> &mean_p, const VectorS<3> &mean_n, bcg_scalar_t stddev_p,
                                     bcg_scalar_t stddev_n);

    [[nodiscard]] static quadric probabilistic_plane_quadric(const VectorS<3> &mean_p, const VectorS<3> &mean_n,
                                     const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_n);

    [[nodiscard]] static quadric triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r);

    [[nodiscard]] static quadric
    probabilistic_triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r,
                                   bcg_scalar_t stddev);

    [[nodiscard]] static quadric
    probabilistic_triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r,
                                   const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_q,
                                   const MatrixS<3, 3> &sigma_r);

    [[nodiscard]] MatrixS<3, 3> A() const;

    [[nodiscard]] VectorS<3> b() const;

    [[nodiscard]] bcg_scalar_t c() const;

    [[nodiscard]] VectorS<3> minimizer() const;

    void clear();

    bcg_scalar_t operator()(const VectorS<3> &p) const;

    quadric &operator+=(const quadric &rhs);

    quadric &operator-=(const quadric &rhs);


    quadric &operator*=(bcg_scalar_t s);

    quadric &operator/=(bcg_scalar_t s);

    quadric operator-() const;

    quadric operator+(const quadric &rhs) const;

    quadric operator-(const quadric &rhs) const;

    quadric operator*(bcg_scalar_t s) const;

    quadric operator/(bcg_scalar_t s) const;

    friend quadric operator*(bcg_scalar_t s, const quadric &Q);
};

}

#endif //BCG_GRAPHICS_BCG_QUADRIC_H
