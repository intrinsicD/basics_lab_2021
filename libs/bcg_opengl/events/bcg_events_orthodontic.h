//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_ORTHODONTIC_H
#define BCG_GRAPHICS_BCG_EVENTS_ORTHODONTIC_H

#include "entt/entt.hpp"
#include "orthodontic/bcg_jaw_component.h"
#include "math/vector/bcg_vector.h"

namespace bcg {

namespace event::orthodontic {

struct set_jaw_component{
    entt::entity id;
};

struct set_tooth_component{
    entt::entity id;
    int fdi_number;
    JawAge age;
};

struct consume_next_left_click{
    entt::entity id;
    int fid_number;
};
struct escape_consume_next_left_click{

};
struct reset_teeth_colors{

};

struct set_tooth_color{
    entt::entity id;
    Vector<float, 3> color;
};

struct render_gui{

};

}

}

#endif //BCG_GRAPHICS_BCG_EVENTS_ORTHODONTIC_H
