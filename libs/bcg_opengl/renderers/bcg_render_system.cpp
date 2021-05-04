//
// Created by alex on 28.10.20.
//

#include "bcg_render_system.h"
#include "points_renderer/bcg_points_renderer.h"
#include "mesh_renderer/bcg_mesh_renderer.h"
#include "picking_renderer/bcg_picking_renderer.h"
#include "graph_renderer/bcg_graph_renderer.h"
#include "vectorfield_renderer/bcg_vectorfield_renderer.h"
#include "curve_renderer/bcg_curve_renderer.h"
#include "deferred_renderer/bcg_deferred_renderer.h"
#include "point_rendering/hq_pbr_ufsi/bcg_hq_pbr_ufsi_renderer.h"

namespace bcg {

render_system::render_system(viewer_state *state) : system("render_system", state) {
    renderers["deferred_renderer"] = std::make_unique<deferred_renderer>(state);
    renderers["point_renderer"] = std::make_unique<points_renderer>(state);
    renderers["graph_renderer"] = std::make_unique<graph_renderer>(state);
    renderers["vectorfield_renderer"] = std::make_unique<vectorfield_renderer>(state);
    renderers["mesh_renderer"] = std::make_unique<mesh_renderer>(state);
    renderers["picking_renderer"] = std::make_unique<picking_renderer>(state);
    renderers["curve_renderer"] = std::make_unique<curve_renderer>(state);
    renderers["hq_pbr_ufsi_renderer"] = std::make_unique<hq_pbr_ufsi_renderer>(state);
}

}