//
// Created by alex on 10.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_H
#define BCG_GRAPHICS_BCG_MESH_H

#include "geometry/graph/bcg_graph.h"

namespace bcg {

struct halfedge_mesh : public halfedge_graph {
    struct face_connectivity {
        halfedge_handle h;

        friend std::ostream &operator<<(std::ostream &stream, const face_connectivity &value) {
            stream << "h: " << value.h.idx;
            return stream;
        }

        operator size_t() const {
            return h.idx;
        }
    };

    face_container faces;
    property<face_connectivity, 1> fconn;
    property<bool, 1> faces_deleted;
    size_t size_faces_deleted;

    halfedge_mesh();

    ~halfedge_mesh() override = default;

    void assign(const halfedge_mesh &other);

    halfedge_mesh &operator=(const halfedge_mesh &other);

    bool operator==(const halfedge_mesh &other) const;

    size_t num_faces() const;

    bool has_garbage() const override;

    void garbage_collection() override;

    bool is_boundary(vertex_handle v) const override;

    bool is_boundary(halfedge_handle h) const override;

    bool is_boundary(edge_handle e) const override;

    bool is_boundary(face_handle f) const;

    bool is_manifold(vertex_handle v) const;

    bool is_manifold() const;

    bool is_triangle_mesh() const;

    bool is_quad_mesh() const;

    bool is_removal_ok(edge_handle e) const;

    bool is_collapse_ok(halfedge_handle h) const;

    bool is_flip_ok(edge_handle e) const;

    face_handle get_face(edge_handle e, bool i) const;

    face_handle get_face(halfedge_handle h) const;

    void set_face(halfedge_handle h, face_handle f);

    halfedge_handle get_halfedge(face_handle f) const;

    void set_halfedge(face_handle f, halfedge_handle h);

    size_t get_valence(face_handle f) const;

    face_handle add_face(const std::vector<vertex_handle> &f_vertices);

    face_handle add_triangle(vertex_handle v0, vertex_handle v1, vertex_handle v2);

    face_handle add_quad(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3);

    void adjust_outgoing_halfedge(vertex_handle v);

    void delete_face(face_handle f);

    void remove_edge(edge_handle e) override;

    void delete_edge(edge_handle e);

    void delete_vertex(vertex_handle v) override;

    struct face_around_vertex_circulator {
        const halfedge_mesh *ds;
        halfedge_handle halfedge;
        bool active;

        explicit face_around_vertex_circulator(const halfedge_mesh *ds = nullptr, vertex_handle v = vertex_handle());

        bool operator==(const face_around_vertex_circulator &rhs) const;

        bool operator!=(const face_around_vertex_circulator &rhs) const;

        face_around_vertex_circulator &operator++();

        face_around_vertex_circulator &operator--();

        face_handle operator*() const;

        operator bool() const;

        face_around_vertex_circulator &begin();

        face_around_vertex_circulator &end();
    };

    face_around_vertex_circulator get_faces(vertex_handle v) const;

    struct vertex_around_face_circulator {
        const halfedge_mesh *ds;
        halfedge_handle halfedge;
        bool active;

        explicit vertex_around_face_circulator(const halfedge_mesh *ds = nullptr, face_handle f = face_handle());

        bool operator==(const vertex_around_face_circulator &rhs) const;

        bool operator!=(const vertex_around_face_circulator &rhs) const;

        vertex_around_face_circulator &operator++();

        vertex_around_face_circulator &operator--();

        vertex_handle operator*() const;

        vertex_around_face_circulator &begin();

        vertex_around_face_circulator &end();
    };

    vertex_around_face_circulator get_vertices(face_handle f) const;


    struct halfedge_around_face_circulator {
        const halfedge_mesh *ds;
        halfedge_handle halfedge;
        bool active;

        explicit halfedge_around_face_circulator(const halfedge_mesh *ds = nullptr, face_handle f = face_handle());

        bool operator==(const halfedge_around_face_circulator &rhs) const;

        bool operator!=(const halfedge_around_face_circulator &rhs) const;

        halfedge_around_face_circulator &operator++();

        halfedge_around_face_circulator &operator--();

        halfedge_handle operator*() const;

        halfedge_around_face_circulator &begin();

        halfedge_around_face_circulator &end();
    };

    halfedge_around_face_circulator get_halfedges(face_handle f) const;

    void triangulate();

    void triangulate(face_handle f);

    void collapse(halfedge_handle h);

    void remove_edge_helper(halfedge_handle h);

    void remove_loop_helper(halfedge_handle h);

    vertex_handle split(face_handle f, const position_t &point);

    vertex_handle split(face_handle f, vertex_handle v);

    halfedge_handle split(edge_handle e, const position_t &point);

    halfedge_handle split(edge_handle e, vertex_handle v);

    halfedge_handle insert_vertex(edge_handle e, const position_t &point);

    halfedge_handle insert_vertex(edge_handle e, vertex_handle v);

    halfedge_handle insert_vertex(halfedge_handle h, vertex_handle v);

    halfedge_handle insert_edge(halfedge_handle h0, halfedge_handle h1);

    void flip(edge_handle e);

    property<VectorI<3>, 3> get_triangles();

    property<VectorI<6>, 6> get_triangles_adjacencies();

    face_handle find_closest_face(const position_t &point) const;

    std::vector<face_handle> find_closest_k_face(const position_t &point, size_t k) const;

    std::vector<face_handle> find_closest_faces(const position_t &point, bcg_scalar_t radius) const;

    face_handle find_closest_face_in_neighborhood(vertex_handle v, const position_t &point) const;

    std::string to_string() const override;
protected:
    face_handle new_face();

    void mark_face_deleted(face_handle f);

    std::vector<halfedge_handle> m_add_face_halfedges;
    std::vector<bool> m_add_face_is_new;
    std::vector<bool> m_add_face_needs_adjust;
    using NextCacheEntry = std::pair<halfedge_handle, halfedge_handle>;
    using NextCache = std::vector<NextCacheEntry>;
    NextCache m_add_face_next_cache;
};

std::ostream &operator<<(std::ostream &stream, const halfedge_mesh &mesh);

}

#endif //BCG_GRAPHICS_BCG_MESH_H
