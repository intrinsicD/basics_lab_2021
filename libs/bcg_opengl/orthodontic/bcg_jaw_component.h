//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_JAW_COMPONENT_H
#define BCG_GRAPHICS_BCG_JAW_COMPONENT_H

#include "entt/entt.hpp"
#include "bcg_tooth_component.h"

namespace bcg {

enum class JawType {
    undefined,
    maxilla,
    mandible,
    __last__
};

enum class JawAge {
    undefined,
    primary,
    permament,
    __last__
};

std::vector<std::string> jaw_type_names();

std::vector<std::string> jaw_age_type_names();

struct jaw_component {
    JawType type = JawType::undefined;
    JawAge age = JawAge::undefined;
    std::unordered_map<int, entt::entity> teeth;
    unsigned int max_number_teeth;
    bool has_teeth(int fdi_number) {
        return teeth.find(fdi_number) != teeth.end();
    }
};

struct jaw_factory {
    jaw_component make_primary_maxilla();

    jaw_component make_primary_mandible();

    jaw_component make_permanent_maxilla();

    jaw_component make_permanent_mandible();
};

}

#endif //BCG_GRAPHICS_BCG_JAW_COMPONENT_H
