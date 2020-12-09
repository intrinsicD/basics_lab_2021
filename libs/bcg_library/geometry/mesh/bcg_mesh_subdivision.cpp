//
// Created by alex on 19.11.20.
//

#include "bcg_mesh_subdivision.h"
#include "geometry/triangle/bcg_triangle.h"
#include "geometry/triangle/bcg_triangle_centers.h"
#include "tbb/tbb.h"

namespace bcg {

void post_postprocessing(halfedge_mesh &mesh) {
    if (!mesh.is_triangle_mesh()) {
        mesh.triangulate();
    }

    auto lines = mesh.edges.get_or_add<VectorI<2>, 2>("edges");
    lines.vector() = mesh.get_connectivity();
    lines.set_dirty();
    auto triangles = mesh.edges.get_or_add<VectorI<3>, 3>("triangles");
    triangles = mesh.get_triangles();
    triangles.set_dirty();
}

void mesh_subdivision_catmull_clark(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto positions = mesh.positions;
    auto v_feature = mesh.vertices.get<bool, 1>("v_feature");
    auto e_feature = mesh.edges.get<bool, 1>("e_feature");

    // reserve memory
    auto nv = mesh.vertices.size();
    auto ne = mesh.edges.size();
    auto nf = mesh.faces.size();
    mesh.vertices.reserve(nv + ne + nf);
    mesh.edges.reserve(2 * ne + 4 * nf);
    mesh.faces.reserve(4 * nf);

    // get properties
    auto v_point = mesh.vertices.add<VectorS<3>, 3>("catmull:vpoint");
    auto e_point = mesh.edges.add<VectorS<3>, 3>("catmull:epoint");
    auto f_point = mesh.faces.add<VectorS<3>, 3>("catmull:fpoint");

    // compute face vertices
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    std::vector<vertex_handle> V;
                    for (const auto v: mesh.get_vertices(f)) {
                        V.push_back(v);
                    }
                    const auto t = triangle3(positions[V[0]], positions[V[1]], positions[V[2]]);
                    f_point[f] = centroid(t);
                }
            }
    );

    // compute edge vertices
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    if (mesh.is_boundary(e) || (e_feature && e_feature[e])) {
                        e_point[e] = 0.5f * (positions[mesh.get_vertex(e, 0)] + positions[mesh.get_vertex(e, 1)]);
                    } else {
                        VectorS<3> p(VectorS<3>::Zero());
                        p += positions[mesh.get_vertex(e, 0)];
                        p += positions[mesh.get_vertex(e, 1)];
                        p += f_point[mesh.get_face(e, 0)];
                        p += f_point[mesh.get_face(e, 1)];
                        p *= 0.25f;
                        e_point[e] = p;
                    }
                }
            }
    );



    // compute new positions for old vertices
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    // isolated vertex?
                    if (mesh.is_isolated(v)) {
                        v_point[v] = positions[v];
                    } else if (mesh.is_boundary(v)) {
                        auto h1 = mesh.halfedge_graph::get_halfedge(v);
                        auto h0 = mesh.get_prev(h1);

                        VectorS<3> p = positions[v];
                        p *= 6.0;
                        p += positions[mesh.get_to_vertex(h1)];
                        p += positions[mesh.get_from_vertex(h0)];
                        p *= 0.125;

                        v_point[v] = p;
                    } else if (v_feature && v_feature[v]) {
                        VectorS<3> p = positions[v];
                        p *= 6.0;
                        int count(0);

                        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                            if (e_feature[mesh.get_edge(h)]) {
                                p += positions[mesh.get_to_vertex(h)];
                                ++count;
                            }
                        }

                        if (count == 2) // vertex is on feature edge
                        {
                            p *= 0.125;
                            v_point[v] = p;
                        } else {
                            v_point[v] = positions[v];
                        }
                    } else {
                        // weights from SIGGRAPH paper "Subdivision Surfaces in Character Animation"

                        const bcg_scalar_t k = mesh.halfedge_graph::get_valence(v);
                        VectorS<3> p(VectorS<3>::Zero());

                        for (const auto vv : mesh.halfedge_graph::get_vertices(v)) {
                            p += positions[vv];
                        }

                        for (const auto f : mesh.get_faces(v)) {
                            p += f_point[f];
                        }

                        p /= (k * k);

                        p += ((k - 2.0) / k) * positions[v];

                        v_point[v] = p;
                    }
                }
            }
    );

    // assign new positions to old vertices
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    positions[v] = v_point[v];
                }
            }
    );

    // split edges
    halfedge_handle h_;
    for (const auto e : mesh.edges) {
        // feature edge?
        if (e_feature && e_feature[e]) {
            auto h = mesh.insert_vertex(e, e_point[e]);
            auto v = mesh.get_to_vertex(h);
            auto e0 = mesh.get_edge(h);
            auto e1 = mesh.get_edge(mesh.get_next(h));

            v_feature[v] = true;
            e_feature[e0] = true;
            e_feature[e1] = true;
        }

            // normal edge
        else {
            h_ = mesh.insert_vertex(e, e_point[e]);
        }
    }

    // split faces
    for (const auto f : mesh.faces) {
        auto h0 = mesh.get_halfedge(f);
        h_ = mesh.insert_edge(h0, mesh.get_next(mesh.get_next(h0)));

        auto h1 = mesh.get_next(h0);
        h_ = mesh.insert_vertex(mesh.get_edge(h1), f_point[f]);

        auto h = mesh.get_next(mesh.get_next(mesh.get_next(h1)));
        while (h != h0) {
            h_ = mesh.insert_edge(h1, h);
            h = mesh.get_next(mesh.get_next(mesh.get_next(h1)));
        }
    }

    // clean-up properties
    mesh.vertices.remove(v_point);
    mesh.edges.remove(e_point);
    mesh.faces.remove(f_point);

    post_postprocessing(mesh);
}

