//
// Created by alex on 04.11.20.
//

#include <iostream>

#include "bcg_loading_system.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_library/geometry/mesh/bcg_meshio.h"
#include "bcg_library/geometry/point_cloud/bcg_point_cloudio.h"
#include "components/bcg_component_loading_backup.h"
#include "bcg_property_map_eigen.h"

namespace bcg {

loading_system::loading_system(viewer_state *state) : system("loading_system", state) {
    state->dispatcher.sink<event::internal::file_drop>().connect<&loading_system::on_file_drop>(this);
    state->dispatcher.sink<event::internal::entity_setup>().connect<&loading_system::on_entity_setup>(this);
}

void loading_system::on_file_drop(const event::internal::file_drop &event) {
    for (const auto &filename : event.filenames) {
        meshio mesh_io(filename, meshio_flags());
        halfedge_mesh mesh;
        mesh_io.read(mesh);
        entt::entity id = entt::null;
        if (mesh.empty() || mesh.edges.size() == 0 || mesh.faces.size() == 0) {
            point_cloudio point_cloud_io(filename, point_cloudio_flags());
            point_cloud pc;
            point_cloud_io.read(pc);
            if (pc.empty()) {
                std::cout << "failed to read " << filename << ". Format unknown.\n";
                return;
            } else {
                id = state->scene.create();
                state->scene().emplace<point_cloud>(id, pc);
                std::cout << "read successful " << filename << "\n";
                state->dispatcher.trigger<event::internal::entity_setup>(id);
                state->dispatcher.trigger<event::point_cloud::setup>(id, filename);
            }
        } else {
            id = state->scene.create();
            state->scene().emplace<halfedge_mesh>(id, mesh);
            std::cout << "read successful " << filename << "\n";
            state->dispatcher.trigger<event::internal::entity_setup>(id);
            state->dispatcher.trigger<event::mesh::setup>(id, filename);
        }

    }
}

void loading_system::on_entity_setup(const event::internal::entity_setup &event){
    auto id = event.id;
    if (state->scene.valid(id)) {
        auto *vertices = state->get_vertices(id);
        if(vertices){
            auto positions = vertices->get<VectorS<3>, 3>("v_position");
            auto &backup = state->scene().emplace<loading_backup>(id);
            backup.aabb = aligned_box3(positions.vector());
            bcg_scalar_t scale = backup.aabb.halfextent().maxCoeff();
            backup.scale = scale;
            if(state->scene.scale == 1){
                state->scene.scale = scale;
            }
            backup.loading_model.linear() = Scaling(scale, scale, scale);
            backup.loading_model.translation() = backup.aabb.center();
            backup.ws_model.setIdentity();
            if(event.scaling){
                Map(positions) = (MapConst(positions).rowwise() - backup.aabb.center().transpose()).array() / state->scene.scale;
            }
            state->dispatcher.trigger<event::transform::world_space::set>(id, backup.ws_model);
            state->dispatcher.trigger<event::aligned_box::set>(id, aligned_box3(positions.vector()));
        }
        state->picker.entity_id = id;
        state->dispatcher.trigger<event::internal::camera_reset>();
    }
}

}