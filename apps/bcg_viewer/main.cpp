#include <iostream>

#include "bcg_opengl/bcg_viewer.h"
#include "bcg_opengl/bcg_imgui.h"
#include "bcg_opengl/guis/bcg_gui_viewer_state.h"
#include "bcg_opengl/guis/bcg_gui_viewer_curve.h"
#include "bcg_opengl/guis/bcg_gui_point_cloud_graph_builder.h"
#include "bcg_opengl/guis/bcg_gui_point_cloud_vertex_pca.h"
#include "bcg_opengl/guis/bcg_gui_point_cloud_curvature_taubin.h"
#include "bcg_opengl/guis/bcg_gui_point_cloud_vertex_outlier_probability.h"
#include "bcg_opengl/guis/bcg_gui_point_cloud_kernel_density_estimation.h"
#include "bcg_opengl/guis/bcg_gui_point_cloud_quadric.h"
#include "bcg_opengl/guis/bcg_gui_graph_vertex_pca.h"
#include "bcg_opengl/guis/bcg_gui_mesh_dihedral_angle.h"
#include "bcg_opengl/guis/bcg_gui_mesh_face_normals.h"
#include "bcg_opengl/guis/bcg_gui_mesh_vertex_normals.h"
#include "bcg_opengl/guis/bcg_gui_mesh_boundary.h"
#include "bcg_opengl/guis/bcg_gui_mesh_vertex_convex_concave.h"
#include "bcg_opengl/guis/bcg_gui_mesh_subdivision.h"
#include "bcg_opengl/guis/bcg_gui_mesh_features.h"
#include "bcg_opengl/guis/bcg_gui_mesh_laplacian.h"
#include "bcg_opengl/guis/bcg_gui_mesh_curvature_taubin.h"
#include "bcg_opengl/guis/bcg_gui_mesh_simplification.h"
#include "bcg_opengl/guis/bcg_gui_mesh_remeshing.h"
#include "bcg_opengl/guis/bcg_gui_mesh_statistics.h"
#include "bcg_opengl/guis/bcg_gui_marching_cubes.h"

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
        if (ImGui::BeginMenu("Point Cloud")) {
            if(ImGui::MenuItem("Build Graph")){
                state->gui.left = gui_point_cloud_graph_builder;
            }
            if(ImGui::MenuItem("Vertex Pca")){
                state->gui.left = gui_point_cloud_vertex_pca;
            }
            if(ImGui::MenuItem("Curvature Taubin")){
                state->gui.left = gui_point_cloud_curvature_taubin;
            }
            if(ImGui::MenuItem("Outliers")){
                state->gui.left = gui_point_cloud_vertex_outlier_probability;
            }
            if(ImGui::MenuItem("Kernel Density Estimation")){
                state->gui.left = gui_point_cloud_kernel_density_estimation;
            }
            if(ImGui::MenuItem("Quadrics")){
                state->gui.left = gui_point_cloud_quadric;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Graph")) {
            if(ImGui::MenuItem("Vertex Pca")){
                state->gui.left = gui_graph_vertex_pca;
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
            if(ImGui::MenuItem("Convex Concave")){
                state->gui.left = gui_mesh_vertex_convex_concave;
            }
            if(ImGui::MenuItem("Features")){
                state->gui.left = gui_mesh_features;
            }
            if(ImGui::MenuItem("Subdivision")){
                state->gui.left = gui_mesh_subdivision;
            }
            if(ImGui::MenuItem("Laplacian")){
                state->gui.left = gui_mesh_laplacian;
            }
            if(ImGui::MenuItem("Curvature Taubin")){
                state->gui.left = gui_mesh_curvature_taubin;
            }
            if(ImGui::MenuItem("Simplification")){
                state->gui.left = gui_mesh_simplification;
            }
            if(ImGui::MenuItem("Remeshing")){
                state->gui.left = gui_mesh_remeshing;
            }
            if(ImGui::MenuItem("Statistics")){
                state->gui.left = gui_mesh_statistics;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Implicit")) {
            if(ImGui::MenuItem("Marching Cubes")){
                state->gui.left = gui_marching_cubes;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Exercises")) {
            if(ImGui::MenuItem("Bezier Curves")){
                state->gui.left = gui_viewer_curve;
            }
            if(ImGui::MenuItem("Marching Cubes")){
                state->gui.left = gui_marching_cubes;
            }
            if(ImGui::MenuItem("Subdivision Surfaces")){
                state->gui.left = gui_mesh_subdivision;
            }
            ImGui::EndMenu();
        }

        return true;
    };

    viewer.run();
    return 0;
}
