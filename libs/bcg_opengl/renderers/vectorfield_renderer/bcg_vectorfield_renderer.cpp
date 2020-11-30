//
// Created by alex on 11.11.20.
//


#include <vector>
#include <string>

#include "bcg_vectorfield_renderer.h"
#include "bcg_viewer_state.h"
#include "bcg_opengl.h"
#include "bcg_material_vectorfield.h"
#include "renderers/bcg_attribute.h"
#include "renderers/bcg_vectorfields.h"
#include "bcg_events_vectorfield_renderer.h"
#include "utils/bcg_string_utils.h"
#include "geometry/aligned_box/bcg_aligned_box.h"

namespace bcg {

vectorfield_renderer::vectorfield_renderer(viewer_state *state) : renderer("vectorfield_renderer", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&vectorfield_renderer::on_startup>(this);
    state->dispatcher.sink<event::internal::shutdown>().connect<&vectorfield_renderer::on_shutdown>(this);
    state->dispatcher.sink<event::vectorfield_renderer::enqueue>().connect<&vectorfield_renderer::on_enqueue>(this);
    state->dispatcher.sink<event::vectorfield_renderer::setup_for_rendering>().connect<&vectorfield_renderer::on_setup_for_rendering>(this);
    state->dispatcher.sink<event::vectorfield_renderer::set_position_attribute>().connect<&vectorfield_renderer::on_set_position_attribute>(
            this);
    state->dispatcher.sink<event::vectorfield_renderer::set_color_attribute>().connect<&vectorfield_renderer::on_set_color_attribute>(
            this);
    state->dispatcher.sink<event::vectorfield_renderer::set_vector_attribute>().connect<&vectorfield_renderer::on_set_vector_attribute>(
            this);
}

void vectorfield_renderer::on_startup(const event::internal::startup &) {
    std::string geometry_shader = state->config.renderers_path + "vectorfield_renderer/vectorfield_geometry_shader.glsl";
    programs["vectorfield_renderer_program"] = state->shaders.load("vectorfield_renderer_program",
                                                             state->config.renderers_path +
                                                             "vectorfield_renderer/vectorfield_vertex_shader.glsl",
                                                             state->config.renderers_path +
                                                             "vectorfield_renderer/vectorfield_fragment_shader.glsl",
                                                             &geometry_shader);
}

void vectorfield_renderer::on_shutdown(const event::internal::shutdown &event) {
    auto view = state->scene.view<vectorfields>();
    for (const auto id : view) {
        auto &vectors = view.get<vectorfields>(id);
        for(auto &item : vectors.vertex_vectorfields){
            item.second.vao.destroy();
        }
        for(auto &item : vectors.edge_vectorfields){
            item.second.vao.destroy();
        }
        for(auto &item : vectors.face_vectorfields){
            item.second.vao.destroy();
        }
    }
    programs["vectorfield_renderer_program"].destroy();
}

void vectorfield_renderer::on_enqueue(const event::vectorfield_renderer::enqueue &event) {
    if (!state->scene.valid(event.id)) return;
    entities_to_draw.emplace_back(event.id);
    if (!state->scene.has<vectorfields>(event.id)) {
        auto &vectors = state->scene.emplace<vectorfields>(event.id);
        for(auto &item : vectors.vertex_vectorfields){
            state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, item.second.attributes);
        }
        for(auto &item : vectors.edge_vectorfields){
            state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, item.second.attributes);
        }
        for(auto &item : vectors.face_vectorfields){
            state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, item.second.attributes);
        }
        state->dispatcher.trigger<event::vectorfield_renderer::setup_for_rendering>(event.id);
    }
}

