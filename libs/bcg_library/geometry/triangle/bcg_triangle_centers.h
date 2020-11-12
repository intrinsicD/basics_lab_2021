//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRIANGLE_CENTERS_H
#define BCG_GRAPHICS_BCG_TRIANGLE_CENTERS_H

#include "bcg_barycentric_coordinates.h"

namespace bcg {

template<int N>
inline VectorS<N> incenter(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    auto p = abc[0] + abc[1] + abc[2];
    return from_barycentric_coords(t, abc[0] / p, abc[1] / p, abc[2] / p);
}

template<int N>
inline VectorS<N> centroid(const triangle<N> &t) {
    auto w = 1.0 / 3.0;
    return from_barycentric_coords(t, w, w, w);
}

template<int N>
inline VectorS<N> circumcenter(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    auto aa = abc[0] * abc[0];
    auto bb = abc[1] * abc[1];
    auto cc = abc[2] * abc[2];
    return from_barycentric_coords(t, aa * (bb + cc - aa),
                                   bb * (cc + aa - bb),
                                   cc * (aa + bb - cc));
}

template<int N>
inline VectorS<N> orthocenter(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    auto aa = abc[0] * abc[0];
    auto bb = abc[1] * abc[1];
    auto cc = abc[2] * abc[2];
    auto apbmc = aa + bb - cc;
    auto ambpc = aa - bb + cc;
    auto bpcma = bb + cc - aa;
    return from_barycentric_coords(t, apbmc * ambpc,
                                   apbmc * bpcma,
                                   ambpc * bpcma);
}

template<int N>
inline VectorS<N> nine_point_center(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    auto aa = abc[0] * abc[0];
    auto bb = abc[1] * abc[1];
    auto cc = abc[2] * abc[2];
    auto bbmcc = bb - cc;
    auto ccmaa = cc - aa;
    auto aambb = aa - bb;
    return from_barycentric_coords(t, aa * (bb + cc) - bbmcc * bbmcc,
                                   bb * (cc + aa) - ccmaa + ccmaa,
                                   cc * (aa + bb) - aambb * aambb);
}

template<int N>
inline VectorS<N> symmedian_point(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    return from_barycentric_coords(t, abc[0] * abc[0],
                                   abc[1] * abc[1],
                                   abc[2] * abc[2]);
}

template<int N>
inline VectorS<N> gergonne_point(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    return from_barycentric_coords(t, 1.0 / (abc[1] + abc[2] - abc[0]),
                                   1.0 / (abc[2] + abc[0] - abc[1]),
                                   1.0 / (abc[0] + abc[1] - abc[2]));
}

template<int N>
inline VectorS<N> nagel_point(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    return from_barycentric_coords(t, abc[1] + abc[2] - abc[0],
                                   abc[2] + abc[0] - abc[1],
                                   abc[0] + abc[1] - abc[2]);
}

template<int N>
inline VectorS<N> mitten_point(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    return from_barycentric_coords(t, abc[0] * (abc[1] + abc[2] - abc[0]),
                                   abc[1] * (abc[2] + abc[0] - abc[1]),
                                   abc[2] * (abc[0] + abc[1] - abc[2]));
}

template<int N>
inline VectorS<N> spieker_point(const triangle<N> &t) {
    auto abc = t.edge_lengths();
    return from_barycentric_coords(t, abc[1] + abc[2],
                                   abc[2] + abc[0],
                                   abc[0] + abc[1]);
}


}

#endif //BCG_GRAPHICS_BCG_TRIANGLE_CENTERS_H
