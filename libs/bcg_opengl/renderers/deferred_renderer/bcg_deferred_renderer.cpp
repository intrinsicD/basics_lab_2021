//
// Created by alex on 15.12.20.
//

#include "bcg_deferred_renderer.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_events_deferred_renderer.h"
#include "bcg_material_deferred.h"
#include "components/bcg_component_transform.h"

namespace bcg {

deferred_renderer::deferred_renderer(viewer_state *state) : renderer("deferred_renderer", state) {
    state->dispatcher.sink<event::deferred_renderer::enqueue>().connect<&deferred_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::deferred_renderer::setup_for_rendering>().connect<&deferred_renderer::on_setup_for_rendering>(
            this);
    state->dispatcher.sink<event::internal::startup>().connect<&deferred_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&deferred_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::internal::begin_frame>().connect<&deferred_renderer::on_begin_frame>(this);
    state->dispatcher.sink<event::internal::render>().connect<&deferred_renderer::on_render>(this);
    state->dispatcher.sink<event::internal::end_frame>().connect<&deferred_renderer::on_end_frame>(this);
    state->dispatcher.sink<event::internal::resize>().connect<&deferred_renderer::on_resize>(this);
}

void deferred_renderer::on_startup(const event::internal::startup &) {
    std::string renderers_path = state->config.get_renderers_path_from_config_file();
    programs["deferred_shading_program"] = state->shaders.load("deferred_shading_program",
                                                               renderers_path +
                                                               "deferred_renderer/deferred_shading_vertex_shader.glsl",
                                                               renderers_path +
                                                               "deferred_renderer/deferred_shading_fragment_shader.glsl");
    programs["g_buffer_program"] = state->shaders.load("g_buffer_program",
                                                       renderers_path +
                                                       "deferred_renderer/g_buffer_vertex_shader.glsl",
                                                       renderers_path +
                                                       "deferred_renderer/g_buffer_fragment_shader.glsl");

    auto &shaderLightingPass = programs["deferred_shading_program"];
    shaderLightingPass.bind();
    shaderLightingPass.set_uniform_i("gPosition", 0);
    shaderLightingPass.set_uniform_i("gNormal", 1);
    shaderLightingPass.set_uniform_i("gAlbedoSpec", 2);

    const unsigned int NR_LIGHTS = 32;
    srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; i++) {
        // calculate slightly random offsets
        float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
        float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
        lightPositions.emplace_back(xPos, yPos, zPos);
        // also calculate random color
        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        lightColors.emplace_back(rColor, gColor, bColor);
    }

    int SCR_WIDTH = state->window.framebuffer_viewport[2];
    int SCR_HEIGHT = state->window.framebuffer_viewport[3];

    gBuffer.create();
    gBuffer.bind();

    auto gPosition = ogl_texture(GL_TEXTURE_2D, "gPosition");
    gPosition.create();
    gPosition.bind();
    gPosition.set_filter_nearest();
    gPosition.set_image_data(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_FLOAT, NULL);
    gBuffer.attach_texture(gPosition, GL_COLOR_ATTACHMENT0);

    auto gNormal = ogl_texture(GL_TEXTURE_2D, "gNormal");
    gNormal.create();
    gNormal.bind();
    gNormal.set_filter_nearest();
    gNormal.set_image_data(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_FLOAT, NULL);
    gBuffer.attach_texture(gNormal, GL_COLOR_ATTACHMENT1);

    auto gAlbedoSpec = ogl_texture(GL_TEXTURE_2D, "gAlbedoSpec");
    gAlbedoSpec.create();
    gAlbedoSpec.bind();
    gAlbedoSpec.set_filter_nearest();
    gAlbedoSpec.set_image_data(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_FLOAT, NULL);
    gBuffer.attach_texture(gAlbedoSpec, GL_COLOR_ATTACHMENT2);

    gBuffer.oepngl_draw_buffers();

    ogl_renderbuffer rbo;
    rbo.create();
    rbo.bind();
    rbo.storage(GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    gBuffer.attach_renderbuffer(rbo, GL_DEPTH_ATTACHMENT);
    gBuffer.check();
    gBuffer.release();
}

void deferred_renderer::on_shutdown(const event::internal::shutdown &) {
    auto view = state->scene().view<material_deferred>();
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
        auto &material = state->scene().emplace<material_deferred>(event.id);
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
    state->dispatcher.trigger<event::transform::world_space::init>(event.id);
    auto &material = state->scene().get_or_emplace<material_deferred>(event.id);
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
    if (shape.triangle_buffer.is_valid()) {
        shape.triangle_buffer.bind();
    }
    material.vao.release();
}

void deferred_renderer::on_begin_frame(const event::internal::begin_frame &) {
    state->scene().each([&](auto id) {
        if (state->scene.has<event::deferred_renderer::enqueue>(id)) {
            state->dispatcher.trigger<event::deferred_renderer::enqueue>(id);
        }
    });
}

void deferred_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);
    gl_state.set_line_smooth(true);

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

        auto &material = state->scene.get<material_deferred>(id);
        Transform model = state->scene.get_entity_world_transform(id);
        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();

        //Render Here
        if (state->scene.has<halfedge_graph>(id)) {
            shape.edge_buffer.bind();
            glDrawElements(GL_LINES, shape.edge_buffer.num_elements, GL_UNSIGNED_INT, 0);
            assert_ogl_error();
            glDrawArrays(GL_POINTS, 0, shape.num_vertices);
        } else if (state->scene.has<halfedge_mesh>(id)) {
            shape.triangle_buffer.bind();
            glDrawElements(GL_TRIANGLES, shape.triangle_buffer.num_elements, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_POINTS, 0, shape.num_vertices);
        }

        assert_ogl_error();
    }
    gBuffer.release();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program = programs["deferred_shading_program"];
    program.bind();
    gBuffer.activate_textures();

    for (unsigned int i = 0; i < lightPositions.size(); i++) {
        program.set_uniform_3f(("lights[" + std::to_string(i) + "].Position").c_str(), 1, lightPositions[i].data());
        program.set_uniform_3f(("lights[" + std::to_string(i) + "].Color").c_str(), 1, lightColors[i].data());
        // update attenuation parameters and calculate radius
        const float linear = 0.7;
        const float quadratic = 1.8;
        program.set_uniform_f(("lights[" + std::to_string(i) + "].Linear").c_str(), linear);
        program.set_uniform_f(("lights[" + std::to_string(i) + "].Quadratic").c_str(), quadratic);
    }
    Vector<float, 3> pos = state->cam.position().cast<float>();
    program.set_uniform_3f("viewPos", 1, pos.data());

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

void deferred_renderer::on_end_frame(const event::internal::end_frame &) {

}

void deferred_renderer::on_resize(const event::internal::resize &) {
    if (!gBuffer.rbo.is_valid()) return;

    int SCR_WIDTH = state->window.framebuffer_viewport[2];
    int SCR_HEIGHT = state->window.framebuffer_viewport[3];
    gBuffer.resize(SCR_WIDTH, SCR_HEIGHT);
}
}