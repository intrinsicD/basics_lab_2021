//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H
#define BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H

#include "entt/entt.hpp"

namespace bcg{

enum class GraphType{
    kdtree_knn = 0,
    kdtree_radius,
    octree_knn,
    octree_radius,
    __last__
};
static std::vector<std::string> graph_type_names(){
    std::vector<std::string> names(static_cast<int>(GraphType::__last__));
    names[static_cast<int>(GraphType::kdtree_knn)] = "kdtree_knn";
    names[static_cast<int>(GraphType::kdtree_radius)] = "kdtree_radius";
    names[static_cast<int>(GraphType::octree_knn)] = "octree_knn";
    names[static_cast<int>(GraphType::octree_radius)] = "octree_radius";
    return names;
}
}
namespace bcg::event::point_cloud{
struct setup{
    entt::entity id;
    std::string filename;
};

struct build_graph{
    entt::entity id;
    GraphType type;
    int num_closest = 0;
    bcg_scalar_t radius = 0;
};

}
#endif //BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H
