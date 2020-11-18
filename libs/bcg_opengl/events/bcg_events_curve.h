//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_CURVE_H
#define BCG_GRAPHICS_BCG_EVENTS_CURVE_H

#include <string>

namespace bcg{
    enum curve_mode{
        disabled,
        drawing_2d,
        drawing_3d
    };
}
namespace bcg::event::curve{
struct disable_drawing{

};
struct enable_2d_drawing{

};
struct enable_3d_drawing{

};
struct setup_curve{
    entt::entity id;
    std::string filename;
};
struct setup{
    entt::entity id;
};
struct make{

};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_CURVE_H
