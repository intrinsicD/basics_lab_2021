//
// Created by alex on 23.11.20.
//

#include "bcg_gui_mesh_laplacian.h"
#include "bcg_viewer_state.h"
#include "bcg_gui_property_selector.h"
#include "geometry/mesh/bcg_mesh_laplacian.h"

namespace bcg {

void gui_mesh_laplacian(viewer_state *state) {
    static std::vector<std::string> stiffness = mesh_laplacian_stiffness();
    static std::vector<std::string> mass = mesh_laplacian_mass();
    static int current_stiffness = 0;
    static int current_mass = 0;
    draw_combobox(&state->window, "stiffness", current_stiffness, stiffness);
    draw_combobox(&state->window, "mass", current_mass, mass);
    auto *edges = state->get_edges(state->picker.entity_id);
    static std::string current_property_name = "";
    if(edges){
        gui_property_selector(state, edges, {1}, "edge_scaling", current_property_name);
    }

    if (ImGui::Button("build laplacian")) {
        state->dispatcher.trigger<event::mesh::laplacian::build>(state->picker.entity_id,
                                                                 static_cast<MeshLaplacianStiffness>(current_stiffness),
                                                                 static_cast<MeshLaplacianMass >(current_mass),
                                                                 current_property_name);
    }
}

}
