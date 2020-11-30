//
// Created by alex on 30.11.20.
//

#include "bcg_gui_point_cloud_graph_builder.h"
#include "bcg_viewer_state.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "renderers/graph_renderer/bcg_material_graph.h"

namespace bcg{

struct viewer_state;

enum class GraphType{
    kdtree_knn = 0,
    kdtree_radius,
    __last__
};
static std::vector<std::string> graph_type_names(){
    std::vector<std::string> names(static_cast<int>(GraphType::__last__));
    names[static_cast<int>(GraphType::kdtree_knn)] = "kdtree_knn";
    names[static_cast<int>(GraphType::kdtree_radius)] = "kdtree_radius";
    return names;
}

void gui_point_cloud_graph_builder(viewer_state *state){
    static std::vector<std::string> names = graph_type_names();
    static int idx = 0;
    draw_combobox(&state->window, "graph type", idx, names);

    static int num_closest = 12;
    static float radius = 0.01;

    if(idx % 2 == 0){
        ImGui::InputInt("num closest", &num_closest);
    }else{
        ImGui::InputFloat("radius", &radius);
    }
    if(ImGui::Button("Build Graph")){
        switch (static_cast<GraphType>(idx)){
            case GraphType::kdtree_knn : {
                state->dispatcher.trigger<event::point_cloud::build::graph_knn>(state->picker.entity_id, num_closest);
                break;
            }
            case GraphType::kdtree_radius : {
                state->dispatcher.trigger<event::point_cloud::build::graph_radius>(state->picker.entity_id, radius);
                break;
            }
            default:{

            }
        }

        if(!state->scene.valid(state->picker.entity_id)) return;
        state->scene.emplace_or_replace<event::graph_renderer::enqueue>(state->picker.entity_id);
        auto &material = state->scene.get_or_emplace<material_graph>(state->picker.entity_id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(state->picker.entity_id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges",0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(state->picker.entity_id, edge_attributes);
        state->dispatcher.trigger<event::graph_renderer::setup_for_rendering>(state->picker.entity_id);
    }
}

}
