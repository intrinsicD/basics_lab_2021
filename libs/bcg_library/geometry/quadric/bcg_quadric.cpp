//
// Created by alex on 03.12.20.
//

#include "bcg_quadric.h"

namespace bcg{

MatrixS<3, 3> cross_product_squared_transpose(const VectorS<3> &v) {
    bcg_scalar_t a = v[0];
    bcg_scalar_t b = v[1];
    bcg_scalar_t c = v[2];
    bcg_scalar_t a2 = a * a;
    bcg_scalar_t b2 = b * b;
    bcg_scalar_t c2 = c * c;

    MatrixS<3, 3> M;

    M(0, 0) = b2 + c2;
    M(1, 1) = a2 + c2;
    M(2, 2) = a2 + b2;
    M(1, 0) = -a * b;
    M(2, 0) = -a * c;
    M(2, 1) = -b * c;
    M(0, 1) = -a * b;
    M(0, 2) = -a * c;
    M(1, 2) = -b * c;

    return M;
}


MatrixS<3, 3> first_order_tri_quad(const VectorS<3> &a, const MatrixS<3, 3> &sigma) {
    MatrixS<3, 3> M;

    bcg_scalar_t xx = a[0] * a[0];
    bcg_scalar_t xy = a[0] * a[1];
    bcg_scalar_t xz = a[0] * a[2];
    bcg_scalar_t yy = a[1] * a[1];
    bcg_scalar_t yz = a[1] * a[2];
    bcg_scalar_t zz = a[2] * a[2];

    bcg_scalar_t two = 2;

    M(0, 0) = -sigma(1, 1) * zz + two * sigma(1, 2) * yz - sigma(2, 2) * yy;
    M(0, 1) = sigma(0, 1) * zz - sigma(0, 2) * yz - sigma(1, 2) * xz + sigma(2, 2) * xy;
    M(0, 2) = -sigma(0, 1) * yz + sigma(0, 2) * yy + sigma(1, 1) * xz - sigma(1, 2) * xy;
    M(1, 1) = -sigma(0, 0) * zz + two * sigma(0, 2) * xz - sigma(2, 2) * xx;
    M(1, 2) = sigma(0, 0) * yz - sigma(0, 1) * xz - sigma(0, 2) * xy + sigma(1, 2) * xx;
    M(2, 2) = -sigma(0, 0) * yy + two * sigma(0, 1) * xy - sigma(1, 1) * xx;

    M(1, 0) = M(0, 1);
    M(2, 0) = M(0, 2);
    M(2, 1) = M(1, 2);

    return M;
}

MatrixS<3, 3> cross_interference_matrix(const MatrixS<3, 3> &A, const MatrixS<3, 3> &B) {
    MatrixS<3, 3> m;

    int constexpr x = 0;
    int constexpr y = 1;
    int constexpr z = 2;

    bcg_scalar_t cxx = A(y, z) * B(y, z);
    bcg_scalar_t cyy = A(x, z) * B(x, z);
    bcg_scalar_t czz = A(x, y) * B(x, y);

    m(x, x) = A(y, y) * B(z, z) - cxx - cxx + A(z, z) * B(y, y);
    m(y, y) = A(x, x) * B(z, z) - cyy - cyy + A(z, z) * B(x, x);
    m(z, z) = A(x, x) * B(y, y) - czz - czz + A(y, y) * B(x, x);

    m(x, y) = -A(x, y) * B(z, z) + A(x, z) * B(y, z)
              + A(y, z) * B(x, z) - A(z, z) * B(x, y);
    m(x, z) = A(x, y) * B(y, z) - A(x, z) * B(y, y)
              - A(y, y) * B(x, z) + A(y, z) * B(x, y);
    m(y, z) = -A(x, x) * B(y, z) + A(x, y) * B(x, z)
              + A(x, z) * B(x, y) - A(y, z) * B(x, x);

    m(y, x) = m(x, y);
    m(z, x) = m(x, z);
    m(z, y) = m(y, z);

    return m;
}

quadric::quadric() : m_A00(0), m_A01(0), m_A02(0), m_A11(0), m_A12(0), m_A22(0), m_b0(0), m_b1(0), m_b2(0), m_c(0) {}

quadric quadric::from_coefficients(const MatrixS<3, 3> &A, const VectorS<3> &b, bcg_scalar_t c) {
    quadric q;
    q.m_A00 = A(0, 0);
    q.m_A01 = A(0, 1);
    q.m_A02 = A(0, 2);
    q.m_A11 = A(1, 1);
    q.m_A12 = A(1, 2);
    q.m_A22 = A(2, 2);
    q.m_b0 = b(0);
    q.m_b1 = b(1);
    q.m_b2 = b(2);
    q.m_c = c;
    return q;
}

quadric quadric::from_coefficients(bcg_scalar_t A00, bcg_scalar_t A01, bcg_scalar_t A02, bcg_scalar_t A11, bcg_scalar_t A12,
                       bcg_scalar_t A22, bcg_scalar_t b0, bcg_scalar_t b1,
                       bcg_scalar_t b2, bcg_scalar_t c) {
    quadric q;
    q.m_A00 = A00;
    q.m_A01 = A01;
    q.m_A02 = A02;
    q.m_A11 = A11;
    q.m_A12 = A12;
    q.m_A22 = A22;
    q.m_b0 = b0;
    q.m_b1 = b1;
    q.m_b2 = b2;
    q.m_c = c;
    return q;
}

quadric quadric::point_quadric(const VectorS<3> &p) {
    return from_coefficients(MatrixS<3, 3>::Identity(), p, p.dot(p));
}

quadric quadric::plane_quadric(const VectorS<3> &p, const VectorS<3> &n) {
    bcg_scalar_t d = p.dot(n);
    return from_coefficients(n * n.transpose(), n * d, d * d);
}

quadric quadric::probabilistic_plane_quadric(const VectorS<3> &mean_p, const VectorS<3> &mean_n, bcg_scalar_t stddev_p,
                                 bcg_scalar_t stddev_n) {
    bcg_scalar_t sn2 = stddev_n * stddev_n;
    bcg_scalar_t sp2 = stddev_p * stddev_p;
    bcg_scalar_t d = mean_p.dot(mean_n);

    return from_coefficients(mean_n * mean_n.transpose() + MatrixS<3, 3>::Identity() * sn2,
                      mean_n * d + mean_p * sn2,
                      d * d + sn2 * mean_p.dot(mean_p) + sp2 * mean_n.dot(mean_n) + 3 * sp2 * sn2);
}

quadric quadric::probabilistic_plane_quadric(const VectorS<3> &mean_p, const VectorS<3> &mean_n,
                                 const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_n) {
    bcg_scalar_t d = mean_p.dot(mean_n);

    return from_coefficients(mean_n * mean_n.transpose() + sigma_n,
                      mean_n * d + sigma_n * mean_p,
                      d * d + mean_p.dot(sigma_n * mean_p) + mean_n.dot(sigma_p * mean_n) +
                      (sigma_n * sigma_p).trace());
}

quadric quadric::triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r) {
    VectorS<3> pxq = p.cross(q);
    VectorS<3> xsum = pxq + q.cross(r) + r.cross(p);
    bcg_scalar_t det = pxq.dot(r);
    return from_coefficients(xsum * xsum.transpose(), xsum * det, det * det);
}

