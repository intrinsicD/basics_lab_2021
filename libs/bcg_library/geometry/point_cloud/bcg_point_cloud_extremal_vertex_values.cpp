//
// Created by alex on 29.03.21.
//

#include "bcg_point_cloud_extremal_vertex_values.h"
#include "tbb/tbb.h"

namespace bcg{

std::vector<std::string> vertex_extremal_type_names(){
    std::vector<std::string> names(static_cast<int>(VertexExtremalType::__last__));
    names[static_cast<int>(VertexExtremalType::minimum)] = "minimum";
    names[static_cast<int>(VertexExtremalType::maximum)] = "maximum";
    names[static_cast<int>(VertexExtremalType::extremal)] = "extremal";
    return names;
}

void point_cloud_extremal_vertex_values_radius(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                               const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size){
    if(!property) return;
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto extremal_values = vertices->get_or_add<bool, 1>("v_extremal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_radius(positions[i], radius);
                    extremal_values[i] = true;
                    for(const auto &idx : result.indices){
                        if(std::abs(property[idx]) >= std::abs(property[i])){
                            extremal_values[i] = false;
                            break;
                        }
                    }
                }
            }
    );
    extremal_values.set_dirty();
}

void point_cloud_extremal_vertex_values_knn(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                            const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size){
    if(!property) return;
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto extremal_values = vertices->get_or_add<bool, 1>("v_extremal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_knn(positions[i], num_closest);
                    extremal_values[i] = true;
                    for(const auto &idx : result.indices){
                        if(std::abs(property[idx]) >= std::abs(property[i])){
                            extremal_values[i] = false;
                            break;
                        }
                    }
                }
            }
    );
    extremal_values.set_dirty();
}

void point_cloud_minimal_vertex_values_radius(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                              const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size){
    if(!property) return;
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto minimal_values = vertices->get_or_add<bool, 1>("v_minimal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_radius(positions[i], radius);
                    minimal_values[i] = true;
                    for(const auto &idx : result.indices){
                        if(property[idx] <= property[i]){
                            minimal_values[i] = false;
                            break;
                        }
                    }
                }
            }
    );
    minimal_values.set_dirty();
}

void point_cloud_minimal_vertex_values_knn(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                           const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size){
    if(!property) return;
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto minimal_values = vertices->get_or_add<bool, 1>("v_minimal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_knn(positions[i], num_closest);
                    minimal_values[i] = true;
                    for(const auto &idx : result.indices){
                        if(property[idx] <= property[i]){
                            minimal_values[i] = false;
                            break;
                        }
                    }
                }
            }
    );
    minimal_values.set_dirty();
}

void point_cloud_maximal_vertex_values_radius(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                              const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size){
    if(!property) return;
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto maximal_values = vertices->get_or_add<bool, 1>("v_maximal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_radius(positions[i], radius);
                    maximal_values[i] = true;
                    for(const auto &idx : result.indices){
                        if(property[idx] >= property[i]){
                            maximal_values[i] = false;
                            break;
                        }
                    }
                }
            }
    );
    maximal_values.set_dirty();
}

void point_cloud_maximal_vertex_values_knn(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                           const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size){
    if(!property) return;
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto maximal_values = vertices->get_or_add<bool, 1>("v_maximal_values");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_knn(positions[i], num_closest);
                    maximal_values[i] = true;
                    for(const auto &idx : result.indices){
                        if(property[idx] >= property[i]){
                            maximal_values[i] = false;
                            break;
                        }
                    }
                }
            }
    );
    maximal_values.set_dirty();
}

}