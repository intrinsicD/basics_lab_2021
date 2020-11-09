//
// Created by alex on 09.11.20.
//

#ifndef BCG_GRAPHICS_BCG_ATTRIBUTE_H
#define BCG_GRAPHICS_BCG_ATTRIBUTE_H

#include <string>

namespace bcg{

struct attribute{
    std::string buffer_name, property_name;
    int index = 0;
    bool enable = false;
    bool update = false;
};

}

#endif //BCG_GRAPHICS_BCG_ATTRIBUTE_H
