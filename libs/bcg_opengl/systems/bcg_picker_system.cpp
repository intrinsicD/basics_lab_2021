//
// Created by alex on 28.10.20.
//

#include "bcg_picker_system.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_selection.h"
#include "components/bcg_component_selection_point_overlay.h"
#include "components/bcg_component_selection_vertex_overlay.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "renderers/graph_renderer/bcg_material_graph.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {

using namespace entt::literals;

picker_system::picker_system(viewer_state *state) : system("picker_system", state) {
    state->dispatcher.sink<event::picker::enable::point>().connect<&picker_system::on_enable_point>(this);
    state->dispatcher.sink<event::picker::enable::vertex>().connect<&picker_system::on_enable_vertex>(this);
    state->dispatcher.sink<event::picker::enable::edge>().connect<&picker_system::on_enable_edge>(this);
    state->dispatcher.sink<event::picker::enable::face>().connect<&picker_system::on_enable_face>(this);
    state->dispatcher.sink<event::picker::disable>().connect<&picker_system::on_disable>(this);
    state->dispatcher.sink<event::picker::pick::point>().connect<&picker_system::on_pick_point>(this);
    state->dispatcher.sink<event::picker::pick::vertex>().connect<&picker_system::on_pick_vertex>(this);
    state->dispatcher.sink<event::picker::pick::edge>().connect<&picker_system::on_pick_edge>(this);
    state->dispatcher.sink<event::picker::pick::face>().connect<&picker_system::on_pick_face>(this);
}

void picker_system::on_enable_point(const event::picker::enable::point &) {
    state->picker.mode = viewer_picker::Mode::points;
}

void picker_system::on_enable_vertex(const event::picker::enable::vertex &) {
    state->picker.mode = viewer_picker::Mode::vertices;
    if(state->scene.valid(state->picker.entity_id) && state->scene.has<component_selection_vertex_overlay>(state->picker.entity_id)){
        auto &selection_vertex_overlay = state->scene.get<component_selection_vertex_overlay>(state->picker.entity_id);
        state->scene().emplace_or_replace<event::points_renderer::enqueue>(selection_vertex_overlay.overlay_id);
    }
}

void picker_system::on_enable_edge(const event::picker::enable::edge &) {
    state->picker.mode = viewer_picker::Mode::edges;
}

void picker_system::on_enable_face(const event::picker::enable::face &) {
    state->picker.mode = viewer_picker::Mode::faces;
}

void picker_system::on_disable(const event::picker::disable &) {
    state->picker.mode = viewer_picker::Mode::disabled;
}

void picker_system::on_pick_point(const event::picker::pick::point &event) {
    if (!state->scene.valid(event.id)) return;
    if (state->picker.mode != viewer_picker::Mode::points) return;
    if (!state->scene.has<component_selection_point_overlay>(event.id)) {
        entt::entity id = state->scene.create();
        auto &pc = state->scene().emplace<point_cloud>(id, point_cloud());
        state->scene().emplace<entt::tag<"PointSelectionOverlay"_hs>>(id);
        state->scene().emplace<material_points>(id);
        state->scene().emplace<event::points_renderer::enqueue>(id);
        state->scene().emplace<component_selection_point_overlay>(event.id, event.id, id, &pc);
        state->dispatcher.trigger<event::hierarchy::set_parent_child>(event.id, id);
    }
    auto &selection = state->scene.get<component_selection_point_overlay>(event.id);

    if(state->keyboard.ctrl_pressed){
        if(state->picker.mode == viewer_picker::Mode::points){
            state->scene().emplace_or_replace<event::points_renderer::enqueue>(selection.overlay_id);
        }
        if(!state->keyboard.shift_pressed){
            selection.clear();
        }
        selection.select(state->picker.model_space_point);
    }
}
/*
void picker_system::on_pick_vertex(const event::picker::pick::vertex &event) {
    if (!state->scene.valid(event.id)) return;
    if (state->picker.mode != viewer_picker::Mode::vertices) return;
    if (!state->scene.has<selected_vertices>(event.id)) {
        state->scene().emplace<selected_vertices>(event.id);
    }
    auto &selection = state->scene.get<selected_vertices>(event.id);
    auto *vertices = state->get_vertices(event.id);
    if(!vertices) return;
    if(!state->picker.vertex_id.is_valid()) return;
    if(state->keyboard.ctrl_pressed){
        auto v_selected = vertices->get_or_add<bool, 1>("v_selected");
        if(!state->keyboard.shift_pressed){
            selection.selected.clear();
            selection.ordering.clear();
            v_selected.reset(false);
        }
        auto iter = selection.selected.find(state->picker.vertex_id);
        if(iter != selection.selected.end()){
            selection.ordering.erase(selection.ordering.begin() + iter->second.index_in_numbering);
            selection.selected.erase(iter);
            v_selected[state->picker.vertex_id] = false;
        }else{
            selection.ordering.push_back(state->picker.vertex_id.idx);
            selection.selected[state->picker.vertex_id.idx] = {state->picker.vertex_id, selection.selected.size()};
            v_selected[state->picker.vertex_id] = true;
        }
        v_selected.set_dirty();
        auto &material = state->scene().get_or_emplace<material_points>(event.id);
        auto &color = material.attributes[1];
        color.property_name = "v_selected";
        state->scene().emplace_or_replace<event::points_renderer::enqueue>(event.id);
        state->dispatcher.trigger<event::points_renderer::set_color_attribute>(event.id, color);
    }

}*/

