//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_GRAPH_H
#define BCG_GRAPHICS_BCG_EVENTS_GRAPH_H

#include "entt/entt.hpp"

namespace bcg{

}
namespace bcg::event::graph{
struct setup{
    entt::entity id;
    std::string filename;
};

namespace edge{
struct centers{
    entt::entity id;
};
}
namespace vertex{
namespace pca{
struct svd{
    entt::entity id;
    bool compute_mean = true;
};
struct weighted_svd{
    entt::entity id;
    bool compute_mean = true;
};
struct eig{
    entt::entity id;
    bool compute_mean = true;
};
struct weighted_eig{
    entt::entity id;
    bool compute_mean = true;
};
}
}
}

#endif //BCG_GRAPHICS_BCG_EVENTS_GRAPH_H
