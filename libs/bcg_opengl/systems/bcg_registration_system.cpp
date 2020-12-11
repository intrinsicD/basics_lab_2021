//
// Created by alex on 07.12.20.
//

#include "bcg_registration_system.h"
#include "bcg_viewer_state.h"
#include "correspondences/bcg_correspondences.h"
#include "registration/bcg_registration.h"
#include "registration/rigid_idp/bcg_rigid_icp.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_rigid.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_affine.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_nonrigid.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_bayes.h"
#include "registration/coherent_point_drift/bcg_coherent_point_drift_test.h"
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
    if (!state->scene.has<registration>(event.source_id)) {
        registration reg;
        state->scene.emplace<registration>(event.source_id, reg);
    }
    auto &reg = state->scene.get<registration>(event.source_id);

    auto *source_vertices = state->get_vertices(event.source_id);
    auto *target_vertices = state->get_vertices(event.target_id);

    auto source_positions = source_vertices->get<VectorS<3>, 3>("v_position");
    auto target_positions = target_vertices->get<VectorS<3>, 3>("v_position");

    auto &source_model = state->scene.get<Transform>(event.source_id);
    const auto &target_model = state->scene.get<Transform>(event.target_id);

    Transform delta = Transform::Identity();
    switch (event.method) {
        case RegistrationMethod::rigid_icp_point2point : {
            state->dispatcher.trigger<event::correspondences::estimate>(event.source_id, event.target_id);
            auto &correspondences = state->scene.get<entity_correspondences>(event.source_id);
            auto Y = correspondences.maps[size_t(event.target_id)].get_source_points(source_positions);
            auto X = correspondences.maps[size_t(event.target_id)].get_target_points(target_positions);
            delta = minimize_point_2_point(Y, source_model, X, target_model, state->config.parallel_grain_size);
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            break;
        }
        case RegistrationMethod::rigid_icp_point2plane : {
            state->dispatcher.trigger<event::correspondences::estimate>(event.source_id, event.target_id);
            auto &correspondences = state->scene.get<entity_correspondences>(event.source_id);
            auto target_normals = target_vertices->get<VectorS<3>, 3>("v_normal");
            auto Y = correspondences.maps[size_t(event.target_id)].get_source_points(source_positions);
            auto X = correspondences.maps[size_t(event.target_id)].get_target_points(target_positions);
            auto N = correspondences.maps[size_t(event.target_id)].get_target_normals(target_normals);
            delta = minimize_point_2_plane(Y, source_model, X, N, target_model, state->config.parallel_grain_size);
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            break;
        }
        case RegistrationMethod::coherent_point_drift_rigid : {
            MatrixS<-1, 3> Y = ((source_model.linear() * MapConst(source_positions).transpose()).colwise() +
                                source_model.translation()).transpose();
            MatrixS<-1, 3> X = ((target_model.linear() * MapConst(target_positions).transpose()).colwise() +
                                target_model.translation()).transpose();
            auto &rigid = state->scene.get_or_emplace<coherent_point_drift_rigid>(event.source_id);
            if(reg.errors.empty()){
                rigid.init(Y, X);
            }
            rigid(rigid.P, Y, X, state->config.parallel_grain_size);
            delta = Translation(rigid.t) * Rotation(MatrixS<3, 3>(rigid.R)) * Scaling(VectorS<3>::Constant(rigid.s));
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            break;
        }
        case RegistrationMethod::coherent_point_drift_affine : {
            MatrixS<-1, 3> Y = ((source_model.linear() * MapConst(source_positions).transpose()).colwise() +
                                source_model.translation()).transpose();
            MatrixS<-1, 3> X = ((target_model.linear() * MapConst(target_positions).transpose()).colwise() +
                                target_model.translation()).transpose();
            auto &affine = state->scene.get_or_emplace<coherent_point_drift_affine>(event.source_id);
            if(reg.errors.empty()){
                affine.init(Y, X);
            }
            affine(affine.P, Y, X, state->config.parallel_grain_size);
            delta = Translation(affine.t) * Transform(MatrixS<3, 3>(affine.B));
            reg.errors.push_back((delta.matrix() - MatrixS<4, 4>::Identity()).norm());
            break;
        }
        case RegistrationMethod::coherent_point_drift_nonrigid : {
            MatrixS<-1, 3> Y = ((source_model.linear() * MapConst(source_positions).transpose()).colwise() +
                                source_model.translation()).transpose();
            MatrixS<-1, 3> X = ((target_model.linear() * MapConst(target_positions).transpose()).colwise() +
                                target_model.translation()).transpose();

            auto &nonrigid = state->scene.get_or_emplace<coherent_point_drift_nonrigid>(event.source_id);
            if(reg.errors.empty()){
                nonrigid.init(Y, X);
            }
            nonrigid(nonrigid.P, Y, X, state->config.parallel_grain_size);
            auto cpd_vector = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_vectors");
            auto cpd_positions = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_nonrigid_position");
            Map(cpd_vector) = (nonrigid.T - Y) * source_model.linear();
            Map(cpd_positions) = (source_model.inverse() * nonrigid.T.transpose().colwise().homogeneous()).transpose();
            reg.errors.push_back(nonrigid.sigma_squared);
            cpd_vector.set_dirty();
            cpd_positions.set_dirty();
            break;
        }
        case RegistrationMethod::coherent_point_drift_bayes : {
            MatrixS<-1, 3> Y = ((source_model.linear() * MapConst(source_positions).transpose()).colwise() +
                                source_model.translation()).transpose();
            MatrixS<-1, 3> X = ((target_model.linear() * MapConst(target_positions).transpose()).colwise() +
                                target_model.translation()).transpose();

            auto &bayes = state->scene.get_or_emplace<coherent_point_drift_bayes>(event.source_id);
            if(reg.errors.empty()){
                bayes.init(Y, X);
            }
            bayes(bayes.P, Y, X, state->config.parallel_grain_size);
            auto cpd_vector = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_vectors");
            auto cpd_positions = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_bayes_position");
            Map(cpd_vector) = (bayes.V) * source_model.linear();
            Map(cpd_positions) = (source_model.inverse() * bayes.U.transpose().colwise().homogeneous()).transpose();
            delta = Translation(bayes.t) * Rotation(MatrixS<3, 3>(bayes.R)) * Scaling(VectorS<3>::Constant(bayes.s));
            reg.errors.push_back(bayes.sigma_squared);
            cpd_vector.set_dirty();
            cpd_positions.set_dirty();
            break;
        }
        case RegistrationMethod::coherent_point_drift_test : {
            MatrixS<-1, 3> Y = ((source_model.linear() * MapConst(source_positions).transpose()).colwise() +
                                source_model.translation()).transpose();
            MatrixS<-1, 3> X = ((target_model.linear() * MapConst(target_positions).transpose()).colwise() +
                                target_model.translation()).transpose();

            auto &nonrigid_test = state->scene.get_or_emplace<coherent_point_drift_test>(event.source_id);
            if(reg.errors.empty()){
                nonrigid_test.init(Y, X);
            }
            nonrigid_test(nonrigid_test.P, Y, X, state->config.parallel_grain_size);
            auto cpd_vector = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_test_vectors");
            auto cpd_positions = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_test_position");
            Map(cpd_vector) = (nonrigid_test.V) * source_model.linear();
            Map(cpd_positions) = (source_model.inverse() * nonrigid_test.U.transpose().colwise().homogeneous()).transpose();
            delta = Translation(nonrigid_test.t) * Rotation(MatrixS<3, 3>(nonrigid_test.R)) * Scaling(VectorS<3>::Constant(nonrigid_test.s));
            reg.errors.push_back(nonrigid_test.sigma_squared);
            cpd_vector.set_dirty();
            cpd_positions.set_dirty();
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
    if (!state->scene.has<registration>(event.source_id)) {
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
    if (!state->scene.has<registration>(event.source_id))return;

    auto &reg = state->scene.get<registration>(event.source_id);
    reg.errors.clear();

    state->scene.remove_if_exists<coherent_point_drift_rigid>(event.source_id);
    state->scene.remove_if_exists<coherent_point_drift_affine>(event.source_id);
    state->scene.remove_if_exists<coherent_point_drift_nonrigid>(event.source_id);
    state->scene.remove_if_exists<coherent_point_drift_bayes>(event.source_id);
    state->scene.remove_if_exists<coherent_point_drift_test>(event.source_id);
}

}