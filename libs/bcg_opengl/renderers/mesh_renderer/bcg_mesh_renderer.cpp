//
// Created by alex on 05.11.20.
//


#include <vector>
#include <string>

#include "bcg_mesh_renderer.h"
#include "viewer/bcg_viewer_state.h"
#include "viewer/bcg_opengl.h"
#include "bcg_material_mesh.h"
#include "renderers/bcg_attribute.h"
#include "bcg_events_mesh_renderer.h"
#include "utils/bcg_string_utils.h"
#include "bcg_library/math/matrix/bcg_matrix_map_eigen.h"
#include "components/bcg_component_transform_world_space.h"
#include "components/bcg_component_transform_object_space.h"

namespace bcg {

mesh_renderer::mesh_renderer(viewer_state *state) : renderer("mesh_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&mesh_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&mesh_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::mesh_renderer::enqueue>().connect<&mesh_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::mesh_renderer::setup_for_rendering>().connect<&mesh_renderer::on_setup_for_rendering>(
            this);
    state->dispatcher.sink<event::mesh_renderer::set_position_attribute>().connect<&mesh_renderer::on_set_position_attribute>(
            this);
    state->dispatcher.sink<event::mesh_renderer::set_normal_attribute>().connect<&mesh_renderer::on_set_normal_attribute>(
            this);
    state->dispatcher.sink<event::mesh_renderer::set_vertex_color_attribute>().connect<&mesh_renderer::on_set_vertex_color_attribute>(
            this);
    state->dispatcher.sink<event::mesh_renderer::set_face_color_attribute>().connect<&mesh_renderer::on_set_face_color_attribute>(
            this);
}

void mesh_renderer::on_startup(const event::internal::startup &) {
    std::string renderers_path = state->config.get_renderers_path_from_config_file();
    programs["mesh_renderer_program"] = state->shaders.load("mesh_renderer_program",
                                                            renderers_path +
                                                            "mesh_renderer/mesh_vertex_shader.glsl",
                                                            renderers_path +
                                                            "mesh_renderer/mesh_fragment_shader.glsl");
}

void mesh_renderer::on_shutdown(const event::internal::shutdown &) {
    auto view = state->scene().view<material_mesh>();
    for (const auto id : view) {
        auto &material = view.get<material_mesh>(id);
        material.vao.destroy();
    }
    programs["mesh_renderer_program"].destroy();
}

void mesh_renderer::on_enqueue(const event::mesh_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->get_faces(event.id)) return;
    entities_to_draw.emplace_back(event.id);

    if (!state->scene.has<material_mesh>(event.id)) {
        auto &material = state->scene().emplace<material_mesh>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
        state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
    } else {
        auto &material = state->scene.get<material_mesh>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        std::vector<attribute> face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
        if (material.use_face_color) {
            auto *faces = state->get_faces(event.id);
            if (faces && faces->has(material.attributes[2].property_name) &&
                faces->get_base_ptr(material.attributes[2].property_name)->is_dirty()) {
                state->dispatcher.trigger<event::mesh_renderer::set_face_color_attribute>(event.id,
                                                                                          material.attributes[2]);
                faces->get_base_ptr(material.attributes[2].property_name)->set_clean();
            }
        }
        for (auto &attribute : material.attributes) {
            attribute.update = false;
        }
    }
}

void mesh_renderer::on_setup_for_rendering(const event::mesh_renderer::setup_for_rendering &event) {
    if (!state->scene.valid(event.id)) return;
    state->dispatcher.trigger<event::transform::world_space::init>(event.id);
    auto &material = state->scene().get_or_emplace<material_mesh>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "mesh";
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
    if (shape.triangle_buffer.is_valid()) {
        shape.triangle_buffer.bind();
    }
    material.vao.release();
}

void mesh_renderer::on_begin_frame(const event::internal::begin_frame &) {
    state->scene().each([&](auto id) {
        if (state->scene.has<event::mesh_renderer::enqueue>(id)) {
            state->dispatcher.trigger<event::mesh_renderer::enqueue>(id);
        }
    });
}

