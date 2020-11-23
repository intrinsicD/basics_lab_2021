//
// Created by alex on 13.10.20.
//

#include "bcg_mesh.h"
#include "triangle/bcg_triangle.h"
#include "triangle/bcg_triangle_distance.h"
#include "distance_query/bcg_distance_triangle_point.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "utils/bcg_stl_utils.h"

namespace bcg {

halfedge_mesh::halfedge_mesh() : halfedge_graph(),
                                 fconn(faces.add<face_connectivity, 1>("f_connectivity")),
                                 faces_deleted(faces.add<bool, 1>("f_deleted", false)),
                                 size_faces_deleted(0) {

}

void halfedge_mesh::assign(const halfedge_mesh &other) {
    if (this != &other) {
        // clear properties

        halfedge_graph::assign(other);
        faces.remove_all();

        // allocate standard properties
        fconn = faces.get_or_add<face_connectivity, 1>("f_connectivity");

        // copy properties from other mesh
        fconn.vector() = other.fconn.vector();

        faces_deleted.vector() = other.faces_deleted.vector();

        // resize (needed by property containers)
        faces.resize(other.faces.size());

        // how many elements are deleted?
        size_faces_deleted = other.size_faces_deleted;
    }
}

halfedge_mesh &halfedge_mesh::operator=(const halfedge_mesh &other) {
    if (this != &other) {
        // deep copy of property containers
        vertices = other.vertices;
        halfedges = other.halfedges;
        edges = other.edges;
        faces = other.faces;

        // property handles contain pointers, have to be reassigned
        positions = vertices.get<position_t, 3>("v_position");
        vertices_deleted = vertices.get<bool, 1>("v_deleted");
        halfedges_deleted = halfedges.get<bool, 1>("h_deleted");
        edges_deleted = edges.get<bool, 1>("e_deleted");
        faces_deleted = faces.get<bool, 1>("f_deleted");
        vconn = vertices.get<vertex_connectivity, 1>("v_connectivity");
        hconn = halfedges.get<halfedge_connectivity, 4>("h_connectivity");
        fconn = faces.get<face_connectivity, 1>("f_connectivity");

        // how many elements are deleted?
        size_vertices_deleted = other.size_vertices_deleted;
        size_halfedges_deleted = other.size_halfedges_deleted;
        size_edges_deleted = other.size_edges_deleted;
        size_faces_deleted = other.size_faces_deleted;
    }
    return *this;
}

bool halfedge_mesh::operator==(const halfedge_mesh &other) const {
    if (vertices.size() != other.vertices.size()) return false;
    if (edges.size() != other.edges.size()) return false;
    if (faces.size() != other.faces.size()) return false;
    for (const auto v : vertices) {
        if ((positions[v] - other.positions[v]).squaredNorm() > scalar_eps) return false;
        if (vconn[v].h != other.vconn[v].h) return false;
    }
    for (const auto e : edges) {
        if (hconn[e].f != other.hconn[e].f) return false;
        if (hconn[e].ph != other.hconn[e].ph) return false;
        if (hconn[e].nh != other.hconn[e].nh) return false;
    }
    for (const auto f : faces) {
        if (fconn[f].h != other.fconn[f].h) return false;
    }
    return true;
}

size_t halfedge_mesh::num_faces() const {
    return faces.size() - size_faces_deleted;
}

bool halfedge_mesh::has_garbage() const {
    return size_faces_deleted > 0 || halfedge_graph::has_garbage();
}

void halfedge_mesh::garbage_collection() {
    size_t nV = vertices.size();
    size_t nE = edges.size();
    size_t nH = halfedges.size();
    size_t nF = faces.size();

    auto vmap = vertices.add<vertex_handle, 1>("v_garbage_collection");
    auto hmap = halfedges.add<halfedge_handle, 1>("h_garbage_collection");
    auto fmap = faces.add<face_handle, 1>("f_garbage_collection");

    for (size_t i = 0; i < nV; ++i) {
        vmap[i] = vertex_handle(i);
    }
    for (size_t i = 0; i < nH; ++i) {
        hmap[i] = halfedge_handle(i);
    }
    for (size_t i = 0; i < nF; ++i) {
        fmap[i] = face_handle(i);
    }

    if (nV > 0) {
        size_t i0 = 0;
        size_t i1 = nV - 1;

        while (true) {
            // find first deleted and last un-deleted
            while (!vertices_deleted[i0] && i0 < i1) {
                ++i0;
            }
            while (vertices_deleted[i1] && i0 < i1) {
                --i1;
            }
            if (i0 >= i1) break;

            // swap
            vertices.swap(i0, i1);
        }

        // remember new size
        nV = vertices_deleted[i0] ? i0 : i0 + 1;
    }

    if (nE > 0) {
        size_t i0 = 0;
        size_t i1 = nE - 1;

        while (true) {
            // find first deleted and last un-deleted
            while (!edges_deleted[i0] && i0 < i1) {
                ++i0;
            }
            while (edges_deleted[i1] && i0 < i1) {
                --i1;
            }
            if (i0 >= i1) break;

            // swap
            edges.swap(i0, i1);
            halfedges.swap(2 * i0, 2 * i1);
            halfedges.swap(2 * i0 + 1, 2 * i1 + 1);
        }

        // remember new size
        nE = edges_deleted[i0] ? i0 : i0 + 1;
        nH = 2 * nE;
    }

    if (nF > 0) {
        size_t i0 = 0;
        size_t i1 = nF - 1;

        while (true) {
            // find 1st deleted and last un-deleted
            while (!faces_deleted[i0] && i0 < i1) {
                ++i0;
            }
            while (faces_deleted[i1] && i0 < i1) {
                --i1;
            }
            if (i0 >= i1) break;

            // swap
            faces.swap(i0, i1);
        }

        // remember new size
        nF = faces_deleted[i0] ? i0 : i0 + 1;
    }

    // update vertex connectivity
    for (size_t i = 0; i < nV; ++i) {
        auto v = vertex_handle(i);
        if (!this->is_isolated(v)) {
            halfedge_graph::set_halfedge(v, hmap[halfedge_graph::get_halfedge(v)]);
        }
    }

    // update halfedge connectivity
    for (size_t i = 0; i < nH; ++i) {
        auto h = halfedge_handle(i);
        halfedge_graph::set_vertex(h, vmap[halfedge_graph::get_to_vertex(h).idx]);
        halfedge_graph::set_next(h, hmap[halfedge_graph::get_next(h)]);
        if (!is_boundary(h)) {
            set_face(h, fmap[get_face(h)]);
        }
    }

    // update handles of faces
    for (size_t i = 0; i < nF; ++i) {
        auto f = face_handle(i);
        set_halfedge(f, hmap[get_halfedge(f)]);
    }

    // remove handle maps
    vertices.remove(vmap);
    halfedges.remove(hmap);
    faces.remove(fmap);

    // finally resize arrays
    vertices.resize(nV);
    vertices.free_unused_memory();
    halfedges.resize(nH);
    halfedges.free_unused_memory();
    edges.resize(nE);
    edges.free_unused_memory();
    faces.resize(nF);
    faces.free_unused_memory();

    size_faces_deleted = 0;
    size_edges_deleted = 0;
    size_halfedges_deleted = 0;
    size_vertices_deleted = 0;

    assert(!has_garbage());
    assert(vertices.is_dirty());
    assert(edges.is_dirty());
    assert(halfedges.is_dirty());
    assert(faces.is_dirty());
}

bool halfedge_mesh::is_boundary(vertex_handle v) const {
    return is_boundary(halfedge_graph::get_halfedge(v));
}

bool halfedge_mesh::is_boundary(halfedge_handle h) const {
    return !(h.is_valid() && get_face(h).is_valid());
}

bool halfedge_mesh::is_boundary(edge_handle e) const {
    return is_boundary(halfedge_graph::get_halfedge(e, 0)) || is_boundary(halfedge_graph::get_halfedge(e, 1));
}

bool halfedge_mesh::is_boundary(face_handle f) const {
    auto h = get_halfedge(f);
    auto hh = h;
    do {
        if (is_boundary(get_opposite(h)))
            return true;
        h = get_next(h);
    } while (h != hh);
    return false;
}

bool halfedge_mesh::is_manifold(vertex_handle v) const {
    size_t count = 0;
    for (const auto h : halfedge_graph::get_halfedges(v)) {
        count += is_boundary(h);
        if (count > 2) return false;
    }
    return true;
}

bool halfedge_mesh::is_manifold() const {
    for (const auto v : vertices) {
        if (!is_manifold(v)) return false;
    }
    return true;
}

bool halfedge_mesh::is_triangle_mesh() const {
    for (const auto f : faces) {
        if (get_valence(f) != 3) return false;
    }
    return true;
}

bool halfedge_mesh::is_quad_mesh() const {
    for (const auto f : faces) {
        if (get_valence(f) != 4) return false;
    }
    return true;
}

bool halfedge_mesh::is_removal_ok(edge_handle e) const {
    auto h0 = halfedge_graph::get_halfedge(e, 0);
    auto h1 = halfedge_graph::get_halfedge(e, 1);
    auto v0 = halfedge_graph::get_to_vertex(h0);
    auto v1 = halfedge_graph::get_to_vertex(h1);
    auto f0 = get_face(h0);
    auto f1 = get_face(h1);

    // boundary?
    if (!f0.is_valid() || !f1.is_valid()) {
        return false;
    }

    // same face?
    if (f0 == f1) {
        return false;
    }

    // are the two faces connect through another vertex?
    for (const auto v : get_vertices(f0)) {
        if (v != v0 && v != v1) {
            for (const auto f : get_faces(v)) {
                if (f == f1) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool halfedge_mesh::is_collapse_ok(halfedge_handle h) const {
    auto v1v0 = halfedge_graph::get_opposite(h);
    auto v0 = halfedge_graph::get_to_vertex(v1v0);
    auto v1 = halfedge_graph::get_to_vertex(h);
    vertex_handle vl, vr;
    halfedge_handle h1, h2;

    // the edges v1-vl and vl-v0 must not be both boundary edges
    if (!is_boundary(h)) {
        vl = halfedge_graph::get_to_vertex(halfedge_graph::get_next(h));
        h1 = halfedge_graph::get_next(h);
        h2 = halfedge_graph::get_next(h1);
        if (is_boundary(halfedge_graph::get_opposite(h1)) && is_boundary(halfedge_graph::get_opposite(h2))) {
            return false;
        }
    }

    // the edges v0-vr and vr-v1 must not be both boundary edges
    if (!is_boundary(v1v0)) {
        vr = halfedge_graph::get_to_vertex(halfedge_graph::get_next(v1v0));
        h1 = halfedge_graph::get_next(v1v0);
        h2 = halfedge_graph::get_next(h1);
        if (is_boundary(halfedge_graph::get_opposite(h1)) && is_boundary(halfedge_graph::get_opposite(h2))) {
            return false;
        }
    }

    // if vl and vr are equal or both invalid -> fail
    if (vl == vr) {
        return false;
    }

    // edge between two boundary vertices should be a boundary edge
    if (is_boundary(v0) && is_boundary(v1) && !is_boundary(h) && !is_boundary(v1v0)) {
        return false;
    }

    // test intersection of the one-rings of v0 and v1
    for (const auto vv : halfedge_graph::get_vertices(v0)) {
        if (vv != v1 && vv != vl && vv != vr) {
            if (halfedge_graph::find_halfedge(vv, v1).is_valid()) {
                return false;
            }
        }
    }

    // passed all tests
    return true;
}

bool halfedge_mesh::is_flip_ok(edge_handle e) const {
    // boundary edges cannot be flipped
    if (is_boundary(e)) {
        return false;
    }

    auto h0 = halfedge_graph::get_halfedge(e, 0);
    auto h1 = halfedge_graph::get_halfedge(e, 1);

    //flip is only allowed in triangles
    if (get_valence(get_face(h0)) != 3 || get_valence(get_face(h1)) != 3) {
        return false;
    }

    // check if the flipped edge is already present in the mesh
    auto v0 = halfedge_graph::get_to_vertex(halfedge_graph::get_next(h0));
    auto v1 = halfedge_graph::get_to_vertex(halfedge_graph::get_next(h1));

    // this is generally a bad sign !!!
    if (v0 == v1) {
        return false;
    }

    return !find_halfedge(v0, v1).is_valid();
}

face_handle halfedge_mesh::get_face(edge_handle e, bool i) const {
    return get_face(halfedge_graph::get_halfedge(e, i));
}

face_handle halfedge_mesh::get_face(halfedge_handle h) const {
    return hconn[h].f;
}

void halfedge_mesh::set_face(halfedge_handle h, face_handle f) {
    hconn[h].f = f;
}

halfedge_handle halfedge_mesh::get_halfedge(face_handle f) const {
    return fconn[f].h;
}

void halfedge_mesh::set_halfedge(face_handle f, halfedge_handle h) {
    fconn[f].h = h;
}

size_t halfedge_mesh::get_valence(face_handle f) const {
    size_t count = 0;
    for (const auto v : get_vertices(f)) {
        count += vertices.is_valid(v) && !vertices_deleted[v];
    }
    return count;
}

face_handle halfedge_mesh::add_face(const std::vector<vertex_handle> &f_vertices) {
    size_t n = f_vertices.size();

    auto &f_halfedges = m_add_face_halfedges;
    auto &h_is_new = m_add_face_is_new;
    auto &h_needs_adjust = m_add_face_needs_adjust;
    f_halfedges.clear();
    f_halfedges.resize(n);
    h_is_new.clear();
    h_is_new.resize(n);
    h_needs_adjust.clear();
    h_needs_adjust.resize(n, false);

    // test for topological errors
    for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
        if (!is_boundary(f_vertices[i])) {
            std::cerr << "add_face: complex vertex!\n";
            return face_handle();
        }

        f_halfedges[i] = halfedge_graph::find_halfedge(f_vertices[i], f_vertices[ii]);
        h_is_new[i] = !f_halfedges[i].is_valid();

        if (!h_is_new[i] && !is_boundary(f_halfedges[i])) {
            std::cerr << "add_face: complex edge!\n";
            return face_handle();
        }
    }

    NextCache &next_cache = m_add_face_next_cache;
    next_cache.clear();
    next_cache.reserve(3 * n);
    halfedge_handle inner_next, inner_prev, outer_prev, outer_next, boundary_prev, boundary_next, patch_start, patch_end;
    // re-link patches if necessary
    for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
        if (!h_is_new[i] && !h_is_new[ii]) {
            inner_prev = f_halfedges[i];
            inner_next = f_halfedges[ii];

            if (halfedge_graph::get_next(inner_prev) != inner_next) {
                // here comes the ugly part... we have to relink a whole patch
                // search a free gap
                // free gap will be between boundary_prev and boundary_next
                outer_prev = halfedge_graph::get_opposite(inner_next);
                outer_next = halfedge_graph::get_opposite(inner_prev);
                boundary_prev = outer_prev;
                do {
                    boundary_prev = halfedge_graph::get_opposite(halfedge_graph::get_next(boundary_prev));
                } while (!is_boundary(boundary_prev) || boundary_prev == inner_prev);
                boundary_next = halfedge_graph::get_next(boundary_prev);
                assert(is_boundary(boundary_prev));
                assert(is_boundary(boundary_next));
                if (!is_boundary(boundary_prev)) {
                    std::cerr << "boundary_prev should be boundary!\n";
                }
                if (!is_boundary(boundary_next)) {
                    std::cerr << "boundary_next should be boundary!\n";
                }

                if (boundary_next == inner_next) {
                    std::cerr << "add_face: patch re-linking failed!\n";
                    return face_handle();
                }

                // other halfedges' handles
                patch_start = halfedge_graph::get_next(inner_prev);
                patch_end = halfedge_graph::get_prev(inner_next);
                // relink
                next_cache.emplace_back(boundary_prev, patch_start);
                next_cache.emplace_back(patch_end, boundary_next);
                next_cache.emplace_back(inner_prev, inner_next);
            }
        }
    }

    // create missing edges
    for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
        if (h_is_new[i]) {
            f_halfedges[i] = halfedge_graph::new_edge(f_vertices[i], f_vertices[ii]);
        }
    }

    // create the face
    face_handle f(new_face());
    set_halfedge(f, f_halfedges[n - 1]);

    vertex_handle v;
    // setup halfedges
    for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
        v = f_vertices[ii];
        inner_prev = f_halfedges[i];
        inner_next = f_halfedges[ii];

        size_t id = 0;
        if (h_is_new[i]) {
            id |= 1;
        }
        if (h_is_new[ii]) {
            id |= 2;
        }

        if (id) {
            outer_prev = halfedge_graph::get_opposite(inner_next);
            outer_next = halfedge_graph::get_opposite(inner_prev);

            // set outer links
            switch (id) {
                case 1: // prev is new, next is old
                    boundary_prev = halfedge_graph::get_prev(inner_next);
                    next_cache.emplace_back(boundary_prev, outer_next);
                    halfedge_graph::set_halfedge(v, outer_next);
                    break;

                case 2: // next is new, prev is old
                    boundary_next = halfedge_graph::get_next(inner_prev);
                    next_cache.emplace_back(outer_prev, boundary_next);
                    halfedge_graph::set_halfedge(v, boundary_next);
                    break;

                case 3: // both are new
                    if (!halfedge_graph::get_halfedge(v).is_valid()) {
                        halfedge_graph::set_halfedge(v, outer_next);
                        next_cache.emplace_back(outer_prev, outer_next);
                    } else {
                        boundary_next = halfedge_graph::get_halfedge(v);
                        boundary_prev = halfedge_graph::get_prev(boundary_next);
                        next_cache.emplace_back(boundary_prev, outer_next);
                        next_cache.emplace_back(outer_prev, boundary_next);
                    }
                    break;
                default:
                    break;
            }

            // set inner link
            next_cache.emplace_back(inner_prev, inner_next);
        } else {
            h_needs_adjust[ii] = (halfedge_graph::get_halfedge(v) == inner_next);
        }

        // set face handle
        set_face(f_halfedges[i], f);
    }

    // process next halfedge cache
    NextCache::const_iterator ncIt(next_cache.begin()), ncEnd(next_cache.end());
    for (; ncIt != ncEnd; ++ncIt) {
        halfedge_graph::set_next(ncIt->first, ncIt->second);
    }

    // adjust vertices' halfedge handle
    for (size_t i = 0; i < n; ++i) {
        if (h_needs_adjust[i]) {
            adjust_outgoing_halfedge(f_vertices[i]);
        }
    }

    assert(faces.is_dirty());
    assert(halfedges.is_dirty());
    return f;
}

face_handle halfedge_mesh::add_triangle(vertex_handle v0, vertex_handle v1, vertex_handle v2) {
    return add_face({v0, v1, v2});
}

face_handle halfedge_mesh::add_quad(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) {
    return add_face({v0, v1, v2, v3});
}

void halfedge_mesh::adjust_outgoing_halfedge(vertex_handle v) {
    auto h = halfedge_graph::get_halfedge(v);
    const auto hh = h;

    if (h.is_valid()) {
        do {
            if (is_boundary(h)) {
                halfedge_graph::set_halfedge(v, h);
                return;
            }
            h = halfedge_graph::rotate_cw(h);
        } while (h != hh);
    }
}

void halfedge_mesh::delete_face(face_handle f) {
    if (faces_deleted[f]) {
        return;
    }
    mark_face_deleted(f);

    // boundary edges of face f to be deleted
    std::vector<edge_handle> deletedEdges;
    deletedEdges.reserve(3);

    // vertices of face f for updating their outgoing halfedge
    std::vector<vertex_handle> verts;
    verts.reserve(3);

    // for all halfedges of face f do:
    //   1) invalidate face handle.
    //   2) collect all boundary halfedges, set them deleted
    //   3) store vertex handles
    halfedge_around_face_circulator hc, hcend;
    hc = hcend = get_halfedges(f);

    do {
        set_face(*hc, face_handle());

        if (is_boundary(halfedge_graph::get_opposite(*hc))) {
            deletedEdges.push_back(halfedge_graph::get_edge(*hc));
        }

        verts.push_back(halfedge_graph::get_to_vertex(*hc));

    } while (++hc != hcend);

    // delete all collected (half)edges
    // delete isolated vertices
    if (!deletedEdges.empty()) {
        auto delit(deletedEdges.begin()), delend(deletedEdges.end());

        halfedge_handle h0, h1, next0, next1, prev0, prev1;
        vertex_handle v0, v1;

        for (; delit != delend; ++delit) {
            h0 = halfedge_graph::get_halfedge(*delit, 0);
            v0 = halfedge_graph::get_to_vertex(h0);
            next0 = halfedge_graph::get_next(h0);
            prev0 = halfedge_graph::get_prev(h0);

            h1 = halfedge_graph::get_halfedge(*delit, 1);
            v1 = halfedge_graph::get_to_vertex(h1);
            next1 = halfedge_graph::get_next(h1);
            prev1 = halfedge_graph::get_prev(h1);

            // adjust next and prev handles
            halfedge_graph::set_next(prev0, next1);
            halfedge_graph::set_next(prev1, next0);

            // mark edge deleted
            mark_edge_deleted(*delit);

            // update v0
            if (halfedge_graph::get_halfedge(v0) == h1) {
                if (next0 == h1) {
                    mark_vertex_deleted(v0);
                } else {
                    halfedge_graph::set_halfedge(v0, next0);
                }
            }

            // update v1
            if (halfedge_graph::get_halfedge(v1) == h0) {
                if (next1 == h0) {
                    mark_vertex_deleted(v1);
                } else {
                    halfedge_graph::set_halfedge(v1, next1);
                }
            }
        }

    }

    // update outgoing halfedge handles of remaining vertices
    auto vit(vertices.begin()), vend(vertices.end());
    for (; vit != vend; ++vit) {
        adjust_outgoing_halfedge(*vit);
    }

    assert(has_garbage());
}

void halfedge_mesh::remove_edge(edge_handle e) {
    if (!is_removal_ok(e)) return;

    auto h0 = halfedge_graph::get_halfedge(e, 0);
    auto h1 = halfedge_graph::get_halfedge(e, 1);

    auto v0 = halfedge_graph::get_to_vertex(h0);
    auto v1 = halfedge_graph::get_to_vertex(h1);

    auto f0 = get_face(h0);
    auto f1 = get_face(h1);

    auto h0_prev = halfedge_graph::get_prev(h0);
    auto h0_next = halfedge_graph::get_next(h0);

    auto h1_prev = halfedge_graph::get_prev(h1);
    auto h1_next = halfedge_graph::get_next(h1);

    // adjust vertex->halfedge
    if (halfedge_graph::get_halfedge(v0) == h1) {
        halfedge_graph::set_halfedge(v0, h0_next);
    }
    if (halfedge_graph::get_halfedge(v1) == h0) {
        halfedge_graph::set_halfedge(v1, h1_next);
    }

    // adjust halfedge->face
    for (const auto h : get_halfedges(f0)) {
        set_face(h, f1);
    }

    // adjust halfedge->halfedge
    halfedge_graph::set_next(h1_prev, h0_next);
    halfedge_graph::set_next(h0_prev, h1_next);

    // adjust face->halfedge
    if (get_halfedge(f1) == h1) {
        set_halfedge(f1, h1_next);
    }

    mark_face_deleted(f0);
    mark_edge_deleted(e);

    assert(has_garbage());
}

void halfedge_mesh::delete_edge(edge_handle e) {
    if (edges_deleted[e]) {
        return;
    }
    auto f0 = get_face(e, 0);
    auto f1 = get_face(e, 1);
    if (faces.is_valid(f0)) {
        delete_face(f0);
    }
    if (faces.is_valid(f1)) {
        delete_face(f1);
    }
    mark_edge_deleted(e);
}

void halfedge_mesh::delete_vertex(vertex_handle v) {
    if (vertices_deleted[v]) return;
    std::vector<face_handle> incident_faces;
    incident_faces.reserve(6);
    for (const auto &f : get_faces(v)) {
        incident_faces.push_back(f);
    }
    for (const auto &f : incident_faces) {
        delete_face(f);
    }
    mark_vertex_deleted(v);
}

halfedge_mesh::face_around_vertex_circulator::face_around_vertex_circulator(const halfedge_mesh *ds, vertex_handle v) :
        ds(ds), active(false) {
    if (ds) {
        halfedge = ds->halfedge_graph::get_halfedge(v);
        if (halfedge.is_valid() && ds->is_boundary(halfedge)) {
            operator++();
        }
    }
}

bool halfedge_mesh::face_around_vertex_circulator::operator==(const face_around_vertex_circulator &rhs) const {
    assert(ds);
    assert(ds == rhs.ds);
    return (active && (halfedge == rhs.halfedge));
}

bool halfedge_mesh::face_around_vertex_circulator::operator!=(const face_around_vertex_circulator &rhs) const {
    return !operator==(rhs);
}

halfedge_mesh::face_around_vertex_circulator &halfedge_mesh::face_around_vertex_circulator::operator++() {
    assert(ds && halfedge.is_valid());
    do {
        halfedge = ds->rotate_ccw(halfedge);
    } while (ds->is_boundary(halfedge));
    active = true;
    return *this;
}

halfedge_mesh::face_around_vertex_circulator &halfedge_mesh::face_around_vertex_circulator::operator--() {
    assert(ds && halfedge.is_valid());
    do {
        halfedge = ds->rotate_cw(halfedge);
    } while (ds->is_boundary(halfedge));
    return *this;
}

face_handle halfedge_mesh::face_around_vertex_circulator::operator*() const {
    assert(ds && halfedge.is_valid());
    return ds->get_face(halfedge);
}

halfedge_mesh::face_around_vertex_circulator::operator bool() const {
    return halfedge.is_valid();
}

halfedge_mesh::face_around_vertex_circulator &halfedge_mesh::face_around_vertex_circulator::begin() {
    active = !halfedge.is_valid();
    return *this;
}

halfedge_mesh::face_around_vertex_circulator &halfedge_mesh::face_around_vertex_circulator::end() {
    active = true;
    return *this;
}

halfedge_mesh::face_around_vertex_circulator halfedge_mesh::get_faces(vertex_handle v) const {
    return face_around_vertex_circulator(this, v);
}

halfedge_mesh::vertex_around_face_circulator::vertex_around_face_circulator(const halfedge_mesh *ds, face_handle f)
        : ds(ds), active(false) {

    if (ds) {
        halfedge = ds->get_halfedge(f);
    }
}

bool halfedge_mesh::vertex_around_face_circulator::operator==(const vertex_around_face_circulator &rhs) const {
    assert(ds);
    assert(ds == rhs.ds);
    return (active && (halfedge == rhs.halfedge));
}

bool halfedge_mesh::vertex_around_face_circulator::operator!=(const vertex_around_face_circulator &rhs) const {
    return !operator==(rhs);
}

halfedge_mesh::vertex_around_face_circulator &halfedge_mesh::vertex_around_face_circulator::operator++() {
    assert(ds && halfedge.is_valid());
    halfedge = ds->halfedge_graph::get_next(halfedge);
    active = true;
    return *this;
}

halfedge_mesh::vertex_around_face_circulator &halfedge_mesh::vertex_around_face_circulator::operator--() {
    assert(ds && halfedge.is_valid());
    halfedge = ds->halfedge_graph::get_prev(halfedge);
    return *this;
}

vertex_handle halfedge_mesh::vertex_around_face_circulator::operator*() const {
    assert(ds && halfedge.is_valid());
    return ds->get_to_vertex(halfedge);
}

halfedge_mesh::vertex_around_face_circulator &halfedge_mesh::vertex_around_face_circulator::begin() {
    active = !halfedge.is_valid();
    return *this;
}

halfedge_mesh::vertex_around_face_circulator &halfedge_mesh::vertex_around_face_circulator::end() {
    active = true;
    return *this;
}

halfedge_mesh::vertex_around_face_circulator halfedge_mesh::get_vertices(face_handle f) const {
    return vertex_around_face_circulator(this, f);
}

halfedge_mesh::halfedge_around_face_circulator::halfedge_around_face_circulator(const halfedge_mesh *ds,
                                                                                face_handle f) : ds(ds), active(false) {
    if (ds) {
        halfedge = ds->get_halfedge(f);
    }
}

bool halfedge_mesh::halfedge_around_face_circulator::operator==(const halfedge_around_face_circulator &rhs) const {
    assert(ds);
    assert(ds == rhs.ds);
    return (active && (halfedge == rhs.halfedge));
}

bool halfedge_mesh::halfedge_around_face_circulator::operator!=(const halfedge_around_face_circulator &rhs) const {
    return !operator==(rhs);
}

halfedge_mesh::halfedge_around_face_circulator &halfedge_mesh::halfedge_around_face_circulator::operator++() {
    assert(ds && halfedge.is_valid());
    halfedge = ds->halfedge_graph::get_next(halfedge);
    active = true;
    return *this;
}

halfedge_mesh::halfedge_around_face_circulator &halfedge_mesh::halfedge_around_face_circulator::operator--() {
    assert(ds && halfedge.is_valid());
    halfedge = ds->halfedge_graph::get_prev(halfedge);
    return *this;
}

halfedge_handle halfedge_mesh::halfedge_around_face_circulator::operator*() const {
    return halfedge;
}

halfedge_mesh::halfedge_around_face_circulator &halfedge_mesh::halfedge_around_face_circulator::begin() {
    active = !halfedge.is_valid();
    return *this;
}

halfedge_mesh::halfedge_around_face_circulator &halfedge_mesh::halfedge_around_face_circulator::end() {
    active = true;
    return *this;
}

halfedge_mesh::halfedge_around_face_circulator halfedge_mesh::get_halfedges(face_handle f) const {
    return halfedge_around_face_circulator(this, f);
}

void halfedge_mesh::triangulate() {
    for (const auto f : faces) {
        if (get_valence(f) != 3) {
            triangulate(f);
        }
    }
}

void halfedge_mesh::triangulate(face_handle f) {
    auto h = get_halfedge(f);
    auto v0 = halfedge_graph::get_from_vertex(h);
    auto nh = halfedge_graph::get_next(h);

    while (halfedge_graph::get_to_vertex(halfedge_graph::get_next(nh)) != v0) {
        auto nnh(halfedge_graph::get_next(nh));

        auto new_f = new_face();
        set_halfedge(new_f, h);

        auto new_h = halfedge_graph::new_edge(halfedge_graph::get_to_vertex(nh), v0);

        halfedge_graph::set_next(h, nh);
        halfedge_graph::set_next(nh, new_h);
        halfedge_graph::set_next(new_h, h);

        set_face(h, new_f);
        set_face(nh, new_f);
        set_face(new_h, new_f);

        h = halfedge_graph::get_opposite(new_h);
        nh = nnh;
    }
    set_halfedge(f, h);  //the last face takes the handle _fh

    halfedge_graph::set_next(h, nh);
    halfedge_graph::set_next(halfedge_graph::get_next(nh), h);

    set_face(h, f);
}

void halfedge_mesh::collapse(halfedge_handle h) {
    //move v0 to v1
    auto h0 = h;
    auto h1 = halfedge_graph::get_prev(h0);
    auto o0 = halfedge_graph::get_opposite(h0);
    auto o1 = halfedge_graph::get_next(o0);

    // remove edge
    remove_edge_helper(h0);

    // remove loops
    if (halfedge_graph::get_next(halfedge_graph::get_next(h1)) == h1) {
        remove_loop_helper(h1);
    }
    if (halfedge_graph::get_next(halfedge_graph::get_next(o1)) == o1) {
        remove_loop_helper(o1);
    }

    assert(faces.is_dirty());
    assert(edges.is_dirty());
}

void halfedge_mesh::remove_edge_helper(halfedge_handle h) {
    auto hn = halfedge_graph::get_next(h);
    auto hp = halfedge_graph::get_prev(h);

    auto o = halfedge_graph::get_opposite(h);
    auto on = halfedge_graph::get_next(o);
    auto op = halfedge_graph::get_prev(o);

    auto fh = get_face(h);
    auto fo = get_face(o);

    auto vh = halfedge_graph::get_to_vertex(h);
    auto vo = halfedge_graph::get_to_vertex(o);

    // halfedge -> vertex
    for (const auto hh : halfedge_graph::get_halfedges(vo)) {
        halfedge_graph::set_vertex(halfedge_graph::get_opposite(hh), vh);
    }

    // halfedge -> halfedge
    halfedge_graph::set_next(hp, hn);
    halfedge_graph::set_next(op, on);

    // face -> halfedge
    if (fh.is_valid()) {
        set_halfedge(fh, hn);
    }
    if (fo.is_valid()) {
        set_halfedge(fo, on);
    }

    // vertex -> halfedge
    if (halfedge_graph::get_halfedge(vh) == o) {
        halfedge_graph::set_halfedge(vh, hn);
    }
    adjust_outgoing_halfedge(vh);
    halfedge_graph::set_halfedge(vo, halfedge_handle());

    mark_vertex_deleted(vo);
    mark_edge_deleted(halfedge_graph::get_edge(h));

    assert(has_garbage());
}

void halfedge_mesh::remove_loop_helper(halfedge_handle h) {
    auto h0 = h;
    auto h1 = halfedge_graph::get_next(h0);

    auto o0 = halfedge_graph::get_opposite(h0);
    auto o1 = halfedge_graph::get_opposite(h1);

    auto v0 = halfedge_graph::get_to_vertex(h0);
    auto v1 = halfedge_graph::get_to_vertex(h1);

    auto fh = get_face(h0);
    auto fo = get_face(o0);

    // is it a loop ?
    assert((halfedge_graph::get_next(h1) == h0) && (h1 != o0));

    // halfedge -> halfedge
    halfedge_graph::set_next(h1, halfedge_graph::get_next(o0));
    halfedge_graph::set_next(halfedge_graph::get_prev(o0), h1);

    // halfedge -> face
    set_face(h1, fo);

    // vertex -> halfedge
    halfedge_graph::set_halfedge(v0, h1);
    adjust_outgoing_halfedge(v0);
    halfedge_graph::set_halfedge(v1, o1);
    adjust_outgoing_halfedge(v1);

    // face -> halfedge
    if (fo.is_valid() && get_halfedge(fo) == o0) {
        set_halfedge(fo, h1);
    }

    // delete stuff
    if (fh.is_valid()) {
        mark_face_deleted(fh);
    }

    mark_edge_deleted(halfedge_graph::get_edge(h));

    assert(has_garbage());
}

vertex_handle halfedge_mesh::split(face_handle f, const position_t &point) {
    return split(f, halfedge_graph::add_vertex(point));
}

vertex_handle halfedge_mesh::split(face_handle f, vertex_handle v) {
    auto hend = get_halfedge(f);
    auto h = halfedge_graph::get_next(hend);

    auto hold = halfedge_graph::new_edge(halfedge_graph::get_to_vertex(hend), v);

    halfedge_graph::set_next(hend, hold);
    set_face(hold, f);

    hold = halfedge_graph::get_opposite(hold);

    while (h != hend) {
        auto hnext = halfedge_graph::get_next(h);

        auto fnew = new_face();
        set_halfedge(fnew, h);

        auto hnew = halfedge_graph::new_edge(halfedge_graph::get_to_vertex(h), v);

        halfedge_graph::set_next(hnew, hold);
        halfedge_graph::set_next(hold, h);
        halfedge_graph::set_next(h, hnew);

        set_face(hnew, fnew);
        set_face(hold, fnew);
        set_face(h, fnew);

        hold = halfedge_graph::get_opposite(hnew);

        h = hnext;
    }

    halfedge_graph::set_next(hold, hend);
    halfedge_graph::set_next(halfedge_graph::get_next(hend), hold);

    set_face(hold, f);

    halfedge_graph::set_halfedge(v, hold);;
    assert(faces.is_dirty());
    assert(edges.is_dirty());
    return v;
}

halfedge_handle halfedge_mesh::split(edge_handle e, const position_t &point) {
    return split(e, halfedge_graph::add_vertex(point));
}

halfedge_handle halfedge_mesh::split(edge_handle e, vertex_handle v) {
    auto h0 = halfedge_graph::get_halfedge(e, 0);
    auto o0 = halfedge_graph::get_halfedge(e, 1);

    auto v2 = halfedge_graph::get_to_vertex(o0);

    auto e1 = halfedge_graph::new_edge(v, v2);
    auto t1 = halfedge_graph::get_opposite(e1);

    auto f0 = get_face(h0);
    auto f3 = get_face(o0);

    halfedge_graph::set_halfedge(v, h0);
    halfedge_graph::set_vertex(o0, v);

    if (!is_boundary(h0)) {
        auto h1 = halfedge_graph::get_next(h0);
        auto h2 = halfedge_graph::get_next(h1);

        auto v1 = halfedge_graph::get_to_vertex(h1);

        auto e0 = halfedge_graph::new_edge(v, v1);
        auto t0 = halfedge_graph::get_opposite(e0);

        auto f1 = new_face();
        set_halfedge(f0, h0);
        set_halfedge(f1, h2);

        set_face(h1, f0);
        set_face(t0, f0);
        set_face(h0, f0);

        set_face(h2, f1);
        set_face(t1, f1);
        set_face(e0, f1);

        halfedge_graph::set_next(h0, h1);
        halfedge_graph::set_next(h1, t0);
        halfedge_graph::set_next(t0, h0);

        halfedge_graph::set_next(e0, h2);
        halfedge_graph::set_next(h2, t1);
        halfedge_graph::set_next(t1, e0);
    } else {
        halfedge_graph::set_next(halfedge_graph::get_prev(h0), t1);
        halfedge_graph::set_next(t1, h0);
        // halfedge handle of vh already is h0
    }

    if (!is_boundary(o0)) {
        auto o1 = halfedge_graph::get_next(o0);
        auto o2 = halfedge_graph::get_next(o1);

        auto v3 = halfedge_graph::get_to_vertex(o1);

        auto e2 = halfedge_graph::new_edge(v, v3);
        auto t2 = halfedge_graph::get_opposite(e2);

        auto f2 = new_face();
        set_halfedge(f2, o1);
        set_halfedge(f3, o0);

        set_face(o1, f2);
        set_face(t2, f2);
        set_face(e1, f2);

        set_face(o2, f3);
        set_face(o0, f3);
        set_face(e2, f3);

        halfedge_graph::set_next(e1, o1);
        halfedge_graph::set_next(o1, t2);
        halfedge_graph::set_next(t2, e1);

        halfedge_graph::set_next(o0, e2);
        halfedge_graph::set_next(e2, o2);
        halfedge_graph::set_next(o2, o0);
    } else {
        halfedge_graph::set_next(e1, halfedge_graph::get_next(o0));
        halfedge_graph::set_next(o0, e1);
        halfedge_graph::set_halfedge(v, e1);
    }

    if (halfedge_graph::get_halfedge(v2) == h0) {
        halfedge_graph::set_halfedge(v2, t1);
    }

    return t1;
}

halfedge_handle halfedge_mesh::insert_vertex(edge_handle e, const position_t &point) {
    return insert_vertex(halfedge_graph::get_halfedge(e, 0), halfedge_graph::add_vertex(point));
}

halfedge_handle halfedge_mesh::insert_vertex(edge_handle e, vertex_handle v) {
    return insert_vertex(halfedge_graph::get_halfedge(e, 0), v);
}

halfedge_handle halfedge_mesh::insert_vertex(halfedge_handle h0, vertex_handle v) {
    // before:
    //
    // v0      h0       v2
    //  o--------------->o
    //   <---------------
    //         o0
    //
    // after:
    //
    // v0  h0   v   h1   v2
    //  o------>o------->o
    //   <------ <-------
    //     o0       o1

    auto h2 = halfedge_graph::get_next(h0);
    auto o0 = halfedge_graph::get_opposite(h0);
    auto o2 = halfedge_graph::get_prev(o0);
    auto v2 = halfedge_graph::get_to_vertex(h0);
    auto fh = get_face(h0);
    auto fo = get_face(o0);

    auto h1 = halfedge_graph::new_edge(v, v2);
    auto o1 = halfedge_graph::get_opposite(h1);

    // adjust halfedge connectivity
    halfedge_graph::set_next(h1, h2);
    halfedge_graph::set_next(h0, h1);
    halfedge_graph::set_vertex(h0, v);
    halfedge_graph::set_vertex(h1, v2);
    set_face(h1, fh);

    halfedge_graph::set_next(o1, o0);
    halfedge_graph::set_next(o2, o1);
    halfedge_graph::set_vertex(o1, v);
    set_face(o1, fo);

    // adjust vertex connectivity
    halfedge_graph::set_halfedge(v2, o1);
    adjust_outgoing_halfedge(v2);
    halfedge_graph::set_halfedge(v, h1);
    adjust_outgoing_halfedge(v);

    // adjust face connectivity
    if (fh.is_valid()) {
        set_halfedge(fh, h0);
    }
    if (fo.is_valid()) {
        set_halfedge(fo, o1);
    }

    fconn.set_dirty();
    assert(vertices.is_dirty());
    assert(edges.is_dirty());
    assert(halfedges.is_dirty());
    assert(faces.is_dirty());
    return o1;
}

halfedge_handle halfedge_mesh::insert_edge(halfedge_handle h0, halfedge_handle h1) {
    assert(get_face(h0) == get_face(h1));
    assert(get_face(h0).is_valid());

    auto v0 = halfedge_graph::get_to_vertex(h0);
    auto v1 = halfedge_graph::get_to_vertex(h1);
    auto h2 = halfedge_graph::get_next(h0);
    auto h3 = halfedge_graph::get_next(h1);
    auto h4 = halfedge_graph::new_edge(v0, v1);
    auto h5 = halfedge_graph::get_opposite(h4);
    auto f0 = get_face(h0);
    auto f1 = new_face();

    set_halfedge(f0, h0);
    set_halfedge(f1, h1);

    halfedge_graph::set_next(h0, h4);
    halfedge_graph::set_next(h4, h3);
    set_face(h4, f0);

    halfedge_graph::set_next(h1, h5);
    halfedge_graph::set_next(h5, h2);
    auto h = h2;
    do {
        set_face(h, f1);
        h = halfedge_graph::get_next(h);
    } while (h != h2);

    assert(edges.is_dirty());
    assert(faces.is_dirty());
    return h4;
}

void halfedge_mesh::flip(edge_handle e) {
    assert(is_flip_ok(e));

    auto a0 = halfedge_graph::get_halfedge(e, 0);
    auto b0 = halfedge_graph::get_halfedge(e, 1);
    auto a1 = halfedge_graph::get_next(a0);
    auto a2 = halfedge_graph::get_next(a1);
    auto b1 = halfedge_graph::get_next(b0);
    auto b2 = halfedge_graph::get_next(b1);
    auto va0 = halfedge_graph::get_to_vertex(a0);
    auto va1 = halfedge_graph::get_to_vertex(a1);
    auto vb0 = halfedge_graph::get_to_vertex(b0);
    auto vb1 = halfedge_graph::get_to_vertex(b1);
    auto fa = get_face(a0);
    auto fb = get_face(b0);

    halfedge_graph::set_vertex(a0, va1);
    halfedge_graph::set_vertex(b0, vb1);

    halfedge_graph::set_next(a0, a2);
    halfedge_graph::set_next(a2, b1);
    halfedge_graph::set_next(b1, a0);
    halfedge_graph::set_next(b0, b2);
    halfedge_graph::set_next(b2, a1);
    halfedge_graph::set_next(a1, b0);

    set_face(a1, fb);
    set_face(b1, fa);

    set_halfedge(fa, a0);
    set_halfedge(fb, b0);

    if (halfedge_graph::get_halfedge(va0) == b0) {
        halfedge_graph::set_halfedge(va0, a1);
    }
    if (halfedge_graph::get_halfedge(vb0) == a0) {
        halfedge_graph::set_halfedge(vb0, b1);
    }

    assert(faces.is_dirty());
    assert(edges.is_dirty());
}

property<VectorI<3>, 3> halfedge_mesh::get_triangles() {
    if (!is_triangle_mesh()) {
        triangulate();
    }
    auto triangles = faces.get_or_add<VectorI<3>, 3>("triangles");

    triangles.reserve(faces.size());
    for (const auto f : faces) {
        auto v = get_vertices(f);
        triangles[f](0) = (unsigned int) (*v).idx;
        triangles[f](1) = (unsigned int) (*(++v)).idx;
        triangles[f](2) = (unsigned int) (*(++v)).idx;
    }
    return triangles;
}

property<VectorI<6>, 6> halfedge_mesh::get_triangles_adjacencies() {
    if (!is_triangle_mesh()) {
        triangulate();
    }
    auto triangle_adjacencies = faces.get_or_add<VectorI<6>, 6>("triangle_adjacencies");
    std::vector<bcg_index_t> adjacency;
    for (const auto f : faces) {
        for (const auto h : get_halfedges(f)) {
            adjacency.push_back((unsigned int) halfedge_graph::get_from_vertex(h).idx);
            if (!is_boundary(h)) {
                auto o = halfedge_graph::get_opposite(h);
                auto op = halfedge_graph::get_prev(o);
                adjacency.push_back((unsigned int) halfedge_graph::get_from_vertex(op).idx);
            } else {
                auto p = halfedge_graph::get_prev(h);
                adjacency.push_back((unsigned int) halfedge_graph::get_from_vertex(p).idx);
            }
        }
        triangle_adjacencies[f] = Map(adjacency);
        adjacency.clear();
    }
    return triangle_adjacencies;
}

face_handle halfedge_mesh::find_closest_face(const position_t &point) const {
    distance_point3_triangle3 distance;
    face_handle closest_yet;
    auto min_distance = scalar_max;
    for (const auto f : faces) {
        auto h = get_halfedge(f);

        triangle3 triangle(positions[halfedge_graph::get_to_vertex(halfedge_graph::get_next(h))],
                           positions[halfedge_graph::get_to_vertex(h)],
                           positions[halfedge_graph::get_from_vertex(h)]);

        auto dist = distance(point, triangle).distance;
        if (dist < min_distance) {
            min_distance = dist;
            closest_yet = f;
        }
        if (dist == 0) break;
    }
    return closest_yet;
}

std::vector<face_handle> halfedge_mesh::find_closest_k_face(const position_t &point, size_t k) const {
    distance_point3_triangle3 distance;
    using DistIndex = std::pair<bcg_scalar_t, face_handle>;
    std::vector<DistIndex> closest_k;

    for (const auto f : faces) {
        auto h = get_halfedge(f);

        triangle3 triangle(positions[halfedge_graph::get_to_vertex(halfedge_graph::get_next(h))],
                           positions[halfedge_graph::get_to_vertex(h)],
                           positions[halfedge_graph::get_from_vertex(h)]);

        auto dist = distance(point, triangle).distance;
        if (closest_k.size() < k + 1) {
            closest_k.emplace_back(dist, f);
            if (closest_k.size() == k) {
                std::sort(closest_k.begin(), closest_k.end(), [](const DistIndex &lhs, const DistIndex &rhs) {
                    return lhs.first < rhs.first;
                });
                break;
            }
            continue;
        }

        if (dist < closest_k.back().first) {
            closest_k.back() = std::make_pair(dist, f);
            std::sort(closest_k.begin(), closest_k.end(), [](const DistIndex &lhs, const DistIndex &rhs) {
                return lhs.first < rhs.first;
            });
        }

    }
    std::vector<face_handle> indices;
    unzip<bcg_scalar_t, face_handle>(closest_k, nullptr, &indices);
    return indices;
}

std::vector<face_handle> halfedge_mesh::find_closest_faces(const position_t &point, bcg_scalar_t radius) const {
    distance_point3_triangle3 distance;
    using DistIndex = std::pair<bcg_scalar_t, face_handle>;
    std::vector<DistIndex> closest;
    for (const auto v : this->vertices) {
        for (const auto h : halfedge_graph::get_halfedges(v)) {
            triangle3 triangle(positions[halfedge_graph::get_to_vertex(halfedge_graph::get_next(h))],
                               positions[halfedge_graph::get_to_vertex(h)],
                               positions[halfedge_graph::get_from_vertex(h)]);

            auto dist = distance(point, triangle).distance;
            if (dist <= radius) {
                closest.emplace_back(dist, get_face(h));
            }
        }
    }

    std::sort(closest.begin(), closest.end(), [](const DistIndex &lhs, const DistIndex &rhs) {
        return lhs.first < rhs.first;
    });
    std::vector<face_handle> indices;
    unzip<bcg_scalar_t, face_handle>(closest, nullptr, &indices);
    return indices;
}

face_handle halfedge_mesh::find_closest_face_in_neighborhood(vertex_handle v, const position_t &point) const {
    distance_point3_triangle3 distance;
    face_handle closest_yet;
    bcg_scalar_t min_dist_yet = scalar_max;

    auto valence = halfedge_graph::get_valence(v);
    size_t count = 0;
    for (const auto f : get_faces(v)) {
        auto h = get_halfedge(f);
        triangle3 triangle(positions[halfedge_graph::get_to_vertex(halfedge_graph::get_next(h))],
                           positions[halfedge_graph::get_to_vertex(h)],
                           positions[v]);

        auto sqr_dist = distance(point, triangle).sqr_distance;
        if (sqr_dist < min_dist_yet) {
            min_dist_yet = sqr_dist;
            closest_yet = f;
        }
        ++count;
        if (count > valence) {
            std::cerr << "infinite loop?\n";
        }
        if (sqr_dist == 0) break;
    }
    return closest_yet;
}

face_handle halfedge_mesh::new_face() {
    faces.push_back();
    assert(faces.is_dirty());
    return face_handle(faces.size() - 1);
}

void halfedge_mesh::mark_face_deleted(face_handle f) {
    if (faces_deleted[f]) return;

    faces_deleted[f] = true;
    ++size_faces_deleted;
    faces_deleted.set_dirty();;
    assert(has_garbage());
}

std::string halfedge_mesh::to_string() const {
    std::stringstream stream;
    stream << halfedge_graph::to_string();
    stream << "mesh\n";

    if (has_garbage()) {
        stream << "size_faces_deleted: " << size_faces_deleted << "\n";
    }
    stream << "face properties:\n" << faces << "\n";
    return stream.str();
}

std::ostream &operator<<(std::ostream &stream, const halfedge_mesh &mesh) {
    stream << mesh.to_string();
    return stream;
}

}