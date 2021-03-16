//
// Created by alex on 16.11.20.
//

#include "bcg_gui_property_selector.h"
#include "viewer/bcg_viewer_state.h"
#include "geometry/bcg_property.h"

namespace bcg{


bool gui_property_selector(viewer_state *state, property_container *container, const std::vector<int> &filter_dims,
                                std::string label,
                                std::string &current_property_name) {
    if (!container) return false;
    ImGui::PushID((container->name + current_property_name + label).c_str());
    auto property_names = container->properties_names(filter_dims);
    if (property_names.empty()) return false;
    property_names.emplace_back("");
    int current_entry = property_names.size() - 1;
    for (int i = 0; i < property_names.size(); ++i) {
        if (property_names[i] == current_property_name) {
            current_entry = i;

            break;
        }
    }
    if(current_entry >= property_names.size()){
        return false;
    }
    auto change = draw_combobox(&state->window, label.c_str(), current_entry, property_names);
    current_property_name = property_names[current_entry];
    ImGui::PopID();
    return change;
}

}