void mesh_subdivision_loop(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto positions = mesh.positions;
    auto v_feature = mesh.vertices.get<bool, 1>("v_feature");
    auto e_feature = mesh.edges.get<bool, 1>("e_feature");

    if (!mesh.is_triangle_mesh()) {
        mesh.triangulate();
    }

    // reserve memory
    auto nv = mesh.vertices.size();
    auto ne = mesh.edges.size();
    auto nf = mesh.faces.size();
    mesh.vertices.reserve(nv + ne);
    mesh.edges.reserve(2 * ne + 3 * nf);
    mesh.faces.reserve(4 * nf);

    // add properties
    auto v_point = mesh.vertices.add<VectorS<3>, 3>("loop:vpoint");
    auto e_point = mesh.edges.add<VectorS<3>, 3>("loop:epoint");

    // compute vertex positions
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    if (mesh.is_isolated(v)) {
                        v_point[v] = positions[v];
                    } else if (mesh.is_boundary(v)) {
                        auto h1 = mesh.halfedge_graph::get_halfedge(v);
                        auto h0 = mesh.get_prev(h1);

                        VectorS<3> p = positions[v];
                        p *= 6.0;
                        p += positions[mesh.get_to_vertex(h1)];
                        p += positions[mesh.get_from_vertex(h0)];
                        p *= 0.125;
                        v_point[v] = p;
                    } else if (v_feature && v_feature[v]) {
                        VectorS<3> p = positions[v];
                        p *= 6.0;
                        int count(0);

                        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                            if (e_feature[mesh.get_edge(h)]) {
                                p += positions[mesh.get_to_vertex(h)];
                                ++count;
                            }
                        }

                        if (count == 2) // vertex is on feature edge
                        {
                            p *= 0.125;
                            v_point[v] = p;
                        } else // keep fixed
                        {
                            v_point[v] = positions[v];
                        }
                    } else {
                        VectorS<3> p(VectorS<3>::Zero());
                        bcg_scalar_t k(0);

                        for (const auto vv : mesh.halfedge_graph::get_vertices(v)) {
                            p += positions[vv];
                            ++k;
                        }
                        p /= k;

                        auto beta = (0.625 - std::pow(0.375 + 0.25 * std::cos(2.0 * pi / k), 2.0));

                        v_point[v] = positions[v] * (1.0 - beta) + beta * p;
                    }
                }
            }
    );

    // compute edge positions
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    // boundary or feature edge?
                    if (mesh.is_boundary(e) || (e_feature && e_feature[e])) {
                        e_point[e] = (positions[mesh.get_vertex(e, 0)] + positions[mesh.get_vertex(e, 1)]) * 0.5;
                    } else {
                        auto h0 = mesh.halfedge_graph::get_halfedge(e, 0);
                        auto h1 = mesh.halfedge_graph::get_halfedge(e, 1);
                        VectorS<3> p = positions[mesh.get_to_vertex(h0)];
                        p += positions[mesh.get_to_vertex(h1)];
                        p *= 3.0;
                        p += positions[mesh.get_to_vertex(mesh.get_next(h0))];
                        p += positions[mesh.get_to_vertex(mesh.get_next(h1))];
                        p *= 0.125;
                        e_point[e] = p;
                    }
                }
            }
    );

    // set new vertex positions
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    positions[v] = v_point[v];
                }
            }
    );

    // insert new vertices on edges
    halfedge_handle h_;
    for (const auto e : mesh.edges) {
        // feature edge?
        if (e_feature && e_feature[e]) {
            auto h = mesh.insert_vertex(e, e_point[e]);
            auto v = mesh.get_to_vertex(h);
            auto e0 = mesh.get_edge(h);
            auto e1 = mesh.get_edge(mesh.get_next(h));

            v_feature[v] = true;
            e_feature[e0] = true;
            e_feature[e1] = true;
        }

            // normal edge
        else {
            h_ = mesh.insert_vertex(e, e_point[e]);
        }
    }

    // split faces
    halfedge_handle h;
    for (const auto f : mesh.faces) {
        h = mesh.get_halfedge(f);
        h_ = mesh.insert_edge(h, mesh.get_next(mesh.get_next(h)));
        h = mesh.get_next(h);
        h_ = mesh.insert_edge(h, mesh.get_next(mesh.get_next(h)));
        h = mesh.get_next(h);
        h_ = mesh.insert_edge(h, mesh.get_next(mesh.get_next(h)));
    }

    // clean-up properties
    mesh.vertices.remove(v_point);
    mesh.edges.remove(e_point);

    post_postprocessing(mesh);
}

