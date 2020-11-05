//
// Created by alex on 28.10.20.
//

#include "bcg_render_system.h"
#include "points_renderer/bcg_points_renderer.h"
#include "test_renderer/bcg_test_renderer.h"

namespace bcg {

render_system::render_system(viewer_state *state) : system("render_system", state) {
    renderers["point_renderer"] = std::make_unique<points_renderer>(state);
    //renderers["test_renderer"] = std::make_unique<test_renderer>(state);
}

}