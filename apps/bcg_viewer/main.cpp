#include <iostream>

#include "bcg_opengl/bcg_viewer.h"
#include "bcg_opengl/bcg_imgui.h"

int main() {
    using namespace bcg;

    viewer viewer;
    viewer.state.gui.menu.show = [](viewer_state *state, gui_menu *self) {
        if (ImGui::BeginMenu("Viewer")) {
            if (ImGui::MenuItem("Info")) {
                state->gui.left = gui_info;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Mesh")) {
            if(ImGui::MenuItem("Dihedeal Angle")){
                state->gui.left = gui_dihedral_angle;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples")) {
            if(ImGui::MenuItem("Mesh")){
                /*state->gui.left = gui_mesh_factory;*/
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

    viewer.run();
    return 0;
}
