//
// Created by alex on 27.11.20.
//
#include "bcg_mesh_simplification.h"

#include <utility>
#include "bcg_mesh_face_normals.h"
#include "distance_query/bcg_distance_triangle_point.h"
#include "utils/bcg_heap.h"

namespace bcg {

MatrixS<3, 3> cross_product_squared_transpose(const VectorS<3> &v) {
    auto a = v[0];
    auto b = v[1];
    auto c = v[2];
    auto a2 = a * a;
    auto b2 = b * b;
    auto c2 = c * c;

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

    auto xx = a[0] * a[0];
    auto xy = a[0] * a[1];
    auto xz = a[0] * a[2];
    auto yy = a[1] * a[1];
    auto yz = a[1] * a[2];
    auto zz = a[2] * a[2];

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

    auto constexpr x = 0;
    auto constexpr y = 1;
    auto constexpr z = 2;

    auto cxx = A(y, z) * B(y, z);
    auto cyy = A(x, z) * B(x, z);
    auto czz = A(x, y) * B(x, y);

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

struct quadric {
private:
    bcg_scalar_t m_A00, m_A01, m_A02, m_A11, m_A12, m_A22, m_b0, m_b1, m_b2, m_c;
public:
    quadric() : m_A00(0), m_A01(0), m_A02(0), m_A11(0), m_A12(0), m_A22(0), m_b0(0), m_b1(0), m_b2(0), m_c(0) {}

    void from_coefficients(const MatrixS<3, 3> &A, const VectorS<3> &b, bcg_scalar_t c) {
        m_A00 = A(0, 0);
        m_A01 = A(0, 1);
        m_A02 = A(0, 2);
        m_A11 = A(1, 1);
        m_A12 = A(1, 2);
        m_A22 = A(2, 2);
        m_b0 = b(0);
        m_b1 = b(1);
        m_b2 = b(2);
        m_c = c;
    }

    void from_coefficients(bcg_scalar_t A00, bcg_scalar_t A01, bcg_scalar_t A02, bcg_scalar_t A11, bcg_scalar_t A12,
                           bcg_scalar_t A22, bcg_scalar_t b0, bcg_scalar_t b1,
                           bcg_scalar_t b2, bcg_scalar_t c) {
        m_A00 = A00;
        m_A01 = A01;
        m_A02 = A02;
        m_A11 = A11;
        m_A12 = A12;
        m_A22 = A22;
        m_b0 = b0;
        m_b1 = b1;
        m_b2 = b2;
        m_c = c;
    }

    void point_quadric(const VectorS<3> &p) {
        from_coefficients(MatrixS<3, 3>::Identity(), p, p.dot(p));
    }

    void plane_quadric(const VectorS<3> &p, const VectorS<3> &n) {
        auto d = p.dot(n);
        from_coefficients(n * n.transpose(), n * d, d * d);
    }

    void probabilistic_plane_quadric(const VectorS<3> &mean_p, const VectorS<3> &mean_n, bcg_scalar_t stddev_p,
                                     bcg_scalar_t stddev_n) {
        auto sn2 = stddev_n * stddev_n;
        auto sp2 = stddev_p * stddev_p;
        auto d = mean_p.dot(mean_n);

        from_coefficients(mean_n * mean_n.transpose() + MatrixS<3, 3>::Identity() * sn2,
                          mean_n * d + mean_p * sn2,
                          d * d + sn2 * mean_p.dot(mean_p) + sp2 * mean_n.dot(mean_n) + 3 * sp2 * sn2);
    }

    void probabilistic_plane_quadric(const VectorS<3> &mean_p, const VectorS<3> &mean_n,
                                     const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_n) {
        auto d = mean_p.dot(mean_n);

        from_coefficients(mean_n * mean_n.transpose() + sigma_n,
                          mean_n * d + sigma_n * mean_p,
                          d * d + mean_p.dot(sigma_n * mean_p) + mean_n.dot(sigma_p * mean_n) +
                          (sigma_n * sigma_p).trace());
    }

    void triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r) {
        VectorS<3> pxq = p.cross(q);
        VectorS<3> xsum = pxq + q.cross(r) + r.cross(p);
        auto det = pxq.dot(r);
        from_coefficients(xsum * xsum.transpose(), xsum * det, det * det);
    }

    void
    probabilistic_triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r,
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

        from_coefficients(A, b, c);
    }

