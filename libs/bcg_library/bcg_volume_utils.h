//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VOLUME_UTILS_H
#define BCG_GRAPHICS_BCG_VOLUME_UTILS_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------
#include "bcg_image.h"
#include "bcg_volume.h"

namespace bcg {

// Evaluates a color image at a point `uv`.
float eval_volume(const image<float> &img, const vec3f &uvw, bool no_interpolation = false, bool clamp_to_edge = false);

// Loads/saves a 1 channel volume.
void load_volume(const std::string &filename, volume<float> &vol);

void save_volume(const std::string &filename, const volume<float> &vol);

// make a simple example volume
void make_voltest(volume<float> &vol, const vec3i &size, float scale = 10, float exponent = 6);

void make_volume_preset(volume<float> &vol, const std::string &type);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

// constructors
template<typename T>
inline volume<T>::volume() : extent{0, 0, 0}, voxels{} {}

template<typename T>
inline volume<T>::volume(const vec3i &size, const T &value)
        : extent{size}, voxels((size_t) size.x * (size_t) size.y * (size_t) size.z, value) {}

template<typename T>
inline volume<T>::volume(const vec3i &size, const T *value)
        : extent{size}, voxels(value, value + (size_t) size.x * (size_t) size.y * (size_t) size.z) {}

// size
template<typename T>
inline bool volume<T>::empty() const {
    return voxels.empty();
}

template<typename T>
inline vec3i volume<T>::volsize() const {
    return extent;
}

template<typename T>
inline int volume<T>::width() const {
    return extent.x;
}

template<typename T>
inline int volume<T>::height() const {
    return extent.y;
}

template<typename T>
inline int volume<T>::depth() const {
    return extent.z;
}

template<typename T>
inline size_t volume<T>::count() const {
    return voxels.size();
}

template<typename T>
inline void volume<T>::clear() {
    extent = {0, 0};
    voxels.clear();
}

template<typename T>
inline void volume<T>::resize(const vec3i &size) {
    if (size == extent) return;
    extent = size;
    voxels.resize((size_t) size.x * (size_t) size.y * (size_t) size.z);
}

template<typename T>
inline void volume<T>::assign(const vec3i &size, const T &value) {
    extent = size;
    voxels.assign((size_t) size.x * (size_t) size.y * (size_t) size.z, value);
}

template<typename T>
inline void volume<T>::shrink_to_fit() {
    voxels.shrink_to_fit();
}

template<typename T>
inline void volume<T>::swap(volume<T> &other) {
    std::swap(extent, other.extent);
    voxels.swap(other.voxels);
}

// element access
template<typename T>
inline T &volume<T>::operator[](size_t i) {
    return voxels[i];
}

template<typename T>
inline const T &volume<T>::operator[](size_t i) const {
    return voxels[i];
}

template<typename T>
inline T &volume<T>::operator[](const vec3i &ijk) {
    return voxels[ijk.z * extent.x * extent.y + ijk.y * extent.x + ijk.x];
}

template<typename T>
inline const T &volume<T>::operator[](const vec3i &ijk) const {
    return voxels[ijk.z * extent.x * extent.y + ijk.y * extent.x + ijk.x];
}

// data access
template<typename T>
inline T *volume<T>::data() {
    return voxels.data();
}

template<typename T>
inline const T *volume<T>::data() const {
    return voxels.data();
}

// iteration
template<typename T>
inline T *volume<T>::begin() {
    return voxels.data();
}

template<typename T>
inline T *volume<T>::end() {
    return voxels.data() + voxels.size();
}

template<typename T>
inline const T *volume<T>::begin() const {
    return voxels.data();
}

template<typename T>
inline const T *volume<T>::end() const {
    return voxels.data() + voxels.size();
}

// equality
template<typename T>
inline bool operator==(const volume<T> &a, const volume<T> &b) {
    return a.size() == b.size() && a.voxels == b.voxels;
}

template<typename T>
inline bool operator!=(const volume<T> &a, const volume<T> &b) {
    return a.size() != b.size() || a.voxels != b.voxels;
}

// swap
template<typename T>
inline void swap(volume<T> &a, volume<T> &b) {
    a.swap(b);
}

}

#endif //BCG_GRAPHICS_BCG_VOLUME_UTILS_H
