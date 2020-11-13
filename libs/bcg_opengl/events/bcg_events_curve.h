//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_CURVE_H
#define BCG_GRAPHICS_BCG_EVENTS_CURVE_H

#include <string>

namespace bcg{
    enum curve_mode{
        DISABLED,
        PLANE_MODE,
        SURFACE_MODE
    };
}
namespace bcg::event::curve{
struct disable_curve_mode{

};
struct enable_plane_curve_mode{

};
struct enable_surface_curve_mode{

};
struct setup_curve{
    entt::entity id;
    std::string filename;
};
struct new_curve{

};
struct delete_curve{

};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_CURVE_H
