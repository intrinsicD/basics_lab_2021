//
// Created by alex on 27.11.20.
//

#include "bcg_mesh_remeshing.h"
#include "bcg_mesh_vertex_normals.h"
#include "bcg_mesh_curvature_taubin.h"
#include "bcg_mesh_edge_cotan.h"
#include "bcg_mesh_triangle_area_from_metric.h"
#include "kdtree/bcg_triangle_kdtree.h"
#include "tbb/tbb.h"

namespace bcg {

struct remeshing {
    remeshing(halfedge_mesh &mesh) : mesh(mesh), refmesh(nullptr), kd_tree(nullptr) {
        points = mesh.positions;

        vertex_normals(mesh, vertex_normal_area_angle);
        vnormal = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    }

    halfedge_mesh &mesh;
    halfedge_mesh *refmesh;

    size_t parallel_grain_size = 1024;

    bool use_projection;
    triangle_kdtree *kd_tree;
    //kdtree_property<bcg_scalar_t> *kdtreeProperty;

    bool uniform;
    bcg_scalar_t target_edge_length;
    bcg_scalar_t min_edge_length;
    bcg_scalar_t max_edge_length;
    bcg_scalar_t approx_error;

    property<VectorS<3>, 3> points;
    property<VectorS<3>, 3> vnormal;
    property<bool, 1> vfeature;
    property<bool, 1> efeature;
    property<bool, 1> vlocked;
    property<bool, 1> elocked;
    property<bcg_scalar_t, 1> vsizing;

    property<VectorS<3>, 3> refpoints;
    property<VectorS<3>, 3> refnormals;
    property<bcg_scalar_t, 1> refsizing;

    void preprocessing();

    void postprocessing();

    void split_long_edges();

    void collapse_short_edges();

    void flip_edges();

    void tangential_smoothing(unsigned int iterations);

    void remove_caps();

    void project_to_reference(vertex_handle v);

    bool is_too_long(vertex_handle v0, vertex_handle v1) const {
        return (points[v0] - points[v1]).norm() >
               4.0 / 3.0 * std::min(vsizing[v0], vsizing[v1]);
    }

    bool is_too_short(vertex_handle v0, vertex_handle v1) const {
        return (points[v0] - points[v1]).norm() <
               4.0 / 5.0 * std::min(vsizing[v0], vsizing[v1]);
    }
};

void remeshing::preprocessing() {
// properties
    vfeature = mesh.vertices.get_or_add<bool, 1>("v_feature", false);
    efeature = mesh.edges.get_or_add<bool, 1>("e_feature", false);
    vlocked = mesh.vertices.get_or_add<bool, 1>("v_locked", false);
    elocked = mesh.edges.get_or_add<bool, 1>("e_locked", false);
    vsizing = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_sizing");

    // lock unselected vertices if some vertices are selected
    auto vselected = mesh.vertices.get<int, 1>("v_selected");

    if (vselected) {
        bool has_selection = false;
        for (const auto v : mesh.vertices) {
            if (vselected[v]) {
                has_selection = true;
                break;
            }
        }

        if (has_selection) {
            tbb::parallel_for(
                    tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                    [&](const tbb::blocked_range<uint32_t> &range) {
                        for (uint32_t i = range.begin(); i != range.end(); ++i) {
                            auto v = vertex_handle(i);
                            vlocked[v] = !vselected[v];
                        }
                    }
            );
            // lock an edge if one of its vertices is locked
            tbb::parallel_for(
                    tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
                    [&](const tbb::blocked_range<uint32_t> &range) {
                        for (uint32_t i = range.begin(); i != range.end(); ++i) {
                            auto e = edge_handle(i);
                            elocked[e] = (vlocked[mesh.get_vertex(e, 0)] || vlocked[mesh.get_vertex(e, 1)]);
                        }
                    }
            );
        }
    }

    // lock feature corners
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    if (vfeature[v]) {
                        int c = 0;
                        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                            if (efeature[mesh.get_edge(h)]) {
                                ++c;
                            }
                        }

                        if (c != 2) {
                            vlocked[v] = true;
                        }
                    }
                }
            }
    );

