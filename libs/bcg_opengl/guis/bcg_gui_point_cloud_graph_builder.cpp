//
// Created by alex on 30.11.20.
//

#include "bcg_gui_point_cloud_graph_builder.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_kdtree_selector.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "renderers/graph_renderer/bcg_material_graph.h"

namespace bcg{

struct viewer_state;

void gui_point_cloud_graph_builder(viewer_state *state){
    kdtree_parameters params = gui_kd_tree_selector(state);

    if(ImGui::Button("Build Graph")){
        switch (params.type){
            case kdtree_parameters::Type::knn : {
                state->dispatcher.trigger<event::point_cloud::build::graph_knn>(state->picker.entity_id, params.num_closest);
                break;
            }
            case kdtree_parameters::Type::radius : {
                state->dispatcher.trigger<event::point_cloud::build::graph_radius>(state->picker.entity_id, params.radius);
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
