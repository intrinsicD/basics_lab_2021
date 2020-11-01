#include <iostream>

#include "bcg_opengl/bcg_viewer.h"
#include "bcg_opengl/bcg_imgui.h"
#include "bcg_opengl/systems/bcg_camera_system.h"
#include "bcg_opengl/systems/bcg_mesh_system.h"
#include "bcg_opengl/systems/bcg_gpu_system.h"
#include "bcg_opengl/renderers/bcg_render_system.h"

int main() {
    using namespace bcg;
    viewer viewer;
    viewer.state.gui.menu.show = [](viewer_state *state, gui_menu *self) {
        if (ImGui::BeginMenu("Viewer")) {
            if (ImGui::MenuItem("Settings")) {
                state->gui.left = gui_settings;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples")) {
            if(ImGui::MenuItem("Mesh")){
                state->gui.left = gui_mesh_factory;
            }
            ImGui::MenuItem("Console", NULL, &self->show_app_console);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Metrics", NULL, &self->show_app_metrics);
            ImGui::EndMenu();
        }

        return true;
    };

    viewer.state.systems["camera_system"] = std::make_unique<camera_system>(&viewer.state);
    viewer.state.systems["mesh_system"] = std::make_unique<mesh_system>(&viewer.state);
    viewer.state.systems["render_system"] = std::make_unique<render_system>(&viewer.state);
    viewer.state.systems["gpu_system"] = std::make_unique<gpu_system>(&viewer.state);
    viewer.run();
    return 0;
}
