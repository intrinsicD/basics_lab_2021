//
// Created by alex on 15.12.20.
//

#include "bcg_deferred_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_events_deferred_renderer.h"
#include "bcg_material_deferred.h"

namespace bcg {

deferred_renderer::deferred_renderer(viewer_state *state) : renderer("deferred_renderer", state){
    state->dispatcher.sink<event::deferred_renderer::enqueue>().connect<&deferred_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::deferred_renderer::setup_for_rendering>().connect<&deferred_renderer::on_setup_for_rendering>(
            this);
    state->dispatcher.sink<event::internal::startup>().connect<&deferred_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&deferred_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::internal::begin_frame>().connect<&deferred_renderer::on_begin_frame>(this);
    state->dispatcher.sink<event::internal::render>().connect<&deferred_renderer::on_render>(this);
    state->dispatcher.sink<event::internal::end_frame>().connect<&deferred_renderer::on_end_frame>(this);
}

void deferred_renderer::on_startup(const event::internal::startup &event) {
    programs["deferred_shading_program"] = state->shaders.load("deferred_shading_program",
                                                               state->config.renderers_path +
                                                               "deferred_renderer/deferred_shading_vertex_shader.glsl",
                                                               state->config.renderers_path +
                                                               "deferred_renderer/deferred_shading_fragment_shader.glsl");
    programs["g_buffer_program"] = state->shaders.load("g_buffer_program",
                                                       state->config.renderers_path +
                                                       "deferred_renderer/g_buffer_vertex_shader.glsl",
                                                       state->config.renderers_path +
                                                       "deferred_renderer/g_buffer_fragment_shader.glsl");

    auto &shaderLightingPass = programs["deferred_shading_program"];
    shaderLightingPass.bind();
    shaderLightingPass.set_uniform_i("gPosition", 0);
    shaderLightingPass.set_uniform_i("gNormal", 1);
    shaderLightingPass.set_uniform_i("gAlbedoSpec", 2);

    const unsigned int NR_LIGHTS = 32;

    int width = state->window.framebuffer_viewport[2];
    int height = state->window.framebuffer_viewport[3];

    ogl_texture gPosition(GL_TEXTURE_2D, "gPosition"), gNormal(GL_TEXTURE_2D, "gNormal"), gAlbedoSpec(GL_TEXTURE_2D, "gAlbedoSpec");
    gBuffer.create();
    gBuffer.bind();
    gPosition.create();
    gPosition.bind();
    gPosition.set_image_data(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, GL_RGBA, GL_FLOAT, NULL);
    gPosition.set_filter_nearest();
    gBuffer.attach_texture(gPosition, GL_COLOR_ATTACHMENT0);

    gNormal.create();
    gNormal.bind();
    gNormal.set_image_data(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, GL_RGBA, GL_FLOAT, NULL);
    gNormal.set_filter_nearest();
    gBuffer.attach_texture(gNormal, GL_COLOR_ATTACHMENT1);

    gAlbedoSpec.create();
    gAlbedoSpec.bind();
    gAlbedoSpec.set_image_data(GL_TEXTURE_2D, 0, GL_RGBA, width, height, GL_RGBA, GL_FLOAT, NULL);
    gAlbedoSpec.set_filter_nearest();
    gBuffer.attach_texture(gAlbedoSpec, GL_COLOR_ATTACHMENT2);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    rboDepth.create();
    rboDepth.bind();
    rboDepth.storage(GL_DEPTH_COMPONENT, width, height);
    gBuffer.attach_renderbuffer(rboDepth, GL_DEPTH_ATTACHMENT);
    gBuffer.check();
    gBuffer.release();
}

void deferred_renderer::on_shutdown(const event::internal::shutdown &event) {
    auto view = state->scene.view<material_deferred>();
    for (const auto id : view) {
        auto &material = view.get<material_deferred>(id);
        material.vao.destroy();
    }
    programs["deferred_shading_program"].destroy();
    programs["g_buffer_program"].destroy();
}

void deferred_renderer::on_enqueue(const event::deferred_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    entities_to_draw.emplace_back(event.id);

    if (!state->scene.has<material_deferred>(event.id)) {
        auto &material = state->scene.emplace<material_deferred>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
        auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
        state->dispatcher.trigger<event::deferred_renderer::setup_for_rendering>(event.id);
    } else {
        auto &material = state->scene.get<material_deferred>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
        auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, face_attributes);
        for (auto &attribute : material.attributes) {
            attribute.update = false;
        }
    }
}

void deferred_renderer::on_setup_for_rendering(const event::deferred_renderer::setup_for_rendering &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    auto &material = state->scene.get_or_emplace<material_deferred>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "deferred";
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

void deferred_renderer::on_begin_frame(const event::internal::begin_frame &event) {
    state->scene.each([&](auto id) {
        if (state->scene.has<event::deferred_renderer::enqueue>(id)) {
            state->dispatcher.trigger<event::deferred_renderer::enqueue>(id);
        }
    });
}

void deferred_renderer::on_render(const event::internal::render &event) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);
    gl_state.set_line_smooth(true);
    gl_state.set_blend(true);
    gl_state.set_blend_func_factors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto program = programs["g_buffer_program"];
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    gBuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_deferred>(id);
        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();

        //Render Here
        if (state->scene.has<halfedge_graph>(id)) {
            shape.edge_buffer.bind();
            glDrawElements(GL_LINES, shape.edge_buffer.num_elements, GL_UNSIGNED_INT, 0);
            glDrawArrays(GL_POINTS, 0, shape.num_vertices);
        } else if(state->scene.has<halfedge_mesh>(id)){
            shape.triangle_buffer.bind();
            glDrawElements(GL_TRIANGLES, shape.triangle_buffer.num_elements, GL_UNSIGNED_INT, 0);
        }else{
            glDrawArrays(GL_POINTS, 0, shape.num_vertices);
        }

        assert_ogl_error();
    }

    gBuffer.release();

    program = programs["deferred_shading_program"];
    glClearColor(1.0, 1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program.bind();
    gBuffer.activate_textures();

    program.set_uniform_3f("viewPos", 1, state->cam.position().data());

    if (!quad_vao.is_valid()) {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        quad_vao.create();
        quad_vbo.create();
        quad_vao.bind();
        quad_vbo.bind();
        quad_vbo.upload(quadVertices, 4, 5, 0, false);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    }
    quad_vao.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    quad_vao.release();

    gBuffer.copy_to_default_framebuffer();

    //render rest forward stuff;
    entities_to_draw.clear();
}

void deferred_renderer::on_end_frame(const event::internal::end_frame &event) {

}

}