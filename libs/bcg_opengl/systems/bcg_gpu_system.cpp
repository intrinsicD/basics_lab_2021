//
// Created by alex on 29.10.20.
//

#include "bcg_gpu_system.h"
#include "bcg_viewer_state.h"
#include "bcg_library/geometry/bcg_mesh.h"

namespace bcg {

gpu_system::gpu_system(viewer_state *state) : system("gpu_system", state) {
    state->dispatcher.sink<event::gpu::update_vertex_attributes>().connect<&gpu_system::on_update_vertex_attributes>(
            this);
}

void gpu_system::on_update_vertex_attributes(const event::gpu::update_vertex_attributes &event) {
    if(!state->scene.valid(event.id)) return;

    property_container *vertices = state->get_vertices(event.id);
    if (vertices) {
        auto &vao = state->scene.get_or_emplace<ogl_vertex_array>(event.id);
        if (!vao) {
            vao.create();
        }

        for (const auto &name : event.attributes_names) {
            if (vertices->has(name) && vertices->get_base_ptr(name)->is_dirty()) {
                auto &buffer = vao.vertex_buffers[name];
                if (!buffer) {
                    buffer.name = name;
                    buffer.create();
                    vao.capture_vertex_buffer(buffer);
                }
                auto *base_ptr = vertices->get_base_ptr(name);
                buffer.upload(base_ptr->void_ptr(), base_ptr->size(), base_ptr->dims(), 0, true);
                auto sb = base_ptr->size_bytes();
                assert(buffer.get_buffer_size_gpu() == sb);
                vertices->get_base_ptr(name)->set_clean();
            }
        }
    }
}

void gpu_system::on_update_edge_attributes(const event::gpu::update_edge_attributes &event) {
    if(!state->scene.valid(event.id)) return;

    property_container *edges = state->get_edges(event.id);

    if (edges) {
        auto &vao = state->scene.get_or_emplace<ogl_vertex_array>(event.id);
        if (!vao) {
            vao.create();
        }

        for (const auto &name : event.attributes_names) {
            if (edges->has(name) && edges->get_base_ptr(name)->is_dirty()) {
                auto &buffer = vao.vertex_buffers[name];
                if (!buffer) {
                    buffer.name = name;
                    buffer.create();
                    vao.capture_vertex_buffer(buffer);
                }
                auto *base_ptr = edges->get_base_ptr(name);
                buffer.upload(base_ptr->void_ptr(), base_ptr->size(), base_ptr->dims(), 0, true);
                edges->get_base_ptr(name)->set_clean();
            }
        }
    }
}

void gpu_system::on_update_face_attributes(const event::gpu::update_face_attributes &event) {
    if(!state->scene.valid(event.id)) return;

    property_container *faces = state->get_faces(event.id);
    auto &mesh = state->scene.get<halfedge_mesh>(event.id);

    if (faces) {
        auto &vao = state->scene.get_or_emplace<ogl_vertex_array>(event.id);
        if (!vao) {
            vao.create();
        }

        for (const auto &name : event.attributes_names) {
            if(name == "triangles"){
                if(!vao.element_buffer){
                    vao.element_buffer.name = name;
                    vao.set_element_buffer(vao.element_buffer);
                }
                if(vao.element_buffer.get_buffer_size_gpu() != faces->size() ){
                    auto triangles = mesh.get_triangles();
                    std::cout << triangles << "\n";
                    vao.element_buffer.upload(triangles.data(), faces->size(), 3, 0, true);
                }
            }else if(name == "triangles_adjacency"){
                if(!vao.adjacency_buffer){
                    vao.adjacency_buffer.name = name;
                    vao.set_element_buffer(vao.adjacency_buffer);
                }
                if(vao.adjacency_buffer.get_buffer_size_gpu() != faces->size() ){
                    auto adjacencies = mesh.get_triangles_adjacencies();
                    vao.adjacency_buffer.upload(adjacencies.data(), faces->size(), 6, 0, true);
                }
            }
            if (faces->has(name) && faces->get_base_ptr(name)->is_dirty()) {
                auto &buffer = vao.vertex_buffers[name];
                if (!buffer) {
                    buffer.name = name;
                    buffer.create();
                    vao.capture_vertex_buffer(buffer);
                }
                auto *base_ptr = faces->get_base_ptr(name);
                buffer.upload(base_ptr->void_ptr(), base_ptr->size(), base_ptr->dims(), 0, true);
                faces->get_base_ptr(name)->set_clean();
            }
        }
    }
}

}