/*    for (const auto v : mesh.vertices) {
        if (vfeature[v]) {
            int c = 0;
            for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                if (efeature[mesh.get_edge(h)]) {
                    ++c;
                }
            }

            if (c != 2) {
                vlocked[v] = true;
            }
        }
    }*/


    // compute sizing field
    if (uniform) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        vsizing[v] = target_edge_length;
                    }
                }
        );
/*        for (const auto v : mesh.vertices) {
            vsizing[v] = target_edge_length;
        }*/
    } else {
        // compute curvature for all mesh vertices, using cotan or Cohen-Steiner
        // do 2 post-smoothing steps to get a smoother sizing field
        mesh_curvature_taubin(mesh, 1, true);
        auto min_curvature = mesh.vertices.get<bcg_scalar_t, 1>("v_mesh_curv_min");
        auto max_curvature = mesh.vertices.get<bcg_scalar_t, 1>("v_mesh_curv_max");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
// maximum absolute curvature
                        bcg_scalar_t c = std::max(std::abs(min_curvature[v]), std::abs(max_curvature[v]));

                        // curvature of feature vertices: average of non-feature neighbors
                        if (vfeature[v]) {
                            vertex_handle vv;
                            bcg_scalar_t w, ww = 0.0;
                            c = 0.0;

                            for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                                vv = mesh.get_to_vertex(h);
                                if (!vfeature[vv]) {
                                    w = std::max<bcg_scalar_t >(0.0, edge_cotan(mesh, mesh.get_edge(h)));
                                    ww += w;
                                    c += w * std::max<bcg_scalar_t>(std::abs(min_curvature[vv]), std::abs(max_curvature[vv]));
                                }
                            }

                            c /= ww;
                        }

                        assert(c != 0);
                        // get edge length from curvature
                        const auto r = 1.0 / c;
                        const auto e = approx_error;
                        bcg_scalar_t h;
                        if (e < r) {
                            // see mathworld: "circle segment" and "equilateral triangle"
                            //h = sqrt(2.0*r*e-e*e) * 3.0 / sqrt(3.0);
                            h = sqrt(6.0 * e * r - 3.0 * e * e); // simplified...
                        } else {
                            // this does not really make sense
                            h = e * 3.0 / sqrt(3.0);
                        }

                        // clamp to min. and max. edge length
                        if (h < min_edge_length) {
                            h = min_edge_length;
                        } else if (h > max_edge_length) {
                            h = max_edge_length;
                        }

                        // store target edge length
                        vsizing[v] = h;
                    }
                }
        );
        /*for (const auto v : mesh.vertices) {
            // maximum absolute curvature
            bcg_scalar_t c = std::max(std::abs(min_curvature[v]), std::abs(max_curvature[v]));

            // curvature of feature vertices: average of non-feature neighbors
            if (vfeature[v]) {
                vertex_handle vv;
                bcg_scalar_t w, ww = 0.0;
                c = 0.0;

                for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                    vv = mesh.get_to_vertex(h);
                    if (!vfeature[vv]) {
                        w = std::max(0.0f, edge_cotan(mesh, mesh.get_edge(h)));
                        ww += w;
                        c += w * std::max(std::abs(min_curvature[vv]), std::abs(max_curvature[vv]));
                    }
                }

                c /= ww;
            }

            assert(c != 0);
            // get edge length from curvature
            const auto r = 1.0 / c;
            const auto e = approx_error;
            bcg_scalar_t h;
            if (e < r) {
                // see mathworld: "circle segment" and "equilateral triangle"
                //h = sqrt(2.0*r*e-e*e) * 3.0 / sqrt(3.0);
                h = sqrt(6.0 * e * r - 3.0 * e * e); // simplified...
            } else {
                // this does not really make sense
                h = e * 3.0 / sqrt(3.0);
            }

            // clamp to min. and max. edge length
            if (h < min_edge_length) {
                h = min_edge_length;
            } else if (h > max_edge_length) {
                h = max_edge_length;
            }

            // store target edge length
            vsizing[v] = h;
        }*/
    }

    if (use_projection) {
        // build reference mesh
        refmesh = new halfedge_mesh();
        refmesh->assign(mesh);
        vertex_normals(*refmesh, vertex_normal_area_angle);
        refpoints = refmesh->positions;
        refnormals = refmesh->vertices.get<VectorS<3>, 3>("v_normal");

        assert(refpoints);
        assert(refnormals);
        // copy sizing field from mesh
        refsizing = refmesh->vertices.get_or_add<bcg_scalar_t, 1>("v_sizing");
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) refmesh->vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        refsizing[v] = vsizing[v];
                    }
                }
        );
