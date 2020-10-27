#include <iostream>

#include "bcg_opengl/bcg_viewer.h"
#include "bcg_opengl/bcg_imgui.h"

int main() {
    using namespace bcg;
    viewer viewer;

    viewer.state.gui.left.show = [](viewer_state *state, gui_element *self) {
        draw_label(&state->window, "mouse left", std::to_string(state->mouse.left));
        draw_coloredit(&state->window, "background", state->colors.background);
        return true;
    };

    viewer.state.gui.right.show = [](viewer_state *state, gui_element *self) {
        draw_label(&state->window, "mouse right", std::to_string(state->mouse.right));
        return true;
    };
    viewer.run();
    return 0;
}