quadric
quadric::probabilistic_triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r,
                               bcg_scalar_t stddev) {
    bcg_scalar_t sigma = stddev * stddev;
    VectorS<3> pxq = p.cross(q);
    VectorS<3> qxr = q.cross(r);
    VectorS<3> rxp = r.cross(p);

    bcg_scalar_t det_pqr = pxq.dot(r);

    VectorS<3> cross_pqr = pxq + qxr + rxp;

    VectorS<3> pmq = p - q;
    VectorS<3> qmr = q - r;
    VectorS<3> rmp = r - p;

    MatrixS<3, 3> A = cross_pqr * cross_pqr.transpose() +
                      (cross_product_squared_transpose(pmq) +
                       cross_product_squared_transpose(qmr) +
                       cross_product_squared_transpose(rmp))
                      * sigma;

    bcg_scalar_t ss = sigma * sigma;
    bcg_scalar_t ss6 = 6 * ss;
    bcg_scalar_t ss2 = 2 * ss;
    A(0, 0) += ss6;
    A(1, 1) += ss6;
    A(2, 2) += ss6;

    VectorS<3> b = cross_pqr * det_pqr;

    b = b - (pmq.cross(pxq) + qmr.cross(qxr) + rmp.cross(rxp)) * sigma;
    b = b + (p + q + r) * ss2;

    bcg_scalar_t c = det_pqr * det_pqr;
    c += sigma * (pxq.dot(pxq) + qxr.dot(qxr) + rxp.dot(rxp)); // 3x (a x b)^T M_c (a x b)
    c += ss2 * (p.dot(p) + q.dot(q) + r.dot(r)); // 3x a^T Ci[S_b, S_c] a
    c += ss6 * sigma; // Tr[S_r Ci[S_p, S_q]]

    return from_coefficients(A, b, c);
}

