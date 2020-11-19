#include <iostream>

#include "bcg_opengl/bcg_viewer.h"
#include "bcg_opengl/bcg_imgui.h"
#include "bcg_opengl/guis/bcg_gui_viewer_state.h"
#include "bcg_opengl/guis/bcg_gui_viewer_curve.h"
#include "bcg_opengl/guis/bcg_gui_mesh_dihedral_angle.h"
#include "bcg_opengl/guis/bcg_gui_mesh_face_normals.h"
#include "bcg_opengl/guis/bcg_gui_mesh_vertex_normals.h"
#include "bcg_opengl/guis/bcg_gui_mesh_boundary.h"

int main() {
    using namespace bcg;

    viewer viewer;
    viewer.state.gui.menu.show = [](viewer_state *state, gui_menu *self) {
        if (ImGui::BeginMenu("Viewer")) {
            if (ImGui::MenuItem("Info")) {
                state->gui.left = gui_viewer_state;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Mesh")) {
            if(ImGui::MenuItem("Dihedeal Angle")){
                state->gui.left = gui_mesh_dihedral_angle;
            }
            if(ImGui::MenuItem("Vertex Normals")){
                state->gui.left = gui_mesh_vertex_normals;
            }
            if(ImGui::MenuItem("Face Normals")){
                state->gui.left = gui_mesh_face_normals;
            }
            if(ImGui::MenuItem("Boundary")){
                state->gui.left = gui_mesh_boundary;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Exercises")) {
            if(ImGui::MenuItem("Bezier Curves")){
                state->gui.left = gui_viewer_curve;
            }
            ImGui::EndMenu();
        }

        return true;
    };

    viewer.run();
    return 0;
}
