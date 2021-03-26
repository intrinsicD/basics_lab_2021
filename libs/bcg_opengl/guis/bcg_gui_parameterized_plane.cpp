//
// Created by alex on 26.03.21.
//

#include "bcg_gui_parameterized_plane.h"
#include "viewer/bcg_viewer_state.h"
#include "geometry/parameterized_plane/bcg_parameterized_plane_fit.h"
#include "bcg_property_map_eigen.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "bcg_gui_property_selector.h"

namespace bcg {

using namespace entt::literals;

void gui_parameterized_plane(viewer_state *state) {
    if (ImGui::Button("Make Plane")) {
        state->dispatcher.trigger<event::mesh::make_parameterized_plane>();
    }
    auto id = state->picker.entity_id;
    static entt::entity plane_id = entt::null;

    if (ImGui::Button("Select Plane") || (state->scene.valid(id) && state->scene.all_of<entt::tag<"parameterized_plane"_hs>>(id))) {
        plane_id = id;
    }
    if (state->scene.valid(plane_id)) {
        auto &plane = state->scene.get<ParameterizedPlane3>(plane_id);
        std::stringstream ss;
        ss << plane;
        VectorS<3> dir1 = plane.directions.col(0);
        VectorS<3> dir2 = plane.directions.col(1);
        draw_input_vec3(&state->window, "base_point", plane.base_point);
        if (draw_input_vec3(&state->window, "direction1", dir1)) {
            plane.directions.col(0) = dir1;
        }
        if (draw_input_vec3(&state->window, "direction2", dir2)) {
            plane.directions.col(1) = dir2;
        }
    }
    if (ImGui::CollapsingHeader("Fitting")) {
        static auto fitting_names = parameterized_plane_fit_names();
        static int e = 0;
        static entt::entity target_id;
        static std::string current_property_name;
        static int max_iterations = 1;
        draw_combobox(&state->window, "fitting method", e, fitting_names);
        vertex_container *target_vertices = nullptr;
        if (state->scene.valid(target_id)) {
            target_vertices = state->get_vertices(target_id);
        }
        switch (static_cast<ReparameterizedFittingTypes>(e)) {
            case ReparameterizedFittingTypes::least_squared_svd : {
                break;
            }
            case ReparameterizedFittingTypes::weighted_least_squared_svd : {
                if (target_vertices) {
                    gui_property_selector(state,target_vertices,{1},"weights",current_property_name);
                }

                break;
            }
            case ReparameterizedFittingTypes::robust_weighted_least_squared_svd : {
                if (target_vertices) {
                    gui_property_selector(state,target_vertices,{1},"weights",current_property_name);
                    ImGui::InputInt("max_iterations", &max_iterations);
                }
                break;
            }
            case ReparameterizedFittingTypes::__last__ : {
                break;
            }
        }
        if (ImGui::Button("Fit")) {
            target_id = id;
            if (!state->scene.valid(plane_id)) {
                state->dispatcher.trigger<event::mesh::make_parameterized_plane>();
                plane_id = state->picker.entity_id;
                state->picker.entity_id = target_id;
            }

            if (state->scene.valid(target_id) &&
                !state->scene.all_of<entt::tag<"parameterized_plane"_hs>>(target_id)) {
                auto &plane = state->scene.get<ParameterizedPlane3>(plane_id);
                target_vertices = state->get_vertices(target_id);
                auto target_positions = target_vertices->get<VectorS<3>, 3>("v_position");
                bool changed = false;
                switch (static_cast<ReparameterizedFittingTypes>(e)) {
                    case ReparameterizedFittingTypes::least_squared_svd : {
                        MatrixS<-1, 3> points = MapConst(target_positions);
                        VectorS<3> mean = points.colwise().mean();
                        least_squares_fit_svd(plane, points, mean);
                        changed = true;
                        break;
                    }
                    case ReparameterizedFittingTypes::weighted_least_squared_svd : {
                        MatrixS<-1, 3> points = MapConst(target_positions);
                        VectorS<3> mean = points.colwise().mean();
                        VectorS<-1> weights = VectorS<-1>::Ones(points.rows());
                        if(!current_property_name.empty()){
                            auto property_weights = target_vertices->get<bcg_scalar_t, 1>(current_property_name);
                            if(property_weights){
                                weights = MapConst(property_weights);
                            }
                        }
                        weighted_least_squares_fit_svd(plane, points, mean, weights);
                        changed = true;
                        break;
                    }
                    case ReparameterizedFittingTypes::robust_weighted_least_squared_svd : {
                        MatrixS<-1, 3> points = MapConst(target_positions);
                        VectorS<3> mean = points.colwise().mean();
                        VectorS<-1> weights = VectorS<-1>::Ones(points.rows());
                        if(!current_property_name.empty()){
                            auto property_weights = target_vertices->get<bcg_scalar_t, 1>(current_property_name);
                            if(property_weights){
                                weights = MapConst(property_weights);
                            }
                        }
                        robust_least_squares_fit_svd(plane, points, mean, weights, max_iterations);
                        changed = true;
                        break;
                    }
                    case ReparameterizedFittingTypes::__last__ : {
                        break;
                    }
                }
                if (changed) {
                    auto &mesh = state->scene.get<halfedge_mesh>(plane_id);
                    Map(mesh.positions) = MapConst(get_vertices(plane));
                    mesh.positions.set_dirty();
                    auto &transform = state->scene.get<Transform>(plane_id);
                    transform.setIdentity();
                }
            }
        }
    }
}

}