void setup_material(material_vectorfield &material, ogl_shape &shape){
    if (!material.vao) {
        material.vao.name = "vectorfield";
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

void vectorfield_renderer::on_setup_for_rendering(const event::vectorfield_renderer::setup_for_rendering &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
    if (!state->scene.has<vectorfields>(event.id)) {
        state->scene.emplace<vectorfields>(event.id);
    }
    auto &vectors = state->scene.get_or_emplace<vectorfields>(event.id);
    auto &shape = state->scene.get<ogl_shape>(event.id);
    for(auto &item : vectors.vertex_vectorfields){
        setup_material(item.second, shape);
    }
    for(auto &item : vectors.edge_vectorfields){
        setup_material(item.second, shape);
    }
    for(auto &item : vectors.face_vectorfields){
        setup_material(item.second, shape);
    }
}

void vectorfield_renderer::on_begin_frame(const event::internal::begin_frame &) {
    state->scene.each([&](auto id) {
        if (state->scene.has<event::vectorfield_renderer::enqueue>(id)) {
            state->dispatcher.trigger<event::vectorfield_renderer::enqueue>(id);
        }
    });
}

void render_vectorfield(material_vectorfield &material, Transform &model, glsl_program &program){
    Matrix<float, 4, 4> model_matrix = model.matrix().cast<float>();
    program.set_uniform_matrix_4f("model", model_matrix.data());

    program.set_uniform_i("material.use_uniform_vector_length", material.use_uniform_size);
    program.set_uniform_f("material.uniform_vector_length", material.uniform_size);

    program.set_uniform_i("material.use_uniform_color", material.use_uniform_color);
    Vector<float, 3> uniform_color = material.uniform_color.cast<float>();
    program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
    float alpha = material.uniform_alpha;
    program.set_uniform_f("material.alpha", alpha);
    material.vao.bind();
    glDrawArrays(GL_POINTS, 0, material.num_vectors);
    assert_ogl_error();
}

void vectorfield_renderer::on_render(const event::internal::render &) {
    if (entities_to_draw.empty()) return;
    gl_state.set_depth_test(true);
    gl_state.set_depth_mask(true);
    gl_state.set_depth_func(GL_LESS);
    gl_state.set_program_point_size(true);

    auto program = glsl_program(programs["vectorfield_renderer_program"], "vectorfield_renderer_program");
    program.bind();
    Matrix<float, 4, 4> proj = state->cam.projection_matrix().cast<float>();
    program.set_uniform_matrix_4f("proj", proj.data());
    Matrix<float, 4, 4> view = state->cam.view_matrix().cast<float>();
    program.set_uniform_matrix_4f("view", view.data());

    for (const auto id : entities_to_draw) {
        if (!state->scene.valid(id)) continue;
        if (!state->scene.has<vectorfields>(id)) continue;

        auto &model = state->scene.get<Transform>(id);
        auto &vectors = state->scene.get<vectorfields>(id);
        for(auto &item : vectors.vertex_vectorfields){
            if(!item.second.vao.is_valid()) continue;
            if(!item.second.enabled) continue;
            render_vectorfield(item.second, model, program);
        }
        for(auto &item : vectors.edge_vectorfields){
            if(!item.second.vao.is_valid()) continue;
            if(!item.second.enabled) continue;
            render_vectorfield(item.second, model, program);
        }
        for(auto &item : vectors.face_vectorfields){
            if(!item.second.vao.is_valid()) continue;
            if(!item.second.enabled) continue;
            render_vectorfield(item.second, model, program);
        }
    }
    entities_to_draw.clear();
}

void vectorfield_renderer::on_end_frame(const event::internal::end_frame &) {

}

void vectorfield_renderer::on_set_position_attribute(const event::vectorfield_renderer::set_position_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &vectors = state->scene.get<vectorfields>(event.id);
    std::vector<attribute> attributes;
    if(event.position.buffer_name == "v_position"){
        auto *vertices = state->get_vertices(event.id);
        auto *base_ptr = vertices->get_base_ptr(event.position.property_name);
        if (base_ptr->dims() != 3) return;
        auto &material = vectors.vertex_vectorfields[event.vectorfield_name];
        material.num_vectors = base_ptr->size();
        auto &position = material.attributes[0];
        position.property_name = event.position.property_name;
        position.enable = true;
        position.update = true;
        attributes = {position};
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, attributes);
    }else if(event.position.buffer_name == "e_position"){
        auto *edges = state->get_edges(event.id);
        auto *base_ptr = edges->get_base_ptr(event.position.property_name);
        if (base_ptr->dims() != 3) return;
        auto &material = vectors.edge_vectorfields[event.vectorfield_name];
        material.num_vectors = base_ptr->size();
        auto &position = material.attributes[0];
        position.property_name = event.position.property_name;
        position.enable = true;
        position.update = true;
        attributes = {position};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, attributes);
    }else if(event.position.buffer_name  == "f_position"){
        auto *faces = state->get_faces(event.id);
        auto *base_ptr = faces->get_base_ptr(event.position.property_name);
        if (base_ptr->dims() != 3) return;
        auto &material = vectors.face_vectorfields[event.vectorfield_name];
        material.num_vectors = base_ptr->size();
        auto &position = material.attributes[0];
        position.property_name = event.position.property_name;
        position.enable = true;
        position.update = true;
        attributes = {position};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, attributes);
    }

    state->dispatcher.trigger<event::vectorfield_renderer::setup_for_rendering>(event.id);
}

