//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_INTERNAL_H
#define BCG_GRAPHICS_BCG_EVENTS_INTERNAL_H

#include <vector>
#include <string>

namespace bcg::event::internal{
struct startup {

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
struct end_frame {

};
struct resize {
    int width, height;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_INTERNAL_H
