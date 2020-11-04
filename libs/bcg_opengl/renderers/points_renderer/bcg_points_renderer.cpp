//
// Created by alex on 29.10.20.
//

#include "bcg_points_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_points.h"
#include "bcg_points_renderer_events.h"

namespace bcg {

points_renderer::points_renderer(viewer_state *state) : renderer("points_renderer", state) {
    state->dispatcher.sink<event::points_renderer::enqueue>().connect<&points_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::internal::startup>().connect<&points_renderer::on_startup>(this);

}

void points_renderer::on_startup(const event::internal::startup &event) {
    programs["point_renderer_program"] = state->shaders.load("point_renderer_program",
                                                             state->config.renderers_path +
                                                             "points_renderer/point_vertex_shader.glsl",
                                                             state->config.renderers_path +
                                                             "points_renderer/point_fragment_shader.glsl");
}

void points_renderer::on_enqueue(const event::points_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    entities_to_draw.emplace_back(event.id);
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    if (!state->scene.has<material_points>(event.id)) {
        state->scene.emplace<material_points>(event.id);
    }
    if (!state->scene.has<ogl_vertex_array>(event.id)) {
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(
                {event.id, {"position", "color", "point_size"}});
    }
}

void points_renderer::on_begin_frame(const event::internal::begin_frame &event) {
    state->scene.each([&](auto id) {
        state->dispatcher.trigger<event::points_renderer::enqueue>(id);
    });
}

void points_renderer::on_render(const event::internal::render &event) {
    //if(entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);
    gl_state.set_program_point_size(true);

    auto program = glsl_program(programs["point_renderer_program"], "point_renderer_program");
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix.cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;
        if (!state->scene.has<material_points>(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_points>(id);
        auto &vao = state->scene.get<ogl_vertex_array>(id);
        vao.bind();

        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        program.set_uniform_i("use_uniform_point_size", material.use_uniform_point_size);
        program.set_uniform_f("uniform_point_size", material.uniform_point_size);

        program.set_uniform_i("use_uniform_color", material.use_uniform_color);
        Vector<float, 4> uniform_color = material.uniform_color.cast<float>();
        program.set_uniform_4f("uniform_color", 1, uniform_color.data());
        auto &pos = vao.vertex_buffers["position"];
        glDrawArrays(GL_POINTS, 0, pos.num_elements);
        assert_ogl_error();
    }

    entities_to_draw.clear();
}

void points_renderer::on_end_frame(const event::internal::end_frame &event) {

}

}