/*        for (const auto v : refmesh->vertices) {
            refsizing[v] = vsizing[v];
        }*/

        // build kd-tree
        kd_tree = new triangle_kdtree(*refmesh, 10);
        //kdtreeProperty = new kdtree_property<bcg_scalar_t >(refpoints, 10);
    }
}

void remeshing::postprocessing() {
// delete kd-tree and reference mesh
    if (use_projection) {
        delete kd_tree;
        delete refmesh;
    }

    auto lines = mesh.edges.get_or_add<VectorI<2>, 2>("edges");
    lines.vector() = mesh.get_connectivity();
    lines.set_dirty();

    auto triangles = mesh.edges.get_or_add<VectorI<3>, 3>("triangles");
    triangles = mesh.get_triangles();
    triangles.set_dirty();

    // remove properties
    mesh.vertices.remove(vlocked);
    mesh.edges.remove(elocked);
    mesh.vertices.remove(vsizing);
}

void remeshing::split_long_edges() {
    vertex_handle vnew, v0, v1;
    edge_handle enew, e0, e1;
    face_handle f0, f1, f2, f3;
    bool ok, is_feature, is_boundary;
    int i;

    for (ok = false, i = 0; !ok && i < 10; ++i) {
        ok = true;

        halfedge_handle h_;
        for (const auto e : mesh.edges) {
            v0 = mesh.get_vertex(e, 0);
            v1 = mesh.get_vertex(e, 1);

            if (!elocked[e] && is_too_long(v0, v1)) {
                const VectorS<3> &p0 = points[v0];
                const VectorS<3> &p1 = points[v1];

                is_feature = efeature[e];
                is_boundary = mesh.is_boundary(e);

                vnew = mesh.add_vertex((p0 + p1) * 0.5f);
                h_ = mesh.split(e, vnew);

                // need normal or sizing for adaptive refinement
                vnormal[vnew] = vertex_normal_area_angle(mesh, vnew);
                vsizing[vnew] = 0.5f * (vsizing[v0] + vsizing[v1]);

                if (is_feature) {
                    enew = is_boundary ? edge_handle(mesh.num_edges() - 2)
                                       : edge_handle(mesh.num_edges() - 3);
                    efeature[enew] = true;
                    vfeature[vnew] = true;
                } else {
                    project_to_reference(vnew);
                }

                ok = false;
            }
        }
    }
}

