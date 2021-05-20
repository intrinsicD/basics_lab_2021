//
// Created by alex on 20.05.21.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_FILTER_H
#define BCG_GRAPHICS_BCG_EVENTS_FILTER_H

#include "entt/entt.hpp"

namespace bcg::event{

struct filter{
    struct threshold{
        entt::entity id;
        std::string property_name;
        bcg_scalar_t threshold;
    };
    struct correspondences{
        struct distance {
            entt::entity source_id;
            entt::entity target_id;
            bcg_scalar_t threshold;
        };
        struct normal_angle{
            entt::entity source_id;
            entt::entity target_id;
            bcg_scalar_t threshold;
        };
        struct mutual{
            entt::entity source_id;
            entt::entity target_id;
        };
    };
};

}

#endif //BCG_GRAPHICS_BCG_EVENTS_FILTER_H
