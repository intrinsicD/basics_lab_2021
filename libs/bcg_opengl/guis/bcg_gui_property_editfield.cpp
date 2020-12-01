//
// Created by alex on 01.12.20.
//

#include "bcg_gui_property_editfield.h"
#include "bcg_gui_property_selector.h"
#include "bcg_viewer_state.h"

namespace bcg {

void edit_field(viewer_state *state, property_container *container, const std::string &name, bool &open) {
    if (!ImGui::Begin(name.c_str(), &open)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    static std::string current_property_name;
    gui_property_selector(state, container, {}, "property", current_property_name);

    if (ImGui::Button("delete")) {
        auto iter = container->container.find(current_property_name);
        if(iter != container->container.end()) {
            container->container.erase(iter);
            current_property_name = "";
        }
    }

    static std::string new_name = current_property_name;
    draw_textinput(&state->window, "name", new_name);

    if (ImGui::Button("rename")) {
        auto iter = container->container.find(current_property_name);
        if(iter != container->container.end()){
            auto p = iter->second;
            p->set_name(new_name);
            container->container[new_name] = p;
            container->container.erase(iter);
            current_property_name = new_name;
        }
    }

    ImGui::End();
}

}