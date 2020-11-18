//
// Created by alex on 16.11.20.
//

#include "bcg_gui_property_container_selector.h"
#include "bcg_viewer_state.h"

namespace bcg{


template<typename Real, int N>
bool draw_combo(viewer_state *, const property<Real, N> prop, size_t &current_entry) {
    if (!prop || prop.empty()) return false;
    if (current_entry >= prop.size()) { current_entry = 0; }
    std::stringstream ss;
    ss << prop[current_entry];
    if (ImGui::BeginCombo(prop.name().c_str(), ss.str().c_str())) {
        for (size_t n = 0; n < prop.size(); ++n) {
            ss.str("");
            ss << prop[n];
            const bool is_selected = (static_cast<size_t>(current_entry) == n);
            ImGui::PushID((prop.name() + std::to_string(n)).c_str());
            if (ImGui::Selectable(ss.str().c_str(), is_selected)) {
                current_entry = n;
            }
            ImGui::PopID();

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return true;
}


template<typename Real, int N>
bool draw_combo(viewer_state *, const property<Vector<Real, N>, N> prop, size_t &current_entry) {
    if (!prop || prop.empty()) return false;
    if (current_entry >= prop.size()) { current_entry = 0; }
    std::stringstream ss;
    ss << prop[current_entry].transpose();
    if (ImGui::BeginCombo(prop.name().c_str(), ss.str().c_str())) {
        for (size_t n = 0; n < prop.size(); ++n) {
            ss.str("");
            ss << prop[n].transpose();
            const bool is_selected = (static_cast<size_t>(current_entry) == n);
            ImGui::PushID((prop.name() + std::to_string(n)).c_str());
            if (ImGui::Selectable(ss.str().c_str(), is_selected)) {
                current_entry = n;
            }
            ImGui::PopID();
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return true;
}

void gui_property_container_selector(viewer_state *state, property_container *container, size_t &current_entry){
    if (!container) return;
    if (ImGui::CollapsingHeader((container->name + " #" + std::to_string(container->size())).c_str())) {
        for (const auto &name : container->properties_names()) {
            ImGui::PushID((container->name + name).c_str());
            auto any = draw_combo(state, container->get<bool, 1>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<bcg_scalar_t, 1>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<int, 1>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<unsigned int, 1>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<size_t, 1>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorS<2>, 2>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorS<3>, 3>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorS<4>, 4>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorI<2>, 2>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorI<3>, 3>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorI<4>, 4>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorI<5>, 5>(name), current_entry);
            if (!any) any = draw_combo(state, container->get<VectorI<6>, 6>(name), current_entry);
            ImGui::PopID();
            if (!any) continue;
        }
        ImGui::PushID((container->name + "connectivity").c_str());
        draw_combo(state, container->get<halfedge_graph::vertex_connectivity, 1>("v_connectivity"), current_entry);
        draw_combo(state, container->get<halfedge_graph::halfedge_connectivity, 4>("h_connectivity"), current_entry);
        draw_combo(state, container->get<halfedge_mesh::face_connectivity, 1>("f_connectivity"), current_entry);
        ImGui::PopID();
    }
}

}