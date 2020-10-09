//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VOLUME_H
#define BCG_GRAPHICS_BCG_VOLUME_H

#include <vector>
#include "bcg_vector.h"

namespace bcg {

// Volume container.
template<typename T>
struct volume {
    // constructors
    volume();

    explicit volume(const vec3i &size, const T &value = {});

    volume(const vec3i &size, const T *value);

    // size
    bool empty() const;

    vec3i volsize() const;

    int width() const;

    int height() const;

    int depth() const;

    size_t count() const;

    void clear();

    void resize(const vec3i &size);

    void assign(const vec3i &size, const T &value);

    void shrink_to_fit();

    void swap(volume &other);

    // element access
    T &operator[](size_t i);

    const T &operator[](size_t i) const;

    T &operator[](const vec3i &ijk);

    const T &operator[](const vec3i &ijk) const;

    // data access
    T *data();

    const T *data() const;

    // iteration
    T *begin();

    T *end();

    const T *begin() const;

    const T *end() const;

private:
    // data
    vec3i extent = {0, 0, 0};
    std::vector<float> voxels = {};
};

// equality
template<typename T>
inline bool operator==(const volume<T> &a, const volume<T> &b);

template<typename T>
inline bool operator!=(const volume<T> &a, const volume<T> &b);

// swap
template<typename T>
inline void swap(volume<T> &a, volume<T> &b);



}
#endif //BCG_GRAPHICS_BCG_VOLUME_H