void vectorfield_renderer::on_set_vector_attribute(const event::vectorfield_renderer::set_vector_attribute &event){
    if (!state->scene.valid(event.id)) return;
    auto &vectors = state->scene.get<vectorfields>(event.id);
    std::vector<attribute> attributes;
    if(contains(event.vector.buffer_name,"v_")){
        auto *vertices = state->get_vertices(event.id);
        auto *base_ptr = vertices->get_base_ptr(event.vector.property_name);
        if (base_ptr->dims() != 3) return;
        auto &material = vectors.vertex_vectorfields[event.vectorfield_name];
        auto &vector = material.attributes[1];
        vector.property_name = event.vector.property_name;
        vector.enable = true;
        vector.update = true;
        attributes = {vector};

        material.uniform_size = state->scene.get<aligned_box3>(event.id).diagonal().norm() / std::sqrt(base_ptr->size());
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, attributes);
    }else if(contains(event.vector.buffer_name,"e_")){
        auto *edges = state->get_edges(event.id);
        auto *base_ptr = edges->get_base_ptr(event.vector.property_name);
        if (base_ptr->dims() != 3) return;
        auto &material = vectors.edge_vectorfields[event.vectorfield_name];
        auto &vector = material.attributes[1];
        vector.property_name = event.vector.property_name;
        vector.enable = true;
        vector.update = true;
        attributes = {vector};
        material.uniform_size = state->scene.get<aligned_box3>(event.id).diagonal().norm() / std::sqrt(base_ptr->size());
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, attributes);
    }else if(contains(event.vector.buffer_name,"f_")){
        auto *faces = state->get_faces(event.id);
        auto *base_ptr = faces->get_base_ptr(event.vector.property_name);
        if (base_ptr->dims() != 3) return;
        auto &material = vectors.face_vectorfields[event.vectorfield_name];
        auto &vector = material.attributes[1];
        vector.property_name = event.vector.property_name;
        vector.enable = true;
        vector.update = true;
        attributes = {vector};
        material.uniform_size = state->scene.get<aligned_box3>(event.id).diagonal().norm() / std::sqrt(base_ptr->size());
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, attributes);
    }

    state->dispatcher.trigger<event::vectorfield_renderer::setup_for_rendering>(event.id);
}

void vectorfield_renderer::on_set_color_attribute(const event::vectorfield_renderer::set_color_attribute &event) {
    if (!state->scene.valid(event.id)) return;
    auto &vectors = state->scene.get<vectorfields>(event.id);
    std::vector<attribute> attributes;
    if(contains(event.color.buffer_name,"v_")){
        auto *vertices = state->get_vertices(event.id);
        if (vertices->get_base_ptr(event.color.property_name)->dims() != 3) return;
        auto &material = vectors.vertex_vectorfields[event.vectorfield_name];
        auto &color = material.attributes[2];
        color.property_name = event.color.property_name;
        color.enable = true;
        color.update = true;
        attributes = {color};
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(event.id, attributes);
    }else if(contains(event.color.buffer_name,"e_")){
        auto *edges = state->get_edges(event.id);
        if (edges->get_base_ptr(event.color.property_name)->dims() != 3) return;
        auto &material = vectors.edge_vectorfields[event.vectorfield_name];
        auto &color = material.attributes[2];
        color.property_name = event.color.property_name;
        color.enable = true;
        color.update = true;
        attributes = {color};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(event.id, attributes);
    }else if(contains(event.color.buffer_name,"f_")){
        auto *faces = state->get_faces(event.id);
        if (faces->get_base_ptr(event.color.property_name)->dims() != 3) return;
        auto &material = vectors.face_vectorfields[event.vectorfield_name];
        auto &color = material.attributes[2];
        color.property_name = event.color.property_name;
        color.enable = true;
        color.update = true;
        attributes = {color};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(event.id, attributes);
    }

    state->dispatcher.trigger<event::vectorfield_renderer::setup_for_rendering>(event.id);
}

}