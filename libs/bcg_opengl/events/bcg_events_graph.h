//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_GRAPH_H
#define BCG_GRAPHICS_BCG_EVENTS_GRAPH_H

#include "entt/entt.hpp"

namespace bcg{
enum class PcaType{
    svd,
    weighted_svd,
    eig,
    weighted_eig
};
static std::vector<std::string> pca_type_names(){
    std::vector<std::string> names(static_cast<int>(GraphType::__last__));
    names[static_cast<int>(PcaType::svd)] = "svd";
    names[static_cast<int>(PcaType::weighted_svd)] = "weighted_svd";
    names[static_cast<int>(PcaType::eig)] = "eig";
    names[static_cast<int>(PcaType::weighted_eig)] = "weighted_eig";
    return names;
}
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
struct pca{
    entt::entity id;
    PcaType type;
    bool compute_mean = true;
};
}
}

#endif //BCG_GRAPHICS_BCG_EVENTS_GRAPH_H
