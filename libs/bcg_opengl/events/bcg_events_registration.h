//
// Created by alex on 07.12.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_REGISTRATION_H
#define BCG_GRAPHICS_BCG_EVENTS_REGISTRATION_H

#include "registration/bcg_registration.h"

namespace bcg::event::registration{
struct align_step{
    entt::entity source_id, target_id;
    RegistrationMethod method;
    bool filter_distance = false, filter_normal_angle = false;
    bcg_scalar_t distance_threshold = 0, normal_threshold = 0;
    bool weight = false;
};
struct align_converge{
    entt::entity source_id, target_id;
    RegistrationMethod method;
    int max_iters;
};
struct reset{
    entt::entity source_id, target_id;
    RegistrationMethod method;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_REGISTRATION_H