void remeshing::collapse_short_edges() {
    vertex_handle v0, v1;
    halfedge_handle h0, h1, h01, h10;
    bool ok, b0, b1, l0, l1, f0, f1;
    int i;
    bool hcol01, hcol10;

    for (ok = false, i = 0; !ok && i < 10; ++i) {
        ok = true;

        for (const auto e : mesh.edges) {
            if (!mesh.edges_deleted[e] && !elocked[e]) {
                h10 = mesh.halfedge_graph::get_halfedge(e, 0);
                h01 = mesh.halfedge_graph::get_halfedge(e, 1);
                v0 = mesh.get_to_vertex(h10);
                v1 = mesh.get_to_vertex(h01);

                if (is_too_short(v0, v1)) {
                    // get status
                    b0 = mesh.is_boundary(v0);
                    b1 = mesh.is_boundary(v1);
                    l0 = vlocked[v0];
                    l1 = vlocked[v1];
                    f0 = vfeature[v0];
                    f1 = vfeature[v1];
                    hcol01 = hcol10 = true;

                    // boundary rules
                    if (b0 && b1) {
                        if (!mesh.is_boundary(e)) {
                            continue;
                        }
                    } else if (b0) {
                        hcol01 = false;
                    } else if (b1) {
                        hcol10 = false;
                    }

                    // locked rules
                    if (l0 && l1) {
                        continue;
                    } else if (l0) {
                        hcol01 = false;
                    } else if (l1) {
                        hcol10 = false;
                    }

                    // feature rules
                    if (f0 && f1) {
                        // edge must be feature
                        if (!efeature[e]) {
                            continue;
                        }

                        // the other two edges removed by collapse must not be features
                        h0 = mesh.get_prev(h01);
                        h1 = mesh.get_next(h10);
                        if (efeature[mesh.get_edge(h0)] || efeature[mesh.get_edge(h1)]) {
                            hcol01 = false;
                        }
                        // the other two edges removed by collapse must not be features
                        h0 = mesh.get_prev(h10);
                        h1 = mesh.get_next(h01);
                        if (efeature[mesh.get_edge(h0)] || efeature[mesh.get_edge(h1)]) {
                            hcol10 = false;
                        }
                    } else if (f0) {
                        hcol01 = false;
                    } else if (f1) {
                        hcol10 = false;
                    }

                    // topological rules
                    bool collapse_ok = mesh.is_collapse_ok(h01);

                    if (hcol01) {
                        hcol01 = collapse_ok;
                    }
                    if (hcol10) {
                        hcol10 = collapse_ok;
                    }

                    // both collapses possible: collapse into vertex w/ higher valence
                    if (hcol01 && hcol10) {
                        if (mesh.halfedge_graph::get_valence(v0) < mesh.halfedge_graph::get_valence(v1)) {
                            hcol10 = false;
                        } else {
                            hcol01 = false;
                        }
                    }

                    // try v1 -> v0
                    if (hcol10) {
                        // don't create too long edges
                        for (const auto vv : mesh.halfedge_graph::get_vertices(v1)) {
                            if (is_too_long(v0, vv)) {
                                hcol10 = false;
                                break;
                            }
                        }

                        if (hcol10) {
                            mesh.collapse(h10);
                            ok = false;
                        }
                    }

                        // try v0 -> v1
                    else if (hcol01) {
                        // don't create too long edges
                        for (const auto vv : mesh.halfedge_graph::get_vertices(v0)) {
                            if (is_too_long(v1, vv)) {
                                hcol01 = false;
                                break;
                            }
                        }

                        if (hcol01) {
                            mesh.collapse(h01);
                            ok = false;
                        }
                    }
                }
            }
        }
    }

    mesh.garbage_collection();
}

void remeshing::flip_edges() {
    vertex_handle v0, v1, v2, v3;
    halfedge_handle h;
    int val0, val1, val2, val3;
    int val_opt0, val_opt1, val_opt2, val_opt3;
    int ve0, ve1, ve2, ve3, ve_before, ve_after;
    bool ok;
    int i;

    // precompute valences
    auto valence = mesh.vertices.get_or_add<int, 1>("valence");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    valence[v] = mesh.halfedge_graph::get_valence(v);
                }
            }
    );
