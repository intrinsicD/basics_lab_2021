//
// Created by alex on 10.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_H

#include "../math/bcg_vector.h"
#include "bcg_property.h"

namespace bcg {

struct point_cloud {
    using position_t = vec3f;
    property_container vertices, object_properties;
    property<position_t, 3> positions;
    size_t size_vertices_deleted;

    point_cloud();

    virtual ~point_cloud() = default;

    void assign(const point_cloud &other);

    point_cloud &operator=(const point_cloud &other);

    bool empty() const;

    virtual bool has_garbage() const;

    virtual void garbage_collection();

    vertex_handle new_vertex();

    vertex_handle add_vertex(const position_t &point);

    virtual void delete_vertex(vertex_handle v);
};

std::ostream operator<<(std::ostream stream, const point_cloud &pc);

vertex_handle find_closest_vertex(const point_cloud &pc, const point_cloud::position_t &point);

std::vector<vertex_handle>
find_closest_k_vertices(const point_cloud &pc, const point_cloud::position_t &point, size_t k);

std::vector<vertex_handle>
find_closest_vertices_radius(const point_cloud &pc, const point_cloud::position_t &point, float radius);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_H
