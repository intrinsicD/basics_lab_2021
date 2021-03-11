//
// Created by alex on 13.11.20.
//

#include <vector>
#include <string>

#include "bcg_curve_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_curve.h"
#include "renderers/bcg_attribute.h"
#include "bcg_events_curve_renderer.h"
#include "bcg_library/math/matrix/bcg_matrix_map_eigen.h"
#include "geometry/curve/bcg_curve_bezier.h"

namespace bcg {

curve_renderer::curve_renderer(viewer_state *state) : renderer("curve_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&curve_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&curve_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::curve_renderer::enqueue>().connect<&curve_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::curve_renderer::setup_for_rendering>().connect<&curve_renderer::on_setup_for_rendering>(
            this);
    state->dispatcher.sink<event::curve_renderer::set_position_attribute>().connect<&curve_renderer::on_set_position_attribute>(
            this);
}

void curve_renderer::on_startup(const event::internal::startup &) {
    std::string renderers_path = state->config.get_renderers_path_from_config_file();
    std::string tess_control = renderers_path + "curve_renderer/curve_tess_control_shader.glsl";
    std::string tess_eval = renderers_path + "curve_renderer/curve_tess_eval_shader.glsl";

    programs["curve_renderer_program"] = state->shaders.load("curve_renderer_program",
                                                             renderers_path +
                                                             "curve_renderer/curve_vertex_shader.glsl",
                                                             renderers_path +
                                                             "curve_renderer/curve_fragment_shader.glsl",
                                                             nullptr,
                                                             &tess_control,
                                                             &tess_eval);
}

void curve_renderer::on_shutdown(const event::internal::shutdown &event) {
    auto view = state->scene.view<material_curve>();
    for (const auto id : view) {
        auto &material = view.get<material_curve>(id);
        material.vao.destroy();
    }
    programs["curve_renderer_program"].destroy();
}

void curve_renderer::on_enqueue(const event::curve_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<curve_bezier>(event.id)) return;
    entities_to_draw.emplace_back(event.id);

    if (!state->scene.has<material_curve>(event.id)) {
        auto &material = state->scene.emplace<material_curve>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
        state->dispatcher.trigger<event::curve_renderer::setup_for_rendering>(event.id);
    } else {
        auto &material = state->scene.get<material_curve>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
        for (auto &attribute : material.attributes) {
            attribute.update = false;
        }
    }
}

void curve_renderer::on_setup_for_rendering(const event::curve_renderer::setup_for_rendering &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    auto &material = state->scene.get_or_emplace<material_curve>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "curve";
        material.vao.create();
    }
    material.vao.bind();
    for (const auto &attribute: material.attributes) {
        auto iter = shape.vertex_buffers.find(attribute.buffer_name);
        if (iter != shape.vertex_buffers.end()) {
            iter->second.bind();
            if (attribute.enable) {
                material.vao.capture_vertex_buffer(attribute.index, iter->second);
            } else {
                material.vao.disable_attribute(attribute.index);
            }
            iter->second.release();
        }
    }
    if (shape.edge_buffer.is_valid()) {
        shape.edge_buffer.bind();
    }
    material.vao.release();
}

void curve_renderer::on_begin_frame(const event::internal::begin_frame &) {
    state->scene.each([&](auto id) {
        if (state->scene.has<event::curve_renderer::enqueue>(id)) {
            state->dispatcher.trigger<event::curve_renderer::enqueue>(id);
        }
    });
}

void curve_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);
    gl_state.set_line_smooth(true);
    gl_state.set_blend(true);
    gl_state.set_blend_func_factors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto program = programs["curve_renderer_program"];
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_curve>(id);
        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        material.upload(program);
/*
        Vector<float, 3> uniform_color = material.uniform_color.cast<float>();
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        float alpha = material.uniform_alpha;
        program.set_uniform_f("material.alpha", alpha);
        program.set_uniform_i("tesselation_level", material.tesselation_level);*/

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();
        for (size_t offset = 0; offset < shape.num_vertices - 3; offset += 3) {

            if (material.show_hermite) {
/*                program.set_uniform_i("show_hermite", material.show_hermite);
                program.set_uniform_i("show_bezier", !material.show_hermite);*/
                material.use_hermite(program);
                glPatchParameteri(GL_PATCH_VERTICES, 4);
                glDrawArrays(GL_PATCHES, offset, 4);
            }
            if (material.show_bezier) {
/*                program.set_uniform_i("show_bezier", material.show_bezier);
                program.set_uniform_i("show_hermite", !material.show_bezier);*/
                material.use_bezier(program);
                glPatchParameteri(GL_PATCH_VERTICES, 4);
                glDrawArrays(GL_PATCHES, offset, 4);
            }
        }

        assert_ogl_error();
    }

    gl_state.set_line_smooth(false);
    gl_state.set_blend(false);
    entities_to_draw.clear();
}

void curve_renderer::on_end_frame(const event::internal::end_frame &) {

}

void curve_renderer::on_set_position_attribute(const event::curve_renderer::set_position_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_curve>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if (vertices->get_base_ptr(event.position.property_name)->dims() != 3) return;
    auto &position = material.attributes[0];
    position.property_name = event.position.property_name;
    position.buffer_name = position.property_name;
    position.enable = true;
    position.update = true;
    std::vector<attribute> vertex_attributes = {position};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
    state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::curve_renderer::setup_for_rendering>(event.id);
}

}
