//
// Created by alex on 29.03.21.
//

#include "bcg_gui_point_cloud_vertex_extremal_values.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_kdtree_selector.h"
#include "viewer/bcg_viewer_state.h"
#include "point_cloud/bcg_point_cloud_extremal_vertex_values.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_material_mesh.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "bcg_opengl/renderers/points_renderer/bcg_material_points.h"
#include "bcg_opengl/renderers/points_renderer/bcg_events_points_renderer.h"

namespace bcg {

void gui_point_cloud_vertex_extremal_values(viewer_state *state) {
    static int e = 0;
    static std::string current_property_name;
    if (state->scene.valid(state->picker.entity_id)) {
        draw_combobox(&state->window, "vertex extremal type", e, vertex_extremal_type_names());
        auto *vertices = state->get_vertices(state->picker.entity_id);
        gui_property_selector(state, vertices, {1}, "property", current_property_name);
        kdtree_parameters params = gui_kd_tree_selector(state);

        if (ImGui::Button("Filter values")) {
            auto &index = state->scene.get<kdtree_property<bcg_scalar_t>>(state->picker.entity_id);
            auto property = vertices->get<bcg_scalar_t, 1>(current_property_name);
            if (property) {
                std::string property_name;
                switch (static_cast<VertexExtremalType>(e)) {
                    case VertexExtremalType::minimum : {
                        if (params.type == kdtree_parameters::Type::knn) {
                            point_cloud_minimal_vertex_values_knn(vertices, property, index, params.num_closest,
                                                                  state->config.parallel_grain_size);
                        } else if (params.type == kdtree_parameters::Type::radius) {
                            point_cloud_minimal_vertex_values_radius(vertices, property, index, params.radius,
                                                                     state->config.parallel_grain_size);
                        }
                        property_name = "v_minimal_values";
                        break;
                    }
                    case VertexExtremalType::maximum : {
                        if (params.type == kdtree_parameters::Type::knn) {
                            point_cloud_maximal_vertex_values_knn(vertices, property, index, params.num_closest,
                                                                  state->config.parallel_grain_size);
                        } else if (params.type == kdtree_parameters::Type::radius) {
                            point_cloud_maximal_vertex_values_radius(vertices, property, index, params.radius,
                                                                     state->config.parallel_grain_size);
                        }
                        property_name = "v_maximal_values";
                        break;
                    }
                    case VertexExtremalType::extremal : {
                        if (params.type == kdtree_parameters::Type::knn) {
                            point_cloud_extremal_vertex_values_knn(vertices, property, index, params.num_closest,
                                                                  state->config.parallel_grain_size);
                        } else if (params.type == kdtree_parameters::Type::radius) {
                            point_cloud_extremal_vertex_values_radius(vertices, property, index, params.radius,
                                                                     state->config.parallel_grain_size);
                        }
                        property_name = "v_extremal_values";
                        break;
                    }
                    case VertexExtremalType::__last__ :{
                        break;
                    }
                }
                if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id) && !property_name.empty()){
                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = property_name;
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                }else{
                    auto &material = state->scene.get<material_points>(state->picker.entity_id);
                    auto &color = material.attributes[1];
                    color.property_name = property_name;
                    state->dispatcher.trigger<event::points_renderer::set_color_attribute>(state->picker.entity_id, color);
                }
            }
        }
    }
}

}