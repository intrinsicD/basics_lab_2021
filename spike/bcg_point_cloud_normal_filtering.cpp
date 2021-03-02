//
// Created by alex on 02.03.21.
//

#include "bcg_point_cloud_normal_filtering.h"
#include "tbb/tbb.h"

namespace bcg{

std::vector<std::string> search_type_names(){
    std::vector<std::string> names(2);
    names[static_cast<int>(SearchType::knn)] = "knn";
    names[static_cast<int>(SearchType::radius)] = "radius";
    return names;
}

void point_cloud_normal_filtering(vertex_container *vertices, kdtree_property<bcg_scalar_t> &index, const pc_nf_parameters &params, size_t parallel_grain_size){
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto normals = vertices->get_or_add<VectorS<3>, 3>("v_normal");

    if(params.search_params.search_type == SearchType::knn){
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);

                        auto result = index.query_knn(positions[v], params.search_params.num_closest);
                        for(const auto vv : result.indices){
                            /* do stuff:
                             * positions[vv]: query neighbor position of vertex v
                             * normals[vv]: query neighbor normal of vertex v
                            */
                        }
                    }
                }
        );
    }else{
        // radius search index.query_radius(positions[v], params.search_params.radius);
    }

    positions.set_dirty(); // set property dirty so that it is uploaded to the gpu and displayed.
}

}