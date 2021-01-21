//
// Created by alex on 21.01.21.
//

#include "bcg_sampling_locally_optimal_projection.h"

namespace bcg{

std::vector<std::string> method_names(){
    std::vector<std::string> names(LopType::__last__);
    names[LopType::lop] = "lop";
    names[LopType::wlop] = "wlop";
    names[LopType::flop] = "flop";
    names[LopType::clop] = "clop";
    names[LopType::custom] = "custom";
    return names;
}


bcg_scalar_t compute_attraction_weight_point(bcg_scalar_t distance, bcg_scalar_t support_radius) const{
    return std::exp(-distance * distance / (support_radius * support_radius / 16));
}

bcg_scalar_t compute_attraction_weight_plane(bcg_scalar_t distance, bcg_scalar_t support_radius) const{
    return std::exp(-distance * distance / (2 * support_radius * support_radius));
}

bcg_scalar_t compute_attraction_weight_density(bcg_scalar_t distance, bcg_scalar_t support_radius) const{
    reutrn compute_attraction_weight_point(distance, support_radius);
}

bcg_scalar_t compute_repulsion_weight_lop(bcg_scalar_t distance) const{
    return 1.0 / (3 * distance * distance * distance);
}

bcg_scalar_t compute_repulsion_weight_wlop(bcg_scalar_t distance) const{
    return -r;
}

bcg_scalar_t compute_repulsion_weight_lop_derivative(bcg_scalar_t distance) const{
    return -1.0 / (distance * distance * distance * distance);
}

bcg_scalar_t compute_repulsion_weight_wlop_derivative(bcg_scalar_t distance) const{
    return -1.0;
}

void local_density_estimation(Vertices &ref_vertices, bcg_scalar_t attraction_radius, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size){
    auto ref_positions = ref_vertices.get<VectorS<3>, 3>("v_position");
    auto ref_density = ref_vertices.get_or_add<bcg_scalar_t, 1>("v_lop_density");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) ref_positions.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    ref_density[v] = 0;
                    auto result = ref_index.query_radius(ref_positions[v], attraction_radius);
                    for (size_t i = 0; i < result.indices.size(); ++i) {
                        ref_density[v] += compute_attraction_weight_density(result.distances[i], attraction_radius);
                    }
                }
            }
    );
}

void locally_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size){
    auto ref_positions = ref_vertices.get<VectorS<3>, 3>("v_position");
    auto ref_density = ref_vertices.get<bcg_scalar_t, 1>("v_lop_density");
    if(!ref_density){
        local_density_estimation(ref_vertices, attraction_radius, ref_index, parallel_grain_size);
        ref_density = ref_vertices.get<bcg_scalar_t, 1>("v_lop_density");
    }
    auto sample_positions = sample_vertices.get<VectorS<3>, 3>("v_position");
    if(!sample_positions){
        std::cerr << "please generate some subsampling before applying any locally optimal projection!\n";
        return;
    }
    auto sample_normals = sample_vertices.get<VectorS<3>, 3>("v_normal");
    auto alpha_samples = sample_vertices.get<bcg_scalar_t, 1>("v_alpha_sum_target");


}

void locally_weighted_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size){

}

void locally_feature_preserving_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size){

}

void locally_continuous_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size){

}

}