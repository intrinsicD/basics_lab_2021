//
// Created by alex on 04.11.20.
//

#include <iostream>

#include "bcg_loading_system.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_library/geometry/mesh/bcg_meshio.h"
#include "bcg_library/geometry/point_cloud/bcg_point_cloudio.h"
#include "components/bcg_component_loading_backup.h"

namespace bcg {

loading_system::loading_system(viewer_state *state) : system("loading_system", state) {
    state->dispatcher.sink<event::internal::file_drop>().connect<&loading_system::on_file_drop>(this);
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
                state->dispatcher.trigger<event::point_cloud::setup>(id, filename);
            }
        } else {
            id = state->scene.create();
            state->scene().emplace<halfedge_mesh>(id, mesh);
            std::cout << "read successful " << filename << "\n";
            state->dispatcher.trigger<event::mesh::setup>(id, filename);
        }

        if (state->scene.valid(id)) {
            auto view = state->scene().view<aligned_box3>();
            state->scene.aabb.set_centered_form(VectorS<3>::Zero(), VectorS<3>::Zero());
            for (const auto &id : view) {
                state->scene.aabb = state->scene.aabb.merge(view.get<aligned_box3>(id));
            }

            bcg_scalar_t scale = 1.0 / state->scene.aabb.halfextent().maxCoeff();
            state->scene.ws_model = Scaling(scale, scale, scale);
        }
    }
}

}