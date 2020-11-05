//
// Created by alex on 05.11.20.
//


#include <vector>
#include <string>

#include "bcg_mesh_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_mesh.h"
#include "bcg_events_mesh_renderer.h"

namespace bcg {

mesh_renderer::mesh_renderer(viewer_state *state) : renderer("mesh_renderer", state) {
    state->dispatcher.sink<event::mesh_renderer::enqueue>().connect<&mesh_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::internal::startup>().connect<&mesh_renderer::on_startup>(this);
}

void mesh_renderer::on_startup(const event::internal::startup &) {
    programs["mesh_renderer_program"] = state->shaders.load("mesh_renderer_program",
                                                            state->config.renderers_path +
                                                            "mesh_renderer/mesh_vertex_shader.glsl",
                                                            state->config.renderers_path +
                                                            "mesh_renderer/mesh_fragment_shader.glsl");
}

void mesh_renderer::on_enqueue(const event::mesh_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;
    entities_to_draw.emplace_back(event.id);
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    if (!state->scene.has<material_mesh>(event.id)) {
        state->scene.emplace<material_mesh>(event.id);
    }
    auto vertices = state->get_vertices(event.id);
    auto faces = state->get_faces(event.id);
    auto vertex_attributes = std::vector<std::string>{"position", "normal", "color"};
    if (vertices->any_dirty(vertex_attributes)) {
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    }
    auto face_attributes = std::vector<std::string>{"triangles"};
    if (!faces->has("triangles") || faces->any_dirty(face_attributes)) {
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
    }
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
        float alpha = material.alpha;
        program.set_uniform_f("material.alpha", alpha);

        auto &vao = state->scene.get<ogl_vertex_array>(id);
        vao.bind();

        glDrawElements(GL_TRIANGLES, vao.element_buffer.num_elements, GL_UNSIGNED_INT, 0);
        assert_ogl_error();
    }

    entities_to_draw.clear();
}

void mesh_renderer::on_end_frame(const event::internal::end_frame &) {

}

}