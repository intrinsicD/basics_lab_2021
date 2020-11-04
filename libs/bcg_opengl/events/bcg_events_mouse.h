//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_MOUSE_H
#define BCG_GRAPHICS_BCG_EVENTS_MOUSE_H

namespace bcg::event::mouse{
struct button {
    int button, action;
};
struct motion {
    double x, y;
};
struct scroll {
    double value;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_MOUSE_H
