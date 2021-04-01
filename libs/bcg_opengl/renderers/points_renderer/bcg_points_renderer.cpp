//
// Created by alex on 29.10.20.
//

#include <vector>
#include <string>

#include "bcg_points_renderer.h"
#include "viewer/bcg_viewer_state.h"
#include "viewer/bcg_opengl.h"
#include "bcg_material_points.h"
#include "renderers/bcg_attribute.h"
#include "bcg_events_points_renderer.h"

namespace bcg {

points_renderer::points_renderer(viewer_state *state) : renderer("points_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&points_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&points_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::internal::uniform_point_size>().connect<&points_renderer::on_uniform_point_size>(
            this);
    state->dispatcher.sink<event::internal::set_uniform_point_size>().connect<&points_renderer::on_set_uniform_point_size>(
            this);
    state->dispatcher.sink<event::points_renderer::enqueue>().connect<&points_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::points_renderer::setup_for_rendering>().connect<&points_renderer::on_setup_for_rendering>(this);
    state->dispatcher.sink<event::points_renderer::set_position_attribute>().connect<&points_renderer::on_set_position_attribute>(
            this);
    state->dispatcher.sink<event::points_renderer::set_color_attribute>().connect<&points_renderer::on_set_color_attribute>(
            this);
    state->dispatcher.sink<event::points_renderer::set_point_size_attribute>().connect<&points_renderer::on_set_point_size_attribute>(
            this);
    state->dispatcher.sink<event::points_renderer::set_normal_attribute>().connect<&points_renderer::on_set_normal_attribute>(
            this);
}

void points_renderer::on_startup(const event::internal::startup &) {
    std::string renderers_path = state->config.get_renderers_path_from_config_file();
    programs["point_renderer_program"] = state->shaders.load("point_renderer_program",
                                                             renderers_path +
                                                             "points_renderer/point_vertex_shader.glsl",
                                                             renderers_path +
                                                             "points_renderer/point_fragment_shader.glsl");
}

void points_renderer::on_shutdown(const event::internal::shutdown &) {
    auto view = state->scene.view<material_points>();
    for (const auto id : view) {
        auto &material = view.get<material_points>(id);
        material.vao.destroy();
    }
    programs["point_renderer_program"].destroy();
}

void points_renderer::on_enqueue(const event::points_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    entities_to_draw.emplace_back(event.id);
    if (!state->scene.all_of<material_points>(event.id)) {
        auto &material = state->scene.emplace<material_points>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(event.id);
    }else{
        auto &material = state->scene.get<material_points>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(event.id);
        for(auto &attribute : material.attributes){
            attribute.update = false;
        }
    }
}

void points_renderer::on_setup_for_rendering(const event::points_renderer::setup_for_rendering &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    auto &material = state->scene.get_or_emplace<material_points>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "points";
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
    material.vao.release();
}

void points_renderer::on_begin_frame(const event::internal::begin_frame &) {
    state->scene.each([&](auto id) {
        if (state->scene.all_of<event::points_renderer::enqueue>(id)) {
            state->dispatcher.trigger<event::points_renderer::enqueue>(id);
        }
    });
}

void points_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);
    gl_state.set_program_point_size(true);

    auto program = glsl_program(programs["point_renderer_program"], "point_renderer_program");
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;
        if (!state->scene.all_of<material_points>(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_points>(id);

        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        material.uniform_size = gl_state.point_size_value;
        material.fovy = state->cam.fovy_radians();
        material.viewport = state->get_viewport().cast<float>();
        material.upload(program);

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();
        glDrawArrays(GL_POINTS, 0, shape.num_vertices);
        assert_ogl_error();
    }
    entities_to_draw.clear();
}

void points_renderer::on_end_frame(const event::internal::end_frame &) {

}

void points_renderer::on_uniform_point_size(const event::internal::uniform_point_size &event) {
    gl_state.set_point_size(
            std::min<bcg_scalar_t>(std::max<bcg_scalar_t>(gl_state.point_size_value + event.value, 1.0), state->config.max_point_size));
}

void points_renderer::on_set_uniform_point_size(const event::internal::set_uniform_point_size &event) {
    gl_state.set_point_size(std::min<bcg_scalar_t>(std::max<bcg_scalar_t>(event.value, 1.0), state->config.max_point_size));
}

void points_renderer::on_set_position_attribute(const event::points_renderer::set_position_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_points>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if (vertices->get_base_ptr(event.position.property_name)->dims() != 3) return;
    auto &position = material.attributes[0];
    position.property_name = event.position.property_name;
    position.buffer_name = position.property_name;
    position.enable = true;
    position.update = true;
    std::vector<attribute> vertex_attributes = {position};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(event.id);
}

void points_renderer::on_set_color_attribute(const event::points_renderer::set_color_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_points>(event.id);
    auto &color = material.attributes[1];
    color.property_name = event.color.property_name;
    color.buffer_name = color.property_name;
    color.enable = true;
    color.update = true;
    material.use_uniform_color = false;
    std::vector<attribute> vertex_attributes = {color};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(event.id);
}

void points_renderer::on_set_point_size_attribute(const event::points_renderer::set_point_size_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_points>(event.id);
    auto *vertices = state->get_vertices(event.id);
    auto *ptr = vertices->get_base_ptr(event.point_size.property_name);
    if (!ptr) return;
    if (ptr->dims() != 1) return;
    auto &point_size = material.attributes[2];
    point_size.property_name = event.point_size.property_name;
    point_size.buffer_name = point_size.property_name;
    point_size.enable = true;
    point_size.update = true;
    material.use_uniform_size = false;
    std::vector<attribute> vertex_attributes = {point_size};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(event.id);
}

void points_renderer::on_set_normal_attribute(const event::points_renderer::set_normal_attribute &event){
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_points>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if (vertices->get_base_ptr(event.normal.property_name)->dims() != 3) return;
    auto &normal = material.attributes[3];
    normal.property_name = event.normal.property_name;
    normal.buffer_name = normal.property_name;
    normal.enable = true;
    normal.update = true;
    material.has_normals = true;
    std::vector<attribute> vertex_attributes = {normal};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::points_renderer::setup_for_rendering>(event.id);
}

}