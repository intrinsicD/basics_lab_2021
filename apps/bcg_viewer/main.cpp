#include <iostream>

#include "bcg_opengl/bcg_viewer.h"
#include "bcg_opengl/bcg_imgui.h"

int main() {
    using namespace bcg;
    viewer viewer;
    viewer.state.gui.menu.show = [](viewer_state *state, gui_menu *self) {
        if (ImGui::BeginMenu("Viewer")) {
            if (ImGui::MenuItem("Settings")) {
                state->gui.left = settings;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples")) {
            ImGui::MenuItem("Main menu bar", NULL, &self->show_app_main_menu_bar);
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
