//
// Created by alex on 27.11.20.
//
#include "bcg_mesh_simplification.h"

#include <utility>
#include "bcg_mesh_face_normals.h"
#include "distance_query/bcg_distance_triangle_point.h"
#include "utils/bcg_heap.h"
#include "math/bcg_probabilistic_quadric.h"

namespace bcg {

using quadric = pq::quadric<pq::math<bcg_scalar_t, VectorS<3>, VectorS<3>, MatrixS<3, 3>>>;

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
    bcg_scalar_t priority(const collapse_data &cd);

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
                                                                                              normal_deviation(
                                                                                                      normal_deviation /
                                                                                                      180.0 * pi),
                                                                                              hausdorff_error(
                                                                                                      hausdorff_error),
                                                                                              aspect_ratio(
                                                                                                      aspect_ratio),
                                                                                              edge_length(edge_length),
                                                                                              max_valence(max_valence)
{

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
        vquadric[v] = quadric();

        if (!mesh.is_isolated(v)) {
            for (const auto f : mesh.get_faces(v)) {
                //vquadric[v] += quadric::probabilistic_plane_quadric(vpoint[v], fnormal[f], 1, 1);
                std::vector<VectorS<3>> V;
                for (const auto vf : mesh.get_vertices(f)) {
                    V.push_back(vpoint[vf]);
                }
                vquadric[v] += quadric::probabilistic_triangle_quadric(V[0], V[1], V[2], 1);
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
    if (normal_deviation < scalar_eps) {
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
bcg_scalar_t simplification::priority(const collapse_data &cd) {
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
