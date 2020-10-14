//
// Created by alex on 10.10.20.
//

#include <cassert>
#include "bcg_point_cloud.h"
#include "../utils/bcg_stl_utils.h"
#include "../math/bcg_math_common.h"

namespace bcg {

point_cloud::point_cloud() : positions(vertices.add<position_t, 3>("position")),
                             vertices_deleted(vertices.add<bool, 1>("deleted")),
                             size_vertices_deleted(0) {

}

void point_cloud::assign(const point_cloud &other) {
    if (this != &other) {
        vertices.remove_all();
        positions = vertices.get_or_add<position_t, 3>("position");
        vertices_deleted = vertices.get_or_add<bool, 1>("deleted");
        positions.vector() = other.positions.vector();
        vertices_deleted.vector() = other.vertices_deleted.vector();

        vertices.resize(other.vertices.size());

        size_vertices_deleted = other.size_vertices_deleted;
    }
}

point_cloud &point_cloud::operator=(const point_cloud &other) {
    if (this != &other) {
        vertices = other.vertices;
        positions = vertices.get_or_add<position_t, 3>("position");
        vertices_deleted = vertices.get_or_add<bool, 1>("deleted");
        size_vertices_deleted = other.size_vertices_deleted;
    }
    return *this;
}

bool point_cloud::empty() const {
    return vertices.size() == 0;
}

bool point_cloud::has_garbage() const {
    return size_vertices_deleted > 0;
}

void point_cloud::garbage_collection() {
    if (!has_garbage()) return;

    size_t nV = vertices.size();
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

    vertices.resize(nV);
    vertices.free_unused_memory();
    size_vertices_deleted = 0;
    assert(!has_garbage());
    assert(vertices.is_dirty());
}


vertex_handle point_cloud::new_vertex() {
    vertices.push_back();
    assert(vertices.is_dirty());
    return vertex_handle(vertices.size() - 1);
}


vertex_handle point_cloud::add_vertex(const position_t &point) {
    auto v = new_vertex();
    positions[v] = point;
    return v;
}

void point_cloud::delete_vertex(vertex_handle v) {
    mark_vertex_deleted(v);
}

size_t point_cloud::num_vertices() const{
    return vertices.size() - size_vertices_deleted;
}

void point_cloud::mark_vertex_deleted(vertex_handle v){
    if (vertices_deleted[v]) return;

    vertices_deleted[v] = true;
    ++size_vertices_deleted;
    vertices.set_dirty();
    assert(vertices.is_dirty());
    assert(has_garbage());
}

std::ostream &operator<<(std::ostream &stream, const point_cloud &pc) {
    stream << "point cloud\n";
    if (pc.has_garbage()) {
        stream << "size_vertices_deleted: " << pc.size_vertices_deleted << "\n";
    }
    stream << pc.vertices << "\n";
    return stream;
}

vertex_handle point_cloud::find_closest_vertex(const point_cloud::position_t &point) {
    vertex_handle closest_yet(0);
    auto min_dist_yet = flt_max;
    for (const auto v : vertices) {
        auto dist = distance(positions[v], point);
        if (dist < min_dist_yet) {
            min_dist_yet = dist;
            closest_yet = v;

            if (dist < flt_eps) break;
        }
    }
    return closest_yet;
}

std::vector<vertex_handle>
point_cloud::find_closest_k_vertices(const point_cloud::position_t &point, size_t k) {
    using DistIndex = std::pair<float, vertex_handle>;
    std::vector<DistIndex> closest_k;

    for (const auto v : vertices) {
        auto dist = distance(positions[v], point);
        if (closest_k.size() < k) {
            closest_k.emplace_back(dist, v);
        } else {
            std::sort(closest_k.begin(), closest_k.end(), [](const DistIndex &lhs, const DistIndex &rhs) {
                return lhs.first < rhs.first;
            });

            closest_k.resize(k);
            if (dist < closest_k.back().first) {
                closest_k.back() = std::make_pair(dist, v);
            }
        }
    }

    std::sort(closest_k.begin(), closest_k.end());
    std::vector<vertex_handle> indices;
    unzip<float, vertex_handle>(closest_k, nullptr, &indices);
    return indices;
}

std::vector<vertex_handle>
point_cloud::find_closest_vertices_radius(const point_cloud::position_t &point, float radius) {
    using DistIndex = std::pair<float, vertex_handle>;
    std::vector<DistIndex> closest;
    for (const auto v: vertices) {
        auto dist = distance(positions[v], point);
        if (dist <= radius) {
            closest.emplace_back(dist, v);
        }
    }

    std::sort(closest.begin(), closest.end());
    std::vector<vertex_handle> indices;
    unzip<float, vertex_handle>(closest, nullptr, &indices);
    return indices;
}

}