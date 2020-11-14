//
// Created by alex on 10.11.20.
//

#include <vector>
#include <string>

#include "bcg_graph_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_graph.h"
#include "renderers/bcg_attribute.h"
#include "bcg_events_graph_renderer.h"
#include "bcg_property_map_eigen.h"
#include "math/bcg_matrix_map_eigen.h"

namespace bcg {

graph_renderer::graph_renderer(viewer_state *state) : renderer("graph_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&graph_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&graph_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::graph_renderer::enqueue>().connect<&graph_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::graph_renderer::setup_for_rendering>().connect<&graph_renderer::on_setup_for_rendering>(this);
    state->dispatcher.sink<event::graph_renderer::set_position_attribute>().connect<&graph_renderer::on_set_position_attribute>(this);
    state->dispatcher.sink<event::graph_renderer::set_color_attribute>().connect<&graph_renderer::on_set_color_attribute>(this);
}

void graph_renderer::on_startup(const event::internal::startup &) {
    programs["graph_renderer_program"] = state->shaders.load("graph_renderer_program",
                                                            state->config.renderers_path +
                                                            "graph_renderer/graph_vertex_shader.glsl",
                                                            state->config.renderers_path +
                                                            "graph_renderer/graph_fragment_shader.glsl");
}

void graph_renderer::on_shutdown(const event::internal::shutdown &event){
    auto view = state->scene.view<material_graph>();
    for(const auto id : view){
        auto &material = view.get<material_graph>(id);
        material.vao.destroy();
    }
    programs["graph_renderer_program"].destroy();
}

void graph_renderer::on_enqueue(const event::graph_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<halfedge_mesh>(event.id)) return;
    entities_to_draw.emplace_back(event.id);

    if(!state->scene.has<material_graph>(event.id)){
        auto &material = state->scene.emplace<material_graph>(event.id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges",0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
        state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(event.id);
    }
}

void graph_renderer::on_setup_for_rendering(const event::graph_renderer::setup_for_rendering &event){
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    auto &material = state->scene.get_or_emplace<material_graph>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    if (!material.vao) {
        material.vao.name = "mesh";
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
        if (state->scene.has<event::graph_renderer::enqueue>(id)) {
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
    Matrix<float, 4, 4> proj = state->cam.projection_matrix.cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &material = state->scene.get<material_graph>(id);

        Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
        program.set_uniform_matrix_4f("model", model_matrix.data());

        program.set_uniform_i("material.use_uniform_color", material.use_uniform_color);
        Vector<float, 3> uniform_color = material.uniform_color.cast<float>();
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        float alpha = material.uniform_alpha;
        program.set_uniform_f("material.alpha", alpha);
        program.set_uniform_i("width", material.width);

        if(material.edge_colors.is_valid()){
            material.edge_colors.activate();
        }
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
    position.enable = true;
    position.update = true;
    std::vector<attribute> vertex_attributes = {position};
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, vertex_attributes);
    state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(event.id);
}

void graph_renderer::on_set_color_attribute(const event::graph_renderer::set_color_attribute &event){
    if (!state->scene.valid(event.id)) return;
    auto &material = state->scene.get<material_graph>(event.id);

    material.color_map = colormap::jet();

    auto *edges = state->get_edges(event.id);
    if(!edges) return;

    auto *base_ptr = edges->get_base_ptr(event.color.property_name);
    std::vector<VectorS<3>> colors;
    if(base_ptr->dims() == 1){
        if(base_ptr->void_ptr() == nullptr){
            VectorS<-1> values = MapConst(edges->get<bool, 1>(base_ptr->name())).cast<float>();
            colors = material.color_map(values, values.minCoeff(), values.maxCoeff());
        }else{
            VectorS<-1> values = MapConst(edges->get<bcg_scalar_t , 1>(base_ptr->name()));
            colors = material.color_map(values, values.minCoeff(), values.maxCoeff());
        }
    }else if(base_ptr->dims() == 3){
        auto values = edges->get<VectorS<3>, 3>(base_ptr->name());
        auto min = MapConst(values).minCoeff();
        auto max = MapConst(values).maxCoeff();
        colors.resize(values.size());
        Map(colors) = (MapConst(values).array() - min) /  (max - min);
    }

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &material.width);
    material.width = std::min((int)colors.size(), material.width);
    int height = colors.size() % material.width;
    colors.resize(material.width * (height + 1));

    if(material.edge_colors){
        material.edge_colors.update_data(colors[0].data(),  material.width, height + 1);
    }else{
        auto &texture = material.edge_colors;
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
            if (program.get_uniform_location(material.edge_colors.name.c_str()) != static_cast<int>(BCG_GL_INVALID_ID)) {
                program.set_uniform_i(material.edge_colors.name.c_str(), unit);
            }
        }
        texture.release();

        std::vector<VectorS<3>> test_data(material.width * (height + 1));
        material.edge_colors.download_data(test_data.data());
        std::cout << MapConst(test_data) << "\n";
    }

    material.use_uniform_color = false;
    state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, material.attributes);
    auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true},
                            attribute{"boundary", "boundary", "boundary", 0, true}};
    state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, edge_attributes);
    state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(event.id);
}

}