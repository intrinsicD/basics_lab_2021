//
// Created by alex on 10.11.20.
//

#include <vector>
#include <string>

#include "bcg_graph_renderer.h"
#include "viewer/bcg_viewer_state.h"
#include "viewer/bcg_opengl.h"
#include "bcg_material_graph.h"
#include "renderers/bcg_attribute.h"
#include "bcg_events_graph_renderer.h"
#include "bcg_property_map_eigen.h"
#include "bcg_library/math/matrix/bcg_matrix_map_eigen.h"

namespace bcg {

graph_renderer::graph_renderer(viewer_state *state) : renderer("graph_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&graph_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&graph_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::graph_renderer::enqueue>().connect<&graph_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::graph_renderer::setup_for_rendering>().connect<&graph_renderer::on_setup_for_rendering>(this);
    state->dispatcher.sink<event::graph_renderer::set_position_attribute>().connect<&graph_renderer::on_set_position_attribute>(this);
    state->dispatcher.sink<event::graph_renderer::set_color_texture>().connect<&graph_renderer::on_set_color_texture>(this);
}

void graph_renderer::on_startup(const event::internal::startup &) {
    std::string renderers_path = state->config.get_renderers_path_from_config_file();
    programs["graph_renderer_program"] = state->shaders.load("graph_renderer_program",
                                                            renderers_path +
                                                            "graph_renderer/graph_vertex_shader.glsl",
                                                            renderers_path +
                                                            "graph_renderer/graph_fragment_shader.glsl");
}

void graph_renderer::on_shutdown(const event::internal::shutdown &){
    auto view = state->scene.view<material_graph>();
    for(const auto id : view){
        auto &material = view.get<material_graph>(id);
        material.vao.destroy();
    }
    programs["graph_renderer_program"].destroy();
}

void graph_renderer::on_enqueue(const event::graph_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    if(!state->get_edges(event.id)) return;
    entities_to_draw.emplace_back(event.id);

    if(!state->scene.all_of<material_graph>(event.id)){
        auto &material = state->scene.emplace<material_graph>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges",0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
        state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(event.id);
    }else{
        auto &material = state->scene.get<material_graph>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges",0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
        for(auto &attribute : material.attributes){
            attribute.update = false;
        }
    }
}

void graph_renderer::on_setup_for_rendering(const event::graph_renderer::setup_for_rendering &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.all_of<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    auto &material = state->scene.get_or_emplace<material_graph>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "graph";
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
    if(shape.edge_buffer.is_valid()){
        shape.edge_buffer.bind();
    }
    material.vao.release();
}

void graph_renderer::on_begin_frame(const event::internal::begin_frame &) {
    state->scene.each([&](auto id) {
        if (state->scene.all_of<event::graph_renderer::enqueue>(id)) {
            state->dispatcher.trigger<event::graph_renderer::enqueue>(id);
        }
    });
}

void graph_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);

    auto program = programs["graph_renderer_program"];
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_graph>(id);

        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());
        material.upload(program);

        auto &shape = state->scene.get<ogl_shape>(id);
        material.vao.bind();
        shape.edge_buffer.bind();
        glDrawElements(GL_LINES, shape.edge_buffer.num_elements, GL_UNSIGNED_INT, 0);
        assert_ogl_error();
    }

    entities_to_draw.clear();
}

void graph_renderer::on_end_frame(const event::internal::end_frame &) {

}

void graph_renderer::on_set_position_attribute(const event::graph_renderer::set_position_attribute &event){
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_graph>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if (vertices->get_base_ptr(event.position.property_name)->dims() != 3) return;
    auto &position = material.attributes[0];
    position.property_name = event.position.property_name;
    position.buffer_name = position.property_name;
    position.enable = true;
    position.update = true;
    std::vector<attribute> vertex_attributes = {position};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(event.id);
}

void graph_renderer::on_set_color_texture(const event::graph_renderer::set_color_texture &event){
    if (!state->scene.valid(event.id)) return;
    if(!state->scene.all_of<event::graph_renderer::enqueue>(event.id)){
        state->scene.emplace_or_replace<event::graph_renderer::enqueue>(event.id);
        state->dispatcher.trigger<event::graph_renderer::enqueue>(event.id);
    }
    auto &material = state->scene.get<material_graph>(event.id);

    material.color_map = colormap::jet();

    auto *edges = state->get_edges(event.id);
    if(!edges) return;

    std::vector<Vector<float, 3>> colors = map_to_colors(edges, event.color.property_name, material.color_map);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &material.width);
    material.width = std::min((int)colors.size(), material.width);
    int height = colors.size() % material.width;
    colors.resize(material.width * (height + 1));

    if(material.edge_colors()){
        material.edge_colors().update_data(colors[0].data(),  material.width, height + 1);
    }else{
        auto &texture = material.edge_colors();
        texture = ogl_texture(GL_TEXTURE_2D, "edge_color");
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

            auto program = programs["graph_renderer_program"];
            program.bind();
            if (program.get_uniform_location(material.edge_colors().name.c_str()) != static_cast<int>(BCG_GL_INVALID_ID)) {
                program.set_uniform_i(material.edge_colors().name.c_str(), unit);
            }
        }
        texture.release();

        std::vector<VectorS<3>> test_data(material.width * (height + 1));
        material.edge_colors().download_data(test_data.data());
        std::cout << MapConst<3>(test_data).block<3, 3>(0, 0) << "\n";
    }

    material.use_uniform_color = false;
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
    auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true},
                            attribute{"boundary", "boundary", "boundary", 0, true}};
    state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
    state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(event.id);
}

}