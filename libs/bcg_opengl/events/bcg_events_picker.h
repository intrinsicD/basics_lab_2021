//
// Created by alex on 29.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_PICKER_H
#define BCG_GRAPHICS_BCG_EVENTS_PICKER_H

#include "entt/entt.hpp"

namespace bcg::event::picker{
namespace enable{
struct point{
    entt::entity id;
};
struct vertex{
    entt::entity id;
};
struct edge{
    entt::entity id;
};
struct face{
    entt::entity id;
};
}
struct disable{

};
namespace pick{
struct point{
    entt::entity id;
};
struct vertex{
    entt::entity id;
};
struct edge{
    entt::entity id;
};
struct face{
    entt::entity id;
};
}
}

#endif //BCG_GRAPHICS_BCG_EVENTS_PICKER_H
