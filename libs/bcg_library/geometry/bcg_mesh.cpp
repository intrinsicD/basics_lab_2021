//
// Created by alex on 13.10.20.
//

#include "bcg_mesh.h"

namespace bcg {

halfedge_mesh::halfedge_mesh() : halfedge_graph(),
                                 fconn(faces.get_or_add<face_connectivity, 1>("connectivity")),
                                 faces_deleted(faces.add<bool, 1>("deleted")),
                                 size_faces_deleted(0) {
    halfedges.remove("connectivity"); //unlink graph h_connectivity
    halfedges.get_or_add<halfedge_connectivity, 1>("connectivity");
}

void halfedge_mesh::assign(const halfedge_mesh &other) {
    if (this != &other) {
        // clear properties
        point_cloud::assign(other);
        halfedges.remove_all();
        edges.remove_all();
        faces.remove_all();

        // allocate standard properties

        vconn = vertices.get<vertex_connectivity, 1>("connectivity");
        hconn = halfedges.get<halfedge_connectivity, 1>("connectivity");
        fconn = faces.get<face_connectivity, 1>("connectivity");

        halfedges_deleted = halfedges.get_or_add<bool, 1>("deleted");
        edges_deleted = edges.get_or_add<bool, 1>("deleted");

        // copy properties from other mesh
        positions.vector() = other.positions.vector();

        vconn.vector() = other.vconn.vector();
        hconn.vector() = other.hconn.vector();
        fconn.vector() = other.fconn.vector();

        halfedges_deleted.vector() = other.halfedges_deleted.vector();
        edges_deleted.vector() = other.edges_deleted.vector();


        // resize (needed by property containers)
        vertices.resize(other.vertices.size());
        edges.resize(other.edges.size());
        halfedges.resize(other.halfedges.size());
        faces.resize(other.faces.size());

        // how many elements are deleted?
        size_vertices_deleted = other.size_vertices_deleted;
        size_halfedges_deleted = other.size_halfedges_deleted;
        size_edges_deleted = other.size_edges_deleted;
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
        positions = vertices.get<position_t, 3>("position");
        vconn = vertices.get<vertex_connectivity, 1>("connectivity");
        hconn = halfedges.get<halfedge_connectivity, 1>("connectivity");
        fconn = faces.get<face_connectivity, 1>("connectivity");

        // how many elements are deleted?
        size_vertices_deleted = other.size_vertices_deleted;
        size_halfedges_deleted = other.size_halfedges_deleted;
        size_edges_deleted = other.size_edges_deleted;
        size_faces_deleted = other.size_faces_deleted;
    }
    return *this;
}

bool halfedge_mesh::has_garbage() const {
    return size_faces_deleted > 0 || halfedge_graph::has_garbage();
}

void halfedge_mesh::garbage_collection() {
    size_t nV = vertices.size();
    size_t nE = edges.size();
    size_t nH = halfedges.size();
    size_t nF = faces.size();

    auto vmap = vertices.add<vertex_handle, 1>("garbage_collection");
    auto hmap = halfedges.add<halfedge_handle, 1>("garbage_collection");
    auto fmap = faces.add<face_handle, 1>("garbage_collection");

    for (size_t i = 0; i < nV; ++i) {
        vmap[i] = i;
    }
    for (size_t i = 0; i < nH; ++i) {
        hmap[i] = i;
    }
    for (size_t i = 0; i < nF; ++i) {
        fmap[i] = i;
    }

    if (nV > 0) {
        size_t i0 = 0;
        size_t i1 = nV - 1;
        auto vertices_deleted = vertices.get<bool, 1>("deleted");
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
        auto edges_deleted = edges.get<bool, 1>("deleted");
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
        auto faces_deleted = faces.get<bool, 1>("deleted");
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
    return !(h && get_face(h));
}

bool halfedge_mesh::is_boundary(edge_handle e) const {
    return is_boundary(halfedge_graph::get_halfedge(e, 0)) || is_boundary(halfedge_graph::get_halfedge(e, 1));
}

bool halfedge_mesh::is_boundary(face_handle f) const {
    for (const auto h : get_halfedges(f)) {
        if (is_boundary(halfedge_graph::get_opposite(h))) {
            return true;
        }
    }
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
    if (!f0 || !f1) {
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
            if (halfedge_graph::find_halfedge(vv, v1)) {
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

    return !find_halfedge(v0, v1);
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
            std::cerr << "add_face: complex vertex!";
            return face_handle();
        }

        f_halfedges[i] = halfedge_graph::find_halfedge(f_vertices[i], f_vertices[ii]);
        h_is_new[i] = !f_halfedges[i];
        if (!h_is_new[i] && !is_boundary(f_halfedges[i])) {
            std::cerr << "add_face: complex edge!";
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
                if (is_boundary(boundary_prev))
                    std::cerr << "boundary_prev should be boundary!";
                if (is_boundary(boundary_next))
                    std::cerr << "boundary_next should be boundary!";

                if (boundary_next == inner_next) {
                    std::cerr << "add_face: patch re-linking failed!";
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
                    if (!halfedge_graph::get_halfedge(v)) {
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

    if (h) {
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
    auto deleted = faces.get_or_add<bool, 1>("deleted");
    if (deleted[f]) {
        return;
    }

    deleted[f] = true;
    ++size_faces_deleted;
    deleted.set_dirty();

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

        auto deleted_vertices = vertices.get_or_add<bool, 1>("deleted");
        auto deleted_edges = edges.get_or_add<bool, 1>("deleted");
        auto deleted_halfedges = halfedges.get_or_add<bool, 1>("deleted");

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
            deleted_edges[*delit] = true;
            deleted_halfedges[h0] = true;
            deleted_halfedges[h1] = true;

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
    for (const auto f : get_faces(v)) {
        incident_faces.push_back(f);
    }
    for (const auto f : incident_faces) {
        delete_face(f);
    }
    mark_vertex_deleted(v);
}

}