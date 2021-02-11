//
// Created by alex on 21.01.21.
//

#include "bcg_gui_locally_optimal_projection.h"
#include "bcg_viewer_state.h"
#include "sampling/bcg_sampling_locally_optimal_projection.h"
#include "bcg_entity_hierarchy.h"
#include "bcg_gui_sampling_octree.h"

namespace bcg{

using namespace entt::literals;

void gui_locally_optimal_projection(viewer_state *state){
    static float attraction_radius = 0;
    static float repulsion_weight = 0;
    ImGui::InputFloat("attraction_radius", &attraction_radius);
    if(ImGui::InputFloat("repulsion_weight", &repulsion_weight)){
        repulsion_weight = std::max(0.0f, std::min(repulsion_weight, 0.5f));
    }
    static std::vector<std::string> names = lop_method_names();
    static std::vector<std::string> repulsion_type = repulsion_type_names();
    static int method_idx = 0;
    static int repulsion_idx = 0;
    static bool use_density_weight = false;
    static bool use_projection_tangential = false;
    ImGui::Checkbox("density weight", &use_density_weight);
    ImGui::Checkbox("projection_tangential", &use_projection_tangential);
    draw_combobox(&state->window, "method lop", method_idx, names);
    draw_combobox(&state->window, "repulsion type", repulsion_idx, repulsion_type);

    static float feature_radius = 0;
    if(static_cast<LopType>(method_idx) == LopType::flop){
        ImGui::InputFloat("feature_radius", &feature_radius);
    }

    if(ImGui::CollapsingHeader("Generate Sampling")){
        gui_sampling_octree(state);
    }

    auto id = state->picker.entity_id;
    if(ImGui::Button("Init")){
        if(state->scene.valid(id)){
            auto &hierarchy = state->scene.get_or_emplace<entity_hierarchy>(state->picker.entity_id);
            entt::entity child_id = entt::null;
            for (const auto &child : hierarchy.children) {
                if (state->scene.has<entt::tag<"subsampled"_hs>>(child.first)) {
                    child_id = child.first;
                    break;
                }
            }

            if(state->scene.valid(child_id)) {
                auto *ref_vertices = state->get_vertices(id);
                auto *sampling_vertices = state->get_vertices(child_id);
                switch (static_cast<LopType>(method_idx)) {
                    case LopType::lop : {
                        auto &projection = state->scene.get_or_emplace<lop>(id);
                        projection.init(*ref_vertices, *sampling_vertices, use_density_weight);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        break;
                    }
                    case LopType::wlop : {
                        auto &projection = state->scene.get_or_emplace<wlop>(id);
                        projection.init(*ref_vertices, *sampling_vertices, use_density_weight);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        break;
                    }
                    case LopType::flop : {
                        auto &projection = state->scene.get_or_emplace<flop>(id);
                        projection.init(*ref_vertices, *sampling_vertices, use_density_weight);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        break;
                    }
                    case LopType::clop : {
                        auto &projection = state->scene.get_or_emplace<clop>(id);
                        projection.init(*ref_vertices, *sampling_vertices, use_density_weight);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        break;
                    }
                    case LopType::__last__:
                        break;
                }
            }else{
                std::cerr << "please generate a subsampling first.\n";
            }
        }
    }
    static bool update_every_frame = false;
    ImGui::Checkbox("update_every_frame", &update_every_frame);
    if(update_every_frame || ImGui::Button("compute_step")){
        if(state->scene.valid(id)) {
            switch (static_cast<LopType>(method_idx)) {
                case LopType::lop : {
                    if(state->scene.has<lop>(id)){
                        auto &projection = state->scene.get<lop>(id);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        projection.compute_step();
                    }
                    break;
                }
                case LopType::wlop : {
                    if(state->scene.has<wlop>(id)){
                        auto &projection = state->scene.get<wlop>(id);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        projection.compute_step();
                    }
                    break;
                }
                case LopType::flop : {
                    if(state->scene.has<flop>(id)){
                        auto &projection = state->scene.get<flop>(id);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        projection.compute_step();
                    }
                    break;
                }
                case LopType::clop : {
                    if(state->scene.has<clop>(id)){
                        auto &projection = state->scene.get<clop>(id);
                        projection.attraction_radius = attraction_radius;
                        projection.repulsion_weight = repulsion_weight;
                        projection.feature_radius = feature_radius;
                        projection.tangential_projection_force = use_projection_tangential;
                        projection.repulsion_type = static_cast<projection_operator::RepulsionType>(repulsion_idx);
                        projection.compute_step();
                    }
                    break;
                }
                case LopType::__last__:
                    break;
            }
        }
    }
}

}