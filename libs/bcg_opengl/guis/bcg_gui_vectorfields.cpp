//
// Created by alex on 16.11.20.
//

#include "bcg_gui_vectorfields.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_material_vectorfield.h"
#include "renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h"

namespace bcg {

void gui_vectorfields(viewer_state *state, vectorfields *vectors, entt::entity id) {
    if (!vectors) return;
    if (gui_property_selector(state, state->get_vertices(id), {3}, "vertex vectorfield",
                              vectors->current_vertex_vectorfield_name)) {
        auto &material = vectors->vertex_vectorfields[vectors->current_vertex_vectorfield_name];
        if (!vectors->current_vertex_vectorfield_name.empty()) {
            auto &position = material.attributes[0];
            position.buffer_name = "v_position";
            position.property_name = "v_position";
            auto &vector = material.attributes[1];
            vector.property_name = vectors->current_vertex_vectorfield_name;
            vector.buffer_name = vectors->current_vertex_vectorfield_name;
            auto *base_ptr = state->get_vertices(id)->get_base_ptr(position.property_name);
            material.num_vectors = base_ptr->size();
            material.enabled = true;
            state->dispatcher.trigger<event::vectorfield_renderer::set_position_attribute>(id,
                                                                                           vectors->current_vertex_vectorfield_name,
                                                                                           position);
            state->dispatcher.trigger<event::vectorfield_renderer::set_vector_attribute>(id,
                                                                                         vectors->current_vertex_vectorfield_name,
                                                                                         vector);
        } else {
            material.enabled = false;
        }
    }

    if (!vectors->current_vertex_vectorfield_name.empty()) {
        gui_material_vectorfield(state, &vectors->vertex_vectorfields[vectors->current_vertex_vectorfield_name],
                                 state->get_vertices(id), id,
                                 vectors->current_vertex_vectorfield_name);
        ImGui::Separator();
    }
    if (gui_property_selector(state, state->get_edges(id), {3}, "edge vectorfield",
                              vectors->current_edge_vectorfield_name)) {

        auto &material = vectors->edge_vectorfields[vectors->current_edge_vectorfield_name];
        if (!vectors->current_edge_vectorfield_name.empty()) {
            auto &position = material.attributes[0];
            position.property_name = "e_position";
            position.buffer_name = "e_position";
            auto &vector = material.attributes[1];
            vector.property_name = vectors->current_edge_vectorfield_name;
            vector.buffer_name = vectors->current_edge_vectorfield_name;
            auto *base_ptr = state->get_edges(id)->get_base_ptr(position.property_name);
            material.num_vectors = base_ptr->size();
            material.enabled = true;
            state->dispatcher.trigger<event::vectorfield_renderer::set_position_attribute>(id,
                                                                                           vectors->current_edge_vectorfield_name,
                                                                                           position);
            state->dispatcher.trigger<event::vectorfield_renderer::set_vector_attribute>(id,
                                                                                         vectors->current_edge_vectorfield_name,
                                                                                         vector);
        } else {
            material.enabled = false;
        }
    }
    if (!vectors->current_edge_vectorfield_name.empty()) {
        gui_material_vectorfield(state, &vectors->edge_vectorfields[vectors->current_edge_vectorfield_name],
                                 state->get_edges(id), id,
                                 vectors->current_edge_vectorfield_name);
        ImGui::Separator();
    }
    if (gui_property_selector(state, state->get_faces(id), {3}, "face vectorfield",
                              vectors->current_face_vectorfield_name)) {
        auto &material = vectors->face_vectorfields[vectors->current_face_vectorfield_name];
        if (!vectors->current_face_vectorfield_name.empty()) {
            auto &position = material.attributes[0];
            position.buffer_name = "f_position";
            position.property_name = "f_position";
            auto &vector = material.attributes[1];
            vector.property_name = vectors->current_face_vectorfield_name;
            vector.buffer_name = vectors->current_face_vectorfield_name;
            auto *base_ptr = state->get_faces(id)->get_base_ptr(position.property_name);
            material.num_vectors = base_ptr->size();
            material.enabled = true;
            state->dispatcher.trigger<event::vectorfield_renderer::set_position_attribute>(id,
                                                                                           vectors->current_face_vectorfield_name,
                                                                                           position);
            state->dispatcher.trigger<event::vectorfield_renderer::set_vector_attribute>(id,
                                                                                         vectors->current_face_vectorfield_name,
                                                                                         vector);
        } else {
            material.enabled = false;
        }
    }
    if (!vectors->current_face_vectorfield_name.empty()) {
        gui_material_vectorfield(state, &vectors->face_vectorfields[vectors->current_face_vectorfield_name],
                                 state->get_faces(id), id,
                                 vectors->current_face_vectorfield_name);
        ImGui::Separator();
    }
}

}