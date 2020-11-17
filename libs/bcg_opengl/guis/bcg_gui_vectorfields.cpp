//
// Created by alex on 16.11.20.
//

#include "bcg_gui_vectorfields.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_material_vectorfield.h"
#include "renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h"

namespace bcg{

void gui_vectorfields(viewer_state *state, vectorfields *vectors, entt::entity id){
    if(!vectors) return;
    static std::string current_vertex_vectorfield_name = "";
    static std::string current_edge_vectorfield_name = "";
    static std::string current_face_vectorfield_name = "";
    if(gui_property_selector(state, state->get_vertices(id), {3}, "vertex vectorfield",
                                  current_vertex_vectorfield_name)){
        if(!current_vertex_vectorfield_name.empty()){
            auto &material = vectors->vertex_vectorfields[current_vertex_vectorfield_name];
            auto &position = material.attributes[0];
            position.buffer_name = "v_position";
            position.property_name = "v_position";
            auto &vector = material.attributes[1];
            vector.property_name = current_vertex_vectorfield_name;
            vector.buffer_name = current_vertex_vectorfield_name;
            state->dispatcher.trigger<event::vectorfield_renderer::set_position_attribute>(id, current_face_vectorfield_name, position);
            state->dispatcher.trigger<event::vectorfield_renderer::set_vector_attribute>(id, current_vertex_vectorfield_name, vector);
        }
    }

    if(!current_vertex_vectorfield_name.empty()){
        gui_material_vectorfield(state, &vectors->vertex_vectorfields[current_vertex_vectorfield_name], state->get_vertices(id), id,
                 current_vertex_vectorfield_name);
        ImGui::Separator();
    }
    if(gui_property_selector(state, state->get_edges(id), {3}, "edge vectorfield",
                                  current_edge_vectorfield_name)){

        if(!current_edge_vectorfield_name.empty()){
            auto &material = vectors->edge_vectorfields[current_edge_vectorfield_name];
            auto &position = material.attributes[0];
            position.property_name = "e_position";
            position.buffer_name = "e_position";
            auto &vector = material.attributes[1];
            vector.property_name = current_edge_vectorfield_name;
            vector.buffer_name = current_edge_vectorfield_name;
            state->dispatcher.trigger<event::vectorfield_renderer::set_position_attribute>(id, current_face_vectorfield_name, position);
            state->dispatcher.trigger<event::vectorfield_renderer::set_vector_attribute>(id, current_edge_vectorfield_name, vector);
        }
    }
    if(!current_edge_vectorfield_name.empty()){
        gui_material_vectorfield(state, &vectors->edge_vectorfields[current_edge_vectorfield_name], state->get_edges(id), id,
                 current_edge_vectorfield_name);
        ImGui::Separator();
    }
    if(gui_property_selector(state, state->get_faces(id), {3}, "face vectorfield",
                                  current_face_vectorfield_name)){
        if(!current_face_vectorfield_name.empty()){
            auto &material = vectors->face_vectorfields[current_face_vectorfield_name];
            auto &position = material.attributes[0];
            position.buffer_name = "f_position";
            position.property_name = "f_position";
            auto &vector = material.attributes[1];
            vector.property_name = current_face_vectorfield_name;
            vector.buffer_name = current_face_vectorfield_name;
            state->dispatcher.trigger<event::vectorfield_renderer::set_position_attribute>(id, current_face_vectorfield_name, position);
            state->dispatcher.trigger<event::vectorfield_renderer::set_vector_attribute>(id, current_face_vectorfield_name, vector);
        }
    }
    if(!current_face_vectorfield_name.empty()){
        gui_material_vectorfield(state, &vectors->face_vectorfields[current_face_vectorfield_name], state->get_faces(id), id,
                 current_face_vectorfield_name);
        ImGui::Separator();
    }
}

}