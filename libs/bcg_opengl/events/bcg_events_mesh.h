//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_MESH_H
#define BCG_GRAPHICS_BCG_EVENTS_MESH_H

#include "entt/entt.hpp"
#include "geometry/mesh/bcg_mesh_laplacian.h"

namespace bcg::event::mesh{
struct setup{
    entt::entity id;
    std::string filename;
};
struct make_triangle{

};
struct make_quad{

};
struct make_box{

};

namespace edge{
struct dihedral_angle{
    entt::entity id;
};
}
namespace face{
struct centers{
    entt::entity id;
};
struct normals{
    entt::entity id;
};
}

namespace vertex_normals{
enum Type{
    UNIFORM,
    AREA,
    ANGLE,
    AREAANGLE
};

struct uniform{
    entt::entity id;
};
struct area{
    entt::entity id;
};
struct angle{
    entt::entity id;
};
struct area_angle{
    entt::entity id;
};
}
struct boundary{
    entt::entity id;
};
struct vertex_convex_concave{
    entt::entity id;
};
struct features{
    entt::entity id;
    bool boundary = true;
    bool angle = true;
    bcg_scalar_t threshold_degrees = 45;
};
namespace laplacian{
struct build{
    entt::entity id;
    MeshLaplacianStiffness s_type;
    MeshLaplacianMass m_type;
    std::string edge_scaling_property_name;
};
}

namespace curvature{
    struct taubin{
        entt::entity id;
        int post_smoothing_steps = 3;
        bool two_ring_neighborhood = true;
    };
};

namespace subdivision{
struct catmull_clark{
    entt::entity id;
};
struct loop{
    entt::entity id;
};
struct sqrt3{
    entt::entity id;
};
}
namespace connected_components{
struct detect{
    entt::entity id;
};
struct split{
    entt::entity id;
};
}
}

#endif //BCG_GRAPHICS_BCG_EVENTS_MESH_H
