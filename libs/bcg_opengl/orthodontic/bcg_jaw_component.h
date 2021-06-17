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
    permanent,
    __last__
};

std::vector<std::string> jaw_type_names();

std::vector<std::string> jaw_age_type_names();

struct jaw_component {
    JawType type = JawType::undefined;
    JawAge age = JawAge::undefined;
    std::unordered_map<int, entt::entity> teeth;
    unsigned int max_number_teeth;

    bool has_teeth(int fdi_number) const;

    std::vector<int> get_incisors() const;

    std::vector<int> get_canines() const;

    std::vector<int> get_premolars() const;

    std::vector<int> get_molars() const;

    std::vector<int> get_all() const;

    std::vector<int> get_undefined() const;
};

struct jaw_factory {
    jaw_component make_primary_maxilla();

    jaw_component make_primary_mandible();

    jaw_component make_permanent_maxilla();

    jaw_component make_permanent_mandible();
};

}

#endif //BCG_GRAPHICS_BCG_JAW_COMPONENT_H