/*    for (const auto v : mesh.vertices) {
        valence[v] = mesh.halfedge_graph::get_valence(v);
    }*/

    for (ok = false, i = 0; !ok && i < 10; ++i) {
        ok = true;

        for (const auto e : mesh.edges) {
            if (!elocked[e] && !efeature[e]) {
                h = mesh.halfedge_graph::get_halfedge(e, 0);
                v0 = mesh.get_to_vertex(h);
                v2 = mesh.get_to_vertex(mesh.get_next(h));
                h = mesh.halfedge_graph::get_halfedge(e, 1);
                v1 = mesh.get_to_vertex(h);
                v3 = mesh.get_to_vertex(mesh.get_next(h));

                if (!vlocked[v0] && !vlocked[v1] && !vlocked[v2] &&
                    !vlocked[v3]) {
                    val0 = valence[v0];
                    val1 = valence[v1];
                    val2 = valence[v2];
                    val3 = valence[v3];

                    val_opt0 = (mesh.is_boundary(v0) ? 4 : 6);
                    val_opt1 = (mesh.is_boundary(v1) ? 4 : 6);
                    val_opt2 = (mesh.is_boundary(v2) ? 4 : 6);
                    val_opt3 = (mesh.is_boundary(v3) ? 4 : 6);

                    ve0 = (val0 - val_opt0);
                    ve1 = (val1 - val_opt1);
                    ve2 = (val2 - val_opt2);
                    ve3 = (val3 - val_opt3);

                    ve0 *= ve0;
                    ve1 *= ve1;
                    ve2 *= ve2;
                    ve3 *= ve3;

                    ve_before = ve0 + ve1 + ve2 + ve3;

                    --val0;
                    --val1;
                    ++val2;
                    ++val3;

                    ve0 = (val0 - val_opt0);
                    ve1 = (val1 - val_opt1);
                    ve2 = (val2 - val_opt2);
                    ve3 = (val3 - val_opt3);

                    ve0 *= ve0;
                    ve1 *= ve1;
                    ve2 *= ve2;
                    ve3 *= ve3;

                    ve_after = ve0 + ve1 + ve2 + ve3;

                    if (ve_before > ve_after && mesh.is_flip_ok(e)) {
                        mesh.flip(e);
                        --valence[v0];
                        --valence[v1];
                        ++valence[v2];
                        ++valence[v3];
                        ok = false;
                    }
                }
            }
        }
    }

    mesh.vertices.remove(valence);
}