void picker_system::on_pick_vertex(const event::picker::pick::vertex &event) {
    if (!state->scene.valid(event.id)) return;
    if (state->picker.mode != viewer_picker::Mode::vertices) return;
    if(!state->scene.has<component_selection_vertex_overlay>(event.id)){
        entt::entity id = state->scene.create();
        auto &pc = state->scene().emplace<point_cloud>(id, point_cloud());
        state->scene().emplace<entt::tag<"VertexSelectionOverlay"_hs>>(id);
        state->scene().emplace<material_points>(id);
        state->scene().emplace<event::points_renderer::enqueue>(id);
        state->scene().emplace<component_selection_vertex_overlay>(event.id, event.id, id, state->get_vertices(event.id), &pc);
        state->dispatcher.trigger<event::hierarchy::set_parent_child>(event.id, id);
    }

    auto &selection_vertex_overlay = state->scene.get<component_selection_vertex_overlay>(event.id);
    if(!state->picker.vertex_id.is_valid()) return;
    if(state->keyboard.ctrl_pressed){
        if(state->picker.mode == viewer_picker::Mode::vertices){
            state->scene().emplace_or_replace<event::points_renderer::enqueue>(selection_vertex_overlay.overlay_id);
        }
        if(!state->keyboard.shift_pressed){
            selection_vertex_overlay.clear();
        }
        if(!selection_vertex_overlay.select(state->picker.vertex_id)){
            selection_vertex_overlay.deselect(state->picker.vertex_id);
        }
    }
}


void picker_system::on_pick_edge(const event::picker::pick::edge &event) {
    if (!state->scene.valid(event.id)) return;
    if (state->picker.mode != viewer_picker::Mode::edges) return;
    if (!state->scene.has<selected_edges>(event.id)) {
        state->scene().emplace<selected_edges>(event.id);
    }
    auto &selection = state->scene.get<selected_edges>(event.id);
    auto *edges = state->get_edges(event.id);
    if(!edges) return;
    if(!state->picker.edge_id.is_valid()) return;
    auto e_selected = edges->get_or_add<bool, 1>("e_selected", false);
    if(!state->keyboard.shift_pressed){
        selection.selected.clear();
        e_selected.reset(false);
    }
    auto iter = selection.selected.find(state->picker.edge_id);
    if(iter != selection.selected.end()){
        selection.selected.erase(iter);
        e_selected[state->picker.edge_id] = false;
    }else{
        selection.selected[state->picker.edge_id.idx] = state->picker.edge_id;
        e_selected[state->picker.edge_id] = true;
    }
    e_selected.set_dirty();
    auto &material = state->scene().get_or_emplace<material_graph>(event.id);
    auto &color = material.attributes[1];
    color.property_name = "e_selected";
    state->dispatcher.trigger<event::graph_renderer::set_color_texture>(event.id, color);
}

void picker_system::on_pick_face(const event::picker::pick::face &event) {
    if (!state->scene.valid(event.id)) return;
    if (state->picker.mode != viewer_picker::Mode::faces) return;
    if (!state->scene.has<selected_faces>(event.id)) {
        state->scene().emplace<selected_faces>(event.id);
    }
    auto &selection = state->scene.get<selected_faces>(event.id);
    auto *faces = state->get_faces(event.id);
    if(!faces) return;
    if(!state->picker.face_id.is_valid()) return;
    auto f_selected = faces->get_or_add<bool, 1>("f_selected", false);
    if(!state->keyboard.shift_pressed){
        selection.selected.clear();
        f_selected.reset(false);
    }
    auto iter = selection.selected.find(state->picker.face_id);
    if(iter != selection.selected.end()){
        selection.selected.erase(iter);
        f_selected[state->picker.face_id] = false;
    }else{
        selection.selected[state->picker.face_id.idx] = state->picker.face_id;
        f_selected[state->picker.face_id] = true;
    }
    f_selected.set_dirty();
    auto &material = state->scene().get_or_emplace<material_mesh>(event.id);
    auto &color = material.attributes[2];
    color.property_name = "f_selected";
    state->scene().emplace_or_replace<event::graph_renderer::enqueue>(event.id);
    state->dispatcher.trigger<event::mesh_renderer::set_face_color_attribute>(event.id, color);
}

}