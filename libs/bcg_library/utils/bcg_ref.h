//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_REF_H
#define BCG_GRAPHICS_BCG_REF_H

#include <memory>

namespace bcg {
template<typename T>
using ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr ref<T> CreateRef(Args &&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
}

#endif //BCG_GRAPHICS_BCG_REF_H