void remeshing::tangential_smoothing(unsigned int iterations) {
    vertex_handle v1, v2, v3, vv;
    edge_handle e;
    bcg_scalar_t w, ww, area;
    VectorS<3> u, n, t, b;

    // add property
    auto update = mesh.vertices.add<VectorS<3>, 3>("v_update");

    // project at the beginning to get valid sizing values and normal vectors
    // for vertices introduced by splitting
    if (use_projection) {
        for (const auto v : mesh.vertices) {
            if (!mesh.is_boundary(v) && !vlocked[v]) {
                project_to_reference(v);
            }
        }
    }

    for (unsigned int iters = 0; iters < iterations; ++iters) {
        /*tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        if (!mesh.is_boundary(v) && !vlocked[v]) {
                            if (vfeature[v]) {
                                u = VectorS<3>::Zero();
                                t = VectorS<3>::Zero();
                                ww = 0;
                                int c = 0;

                                for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                                    if (efeature[mesh.get_edge(h)]) {
                                        vv = mesh.get_to_vertex(h);

                                        b = points[v];
                                        b += points[vv];
                                        b *= 0.5;

                                        w = (points[v] - points[vv]).norm() / (0.5 * (vsizing[v] + vsizing[vv]));
                                        ww += w;
                                        u += w * b;

                                        if (c == 0) {
                                            t += (points[vv] - points[v]).normalized();
                                            ++c;
                                        } else {
                                            ++c;
                                            t -= (points[vv] - points[v]).normalized();
                                        }
                                    }
                                }

                                //assert(c == 2);

                                u *= (1.0 / ww);
                                u -= points[v];
                                t = t.normalized();
                                u = t * u.dot(t);

                                update[v] = u;
                            } else {
                                u = VectorS<3>::Zero();
                                t = VectorS<3>::Zero();
                                ww = 0;

                                for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                                    v1 = v;
                                    v2 = mesh.get_to_vertex(h);
                                    v3 = mesh.get_to_vertex(mesh.get_next(h));

                                    b = points[v1];
                                    b += points[v2];
                                    b += points[v3];
                                    b *= (1.0 / 3.0);

                                    area = triangle_area_from_metric((points[v2] - points[v1]).norm(),
                                                                     (points[v3] - points[v1]).norm(),
                                                                     (points[v3] - points[v2]).norm());
                                    w = area / pow((vsizing[v1] + vsizing[v2] + vsizing[v3]) / 3.0, 2.0);

                                    u += w * b;
                                    ww += w;
                                }

                                u /= ww;
                                u -= points[v];
                                n = vnormal[v];
                                u -= n * u.dot(n);

                                update[v] = u;
                            }
                        }
                    }
                }
        );*/
        for (const auto v : mesh.vertices) {
            if (!mesh.is_boundary(v) && !vlocked[v]) {
                if (vfeature[v]) {
                    u = VectorS<3>::Zero();
                    t = VectorS<3>::Zero();
                    ww = 0;
                    int c = 0;

                    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                        if (efeature[mesh.get_edge(h)]) {
                            vv = mesh.get_to_vertex(h);

                            b = points[v];
                            b += points[vv];
                            b *= 0.5;

                            w = (points[v] - points[vv]).norm() / (0.5 * (vsizing[v] + vsizing[vv]));
                            ww += w;
                            u += w * b;

                            if (c == 0) {
                                t += (points[vv] - points[v]).normalized();
                                ++c;
                            } else {
                                ++c;
                                t -= (points[vv] - points[v]).normalized();
                            }
                        }
                    }

                    //assert(c == 2);

                    u *= (1.0 / ww);
                    u -= points[v];
                    t = t.normalized();
                    u = t * u.dot(t);

                    update[v] = u;
                } else {
                    u = VectorS<3>::Zero();
                    t = VectorS<3>::Zero();
                    ww = 0;

                    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                        v1 = v;
                        v2 = mesh.get_to_vertex(h);
                        v3 = mesh.get_to_vertex(mesh.get_next(h));

                        b = points[v1];
                        b += points[v2];
                        b += points[v3];
                        b *= (1.0 / 3.0);

                        area = triangle_area_from_metric((points[v2] - points[v1]).norm(),
                                                         (points[v3] - points[v1]).norm(),
                                                         (points[v3] - points[v2]).norm());
                        w = area / pow((vsizing[v1] + vsizing[v2] + vsizing[v3]) / 3.0, 2.0);

                        u += w * b;
                        ww += w;
                    }

                    u /= ww;
                    u -= points[v];
                    n = vnormal[v];
                    u -= n * u.dot(n);

                    update[v] = u;
                }
            }
        }

        // update vertex positions

        for (const auto v : mesh.vertices) {
            if (!mesh.is_boundary(v) && !vlocked[v]) {
                points[v] += update[v];
            }
        }

        // update normal vectors (if not done so through projection)
        vertex_normals(mesh, vertex_normal_area_angle);
    }

    // project at the end
    if (use_projection) {
        for (const auto v : mesh.vertices) {
            if (!mesh.is_boundary(v) && !vlocked[v]) {
                project_to_reference(v);
            }
        }
    }

    // remove property
    mesh.vertices.remove(update);
}

void remeshing::remove_caps() {
    halfedge_handle h;
    vertex_handle v, vb, vd;
    face_handle fb, fd;
    bcg_scalar_t a0, a1, amin, aa(::cos(170.0 * pi / 180.0));
    VectorS<3> a, b, c, d;

    for (const auto e : mesh.edges) {
        if (!elocked[e] && mesh.is_flip_ok(e)) {
            h = mesh.halfedge_graph::get_halfedge(e, 0);
            a = points[mesh.get_to_vertex(h)];

            h = mesh.get_next(h);
            b = points[vb = mesh.get_to_vertex(h)];

            h = mesh.halfedge_graph::get_halfedge(e, 1);
            c = points[mesh.get_to_vertex(h)];

            h = mesh.get_next(h);
            d = points[vd = mesh.get_to_vertex(h)];

            a0 = (a - b).normalized().dot((c - b).normalized());
            a1 = (a - d).normalized().dot((c - d).normalized());

            if (a0 < a1) {
                amin = a0;
                v = vb;
            } else {
                amin = a1;
                v = vd;
            }

            // is it a cap?
            if (amin < aa) {
                // feature edge and feature vertex -> seems to be intended
                if (efeature[e] && vfeature[v]) {
                    continue;
                }

                // project v onto feature edge
                if (efeature[e]) {
                    points[v] = (a + c) * 0.5f;
                }

                // flip
                mesh.flip(e);
            }
        }
    }
}

