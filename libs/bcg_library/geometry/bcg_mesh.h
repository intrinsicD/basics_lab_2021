//
// Created by alex on 10.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_H
#define BCG_GRAPHICS_BCG_MESH_H

#include "bcg_property.h"
#include "../math/bcg_vector.h"

namespace bcg {

struct halfedge_mesh {
    using position_t = vec3f;
    property_container vertices, halfedges, edges, faces, object_properties;

    struct vertex_connectivity {
        halfedge_handle h;
    };

    struct halfedge_connectivity {
        vertex_handle v;
        halfedge_handle nh;
        halfedge_handle ph;
        face_handle f;
    };

    struct face_connectivity {
        halfedge_handle h;
    };

    property<position_t, 3> position;
    property<vertex_connectivity, 1> vconn;
    property<halfedge_connectivity, 1> hconn;

    halfedge_mesh();

    bool empty();

    void garbage_collection();

    void print() const;

    vertex_handle add_vertex(const position_t &point);

    void delete_vertex(vertex_handle v);

    vertex_handle find_closest_vertex(const vertex_handle &point) const;

    std::vector<vertex_handle> find_closest_k_vertices(const position_t &point, size_t k) const;

    std::vector<vertex_handle> find_closest_vertices_radius(const position_t &point, float radius) const;

    bool is_isolated(vertex_handle v) const;

    halfedge_handle get_halfedge(vertex_handle v) const;

    void set_halfedge(vertex_handle v, halfedge_handle h);

    vertex_handle get_to_vertex(halfedge_handle h) const;

    void set_to_vertex(halfedge_handle h, vertex_handle v) const;

    vertex_handle get_from_vertex(halfedge_handle h) const;

    halfedge_handle get_next(halfedge_handle h) const;

    void set_next(halfedge_handle h, halfedge_handle nh) const;

    halfedge_handle get_prev(halfedge_handle h) const;

    halfedge_handle get_opposite(halfedge_handle h) const;

    halfedge_handle rotate_cw(halfedge_handle h) const;

    halfedge_handle rotate_ccw(halfedge_handle h) const;

    edge_handle get_edge(halfedge_handle h) const;

    halfedge_handle get_halfedge(edge_handle e, bool i) const;

    vertex_handle get_vertex(edge_handle e, bool i) const;

    int get_valence(vertex_handle v) const;

    float get_length(halfedge_handle h) const;

    float get_length(edge_handle h) const;

    position_t get_center(halfedge_handle h) const;

    position_t get_center(edge_handle h) const;

    position_t get_vector(halfedge_handle h) const;

    position_t get_vector(edge_handle h) const;

    halfedge_handle add_edge(vertex_handle v0, vertex_handle v1);

    void remove_edge(edge_handle e);

    halfedge_handle find_halfedge(vertex_handle v0, vertex_handle v1);

    edge_handle find_edge(vertex_handle v0, vertex_handle v1);

    struct vertex_around_vertex_circulator {
        const halfedge_mesh *ds;
        halfedge_handle halfedge;
        bool active;

        explicit vertex_around_vertex_circulator(const halfedge_mesh *ds = nullptr, vertex_handle v = vertex_handle());

        inline bool operator==(const vertex_around_vertex_circulator &rhs) const;

        inline bool operator!=(const vertex_around_vertex_circulator &rhs) const;

        inline vertex_around_vertex_circulator &operator++();

        inline vertex_around_vertex_circulator &operator--();

        inline vertex_handle operator*() const;

        inline operator bool() const;

        inline vertex_around_vertex_circulator &begin();

        inline vertex_around_vertex_circulator &end();
    };

    vertex_around_vertex_circulator get_vertices(vertex_handle v) const;

    struct halfedge_around_vertex_circulator {
        const halfedge_mesh *ds;
        halfedge_handle halfedge;
        bool active;

        explicit halfedge_around_vertex_circulator(const halfedge_mesh *ds = nullptr,
                                                   vertex_handle v = vertex_handle());

        inline bool operator==(const halfedge_around_vertex_circulator &rhs) const;

        inline bool operator!=(const halfedge_around_vertex_circulator &rhs) const;

        inline halfedge_around_vertex_circulator &operator++();

        inline halfedge_around_vertex_circulator &operator--();

        inline halfedge_handle operator*() const;

        inline operator bool() const;

        inline halfedge_around_vertex_circulator &begin();

        inline halfedge_around_vertex_circulator &end();
    };

    halfedge_around_vertex_circulator get_halfedges(vertex_handle v) const;

    std::vector<vec2i> get_edge_connectivity() const;

    edge_handle find_closest_edge(const position_t &point) const;

    std::vector<edge_handle> find_closest_k_edges(const position_t &point, size_t k) const;

    std::vector<edge_handle> find_closest_edges_radius(const position_t &point, float radius) const;

    edge_handle find_closest_edge_in_neighborhood(vertex_handle v, const position_t &point) const;

protected:
    halfedge_handle new_edge(vertex_handle v0, vertex_handle v1);

};

}

#endif //BCG_GRAPHICS_BCG_MESH_H
