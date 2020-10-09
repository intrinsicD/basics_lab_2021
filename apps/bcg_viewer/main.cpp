#include <iostream>

#include "bcg_opengl/bcg_viewer.h"

int main() {
    using namespace bcg;
    viewer viewer;

    viewer.callbacks.widgets_cb = [](gui_window *win, const gui_input &input){
        draw_label(win, "mouse left", std::to_string(input.mouse_left));
    };
    viewer.run();
    return 0;
}