void remeshing::project_to_reference(vertex_handle v) {
    if (!use_projection) {
        return;
    }

    // find closest triangle of reference mesh
    triangle_kdtree::NearestNeighbor nn = kd_tree->nearest(points[v]);
    auto fvIt = refmesh->get_vertices(nn.face);
    /*auto result = kdtreeProperty->query_knn(points[v], 1);

    // get face data
    vertex_handle vn = result.indices[0];
    face_handle f = refmesh->find_closest_face_in_neighborhood(vn, points[v]);
    auto fvIt = refmesh->get_vertices(f);*/

    vertex_handle v0 = (*fvIt);
    vertex_handle v1 = (*(++fvIt));
    vertex_handle v2 = (*(++fvIt));

    triangle3 t(refpoints[v0], refpoints[v1], refpoints[2]);

    // set result
    points[v] = nn.result.closest;
    triangle3 tn(refnormals[v0], refnormals[v1], refnormals[2]);
    vnormal[v] = from_barycentric_coords(tn, nn.result.barycentric_coords).normalized();
    vsizing[v] = refsizing[v0] * nn.result.barycentric_coords[0] + refsizing[v1] * nn.result.barycentric_coords[1] +
                 refsizing[v2] * nn.result.barycentric_coords[2];

/*    triangle3 tn(refnormals[v0] ,refnormals[v1], refnormals[2]);
    auto barycentric_coords = to_barycentric_coordinates(t, points[v]);
    points[v] = from_barycentric_coords(t, barycentric_coords);
    vnormal[v] = from_barycentric_coords(tn, barycentric_coords).normalized();
    vsizing[v] = refsizing[v0] * barycentric_coords[0] + refsizing[v1] * barycentric_coords[1] + refsizing[v2] * barycentric_coords[2];*/
}

void mesh_remeshing_uniform(halfedge_mesh &mesh, bcg_scalar_t edge_length, unsigned int iterations,
                            bool use_projection) {

    if (!mesh.is_triangle_mesh()) {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    remeshing meshing(mesh);

    meshing.uniform = true;
    meshing.use_projection = use_projection;
    meshing.target_edge_length = edge_length;

    meshing.preprocessing();

    for (unsigned int i = 0; i < iterations; ++i) {
        meshing.split_long_edges();

        vertex_normals(mesh, vertex_normal_area_angle);

        meshing.collapse_short_edges();

        meshing.flip_edges();

        meshing.tangential_smoothing(5);
    }

    meshing.remove_caps();

    meshing.postprocessing();
}

void mesh_remeshing_adaptive(halfedge_mesh &mesh, bcg_scalar_t min_edge_length, bcg_scalar_t max_edge_length,
                             bcg_scalar_t approx_error, unsigned int iterations, bool use_projection) {
    if (!mesh.is_triangle_mesh()) {
        std::cerr << "Not a triangle mesh!" << std::endl;
        return;
    }

    remeshing meshing(mesh);

    meshing.uniform = false;
    meshing.min_edge_length = min_edge_length;
    meshing.max_edge_length = max_edge_length;
    meshing.approx_error = approx_error;
    meshing.use_projection = use_projection;

    meshing.preprocessing();

    for (unsigned int i = 0; i < iterations; ++i) {
        meshing.split_long_edges();

        vertex_normals(mesh, vertex_normal_area_angle);

        meshing.collapse_short_edges();

        meshing.flip_edges();

        meshing.tangential_smoothing(5);
    }

    meshing.remove_caps();

    meshing.postprocessing();
}

}
