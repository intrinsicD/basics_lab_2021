//
// Created by alex on 05.11.20.
//

#include <vector>
#include <string>

#include "bcg_test_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_test.h"
#include "bcg_events_test_renderer.h"

namespace bcg {

test_renderer::test_renderer(viewer_state *state) : renderer("test_renderer", state) {
    state->dispatcher.sink<event::test_renderer::enqueue>().connect<&test_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::internal::startup>().connect<&test_renderer::on_startup>(this);
}

void test_renderer::on_startup(const event::internal::startup &event) {
    programs["test_renderer_program"] = state->shaders.load("test_renderer_program",
                                                             state->config.renderers_path +
                                                             "test_renderer/test_vertex_shader.glsl",
                                                             state->config.renderers_path +
                                                             "test_renderer/test_fragment_shader.glsl");
}

void test_renderer::on_enqueue(const event::test_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    entities_to_draw.emplace_back(event.id);
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    if (!state->scene.has<ogl_vertex_array>(event.id)) {
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id,
                                                                        std::vector<std::string>{"position"});
    }
}

void test_renderer::on_begin_frame(const event::internal::begin_frame &event) {
    state->scene.each([&](auto id) {
        state->dispatcher.trigger<event::test_renderer::enqueue>(id);
    });
}

void test_renderer::on_render(const event::internal::render &event) {
    //if(entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);

    auto program = programs["test_renderer_program"];
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix.cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;
        auto &vao = state->scene.get<ogl_vertex_array>(id);
        auto &model = state->scene.get<Transform>(id);
        auto &pos = vao.vertex_buffers["position"];

        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, pos.num_elements);
        assert_ogl_error();
    }

    entities_to_draw.clear();
}

void test_renderer::on_end_frame(const event::internal::end_frame &event) {

}

}