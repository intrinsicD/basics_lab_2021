//
// Created by alex on 29.04.21.
//

#include <vector>
#include <string>

#include "bcg_hq_pbr_ufsi_renderer.h"
#include "bcg_hq_pbr_ufsi_material.h"
#include "bcg_library/math/bcg_linalg.h"
#include "viewer/bcg_viewer_state.h"
#include "utils/bcg_path.h"
#include "components/bcg_component_transform.h"

namespace bcg {

hq_pbr_ufsi_renderer::hq_pbr_ufsi_renderer(viewer_state *state) : renderer("hq_pbr_ufsi_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&hq_pbr_ufsi_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&hq_pbr_ufsi_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::hq_pbr_ufsi_renderer::enqueue>().connect<&hq_pbr_ufsi_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::hq_pbr_ufsi_renderer::setup_material>().connect<&hq_pbr_ufsi_renderer::on_setup_material>(
            this);
    state->dispatcher.sink<event::internal::begin_frame>().connect<&hq_pbr_ufsi_renderer::on_begin_frame>(this);
    state->dispatcher.sink<event::internal::end_frame>().connect<&hq_pbr_ufsi_renderer::on_end_frame>(this);
    state->dispatcher.sink<event::internal::render>().connect<&hq_pbr_ufsi_renderer::on_render>(this);
    state->dispatcher.sink<event::hq_pbr_ufsi_renderer::set_position>().connect<&hq_pbr_ufsi_renderer::on_set_position>(this);
    state->dispatcher.sink<event::hq_pbr_ufsi_renderer::set_color>().connect<&hq_pbr_ufsi_renderer::on_set_color>(this);
    state->dispatcher.sink<event::hq_pbr_ufsi_renderer::set_normal>().connect<&hq_pbr_ufsi_renderer::on_set_normal>(this);
    state->dispatcher.sink<event::internal::uniform_point_size>().connect<&hq_pbr_ufsi_renderer::on_set_uniform_point_size>(
            this);
}

void hq_pbr_ufsi_renderer::on_startup(const event::internal::startup &) {
    std::string renderers_path = state->config.get_renderers_path_from_config_file();
    programs["hq_pbr_ufsi_renderer_program"] = state->shaders.load("hq_pbr_ufsi_renderer_program",
                                                                   renderers_path +
                                                                   "point_rendering/hq_pbr_ufsi/hq_pbr_ufsi_vertex_shader.glsl",
                                                                   renderers_path +
                                                                   "point_rendering/hq_pbr_ufsi/hq_pbr_ufsi_fragment_shader.glsl");

}

void hq_pbr_ufsi_renderer::on_shutdown(const event::internal::shutdown &) {
    auto view = state->scene().view<hq_pbr_ufsi_material>();
    for (const auto id : view) {
        auto &material = view.get<hq_pbr_ufsi_material>(id);
        material.vao.destroy();
    }
    programs["hq_pbr_ufsi_renderer_program"].destroy();
}

void hq_pbr_ufsi_renderer::on_enqueue(const event::hq_pbr_ufsi_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    if (state->get_vertices(event.id) == nullptr) return;
    entities_to_draw.emplace_back(event.id);
    if (!state->scene.has<hq_pbr_ufsi_material>(event.id)) {
        on_setup_material({event.id});
    }
}

void hq_pbr_ufsi_renderer::on_setup_material(const event::hq_pbr_ufsi_renderer::setup_material &event) {
    if (!state->scene.valid(event.id)) return;
    state->dispatcher.trigger<event::transform::world_space::init>(event.id);
    auto &material = state->scene().get_or_emplace<hq_pbr_ufsi_material>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "hq_pbr_ufsi";
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

void hq_pbr_ufsi_renderer::on_begin_frame(const event::internal::begin_frame &) {
    auto view = state->scene().view<event::hq_pbr_ufsi_renderer::enqueue>();
    for (const auto id : view) {
        state->dispatcher.trigger<event::hq_pbr_ufsi_renderer::enqueue>(id);
    }
}

void hq_pbr_ufsi_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);
    gl_state.set_program_point_size(true);

    auto program = glsl_program(programs["hq_pbr_ufsi_renderer_program"], "hq_pbr_ufsi_renderer_program");
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;
        if (!state->scene.has<hq_pbr_ufsi_material>(id)) continue;

        Transform model = state->scene.get_entity_world_transform(id);
        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        auto &material = state->scene.get<hq_pbr_ufsi_material>(id);
        material.field_of_view_radians = state->cam.fovy_radians();
        material.screen_height_in_pixels = state->get_viewport()[3];
        material.uniform_size = gl_state.point_size_value;
        material.upload(program);

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();
        glDrawArrays(GL_POINTS, 0, shape.num_vertices);
        assert_ogl_error();
    }
    entities_to_draw.clear();
}

void hq_pbr_ufsi_renderer::on_end_frame(const event::internal::end_frame &) {

}

void hq_pbr_ufsi_renderer::on_set_position(const event::hq_pbr_ufsi_renderer::set_position &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;
    if (!vertices->has(event.property_name)) return;
    if (vertices->get_base_ptr(event.property_name)->dims() != 3) return;
    auto &material = state->scene().get_or_emplace<hq_pbr_ufsi_material>(event.id);
    auto &position = material.attributes[0];
    position.buffer_name = event.property_name;
    position.property_name = event.property_name;
    position.enable = true;
    position.update = true;
    std::vector<attribute> vertex_attributes = {position};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    on_setup_material({event.id});
}

void hq_pbr_ufsi_renderer::on_set_color(const event::hq_pbr_ufsi_renderer::set_color &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;
    if (!vertices->has(event.property_name)) return;
    if (vertices->get_base_ptr(event.property_name)->dims() != 3) return;
    auto &material = state->scene().get_or_emplace<hq_pbr_ufsi_material>(event.id);
    auto &color = material.attributes[1];
    color.property_name = event.property_name;
    color.buffer_name = event.property_name + "_color";
    color.enable = true;
    color.update = true;
    std::vector<attribute> vertex_attributes = {color};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    on_setup_material({event.id});
}

void hq_pbr_ufsi_renderer::on_set_normal(const event::hq_pbr_ufsi_renderer::set_normal &event) {
    if (!state->scene.valid(event.id)) return;
    auto *vertices = state->get_vertices(event.id);
    if (!vertices) return;
    if (!vertices->has(event.property_name)) return;
    if (vertices->get_base_ptr(event.property_name)->dims() != 3) return;
    auto &material = state->scene().get_or_emplace<hq_pbr_ufsi_material>(event.id);
    auto &normal = material.attributes[2];
    normal.property_name = event.property_name;
    normal.buffer_name = event.property_name;
    normal.enable = true;
    normal.update = true;
    std::vector<attribute> vertex_attributes = {normal};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    on_setup_material({event.id});
}

void hq_pbr_ufsi_renderer::on_set_uniform_point_size(const event::internal::uniform_point_size &event) {
    gl_state.set_point_size(
            std::min<bcg_scalar_t>(std::max<bcg_scalar_t>(gl_state.point_size_value + event.value, 1.0),
                                   state->config.max_point_size));
}

}