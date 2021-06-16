//
// Created by alex on 15.06.21.
//

#include "bcg_gui_orthodontic_jaw.h"
#include "viewer/bcg_viewer_state.h"
#include "orthodontic/bcg_jaw_component.h"
#include "orthodontic/bcg_tooth_component.h"
#include "events/bcg_events_orthodontic.h"

namespace bcg {

void gui_orthodontic_jaw(viewer_state *state) {
    state->dispatcher.trigger<event::orthodontic::render_gui>();
}

}