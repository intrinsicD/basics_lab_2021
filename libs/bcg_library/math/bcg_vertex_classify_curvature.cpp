//
// Created by alex on 01.12.20.
//

#include "bcg_vertex_classify_curvature.h"
#include "tbb/tbb.h"

namespace bcg {

void vertex_classify_curvature(vertex_container *vertices, property<bcg_scalar_t, 1> min_curvature,
                               property<bcg_scalar_t, 1> max_curvature, size_t parallel_grain_size) {
    auto spherical = vertices->get_or_add<bool, 1>("v_curv_spherical");
    auto planar = vertices->get_or_add<bool, 1>("v_curv_planar");
    auto elliptic = vertices->get_or_add<bool, 1>("v_curv_elliptic");
    auto parabolic = vertices->get_or_add<bool, 1>("v_curv_parabolic");
    auto hyperbolic = vertices->get_or_add<bool, 1>("v_curv_hyperbolic");
    auto anisotropic = vertices->get_or_add<bool, 1>("v_curv_anisotropic");
    auto isotropic = vertices->get_or_add<bool, 1>("v_curv_isotropic");
    auto cassification = vertices->get_or_add<bcg_scalar_t, 1>("v_curv_calssification");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    bcg_scalar_t kmin = min_curvature[v];
                    bcg_scalar_t kmax = max_curvature[v];
                    bcg_scalar_t gauss = kmin * kmax;
                    elliptic[v] = (gauss > 0 && kmin < 0 && kmax < 0);
                    spherical[v] = (gauss > 0 && !elliptic[v]);
                    hyperbolic[v] = (gauss < 0 && kmin < 0 && kmax > 0);
                    parabolic[v] = (kmin < 0 && (!elliptic[v] && !hyperbolic[v] && !spherical[v]));
                    anisotropic[v] = elliptic[v] || parabolic[v] || hyperbolic[v];
                    planar[v] = (!anisotropic[v] && !spherical[v]);
                    isotropic[v] = spherical[v] || planar[v];
                    cassification[v] = (spherical[v] ? 1 : (planar[v] ? 2 : (elliptic[v] ? 3 : (parabolic[v] ? 4
                                                                                                             : (hyperbolic[v]
                                                                                                                ? 5
                                                                                                                : 0)))));
                }
            }
    );
    spherical.set_dirty();
    elliptic.set_dirty();
    parabolic.set_dirty();
    planar.set_dirty();
    hyperbolic.set_dirty();
    anisotropic.set_dirty();
    isotropic.set_dirty();
    cassification.set_dirty();
}

}