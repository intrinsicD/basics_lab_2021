//
// Created by alex on 06.11.20.
//

#include <vector>
#include <string>
#include <bcg_library/geometry/bcg_property_map_eigen.h>

#include "bcg_picking_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_picking.h"
#include "bcg_events_picking_renderer.h"
#include "kdtree/kdtree.h"

namespace bcg {

picking_renderer::picking_renderer(viewer_state *state) : renderer("picking_renderer", state) {
    state->dispatcher.sink<event::picking_renderer::enqueue>().connect<&picking_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::picking_renderer::setup_for_rendering>().connect<&picking_renderer::on_setup_for_rendering>(this);
    state->dispatcher.sink<event::mouse::button>().connect<&picking_renderer::on_mouse_button>(this);
    state->dispatcher.sink<event::internal::startup>().connect<&picking_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&picking_renderer::on_shutdown>(this);
}

void picking_renderer::on_startup(const event::internal::startup &) {
    programs["picking_renderer_program"] = state->shaders.load("picking_renderer_program",
                                                               state->config.renderers_path +
                                                               "picking_renderer/picking_vertex_shader.glsl",
                                                               state->config.renderers_path +
                                                               "picking_renderer/picking_fragment_shader.glsl");
}

void picking_renderer::on_shutdown(const event::internal::shutdown &event) {
    programs["picking_renderer_program"].destroy();
}

void picking_renderer::on_enqueue(const event::picking_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    entities_to_draw.emplace_back(event.id);

    if (!state->scene.has<material_picking>(event.id)) {
        auto &material = state->scene.emplace<material_picking>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        state->dispatcher.trigger<event::picking_renderer::setup_for_rendering>(event.id);
    }
}

void picking_renderer::on_setup_for_rendering(const event::picking_renderer::setup_for_rendering &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }

    auto &material = state->scene.get_or_emplace<material_picking>(event.id, event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "picking";
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
    if(shape.triangle_buffer){
        shape.triangle_buffer.bind();
    }
    material.vao.release();
}

void picking_renderer::on_begin_frame(const event::internal::begin_frame &) {
    auto view = state->scene.view<event::picking_renderer::enqueue>();
    for (auto id : view) {
        state->dispatcher.trigger<event::picking_renderer::enqueue>(id);
    }
}

void picking_renderer::on_mouse_button(const event::mouse::button &event) {
    if (entities_to_draw.empty()) return;
    if (!state->mouse.left) return;

    const auto &vp = state->window.framebuffer_viewport;
    int x = static_cast<int>( state->mouse.cursor_position[0] * state->window.high_dpi_scaling);
    int y = static_cast<int>( vp[3] - state->mouse.cursor_position[1] * state->window.high_dpi_scaling);
    gl_state.set_scissor_test(true);
    gl_state.set_scissor_values(x, y, 1, 1);
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);

    auto program = programs["picking_renderer_program"];
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix.cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_picking>(id);

        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        Vector<float, 3> picking_color = material.picking_color.cast<float>();
        program.set_uniform_3f("material.picking_color", 1, picking_color.data());

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();

        if (shape.edge_buffer) {
            glDrawElements(GL_LINES, shape.edge_buffer.num_elements, GL_UNSIGNED_INT, 0);
        } else if(shape.triangle_buffer){
            glDrawElements(GL_TRIANGLES, shape.triangle_buffer.num_elements, GL_UNSIGNED_INT, 0);
        }else{
            glDrawArrays(GL_POINTS, 0, shape.num_vertices);
        }
        assert_ogl_error();
    }

    entities_to_draw.clear();

    float zf = 1.0;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zf);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned char data[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    gl_state.set_scissor_test(false);
    if (zf == 1.0f) {
        state->picker.valid = false;
        return;
    } else {
        state->picker.valid = true;
    }

    float xf = float(x - vp[0]) / ((float) vp[2]) * 2.0f - 1.0f;
    float yf = float(y - vp[1]) / ((float) vp[3]) * 2.0f - 1.0f;
    zf = zf * 2.0f - 1.0f;
    VectorS<4> p = (state->cam.projection_matrix * state->cam.view_matrix()).inverse() *
                   VectorS<4>(xf, yf, zf, 1.0);
    p /= p[3];
    state->picker.world_space_point = p.head<3>();

    auto id = entt::entity(data[0] + data[1] * 256 + data[2] * 256 * 256);
    if (!state->scene.valid(id) || !state->scene.has<Transform>(id)) {
        state->picker.valid = false;
        return;
    }
    state->picker.entity_id = id;
    auto &model = state->scene.get<Transform>(id);
    state->picker.model_space_point = model.inverse() * state->picker.world_space_point;
    state->picker.view_space_point = (state->cam.view_matrix() *
                                      state->picker.world_space_point.homogeneous()).head<3>();

    if (!state->scene.has<kdtree_property<bcg_scalar_t>>(id)) {
        auto *vertices = state->get_vertices(id);
        auto positions = vertices->get<VectorS<3>, 3>("v_position");
        auto &kd_tree = state->scene.get_or_emplace<kdtree_property<bcg_scalar_t>>(id);
        kd_tree.build(positions);
    }

    auto &kd_tree = state->scene.get<kdtree_property<bcg_scalar_t>>(id);
    auto result = kd_tree.query_knn(state->picker.model_space_point, 1);
    state->picker.vertex_id = result.indices[0];

    if (state->scene.has<halfedge_graph>(id)) {
        auto &graph = state->scene.get<halfedge_graph>(id);
        state->picker.edge_id = graph.find_closest_edge_in_neighborhood(state->picker.vertex_id,
                                                                        state->picker.model_space_point);
    } else {
        state->picker.edge_id = BCG_INVALID_ID;
    }

    if (state->scene.has<halfedge_mesh>(id)) {
        auto &mesh = state->scene.get<halfedge_mesh>(id);
        state->picker.edge_id = mesh.find_closest_edge_in_neighborhood(state->picker.vertex_id,
                                                                       state->picker.model_space_point);
        state->picker.face_id = mesh.find_closest_face_in_neighborhood(state->picker.vertex_id,
                                                                       state->picker.model_space_point);
    } else {
        state->picker.edge_id = BCG_INVALID_ID;
        state->picker.face_id = BCG_INVALID_ID;
    }
}

void picking_renderer::on_end_frame(const event::internal::end_frame &) {

}

}