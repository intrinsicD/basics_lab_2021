//
// Created by alex on 05.11.20.
//


#include <vector>
#include <string>

#include "bcg_mesh_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_mesh.h"
#include "renderers/bcg_attribute.h"
#include "bcg_events_mesh_renderer.h"

namespace bcg {

mesh_renderer::mesh_renderer(viewer_state *state) : renderer("mesh_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&mesh_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&mesh_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::mesh_renderer::enqueue>().connect<&mesh_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::mesh_renderer::setup_for_rendering>().connect<&mesh_renderer::on_setup_for_rendering>(this);
    state->dispatcher.sink<event::mesh_renderer::set_position_attribute>().connect<&mesh_renderer::on_set_position_attribute>(this);
    state->dispatcher.sink<event::mesh_renderer::set_normal_attribute>().connect<&mesh_renderer::on_set_normal_attribute>(this);
    state->dispatcher.sink<event::mesh_renderer::set_color_attribute>().connect<&mesh_renderer::on_set_color_attribute>(this);
}

void mesh_renderer::on_startup(const event::internal::startup &) {
    programs["mesh_renderer_program"] = state->shaders.load("mesh_renderer_program",
                                                            state->config.renderers_path +
                                                            "mesh_renderer/mesh_vertex_shader.glsl",
                                                            state->config.renderers_path +
                                                            "mesh_renderer/mesh_fragment_shader.glsl");
}

void mesh_renderer::on_shutdown(const event::internal::shutdown &event){
    auto view = state->scene.view<material_mesh>();
    for(const auto id : view){
        auto &material = view.get<material_mesh>(id);
        material.vao.destroy();
    }
    programs["mesh_renderer_program"].destroy();
}

void mesh_renderer::on_enqueue(const event::mesh_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;
    entities_to_draw.emplace_back(event.id);

    if(!state->scene.has<material_mesh>(event.id)){
        auto &material = state->scene.emplace<material_mesh>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto face_attributes = {attribute{"triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
        state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
    }
}

void mesh_renderer::on_setup_for_rendering(const event::mesh_renderer::setup_for_rendering &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    auto &material = state->scene.get_or_emplace<material_mesh>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "mesh";
        material.vao.create();
    }
    material.vao.bind();
    for(const auto &attribute: material.attributes){
        auto iter = shape.vertex_buffers.find(attribute.buffer_name);
        if(iter != shape.vertex_buffers.end()){
            iter->second.bind();
            if(attribute.enable){
                material.vao.capture_vertex_buffer(attribute.index, iter->second);
            }else{
                material.vao.disable_attribute(attribute.index);
            }
            iter->second.release();
        }
    }
    if(shape.element_buffer.is_valid()){
        shape.element_buffer.bind();
    }
    if(shape.adjacency_buffer.is_valid()){
        shape.adjacency_buffer.bind();
    }
    material.vao.release();
}

void mesh_renderer::on_begin_frame(const event::internal::begin_frame &) {
    state->scene.each([&](auto id) {
        state->dispatcher.trigger<event::mesh_renderer::enqueue>(id);
    });
}

void mesh_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);

    auto program = programs["mesh_renderer_program"];
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix.cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_mesh>(id);

        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        program.set_uniform_i("material.use_uniform_color", material.use_uniform_color);
        Vector<float, 3> ambient = material.ambient.cast<float>();
        program.set_uniform_3f("material.ambient", 1, ambient.data());
        Vector<float, 3> diffuse = material.diffuse.cast<float>();
        program.set_uniform_3f("material.diffuse", 1, diffuse.data());
        Vector<float, 3> specular = material.specular.cast<float>();
        program.set_uniform_3f("material.specular", 1, specular.data());
        float shininess = material.shininess;
        program.set_uniform_f("material.shininess", shininess);
        float alpha = material.uniform_alpha;
        program.set_uniform_f("material.alpha", alpha);

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();

        glDrawElements(GL_TRIANGLES, shape.element_buffer.num_elements, GL_UNSIGNED_INT, 0);
        assert_ogl_error();
    }

    entities_to_draw.clear();
}

void mesh_renderer::on_end_frame(const event::internal::end_frame &) {

}

void mesh_renderer::on_set_position_attribute(const event::mesh_renderer::set_position_attribute &event){
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_mesh>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if (vertices->get_base_ptr(event.position.property_name)->dims() != 3) return;
    auto &position = material.attributes[0];
    position.property_name = event.position.property_name;
    position.enable = true;
    position.update = true;
    std::vector<attribute> vertex_attributes = {position};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
}

void mesh_renderer::on_set_normal_attribute(const event::mesh_renderer::set_normal_attribute &event){
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_mesh>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if (vertices->get_base_ptr(event.normal.property_name)->dims() != 3) return;
    auto &position = material.attributes[1];
    position.property_name = event.normal.property_name;
    position.enable = true;
    position.update = true;
    std::vector<attribute> vertex_attributes = {position};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
}

void mesh_renderer::on_set_color_attribute(const event::mesh_renderer::set_color_attribute &event){
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_mesh>(event.id);
    //TODO convert to colormap if 1 dimensional.
    auto &color = material.attributes[2];
    color.property_name = event.color.property_name;
    color.enable = true;
    color.update = true;
    material.use_uniform_color = false;
    std::vector<attribute> vertex_attributes = {color};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
}

}