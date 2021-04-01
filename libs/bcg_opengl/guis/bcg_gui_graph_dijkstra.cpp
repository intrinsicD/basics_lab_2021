//
// Created by alex on 31.03.21.
//

#include "bcg_gui_graph_dijkstra.h"
#include "bcg_gui_property_selector.h"
#include "bcg_property_map_eigen.h"
#include "viewer/bcg_viewer_state.h"
#include "viewer/bcg_selection.h"
#include "graph/bcg_graph_dijkstra.h"
#include "bcg_opengl/renderers/points_renderer/bcg_material_points.h"
#include "bcg_opengl/renderers/points_renderer/bcg_events_points_renderer.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_material_mesh.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

void gui_graph_dijkstra(viewer_state *state) {
    if (state->scene.valid(state->picker.entity_id)) {
        auto *vertices = state->get_vertices(state->picker.entity_id);
        static std::string current_selected_heuristic;
        static std::string current_selected_guide_vectorfield;
        if(vertices){
            gui_property_selector(state, vertices, {1}, "heuristic",current_selected_heuristic);
            gui_property_selector(state, vertices, {3}, "guide_vectorfield",current_selected_guide_vectorfield);
        }
        if (ImGui::Button("Shortest paths from")) {
            auto heuristic = vertices->get<bcg_scalar_t, 1>(current_selected_heuristic);
            auto guide_vectorfield = vertices->get<VectorS<3>, 3>(current_selected_guide_vectorfield);
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                graph_shortest_paths_from(mesh, state->picker.vertex_id, heuristic, guide_vectorfield);
                auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                auto &color = material.attributes[2];
                color.property_name = "v_dijkstra_distance";
                state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
            }else if(state->scene.all_of<halfedge_graph>(state->picker.entity_id)){
                auto &graph = state->scene.get<halfedge_graph>(state->picker.entity_id);
                graph_shortest_paths_from(graph, state->picker.vertex_id, heuristic, guide_vectorfield);
                auto &material = state->scene.get<material_points>(state->picker.entity_id);
                auto &color = material.attributes[1];
                color.property_name = "v_dijkstra_distance";
                state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
            }
        }
        if (ImGui::Button("Shortest path between")) {
            if(state->scene.all_of<selected_vertices>(state->picker.entity_id)){
                auto &selection = state->scene.get<selected_vertices>(state->picker.entity_id);
                auto merged_path = vertices->get_or_add<bcg_scalar_t, 1>("v_merged_shortest_path");
                Map(merged_path).setZero();
                auto heuristic = vertices->get<bcg_scalar_t, 1>(current_selected_heuristic);
                auto guide_vectorfield = vertices->get<VectorS<3>, 3>(current_selected_guide_vectorfield);

                for(size_t i = 0; i < selection.ordering.size() - 1; ++i){
                    Path result_path;
                    if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                        auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                        result_path = graph_shortest_path_between(mesh, selection.ordering[i], selection.ordering[i+1], heuristic, guide_vectorfield);
                    }else if(state->scene.all_of<halfedge_graph>(state->picker.entity_id)){
                        auto &graph = state->scene.get<halfedge_graph>(state->picker.entity_id);
                        result_path = graph_shortest_path_between(graph, selection.ordering[i], selection.ordering[i+1], heuristic, guide_vectorfield);
                    }
                    for(const auto &v : result_path.vertices){
                        merged_path[v] = 1;
                    }
                }
                merged_path.set_dirty();

                if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = "v_merged_shortest_path";
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                }else{
                    auto &material = state->scene.get<material_points>(state->picker.entity_id);
                    auto &color = material.attributes[1];
                    color.property_name = "v_merged_shortest_path";
                    state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
                }
            }
        }
    }
}

}