quadric
quadric::probabilistic_triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r,
                               const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_q,
                               const MatrixS<3, 3> &sigma_r) {
    VectorS<3> pxq = p.cross(q);
    VectorS<3> qxr = q.cross(r);
    VectorS<3> rxp = r.cross(p);

    bcg_scalar_t det_pqr = pxq.dot(r);

    VectorS<3> cross_pqr = pxq + qxr + rxp;

    VectorS<3> pmq = p - q;
    VectorS<3> qmr = q - r;
    VectorS<3> rmp = r - p;

    MatrixS<3, 3> ci_pq = cross_interference_matrix(sigma_p, sigma_q);
    MatrixS<3, 3> ci_qr = cross_interference_matrix(sigma_q, sigma_r);
    MatrixS<3, 3> ci_rp = cross_interference_matrix(sigma_r, sigma_p);

    MatrixS<3, 3> A = cross_pqr * cross_pqr.transpose();

    A = A - first_order_tri_quad(pmq, sigma_r);
    A = A - first_order_tri_quad(qmr, sigma_p);
    A = A - first_order_tri_quad(rmp, sigma_q);

    A = A + ci_pq + ci_qr + ci_rp;

    A(1, 0) = A(0, 1);
    A(2, 0) = A(0, 2);
    A(2, 1) = A(1, 2);

    VectorS<3> b = cross_pqr * det_pqr;

    b = b - pmq.cross(sigma_r * pxq);
    b = b - qmr.cross(sigma_p * qxr);
    b = b - rmp.cross(sigma_q * rxp);

    b = b + ci_pq * r;
    b = b + ci_qr * p;
    b = b + ci_rp * q;

    bcg_scalar_t c = det_pqr * det_pqr;

    c += pxq.dot(sigma_r * pxq);
    c += qxr.dot(sigma_p * qxr);
    c += rxp.dot(sigma_q * rxp);

    c += p.dot(ci_qr * p);
    c += q.dot(ci_rp * q);
    c += r.dot(ci_pq * r);

    c += (sigma_r * ci_pq).trace();

    return from_coefficients(A, b, c);
}

[[nodiscard]] MatrixS<3, 3> quadric::A() const {
    MatrixS<3, 3> m;
    m(0, 0) = m_A00;
    m(0, 1) = m_A01;
    m(0, 2) = m_A02;
    m(1, 0) = m_A01;
    m(1, 1) = m_A11;
    m(1, 2) = m_A12;
    m(2, 0) = m_A02;
    m(2, 1) = m_A12;
    m(2, 2) = m_A22;
    return m;
}

[[nodiscard]] VectorS<3> quadric::b() const {
    return {m_b0, m_b1, m_b2};
}

[[nodiscard]] bcg_scalar_t quadric::c() const {
    return m_c;
}

