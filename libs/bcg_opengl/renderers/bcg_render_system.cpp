//
// Created by alex on 28.10.20.
//

#include "bcg_render_system.h"
#include "points_renderer/bcg_points_renderer.h"
#include "mesh_renderer/bcg_mesh_renderer.h"
#include "picking_renderer/bcg_picking_renderer.h"

namespace bcg {

render_system::render_system(viewer_state *state) : system("render_system", state) {
    renderers["point_renderer"] = std::make_unique<points_renderer>(state);
    renderers["mesh_renderer"] = std::make_unique<mesh_renderer>(state);
    renderers["picking_renderer"] = std::make_unique<picking_renderer>(state);
}

}