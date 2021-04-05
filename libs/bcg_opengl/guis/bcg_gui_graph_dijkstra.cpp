//
// Created by alex on 31.03.21.
//

#include "bcg_gui_graph_dijkstra.h"
#include "bcg_gui_property_selector.h"
#include "bcg_property_map_eigen.h"
#include "viewer/bcg_viewer_state.h"
#include "viewer/bcg_selection.h"
#include "graph/bcg_graph_dijkstra.h"
#include "mesh/bcg_mesh_split_path.h"
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
        static std::vector<Path> paths;
        static float shortest_path_length = 0;
        if (ImGui::Button("Shortest path between")) {
            if(state->scene.all_of<selected_vertices>(state->picker.entity_id)){
                auto &selection = state->scene.get<selected_vertices>(state->picker.entity_id);
                auto merged_path = vertices->get_or_add<bcg_scalar_t, 1>("v_merged_shortest_path");

                Map(merged_path).setZero();
                auto heuristic = vertices->get<bcg_scalar_t, 1>(current_selected_heuristic);
                auto guide_vectorfield = vertices->get<VectorS<3>, 3>(current_selected_guide_vectorfield);

                if(paths.empty()){
                    paths.emplace_back();
                }
                paths.back().vertices.clear();
                shortest_path_length = 0;
                for(size_t i = 0; i < selection.ordering.size() - 1; ++i){
                    Path result;
                    if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                        auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                        result = graph_shortest_path_between(mesh, selection.ordering[i], selection.ordering[i+1], heuristic, guide_vectorfield);
                    }else if(state->scene.all_of<halfedge_graph>(state->picker.entity_id)){
                        auto &graph = state->scene.get<halfedge_graph>(state->picker.entity_id);
                        result = graph_shortest_path_between(graph, selection.ordering[i], selection.ordering[i+1], heuristic, guide_vectorfield);
                    }
                    shortest_path_length += result.length;
                    for(const auto &v : result.vertices){
                        paths.back().vertices.push_back(v);
                    }
                    for(const auto &e : result.edges){
                        paths.back().edges.push_back(e);
                    }
                }
                auto *edges = state->get_edges(state->picker.entity_id);
                if(edges){
                    auto merged_path_edges = edges->get_or_add<bcg_scalar_t, 1>("e_merged_shortest_path");
                    merged_path_edges.reset(0);
                    for(const auto &path : paths){
                        for(const auto &e : path.edges){
                            merged_path_edges[e] = 1;
                        }
                    }
                    merged_path_edges.set_dirty();
                }
                for(const auto &path : paths){
                    for(const auto &v : path.vertices){
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
        ImGui::LabelText("shortest_path_length", "%f", shortest_path_length);
        bool show_heuristic = false;
        if(ImGui::Button("New Path")){
            paths.emplace_back();
            show_heuristic = true;
        }
        if(ImGui::Button("Clear Paths")){
            paths.clear();
            auto merged_path = vertices->get_or_add<bcg_scalar_t, 1>("v_merged_shortest_path");
            merged_path.reset(0);
            merged_path.set_dirty();
            show_heuristic = true;
        }

        if(show_heuristic){
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                auto &color = material.attributes[2];
                color.property_name = current_selected_heuristic;
                state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
            }else{
                auto &material = state->scene.get<material_points>(state->picker.entity_id);
                auto &color = material.attributes[1];
                color.property_name = current_selected_heuristic;
                state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
            }
        }
        ImGui::LabelText("num paths", "%zu", paths.size());
        if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
            if(ImGui::Button("Remove path")){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                auto merged_path = vertices->get_or_add<bcg_scalar_t, 1>("v_merged_shortest_path");
                mesh_remove_path(mesh, merged_path);
            }
            if(ImGui::Button("Split path")){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                auto merged_path = vertices->get_or_add<bcg_scalar_t, 1>("v_merged_shortest_path");
                auto components = mesh_split_path(mesh, merged_path);
                size_t count = 0;
                for(const auto &component : components){
                    auto id = state->scene.create();
                    state->scene.emplace<halfedge_mesh>(id, component);
                    state->dispatcher.trigger<event::mesh::setup>(id, "split_part " + std::to_string(count), false);
                    ++count;
                }
            }
        }
    }
}

}