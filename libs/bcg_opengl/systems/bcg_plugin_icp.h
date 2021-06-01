//
// Created by alex on 27.05.21.
//

#ifndef BCG_GRAPHICS_BCG_PLUGIN_ICP_H
#define BCG_GRAPHICS_BCG_PLUGIN_ICP_H

#include "bcg_systems.h"

namespace bcg {

struct plugin_icp : public system {
    explicit plugin_icp(viewer_state *state);

    void on_set_ids(const event::point_cloud::setup &event);



    /*1. which points participate (sampling)
     *  - all
     *  1.1 grid based sampling
     *      - first
     *      - last
     *      - random in voxel
     *      - closest to voxel center
     *      - medioid
     *2. which features participate (features)
     *3. how corresponences are computed (corrs estimation)
     *3.1 which corresponences are kept (corrs filtering)
     *3.2 how corresponences are weighted (corrs weighting)
     *4 which error is minimized and computed (error metric)
     */
};

}

#endif //BCG_GRAPHICS_BCG_PLUGIN_ICP_H
