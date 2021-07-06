//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_INTERNAL_H
#define BCG_GRAPHICS_BCG_EVENTS_INTERNAL_H

#include <vector>
#include <string>
#include "entt/entt.hpp"

namespace bcg::event::internal{
struct startup{

};
struct shutdown {

};
struct keyboard {
    int button, action;
};
struct file_drop {
    std::vector<std::string> filenames{};
};
struct update {

};
struct begin_frame {

};
struct render {

};
struct render_gui{

};
struct end_frame {

};
struct resize {
    int width, height;
};
struct uniform_point_size{
    double value;
};
struct set_uniform_point_size{
    double value;
};
struct destroy{
    entt::entity id;
};
struct camera_reset{

};
struct entity_setup{
    entt::entity id;
    bool scaling = true;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_INTERNAL_H