[[nodiscard]] VectorS<3> quadric::minimizer() const {
    bcg_scalar_t ad = m_A00 * m_A11;
    bcg_scalar_t ae = m_A00 * m_A12;
    bcg_scalar_t af = m_A00 * m_A22;
    bcg_scalar_t bc = m_A01 * m_A02;
    bcg_scalar_t be = m_A01 * m_A12;
    bcg_scalar_t bf = m_A01 * m_A22;
    bcg_scalar_t df = m_A11 * m_A22;
    bcg_scalar_t ce = m_A02 * m_A12;
    bcg_scalar_t cd = m_A02 * m_A11;

    bcg_scalar_t be_cd = be - cd;
    bcg_scalar_t bc_ae = bc - ae;
    bcg_scalar_t ce_bf = ce - bf;

    bcg_scalar_t denom = 1.0 / (m_A00 * df + 2.0 * m_A01 * ce - ae * m_A12 - bf * m_A01 - cd * m_A02);
    bcg_scalar_t nom0 = m_b0 * (df - m_A12 * m_A12) + m_b1 * ce_bf + m_b2 * be_cd;
    bcg_scalar_t nom1 = m_b0 * ce_bf + m_b1 * (af - m_A02 * m_A02) + m_b2 * bc_ae;
    bcg_scalar_t nom2 = m_b0 * be_cd + m_b1 * bc_ae + m_b2 * (ad - m_A01 * m_A01);

    return VectorS<3>(nom0 * denom, nom1 * denom, nom2 * denom);
}

void quadric::clear() {
    m_A00 = 0;
    m_A01 = 0;
    m_A02 = 0;
    m_A11 = 0;
    m_A12 = 0;
    m_A22 = 0;
    m_b0 = 0;
    m_b1 = 0;
    m_b2 = 0;
    m_c = 0;
}

bcg_scalar_t quadric::operator()(const VectorS<3> &p) const {
    return p.transpose() * (A() * p - 2 * b()) + c();
}

quadric &quadric::operator+=(const quadric &rhs) {
    m_A00 += rhs.m_A00;
    m_A01 += rhs.m_A01;
    m_A02 += rhs.m_A02;
    m_A11 += rhs.m_A11;
    m_A12 += rhs.m_A12;
    m_A22 += rhs.m_A22;

    m_b0 += rhs.m_b0;
    m_b1 += rhs.m_b1;
    m_b2 += rhs.m_b2;

    m_c += rhs.m_c;

    return *this;
}

quadric &quadric::operator-=(const quadric &rhs) {
    m_A00 -= rhs.m_A00;
    m_A01 -= rhs.m_A01;
    m_A02 -= rhs.m_A02;
    m_A11 -= rhs.m_A11;
    m_A12 -= rhs.m_A12;
    m_A22 -= rhs.m_A22;

    m_b0 -= rhs.m_b0;
    m_b1 -= rhs.m_b1;
    m_b2 -= rhs.m_b2;

    m_c -= rhs.m_c;

    return *this;
}


quadric &quadric::operator*=(bcg_scalar_t s) {
    m_A00 *= s;
    m_A01 *= s;
    m_A02 *= s;
    m_A11 *= s;
    m_A12 *= s;
    m_A22 *= s;

    m_b0 *= s;
    m_b1 *= s;
    m_b2 *= s;

    m_c *= s;

    return *this;
}

quadric &quadric::operator/=(bcg_scalar_t s) {
    return operator*=(1.0 / s);
}

quadric quadric::operator-() const {
    return from_coefficients(-m_A00, -m_A01, -m_A02, -m_A11, -m_A12, -m_A22, -m_b0, -m_b1, -m_b2, -m_c);
}

quadric quadric::operator+(const quadric &rhs) const {
    quadric r = *this; // copy
    r += rhs;
    return r;
}

quadric quadric::operator-(const quadric &rhs) const {
    quadric r = *this; // copy
    r -= rhs;
    return r;
}

quadric quadric::operator*(bcg_scalar_t s) const {
    quadric r = *this; // copy
    r *= s;
    return r;
}

quadric quadric::operator/(bcg_scalar_t s) const {
    quadric r = *this; // copy
    r /= s;
    return r;
}

quadric operator*(bcg_scalar_t s, const quadric &Q) { return Q * s; }

}