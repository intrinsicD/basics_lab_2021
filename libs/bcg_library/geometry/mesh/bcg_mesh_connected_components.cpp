//
// Created by alex on 19.11.20.
//

#include <queue>
#include "bcg_mesh_connected_components.h"
#include "geometry/bcg_property_map_eigen.h"

namespace bcg {

size_t search_from_vertex(halfedge_mesh &mesh, vertex_handle v, int marker) {
    auto visited = mesh.vertices.get_or_add<bool, 1>("v_visited");
    auto connected_components = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_connected_component");

    std::queue<vertex_handle> todo;
    todo.push(v);
    size_t visited_count = 0;
    visited[v] = true;
    while (!todo.empty()) {
        auto v = todo.front();
        todo.pop();
        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
            auto vv = mesh.get_to_vertex(h);
            if (!visited[vv]) {
                todo.push(vv);
                visited[vv] = true;
            }
        }
        visited[v] = true;
        connected_components[v] = marker;
        ++visited_count;
    }
    return visited_count;
}

size_t search_from_face(halfedge_mesh &mesh, face_handle f, int marker) {
    auto visited = mesh.faces.get_or_add<bool, 1>("f_visited");
    auto connected_components = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_connected_component");

    std::queue<face_handle> todo;
    todo.push(f);
    size_t visited_count = 0;
    while (!todo.empty()) {
        auto current_face = todo.front();
        todo.pop();
        for (const auto h : mesh.get_halfedges(current_face)) {
            if (!mesh.is_boundary(h)) {
                auto oh = mesh.get_opposite(h);
                auto nf = mesh.get_face(oh);
                if (!visited[nf]) {
                    todo.push(nf);
                }
            }
        }
        visited[current_face] = true;
        connected_components[current_face] = marker;
        ++visited_count;
    }
    return visited_count;
}

size_t mesh_connected_components_detect(halfedge_mesh &mesh) {
    auto visited = mesh.vertices.get_or_add<bool, 1>("v_visited");
    visited.reset(false);
    auto connected_components = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_connected_component");
    connected_components.reset(0);

    size_t num_connected_components = 0;
    size_t visited_count = 0;

    for (const auto &v : mesh.vertices) {
        if (!visited[v]) {
            visited_count += search_from_vertex(mesh, v, num_connected_components);
            ++num_connected_components;
        }
        if (visited_count >= mesh.vertices.size()) {
            //early out if all are already visited
            std::cout << "early out... \n";
            break;
        }
    }

    std::cout << "Detected " + std::to_string(num_connected_components) + " Connected Components\n";
    return num_connected_components;
}

std::vector<halfedge_mesh> mesh_connected_components_split(halfedge_mesh &mesh) {
    if (!mesh.vertices.has("v_connected_component")) {
        mesh_connected_components_detect(mesh);
    }
    auto connected_components = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_connected_component");
    size_t num_connected_components = MapConst(connected_components).maxCoeff() + 1;

    if (num_connected_components < 2) return {};

    std::vector<halfedge_mesh> parts(num_connected_components);

    auto index_map = mesh.vertices.get_or_add<int, 1>("v_index_map");
    auto component_map = mesh.vertices.get_or_add<int, 1>("v_component_map");

    vertex_handle v_new;
    for (const auto v : mesh.vertices) {
        auto cc = connected_components[v];
        index_map[v] = parts[cc].vertices.size();
        component_map[v] = cc;
        parts[cc].add_vertex(mesh.positions[v]);
    }

    face_handle f_new;
    for (const auto f : mesh.faces) {
        std::vector<vertex_handle> face;
        int component = -1;
        for (const auto v : mesh.get_vertices(f)) {
            face.emplace_back(index_map[v]);
            component = connected_components[v];
        }
        if (component == -1) std::abort();
        parts[component].add_face(face);
    }

    mesh.vertices.remove(index_map);
    mesh.vertices.remove(component_map);
    return parts;
}

}