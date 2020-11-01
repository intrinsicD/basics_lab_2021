//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_H
#define BCG_GRAPHICS_BCG_EVENTS_H

#include <vector>
#include "entt/entt.hpp"

namespace bcg::event {
struct startup {

};
struct shutdown {

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
struct end_frame {

};
namespace gpu {
struct update_vertex_attributes {
    entt::entity id;
    std::vector<std::string> attributes_names;
};
struct update_edge_attributes {
    entt::entity id;
    std::vector<std::string> attributes_names;
};
struct update_face_attributes {
    entt::entity id;
    std::vector<std::string> attributes_names;
};
}
}

#endif //BCG_GRAPHICS_BCG_EVENTS_H