    void
    probabilistic_triangle_quadric(const VectorS<3> &p, const VectorS<3> &q, const VectorS<3> &r,
                                   const MatrixS<3, 3> &sigma_p, const MatrixS<3, 3> &sigma_q,
                                   const MatrixS<3, 3> &sigma_r) {
        VectorS<3> pxq = p.cross(q);
        VectorS<3> qxr = q.cross(r);
        VectorS<3> rxp = r.cross(p);

        auto det_pqr = pxq.dot(r);

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

        auto c = det_pqr * det_pqr;

        c += pxq.dot(sigma_r * pxq);
        c += qxr.dot(sigma_p * qxr);
        c += rxp.dot(sigma_q * rxp);

        c += p.dot(ci_qr * p);
        c += q.dot(ci_rp * q);
        c += r.dot(ci_pq * r);

        c += (sigma_r * ci_pq).trace();

        from_coefficients(A, b, c);
    }

    [[nodiscard]] MatrixS<3, 3> A() const {
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

    [[nodiscard]] VectorS<3> b() const {
        return {m_b0, m_b1, m_b2};
    }

    [[nodiscard]] bcg_scalar_t c() const {
        return m_c;
    }

    [[nodiscard]] VectorS<3> minimizer() const {
        auto ad = m_A00 * m_A11;
        auto ae = m_A00 * m_A12;
        auto af = m_A00 * m_A22;
        auto bc = m_A01 * m_A02;
        auto be = m_A01 * m_A12;
        auto bf = m_A01 * m_A22;
        auto df = m_A11 * m_A22;
        auto ce = m_A02 * m_A12;
        auto cd = m_A02 * m_A11;

        auto be_cd = be - cd;
        auto bc_ae = bc - ae;
        auto ce_bf = ce - bf;

        auto denom = 1.0 / (m_A00 * df + 2.0 * m_A01 * ce - ae * m_A12 - bf * m_A01 - cd * m_A02);
        auto nom0 = m_b0 * (df - m_A12 * m_A12) + m_b1 * ce_bf + m_b2 * be_cd;
        auto nom1 = m_b0 * ce_bf + m_b1 * (af - m_A02 * m_A02) + m_b2 * bc_ae;
        auto nom2 = m_b0 * be_cd + m_b1 * bc_ae + m_b2 * (ad - m_A01 * m_A01);

        return {nom0 * denom, nom1 * denom, nom2 * denom};
    }

    void clear() {
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

    bcg_scalar_t operator()(const VectorS<3> &p) const {
        return p.transpose() * (A() * p - 2 * b()) + c();
    }

    quadric &operator+=(const quadric &rhs) {
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

    quadric &operator-=(const quadric &rhs) {
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


    quadric &operator*=(bcg_scalar_t s) {
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

    quadric &operator/=(bcg_scalar_t s) {
        return operator*=(1.0 / s);
    }

    quadric operator-() const {
        quadric q;
        q.from_coefficients(-m_A00, -m_A01, -m_A02, -m_A11, -m_A12, -m_A22, -m_b0, -m_b1, -m_b2, -m_c);
        return q;
    }

    quadric operator+(const quadric &rhs) const {
        auto r = *this; // copy
        r += rhs;
        return r;
    }

    quadric operator-(const quadric &rhs) const {
        auto r = *this; // copy
        r -= rhs;
        return r;
    }

    quadric operator*(bcg_scalar_t s) const {
        auto r = *this; // copy
        r *= s;
        return r;
    }

    quadric operator/(bcg_scalar_t s) const {
        auto r = *this; // copy
        r /= s;
        return r;
    }

    friend quadric operator*(bcg_scalar_t s, const quadric &Q) { return Q * s; }
};

struct normal_cone {
    normal_cone() : angle(0.0) {}

    explicit normal_cone(const VectorS<3> &normal, double angle = 0.0)
            : center_normal(normal), angle(angle) {
    }

    normal_cone &merge(const VectorS<3> &n) { return merge(normal_cone(n)); }

    normal_cone &merge(const normal_cone &nc) {
        const auto dp = center_normal.dot(nc.center_normal);

        // axes point in same direction
        if (dp > 0.99999) {
            angle = std::max(angle, nc.angle);
        }

            // axes point in opposite directions
        else if (dp < -0.99999) {
            angle = 2 * pi;
        } else {
            // new angle
            auto center_angle = acos(dp);
            auto min_angle = std::min<double>(-angle, center_angle - nc.angle);
            auto max_angle = std::max<double>(angle, center_angle + nc.angle);
            angle = 0.5 * (max_angle - min_angle);

            // axis by SLERP
            auto axis_angle = 0.5 * (min_angle + max_angle);
            center_normal = ((center_normal * sin(center_angle - axis_angle) +
                              nc.center_normal * sin(axis_angle)) /
                             sin(center_angle));
        }

        return *this;
    }

    VectorS<3> center_normal;
    bcg_scalar_t angle;
};

class heap_interface {
public:
    heap_interface(property<bcg_scalar_t, 1> prio, property<int, 1> pos) : prio(std::move(prio)), pos(std::move(pos)) {}

    bool less(vertex_handle v0, vertex_handle v1) { return prio[v0] < prio[v1]; }

    bool greater(vertex_handle v0, vertex_handle v1) { return prio[v0] > prio[v1]; }

    int get_heap_position(vertex_handle v) { return pos[v]; }

    void set_heap_position(vertex_handle v, int position) { pos[v] = position; }

private:
    property<bcg_scalar_t, 1> prio;
    property<int, 1> pos;
};

/*
            vl
            *
           / \
          /   \
         / fl  \
     v0 *------>* v1
         \ fr  /
          \   /
           \ /
            *
            vr
*/
struct collapse_data {
    collapse_data(halfedge_mesh &sm, halfedge_handle h);

    halfedge_mesh &mesh;

    halfedge_handle v0v1; // Halfedge to be collapsed
    halfedge_handle v1v0; // Reverse halfedge
    vertex_handle v0;     // Vertex to be removed
    vertex_handle v1;     // Remaining vertex
    face_handle fl;       // Left face
    face_handle fr;       // Right face
    vertex_handle vl;     // Left vertex
    vertex_handle vr;     // Right vertex
    halfedge_handle v1vl, vlv0, v0vr, vrv1;
};

collapse_data::collapse_data(halfedge_mesh &sm, halfedge_handle h) : mesh(sm){
    v0v1 = std::move(h);
    v1v0 = mesh.get_opposite(v0v1);
    v0 = mesh.get_to_vertex(v1v0);
    v1 = mesh.get_to_vertex(v0v1);
    fl = mesh.get_face(v0v1);
    fr = mesh.get_face(v1v0);

    // get vl
    if (fl.is_valid()) {
        v1vl = mesh.get_next(v0v1);
        vlv0 = mesh.get_next(v1vl);
        vl = mesh.get_to_vertex(v1vl);
    }

    // get vr
    if (fr.is_valid()) {
        v0vr = mesh.get_next(v1v0);
        vrv1 = mesh.get_prev(v0vr);
        vr = mesh.get_from_vertex(vrv1);
    }
}

struct simplification {
    using points = std::vector<VectorS<3>>;
    using priority_queue = heap<vertex_handle, heap_interface>;

    simplification(halfedge_mesh &mesh, bcg_scalar_t aspect_ratio,
                   bcg_scalar_t edge_length, unsigned int max_valence,
                   bcg_scalar_t normal_deviation, bcg_scalar_t hausdorff_error);

    halfedge_mesh &mesh;

    bool initialized;

    property<bcg_scalar_t, 1> vpriority;
    property<halfedge_handle, 1> vtarget;
    property<int, 1> heap_pos;
    property<quadric, 1> vquadric;
    property<normal_cone, 1> normal_cones;
    property<points, 1> face_points;

    property<VectorS<3>, 3> vpoint;
    property<VectorS<3>, 3> fnormal;
    property<int, 1> vselected;
    property<bool, 1> vfeature;
    property<bool, 1> efeature;

    priority_queue *queue;

    bool has_selection{};
    bool has_features{};
    bcg_scalar_t normal_deviation;
    bcg_scalar_t hausdorff_error;
    bcg_scalar_t aspect_ratio;
    bcg_scalar_t edge_length;
    unsigned int max_valence;

    //! Simplify mesh to \p n vertices.
    void simplify(unsigned int n_vertices);

private:
    void enqueuevertex(vertex_handle v);

    // is collapsing the halfedge h allowed?
    bool is_collapse_legal(const collapse_data &cd);

    // what is the priority of collapsing the halfedge h
    float priority(const collapse_data &cd);

    // postprocess halfedge collapse
    void postprocess_collapse(const collapse_data &cd);

    // compute aspect ratio for face f
    bcg_scalar_t compute_aspect_ratio(face_handle f) const;


    // compute distance from p to triagle f
    bcg_scalar_t distance(face_handle f, const VectorS<3> &p) const;
};

simplification::simplification(halfedge_mesh &mesh, bcg_scalar_t aspect_ratio,
                               bcg_scalar_t edge_length, unsigned int max_valence,
                               bcg_scalar_t normal_deviation, bcg_scalar_t hausdorff_error) : mesh(mesh),
                                                                                              aspect_ratio(
                                                                                                      aspect_ratio),
                                                                                              edge_length(edge_length),
                                                                                              max_valence(max_valence),
                                                                                              normal_deviation(
                                                                                                      normal_deviation /
                                                                                                      180.0 * pi),
                                                                                              hausdorff_error(
                                                                                                      hausdorff_error) {

    if (!mesh.is_triangle_mesh()) {
        mesh.triangulate();
    }

    // add properties
    vquadric = mesh.vertices.get_or_add<quadric, 1>("v_quadric");

    // get properties
    vpoint = mesh.vertices.get<VectorS<3>, 3>("v_position");
    assert(vpoint);
    // compute face normals
    face_normals(mesh);
    fnormal = mesh.faces.get<VectorS<3>, 3>("f_normal");
    assert(fnormal);
    // properties
    if (normal_deviation > 0.0) {
        normal_cones = mesh.faces.get_or_add<normal_cone, 1>("f_normalCone");
    } else {
        mesh.faces.remove(normal_cones);
    }
    if (hausdorff_error > 0.0) {
        face_points = mesh.faces.get_or_add<points, 1>("f_points");
    } else {
        mesh.faces.remove(face_points);
    }

    // vertex selection
    has_selection = false;
    vselected = mesh.vertices.get<int, 1>("v_selected");
    if (vselected) {
        for (const auto v : mesh.vertices) {
            if (vselected[v]) {
                has_selection = true;
                break;
            }
        }
    }

    // feature vertices/edges
    has_features = false;
    vfeature = mesh.vertices.get<bool, 1>("v_feature");
    efeature = mesh.edges.get<bool, 1>("e_feature");
    if (vfeature && efeature) {
        for (const auto v : mesh.vertices) {
            if (vfeature[v]) {
                has_features = true;
                break;
            }
        }
    }

    // initialize quadrics
    for (const auto v : mesh.vertices) {
        vquadric[v].clear();

        if (!mesh.is_isolated(v)) {
            quadric q;
            for (const auto f : mesh.get_faces(v)) {
                q.probabilistic_plane_quadric(vpoint[v], fnormal[f], 0, 1);
                vquadric[v] += q;
            }
        }
    }

    // initialize normal cones
    if (normal_deviation) {
        for (const auto f : mesh.faces) {
            normal_cones[f] = normal_cone(fnormal[f]);
        }
    }

    // initialize faces' point list
    if (hausdorff_error) {
        for (const auto f : mesh.faces) {
            points().swap(face_points[f]); // free mem
        }
    }

    initialized = true;
}

void simplification::simplify(unsigned int n_vertices) {
    if (!mesh.is_triangle_mesh()) {
        mesh.triangulate();
        return;
    }

    unsigned int nv(mesh.vertices.size());

    std::vector<vertex_handle> one_ring;
    std::vector<vertex_handle>::iterator or_it, or_end;
    halfedge_handle h;
    vertex_handle v;

    // add properties for priority queue
    vpriority = mesh.vertices.add<bcg_scalar_t, 1>("v_prio");
    heap_pos = mesh.vertices.add<int, 1>("v_heap");
    vtarget = mesh.vertices.add<halfedge_handle, 1>("v_target");

    // build priority queue
    heap_interface hi(vpriority, heap_pos);
    queue = new priority_queue(hi);
    queue->reserve(mesh.vertices.size());
    for (const auto v : mesh.vertices) {
        queue->reset_heap_position(v);
        enqueuevertex(v);
    }

    while (nv > n_vertices && !queue->empty()) {
        // get 1st element
        v = queue->front();
        queue->pop_front();
        h = vtarget[v];
        collapse_data cd(mesh, h);

        // check this (again)
        if (!mesh.is_collapse_ok(h)) {
            continue;
        }

        // store one-ring
        one_ring.clear();
        for (const auto vv : mesh.halfedge_graph::get_vertices(cd.v0)) {
            one_ring.push_back(vv);
        }

        // perform collapse
        mesh.collapse(h);

        --nv;
        //if (nv % 1000 == 0) std::cerr << nv << "\r";

        // postprocessing, e.g., update quadrics
        postprocess_collapse(cd);

        // update queue
        for (or_it = one_ring.begin(), or_end = one_ring.end(); or_it != or_end; ++or_it) {
            enqueuevertex(*or_it);
        }
    }

    // clean up
    delete queue;
    mesh.garbage_collection();
    mesh.vertices.remove(vpriority);
    mesh.vertices.remove(heap_pos);
    mesh.vertices.remove(vtarget);

    vpoint.set_dirty();
    face_normals(mesh);

    auto lines = mesh.edges.get_or_add<VectorI<2>, 2>("edges");
    lines.vector() = mesh.get_connectivity();
    lines.set_dirty();

    auto triangles = mesh.edges.get_or_add<VectorI<3>, 3>("triangles");
    triangles = mesh.get_triangles();
    triangles.set_dirty();
}

void simplification::enqueuevertex(vertex_handle v) {
    bcg_scalar_t prio, min_prio(scalar_max);
    halfedge_handle min_h;

    // find best out-going halfedge
    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
        collapse_data cd(mesh, h);
        if (is_collapse_legal(cd)) {
            prio = priority(cd);
            if (prio != -1.0 && prio < min_prio) {
                min_prio = prio;
                min_h = h;
            }
        }
    }

    // target found -> put vertex on heap
    if (min_h.is_valid()) {
        vpriority[v] = min_prio;
        vtarget[v] = min_h;

        if (queue->is_stored(v)) {
            queue->update(v);
        } else {
            queue->insert(v);
        }
    }

        // not valid -> remove from heap
    else {
        if (queue->is_stored(v)) {
            queue->remove(v);
        }

        vpriority[v] = -1;
        vtarget[v] = min_h;
    }
}

bool simplification::is_collapse_legal(const collapse_data &cd) {
    // test selected vertices

    if (has_selection && !vselected[cd.v0]) {
        return false;
    }


    // test features
    if (has_features) {
        if (vfeature[cd.v0] && !efeature[mesh.get_edge(cd.v0v1)]) {
            return false;
        }

        if (cd.vl.is_valid() && efeature[mesh.get_edge(cd.vlv0)]) {
            return false;
        }

        if (cd.vr.is_valid() && efeature[mesh.get_edge(cd.v0vr)]) {
            return false;
        }
    }

    // do not collapse boundary vertices to interior vertices
    if (mesh.is_boundary(cd.v0) && !mesh.is_boundary(cd.v1)) {
        return false;
    }

    // there should be at least 2 incident faces at v0
    if (mesh.rotate_cw(mesh.rotate_cw(cd.v0v1)) == cd.v0v1) {
        return false;
    }

    // topological check
    if (!mesh.is_collapse_ok(cd.v0v1)) {
        return false;
    }

    // check maximal valence
    if (max_valence > 0) {
        unsigned int val0 = mesh.halfedge_graph::get_valence(cd.v0);
        unsigned int val1 = mesh.halfedge_graph::get_valence(cd.v1);
        unsigned int val = val0 + val1 - 1;
        if (cd.fl.is_valid()) {
            --val;
        }
        if (cd.fr.is_valid()) {
            --val;
        }
        if (val > max_valence && !(val < std::max(val0, val1))) {
            return false;
        }
    }

    // remember the positions of the endpoints
    const VectorS<3> p0 = vpoint[cd.v0];
    const VectorS<3> p1 = vpoint[cd.v1];

    // check for maximum edge length
    if (edge_length) {
        for (const auto v : mesh.halfedge_graph::get_vertices(cd.v0)) {
            if (v != cd.v1 && v != cd.vl && v != cd.vr) {
                if ((vpoint[v] - p1).norm() > edge_length) {
                    return false;
                }
            }
        }
    }

    // check for flipping normals
    if (normal_deviation == 0.0) {
        vpoint[cd.v0] = p1;
        for (const auto f : mesh.get_faces(cd.v0)) {
            if (f != cd.fl && f != cd.fr) {
                VectorS<3> n0 = fnormal[f];
                VectorS<3> n1 = face_normal(mesh, f);
                if (n0.dot(n1) < 0.0) {
                    vpoint[cd.v0] = p0;
                    return false;
                }
            }
        }
        vpoint[cd.v0] = p0;
    }

        // check normal cone
    else {
        vpoint[cd.v0] = p1;

        face_handle fll, frr;
        if (cd.vl.is_valid()) {
            fll = mesh.get_face(mesh.get_opposite(mesh.get_prev(cd.v0v1)));
        }
        if (cd.vr.is_valid()) {
            frr = mesh.get_face(mesh.get_opposite(mesh.get_next(cd.v1v0)));
        }

        for (const auto f : mesh.get_faces(cd.v0)) {
            if (f != cd.fl && f != cd.fr) {
                normal_cone nc = normal_cones[f];
                nc.merge(face_normal(mesh, f));

                if (f == fll) {
                    nc.merge(normal_cones[cd.fl]);
                }
                if (f == frr) {
                    nc.merge(normal_cones[cd.fr]);
                }

                if (nc.angle > 0.5 * normal_deviation) {
                    vpoint[cd.v0] = p0;
                    return false;
                }
            }
        }

        vpoint[cd.v0] = p0;
    }

    // check aspect ratio
    if (aspect_ratio) {
        bcg_scalar_t ar0(0), ar1(0);

        for (const auto f : mesh.get_faces(cd.v0)) {
            if (f != cd.fl && f != cd.fr) {
                // worst aspect ratio after collapse
                vpoint[cd.v0] = p1;
                ar1 = std::max(ar1, compute_aspect_ratio(f));
                // worst aspect ratio before collapse
                vpoint[cd.v0] = p0;
                ar0 = std::max(ar0, compute_aspect_ratio(f));
            }
        }

        // aspect ratio is too bad, and it does also not improve
        if (ar1 > aspect_ratio && ar1 > ar0) {
            return false;
        }
    }

    // check Hausdorff error
    if (hausdorff_error) {
        points p;
        bool ok;

        // collect points to be tested
        for (const auto f : mesh.get_faces(cd.v0)) {
            std::copy(face_points[f].begin(), face_points[f].end(), std::back_inserter(p));
        }
        p.push_back(vpoint[cd.v0]);

        // test points against all faces
        vpoint[cd.v0] = p1;
        for (const auto &point : p) {
            ok = false;

            for (const auto f : mesh.get_faces(cd.v0)) {
                if (f != cd.fl && f != cd.fr) {
                    if (distance(f, point) < hausdorff_error) {
                        ok = true;
                        break;
                    }
                }
            }

            if (!ok) {
                vpoint[cd.v0] = p0;
                return false;
            }
        }
        vpoint[cd.v0] = p0;
    }

    // collapse passed all tests -> ok
    return true;
}

// what is the priority of collapsing the halfedge h
float simplification::priority(const collapse_data &cd) {
    // computer quadric error metric
    quadric Q = vquadric[cd.v0];
    Q += vquadric[cd.v1];
    return Q(vpoint[cd.v1]);
}

// postprocess halfedge collapse
void simplification::postprocess_collapse(const collapse_data &cd) {
    // update error quadrics
    vquadric[cd.v1] += vquadric[cd.v0];

    // update normal cones
    if (normal_deviation) {
        for (const auto f : mesh.get_faces(cd.v1)) {
            normal_cones[f].merge(face_normal(mesh, f));
        }

        if (cd.vl.is_valid()) {
            auto f = mesh.get_face(cd.v1vl);
            if (f.is_valid()) {
                normal_cones[f].merge(normal_cones[cd.fl]);
            }
        }

        if (cd.vr.is_valid()) {
            auto f = mesh.get_face(cd.vrv1);
            if (f.is_valid()) {
                normal_cones[f].merge(normal_cones[cd.fr]);
            }
        }
    }

    // update Hausdorff error
    if (hausdorff_error) {
        points p;

        // collect points to be distributed

        // points of v1's one-ring
        for (const auto f : mesh.get_faces(cd.v1)) {
            std::copy(face_points[f].begin(), face_points[f].end(),
                      std::back_inserter(p));
            face_points[f].clear();
        }

        // points of the 2 removed triangles
        if (cd.fl.is_valid()) {
            std::copy(face_points[cd.fl].begin(), face_points[cd.fl].end(),
                      std::back_inserter(p));
            points().swap(face_points[cd.fl]); // free mem
        }
        if (cd.fr.is_valid()) {
            std::copy(face_points[cd.fr].begin(), face_points[cd.fr].end(),
                      std::back_inserter(p));
            points().swap(face_points[cd.fr]); // free mem
        }

        // the removed vertex
        p.push_back(vpoint[cd.v0]);

        // test points against all faces
        bcg_scalar_t d, dd;
        face_handle ff;

        for (const auto &point : p) {
            dd = scalar_max;

            for (const auto f : mesh.get_faces(cd.v1)) {
                d = distance(f, point);
                if (d < dd) {
                    ff = f;
                    dd = d;
                }
            }

            face_points[ff].push_back(point);
        }
    }
}

// compute aspect ratio for face f
bcg_scalar_t simplification::compute_aspect_ratio(face_handle f) const {
    // min height is area/maxLength
    // aspect ratio = length / height
    //              = length * length / area

    auto fvit = mesh.get_vertices(f);

    const VectorS<3> p0 = vpoint[*fvit];
    const VectorS<3> p1 = vpoint[*(++fvit)];
    const VectorS<3> p2 = vpoint[*(++fvit)];

    const VectorS<3> d0 = p0 - p1;
    const VectorS<3> d1 = p1 - p2;
    const VectorS<3> d2 = p2 - p0;

    const bcg_scalar_t l0 = d0.squaredNorm();
    const bcg_scalar_t l1 = d1.squaredNorm();
    const bcg_scalar_t l2 = d2.squaredNorm();

    // max squared edge length
    const bcg_scalar_t l = std::max(l0, std::max(l1, l2));

    // triangle area
    const bcg_scalar_t a = d0.cross(d1).norm();

    return l / a;
}


// compute distance from p to triagle f
bcg_scalar_t simplification::distance(face_handle f, const VectorS<3> &p) const {
    auto fvit = mesh.get_vertices(f);

    const VectorS<3> p0 = vpoint[*fvit];
    const VectorS<3> p1 = vpoint[*(++fvit)];
    const VectorS<3> p2 = vpoint[*(++fvit)];

    distance_point3_triangle3 distance;
    auto result = distance(p, triangle3(p0, p1, p2));
    return result.distance;
}

void mesh_simplification(halfedge_mesh &mesh, unsigned int n_vertices, bcg_scalar_t aspect_ratio,
                         bcg_scalar_t edge_length, unsigned int max_valence,
                         bcg_scalar_t normal_deviation, bcg_scalar_t hausdorff_error) {
    simplification simplfy(mesh, aspect_ratio, edge_length, max_valence, normal_deviation, hausdorff_error);
    simplfy.simplify(n_vertices);
}

}
