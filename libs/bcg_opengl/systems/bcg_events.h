//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_H
#define BCG_GRAPHICS_BCG_EVENTS_H

#include <vector>

namespace bcg::event{
struct startup{

};
struct shutdown{

};
struct file_drop{
    std::vector<std::string> filenames{};
};
struct update{

};

}

#endif //BCG_GRAPHICS_BCG_EVENTS_H