void mesh_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);

    auto program = programs["mesh_renderer_program"];
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;

        auto &material = state->scene.get<material_mesh>(id);

        Transform model = state->scene.get_entity_world_transform(id);
        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        material.upload(program);

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();
        shape.triangle_buffer.bind();
        glDrawElements(GL_TRIANGLES, shape.triangle_buffer.num_elements, GL_UNSIGNED_INT, 0);
        assert_ogl_error();
    }

    entities_to_draw.clear();
}

void mesh_renderer::on_end_frame(const event::internal::end_frame &) {

}

void mesh_renderer::on_set_position_attribute(const event::mesh_renderer::set_position_attribute &event) {
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
    auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true},
                            attribute{"boundary", "boundary", "boundary", 0, true}};
    state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
    state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
}

void mesh_renderer::on_set_normal_attribute(const event::mesh_renderer::set_normal_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_mesh>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if (vertices->get_base_ptr(event.normal.property_name)->dims() != 3) return;
    auto &normal = material.attributes[1];
    normal.property_name = event.normal.property_name;
    normal.buffer_name = normal.property_name;
    normal.enable = true;
    normal.update = true;
    std::vector<attribute> vertex_attributes = {normal};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
}

void mesh_renderer::on_set_vertex_color_attribute(const event::mesh_renderer::set_vertex_color_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_mesh>(event.id);

    auto &color = material.attributes[2];
    color.property_name = event.color.property_name;
    if (event.color.buffer_name.empty() || contains(event.color.buffer_name, "_color")) {
        color.buffer_name = color.property_name + "_color";
    } else {
        color.buffer_name = color.property_name;
    }
    color.enable = true;
    color.update = true;
    material.use_uniform_color = false;
    material.use_face_color = false;
    std::vector<attribute> vertex_attributes = {color};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
}

void mesh_renderer::on_set_face_color_attribute(const event::mesh_renderer::set_face_color_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_mesh>(event.id);

    auto *faces = state->get_faces(event.id);
    std::vector<Vector<float, 3>> colors = map_to_colors(faces, event.color.property_name, material.color_map);

    material.attributes[2].property_name = event.color.property_name;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &material.width);
    material.width = std::min((int) colors.size(), material.width);
    int height = colors.size() % material.width;
    size_t new_size = material.width * (height + 1);
    if (colors.size() != new_size) {
        colors.resize(new_size);
    }

    if (material.face_colors()) {
        material.face_colors().update_data(colors[0].data(), material.width, height + 1);
    } else {
        auto &texture = material.face_colors();
        texture = ogl_texture(GL_TEXTURE_2D, "face_color");
        int unit = 0;
        if (!texture.is_valid()) {
            texture.create();
            texture.activate(unit);
        }
        texture.bind();
        texture.set_wrap_clamp_to_border();
        texture.set_filter_linear();
        auto *data = colors[0].data();
        int channels = 3;
        if (data) {
            auto internal_format = TextureFormat(ogl_types::GetType(data), channels);
            auto format = TextureDataFormat(ogl_types::GetType(data), channels);

            texture.set_image_data(GL_TEXTURE_2D, 0, internal_format, material.width, height + 1, format,
                                   ogl_types::GetGLType(data), data);

            auto program = programs["mesh_renderer_program"];
            program.bind();
            if (program.get_uniform_location(material.face_colors().name.c_str()) !=
                static_cast<int>(BCG_GL_INVALID_ID)) {
                program.set_uniform_i(material.face_colors().name.c_str(), unit);
            }
        }
        texture.release();

/*            std::vector<VectorS<3>> test_data(material.width * (height + 1));
            material.face_colors.download_data(test_data.data());
            std::cout << MapConst(test_data) << "\n";*/
    }

    material.use_face_color = true;
    material.use_uniform_color = false;
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
    auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true},
                            attribute{"boundary", "boundary", "boundary", 0, true}};
    state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
    state->dispatcher.trigger<event::mesh_renderer::setup_for_rendering>(event.id);
}

}