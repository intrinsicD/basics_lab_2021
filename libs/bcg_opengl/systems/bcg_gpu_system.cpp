//
// Created by alex on 29.10.20.
//

#include "bcg_gpu_system.h"
#include "bcg_viewer_state.h"
#include "math/bcg_matrix_map_eigen.h"
#include "geometry/bcg_property_map_eigen.h"
#include "bcg_library/geometry/mesh/bcg_mesh.h"

namespace bcg {

gpu_system::gpu_system(viewer_state *state) : system("gpu_system", state) {
    state->dispatcher.sink<event::gpu::update_vertex_attributes>().connect<&gpu_system::on_update_vertex_attributes>(
            this);
    state->dispatcher.sink<event::gpu::update_edge_attributes>().connect<&gpu_system::on_update_edge_attributes>(
            this);
    state->dispatcher.sink<event::gpu::update_face_attributes>().connect<&gpu_system::on_update_face_attributes>(
            this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&gpu_system::on_shutdown>(this);
}

void gpu_system::on_update_vertex_attributes(const event::gpu::update_vertex_attributes &event) {
    if (!state->scene.valid(event.id)) return;

    property_container *vertices = state->get_vertices(event.id);
    if (vertices) {
        auto &shape = state->scene.get_or_emplace<ogl_shape>(event.id);
        const auto &attributes = event.attributes;
        for (const auto &attribute : attributes) {
            if (vertices->has(attribute.property_name)) {
                auto &buffer = shape.vertex_buffers[attribute.buffer_name];
                bool first = false;
                if (!buffer) {
                    buffer.name = attribute.buffer_name;
                    buffer.create();
                    first = true;
                }

                auto *base_ptr = vertices->get_base_ptr(attribute.property_name);

                if(base_ptr->is_dirty() || first || attribute.update){
                    buffer.bind();
                    if(base_ptr->void_ptr() == nullptr){
                        Matrix<float, -1, 1> p = MapConst(vertices->get<bool, 1>(attribute.property_name)).cast<float>();
                        buffer.upload(p.data(), base_ptr->size(), base_ptr->dims(), 0, true);
                    }else{
                        buffer.upload(base_ptr->void_ptr(), base_ptr->size(), base_ptr->dims(), 0, true);
                    }
                    base_ptr->set_clean();
/*                    auto sb = base_ptr->size_bytes();
                    assert(buffer.get_buffer_size_gpu() == sb && buffer);*/
                    buffer.release();
                }
            }
        }
    }
}

void gpu_system::on_update_edge_attributes(const event::gpu::update_edge_attributes &event) {
    if (!state->scene.valid(event.id)) return;

    property_container *edges = state->get_edges(event.id);

    if (edges) {
        auto &shape = state->scene.get_or_emplace<ogl_shape>(event.id);

        const auto &attributes = event.attributes;
        for (const auto &attribute : attributes) {
            if (edges->has(attribute.property_name)) {
                auto &buffer = shape.vertex_buffers[attribute.buffer_name];

                if (!buffer) {
                    buffer.name = attribute.buffer_name;
                    buffer.create();
                }

                auto *base_ptr = edges->get_base_ptr(attribute.property_name);

                if(base_ptr->is_dirty()){
                    buffer.bind();
                    buffer.upload(base_ptr->void_ptr(), base_ptr->size(), base_ptr->dims(), 0, true);
                    base_ptr->set_clean();
                    buffer.release();
                }
            }
        }
    }
}

void gpu_system::on_update_face_attributes(const event::gpu::update_face_attributes &event) {
    if (!state->scene.valid(event.id)) return;

    property_container *faces = state->get_faces(event.id);
    auto &mesh = state->scene.get<halfedge_mesh>(event.id);

    if (faces) {
        auto &shape = state->scene.get_or_emplace<ogl_shape>(event.id);

        const auto &attributes = event.attributes;
        for (const auto &attribute : attributes) {
            if (faces->get<halfedge_mesh::face_connectivity, 1>("connectivity").is_dirty()) {
                if (attribute.property_name == "triangles") {
                    if (!shape.element_buffer) {
                        shape.element_buffer.name = attribute.buffer_name;
                        shape.element_buffer.create();
                    }

                    auto triangles = mesh.get_triangles();
                    std::cout << triangles << "\n";
                    shape.element_buffer.bind();
                    shape.element_buffer.upload(triangles.data(), faces->size(), 3, 0, true);
                }
                if (attribute.property_name == "triangles_adjacency") {
                    if (!shape.adjacency_buffer) {
                        shape.adjacency_buffer.name = attribute.buffer_name;
                        shape.adjacency_buffer.create();
                    }

                    auto triangle_adjacencies = mesh.get_triangles_adjacencies();
                    std::cout << triangle_adjacencies << "\n";
                    shape.adjacency_buffer.bind();
                    shape.adjacency_buffer.upload(triangle_adjacencies.data(), faces->size(), 3, 0, true);
                }
            } else if (faces->has(attribute.property_name)) {
                if (attribute.property_name == "triangles_adjacency" || attribute.property_name == "triangles") continue;
                auto &buffer = shape.vertex_buffers[attribute.buffer_name];

                if (!buffer) {
                    buffer.name = attribute.buffer_name;
                    buffer.create();
                }

                auto *base_ptr = faces->get_base_ptr(attribute.property_name);

                if(base_ptr->is_dirty()){
                    buffer.bind();
                    buffer.upload(base_ptr->void_ptr(), base_ptr->size(), base_ptr->dims(), 0, true);
                    base_ptr->set_clean();
                    buffer.release();
                }
            }
        }
        faces->get<halfedge_mesh::face_connectivity, 1>("connectivity").set_clean();
    }
}

void gpu_system::on_shutdown(const event::internal::shutdown &event) {
    auto view = state->scene.view<ogl_shape>();
    for (const auto id : view) {
        auto &shape = view.get<ogl_shape>(id);
        for (auto &item : shape.vertex_buffers) {
            item.second.destroy();
        }
        shape.element_buffer.destroy();
        shape.adjacency_buffer.destroy();
    }
}

}