//
// Created by alex on 13.07.21.
//

#include "bcg_gui_mesh_geodesic_distances_heat_method.h"
#include "viewer/bcg_viewer_state.h"
#include "math/laplacian/bcg_laplacian_heat_method.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "igl/heat_geodesics.h"
#include "bcg_property_map_eigen.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "components/bcg_component_selection_vertex_overlay.h"

namespace bcg {

void gui_mesh_geodesic_heat_method(viewer_state *state) {
    static float m = 1;
    static float t = 1;
    static float h = 1;
    static bool use_user_defined_t = false;
    auto id = state->picker.entity_id;
    ImGui::Checkbox("user_defined t", &use_user_defined_t);
    if (use_user_defined_t) {
        if(ImGui::InputFloat("m", &m)){
            t = m * h * h;
        }
        if(ImGui::InputFloat("h", &h)){
            t = m * h * h;
        }
        ImGui::InputFloat("t", &t);
        if (ImGui::Button("Compute best t from edge length")) {
            if (state->scene.valid(id) && state->scene.has<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                h = compute_t(mesh);
                t = m * h * h;
            }
        }
    }
    ImGui::Separator();
    static bool vertex_selection_mode = state->picker.mode == viewer_picker::Mode::vertices;
    if (ImGui::Checkbox("vertex_selection_mode", &vertex_selection_mode)) {
        if (vertex_selection_mode) {
            state->dispatcher.trigger<event::picker::enable::vertex>();
        } else {
            state->dispatcher.trigger<event::picker::disable>();
        }
        vertex_selection_mode = state->picker.mode == viewer_picker::Mode::vertices;
    }

    static igl::HeatGeodesicsData<bcg_scalar_t> data;
    ImGui::Checkbox("Intrinsic Delaunay", &data.use_intrinsic_delaunay);
    if (ImGui::Button("Compute Precompute")) {
        if (state->scene.valid(id) && state->scene.has<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            MatrixS<-1, 3> P = MapConst(mesh.positions);
            Matrix<int, -1, 3> F = MapConst(mesh.get_triangles()).cast<int>();
            if (!use_user_defined_t) {
                igl::heat_geodesics_precompute(P, F, data);
            } else {
                igl::heat_geodesics_precompute(P, F, static_cast<bcg_scalar_t>(t), data);
            }
        }
    }
    if (ImGui::Button("Compute Geodesics")) {
        if (state->scene.valid(id) && state->scene.has<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            auto v_distances = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_geodesic_distance_heat_method");
            MatrixS<-1, 1> D(mesh.num_vertices(), 1);
            auto &selection = state->scene.get<component_selection_vertex_overlay>(id);
            Vector<unsigned int, -1> G = MapConst(selection.parent_indices).cast<unsigned int>();
            igl::heat_geodesics_solve(data, G, D);
            Map(v_distances) = D;
/*                auto &laplacian = state->scene.get<mesh_laplacian>(state->picker.entity_id);
                heat_method(mesh, laplacian, t, state->config.parallel_grain_size);*/
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            auto &color = material.attributes[2];
            color.property_name = "v_geodesic_distance_heat_method";
            state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id,
                                                                                        color);
        }
    }
}


}