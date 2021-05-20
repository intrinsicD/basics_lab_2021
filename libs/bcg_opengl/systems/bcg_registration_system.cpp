//
// Created by alex on 07.12.20.
//

#include "bcg_registration_system.h"
#include "viewer/bcg_viewer_state.h"
#include "correspondences/bcg_correspondences.h"
#include "registration/bcg_registration.h"
#include "bcg_library/registration/bcg_rigid_icp.h"
#include "bcg_library/registration/bcg_coherent_point_drift.h"
#include "bcg_property_map_eigen.h"

namespace bcg {

registration_system::registration_system(viewer_state *state) : system("registration_system", state) {
    state->dispatcher.sink<event::registration::align_step>().connect<&registration_system::on_align_step>(this);
    state->dispatcher.sink<event::registration::align_converge>().connect<&registration_system::on_align_converge>(
            this);
    state->dispatcher.sink<event::registration::reset>().connect<&registration_system::on_reset>(this);
}

void registration_system::on_align_step(const event::registration::align_step &event) {
    if (!state->scene.valid(event.source_id)) return;
    if (!state->scene.valid(event.target_id)) return;
    if (event.source_id == event.target_id) return;
    if (!state->scene.all_of<registration>(event.source_id)) {
        registration reg;
        state->scene.emplace<registration>(event.source_id, reg);
    }
    auto &reg = state->scene.get<registration>(event.source_id);

    auto *source_vertices = state->get_vertices(event.source_id);
    auto *target_vertices = state->get_vertices(event.target_id);

    auto source_positions = source_vertices->get<VectorS<3>, 3>("v_position");
    auto target_positions = target_vertices->get<VectorS<3>, 3>("v_position");

    auto &source_model = state->scene.get<Transform>(event.source_id);
    auto &target_model = state->scene.get<Transform>(event.target_id);

    Transform delta = Transform::Identity();
    switch (event.method) {
        case RegistrationMethod::rigid_icp_point2point : {
            state->dispatcher.trigger<event::correspondences::estimate>(event.source_id, event.target_id);
            auto &correspondences = state->scene.get<entity_correspondences>(event.source_id);
            auto Y = correspondences.maps[size_t(event.target_id)].get_source(source_positions);
            auto X = correspondences.maps[size_t(event.target_id)].get_target(target_positions);
            delta = minimize_point_2_point(Y, source_model, X, target_model, state->config.parallel_grain_size);
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            break;
        }
        case RegistrationMethod::rigid_icp_point2plane : {
            state->dispatcher.trigger<event::correspondences::estimate>(event.source_id, event.target_id);
            auto &correspondences = state->scene.get<entity_correspondences>(event.source_id);
            auto target_normals = target_vertices->get<VectorS<3>, 3>("v_normal");
            auto Y = correspondences.maps[size_t(event.target_id)].get_source(source_positions);
            auto X = correspondences.maps[size_t(event.target_id)].get_target(target_positions);
            auto N = correspondences.maps[size_t(event.target_id)].get_target(target_normals);
            delta = minimize_point_2_plane(Y, source_model, X, N, target_model, state->config.parallel_grain_size);
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            break;
        }
        case RegistrationMethod::coherent_point_drift_rigid : {
            auto &rigid = state->scene.get_or_emplace<coherent_point_drift_rigid>(event.source_id);
            if(reg.errors.empty() || !rigid.initialized){
                rigid.init(source_vertices, source_model, target_vertices, target_model);
                rigid.initialized = true;
            }
            rigid.parallel_grain_size = state->config.parallel_grain_size;
            rigid.compute_step();
            delta = Translation(rigid.t) * Rotation(rigid.R) * Scaling(VectorS<3>::Constant(rigid.s));
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            rigid.likelihood.push_back(MapConst(rigid.P1).prod());
            break;
        }
        case RegistrationMethod::coherent_point_drift_affine : {
            auto &affine = state->scene.get_or_emplace<coherent_point_drift_affine>(event.source_id);
            if(reg.errors.empty() || !affine.initialized){
                affine.init(source_vertices, source_model, target_vertices, target_model);
                affine.initialized = true;
            }
            affine.parallel_grain_size = state->config.parallel_grain_size;
            affine.compute_step();
            delta = Translation(affine.t) * Transform(affine.B);
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            affine.likelihood.push_back(MapConst(affine.P1).prod());
            break;
        }
        case RegistrationMethod::coherent_point_drift_nonrigid : {
            auto &nonrigid = state->scene.get_or_emplace<coherent_point_drift_nonrigid>(event.source_id);
            if(reg.errors.empty() || !nonrigid.initialized){
                nonrigid.init(source_vertices, source_model, target_vertices, target_model);
                nonrigid.initialized = true;
            }
            nonrigid.parallel_grain_size = state->config.parallel_grain_size;
            nonrigid.compute_step();
            reg.errors.push_back((MapConst(nonrigid.PX) - MapConst(nonrigid.P1).asDiagonal() * nonrigid.Y).rowwise().squaredNorm().sum());
            nonrigid.likelihood.push_back(MapConst(nonrigid.P1).prod());
            break;
        }
        case RegistrationMethod::coherent_point_drift_nonrigid2 : {
            auto &nonrigid = state->scene.get_or_emplace<coherent_point_drift_nonrigid2>(event.source_id);
            if(reg.errors.empty() || !nonrigid.initialized){
                nonrigid.init(source_vertices, source_model, target_vertices, target_model);
                nonrigid.initialized = true;
            }
            nonrigid.parallel_grain_size = state->config.parallel_grain_size;
            nonrigid.compute_step();
            delta = Translation(nonrigid.t) * Rotation(nonrigid.R) * Scaling(VectorS<3>::Constant(nonrigid.s));
            reg.errors.push_back((MapConst(nonrigid.PX) - MapConst(nonrigid.P1).asDiagonal() * nonrigid.Y).rowwise().squaredNorm().sum());
            nonrigid.likelihood.push_back(MapConst(nonrigid.P1).prod());
            break;
        }
        case RegistrationMethod::coherent_point_drift_bayes : {
            auto &bayes = state->scene.get_or_emplace<coherent_point_drift_bayes>(event.source_id);
            if(reg.errors.empty() || !bayes.initialized){
                bayes.init(source_vertices, source_model, target_vertices, target_model);
                bayes.initialized = true;
            }
            bayes.parallel_grain_size = state->config.parallel_grain_size;
            bayes.compute_step();
            reg.errors.push_back((MapConst(bayes.PX) - MapConst(bayes.P1).asDiagonal() * bayes.Y).rowwise().squaredNorm().sum());
            bayes.likelihood.push_back(MapConst(bayes.P1).prod());
            delta = Translation(bayes.t) * Rotation(bayes.R) * Scaling(VectorS<3>::Constant(bayes.s));
            source_model = bayes.backup_source_model;
            break;
        }
        case RegistrationMethod::__last__: {
            break;
        }
        default : {
            break;
        }
    }
    source_model = delta * source_model;
}

void registration_system::on_align_converge(const event::registration::align_converge &event) {
    if (!state->scene.valid(event.source_id)) return;
    if (!state->scene.valid(event.target_id)) return;
    if (!state->scene.all_of<registration>(event.source_id)) {
        state->scene.emplace<registration>(event.source_id);
    }
    auto &reg = state->scene.get<registration>(event.source_id);

    if (reg.current_iteration < reg.max_iterations) {
        state->dispatcher.trigger<event::registration::align_step>(event.source_id, event.target_id, event.method);
    }
}

void registration_system::on_reset(const event::registration::reset &event) {
    if (!state->scene.valid(event.source_id)) return;
    if (!state->scene.valid(event.target_id)) return;
    if (!state->scene.all_of<registration>(event.source_id))return;

    auto &reg = state->scene.get<registration>(event.source_id);
    reg.errors.clear();

    if(state->scene.all_of<coherent_point_drift_rigid>(event.source_id)){
        auto &cpd = state->scene.get<coherent_point_drift_rigid>(event.source_id);
        cpd.reset();
        state->scene.remove_if_exists<coherent_point_drift_rigid>(event.source_id);
    }
    if(state->scene.all_of<coherent_point_drift_affine>(event.source_id)){
        auto &cpd = state->scene.get<coherent_point_drift_affine>(event.source_id);
        cpd.reset();
        state->scene.remove_if_exists<coherent_point_drift_affine>(event.source_id);
    }
    if(state->scene.all_of<coherent_point_drift_nonrigid>(event.source_id)){
        auto &cpd = state->scene.get<coherent_point_drift_nonrigid>(event.source_id);
        cpd.reset();
        state->scene.remove_if_exists<coherent_point_drift_nonrigid>(event.source_id);
    }
    if(state->scene.all_of<coherent_point_drift_bayes>(event.source_id)){
        auto &cpd = state->scene.get<coherent_point_drift_bayes>(event.source_id);
        cpd.reset();
        state->scene.remove_if_exists<coherent_point_drift_bayes>(event.source_id);
    }
}

}