void mesh_subdivision_sqrt3(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto positions = mesh.positions;
    auto v_feature = mesh.vertices.get<bool, 1>("v_feature");
    auto e_feature = mesh.edges.get<bool, 1>("e_feature");

    // reserve memory
    auto nv = mesh.vertices.size();
    auto ne = mesh.edges.size();
    auto nf = mesh.faces.size();

    mesh.vertices.reserve(nv + nf);
    mesh.edges.reserve(ne + 3 * nf);
    mesh.faces.reserve(3 * nf);

    // remember end of old vertices and edges
    auto vend = mesh.vertices.end();
    auto eend = mesh.edges.end();

    // compute new positions of old vertices
    auto new_pos = mesh.vertices.add<VectorS<3>, 3>("sqrt3:np");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    ///Exercise: Please implement the new positions of the old vertices
                    ///          - you can query for the valence of a vertex using:
                    ///                  auto valence = mesh.halfedge_graph::get_valence(v);
                    ///          - you can iterate over the vertices in the one-ring around a vertex v using:
                    ///                        for (const auto vv : mesh.halfedge_graph::get_vertices(v)) {/*your code here*/}
                    ///          - the property new_pos can be used to store the new positions of the old vertices
                    ///          - the formula can be found on Slideset-Surface-RepresentationsI.pdf Slide 53
                    auto n = mesh.halfedge_graph::get_valence(v);
                    auto beta = (4.0 - 2.0 * cos(2.0 * pi / n)) / 9.0;
                    VectorS<3> p(VectorS<3>::Zero());

                    for (const auto vv : mesh.halfedge_graph::get_vertices(v)) {
                        p += positions[vv];
                    }

                    p = (1.0 - beta) * positions[v] + beta / n * p;
                    new_pos[v] = p;

                    if (mesh.is_boundary(v) || (v_feature && v_feature[v])) {
                        new_pos[v] = positions[v];
                    }

                }
            }
    );

    // split faces
    vertex_handle v_;
    for (const auto f : mesh.faces) {
        ///Exercise: Please implement the face splits
        ///          - you can iterate over the vertices of a face using:
        ///                  for (const auto v: mesh.get_vertices(f)) { /*your code here*/ }
        ///          - split the faces at the centroid of each face
        std::vector<vertex_handle> V;
        for (const auto v: mesh.get_vertices(f)) {
            V.push_back(v);
        }

        const auto t = triangle3(positions[V[0]], positions[V[1]], positions[V[2]]);
        v_ = mesh.split(f, centroid(t));
    }

    // set new positions of old vertices
    for (auto vit = mesh.vertices.begin(); vit != vend; ++vit) {
        if (mesh.is_boundary(*vit) || (v_feature && v_feature[*vit])) continue;
        positions[*vit] = new_pos[*vit];
    }

    mesh.vertices.remove(new_pos);

    // flip old edges
    for (auto eit = mesh.edges.begin(); eit != eend; ++eit) {
        edge_handle e = *eit;
        ///Exercise: Please implement the edge flips
        ///          - make sure you check whether a flip is feasable with mesh.is_flip_ok(e)
        ///          - make sure you don't flip an edge which is a feature edge which is stored in e_feature
        if (!mesh.is_flip_ok(e) || (e_feature && e_feature[e])) continue;
        mesh.flip(e);
    }

    post_postprocessing(mesh);
}

}