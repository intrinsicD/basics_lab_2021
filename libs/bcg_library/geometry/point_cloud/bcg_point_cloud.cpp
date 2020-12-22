//
// Created by alex on 10.10.20.
//

#include <cassert>
#include "bcg_point_cloud.h"
#include "utils/bcg_stl_utils.h"

namespace bcg {

point_cloud::point_cloud() : size_vertices_deleted(0) {
    positions = vertices.add<position_t, 3>("v_position");
    vertices_deleted = vertices.add<bool, 1>("v_deleted");

    if(!positions){
        std::cerr << "positions not valid\n";
    }

    if(!vertices_deleted){
        std::cerr << "vertices_deleted not valid\n";
    }
    assert(positions);
    assert(vertices_deleted);
}

void point_cloud::assign(const point_cloud &other) {
    if (this != &other) {
        vertices.remove_all();
        positions = vertices.get_or_add<position_t, 3>("v_position");
        vertices_deleted = vertices.get_or_add<bool, 1>("v_deleted");
        positions.vector() = other.positions.vector();
        vertices_deleted.vector() = other.vertices_deleted.vector();

        vertices.resize(other.vertices.size());

        size_vertices_deleted = other.size_vertices_deleted;
    }
}

point_cloud &point_cloud::operator=(const point_cloud &other) {
    if (this != &other) {
        vertices = other.vertices;
        positions = vertices.get_or_add<position_t, 3>("v_position");
        vertices_deleted = vertices.get_or_add<bool, 1>("v_deleted");
        size_vertices_deleted = other.size_vertices_deleted;
    }
    return *this;
}

void point_cloud::clear() {
    // remove all properties
    vertices.clear();

    // really free their memory
    vertices.free_unused_memory();

    // add the standard properties back
    positions = vertices.add<position_t, 3>("v_position");
    vertices_deleted = vertices.add<bool, 1>("v_deleted");
    size_vertices_deleted = 0;
}

bool point_cloud::empty() const {
    return vertices.size() == 0;
}

bool point_cloud::is_valid(vertex_handle v) const {
    return v.idx < vertices.size();
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
    if (v.is_valid()) positions[v] = point;
    return v;
}

void point_cloud::delete_vertex(vertex_handle v) {
    mark_vertex_deleted(v);
}

size_t point_cloud::num_vertices() const {
    return vertices.size() - size_vertices_deleted;
}

void point_cloud::mark_vertex_deleted(vertex_handle v) {
    if (vertices_deleted[v]) return;

    vertices_deleted[v] = true;
    ++size_vertices_deleted;
    vertices.set_dirty();
    assert(vertices.is_dirty());
    assert(has_garbage());
}

vertex_handle point_cloud::find_closest_vertex(const point_cloud::position_t &point) {
    vertex_handle closest_yet(0);
    auto min_dist_yet = scalar_max;
    for (const auto v : vertices) {
        auto dist = (positions[v] - point).norm();
        if (dist < min_dist_yet) {
            min_dist_yet = dist;
            closest_yet = v;

            if (dist < scalar_eps) break;
        }
    }
    return closest_yet;
}

std::vector<vertex_handle>
point_cloud::find_closest_k_vertices(const point_cloud::position_t &point, size_t k) {
    using DistIndex = std::pair<bcg_scalar_t, vertex_handle>;
    std::vector<DistIndex> closest_k;

    for (const auto v : vertices) {
        auto dist = (positions[v] - point).norm();
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
    unzip<bcg_scalar_t, vertex_handle>(closest_k, nullptr, &indices);
    return indices;
}

std::vector<vertex_handle>
point_cloud::find_closest_vertices_radius(const point_cloud::position_t &point, bcg_scalar_t radius) {
    using DistIndex = std::pair<bcg_scalar_t, vertex_handle>;
    std::vector<DistIndex> closest;
    for (const auto v: vertices) {
        auto dist = (positions[v] - point).norm();
        if (dist <= radius) {
            closest.emplace_back(dist, v);
        }
    }

    std::sort(closest.begin(), closest.end());
    std::vector<vertex_handle> indices;
    unzip<bcg_scalar_t, vertex_handle>(closest, nullptr, &indices);
    return indices;
}

std::string point_cloud::to_string() const {
    std::stringstream stream;
    stream << "point cloud\n";
    if (has_garbage()) {
        stream << "size_vertices_deleted: " << size_vertices_deleted << "\n";
    }
    stream << "vertex properties:\n" << vertices << "\n";
    return stream.str();
}

std::ostream &operator<<(std::ostream &stream, const point_cloud &pc) {
    stream << pc.to_string() << "\n";
    return stream;
}

}