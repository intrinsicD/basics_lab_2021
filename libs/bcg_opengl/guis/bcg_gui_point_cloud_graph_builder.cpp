//
// Created by alex on 30.11.20.
//

#include "bcg_gui_point_cloud_graph_builder.h"
#include "bcg_viewer_state.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "renderers/graph_renderer/bcg_material_graph.h"

namespace bcg{

struct viewer_state;

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
        switch (idx){
            case 0 : {
                state->dispatcher.trigger<event::point_cloud::build_graph>(state->picker.entity_id, static_cast<GraphType>(idx), num_closest);
                break;
            }
            case 1 : {
                state->dispatcher.trigger<event::point_cloud::build_graph>(state->picker.entity_id, static_cast<GraphType>(idx), 0, radius);
                break;
            }
            case 2 : {
                state->dispatcher.trigger<event::point_cloud::build_graph>(state->picker.entity_id, static_cast<GraphType>(idx), num_closest);
                break;
            }
            case 3 : {
                state->dispatcher.trigger<event::point_cloud::build_graph>(state->picker.entity_id, static_cast<GraphType>(idx), 0, radius);
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
