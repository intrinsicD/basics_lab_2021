//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_IMAGE_H
#define BCG_GRAPHICS_BCG_IMAGE_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include <string>
#include <utility>
#include <vector>
#include "bcg_math.h"

namespace bcg {

// Image container.
template<typename T>
struct image {
    // constructors
    image();

    explicit image(const vec2i &size, const T &value = {});

    image(const vec2i &size, const T *value);

    // size
    bool empty() const;

    vec2i imsize() const;

    int width() const;

    int height() const;

    size_t count() const;

    bool contains(const vec2i &ij) const;

    void clear();

    void resize(const vec2i &size);

    void assign(const vec2i &size, const T &value = {});

    void shrink_to_fit();

    void swap(image &other);

    // element access
    T &operator[](size_t i);

    const T &operator[](size_t i) const;

    T &operator[](const vec2i &ij);

    const T &operator[](const vec2i &ij) const;

    // data access
    T *data();

    const T *data() const;

    // iteration
    T *begin();

    T *end();

    const T *begin() const;

    const T *end() const;

    // [experimental] data access as std::vector --- will be replaced by views
    std::vector<T> &data_vector();

    const std::vector<T> &data_vector() const;

private:
    // data
    vec2i extent = {0, 0};
    std::vector<T> pixels = {};
};

// equality
template<typename T>
inline bool operator==(const image<T> &a, const image<T> &b);

template<typename T>
inline bool operator!=(const image<T> &a, const image<T> &b);

// swap
template<typename T>
inline void swap(image<T> &a, image<T> &b);

// Evaluates a color image at a point `uv`.
vec4f eval_image(const image<vec4f> &img, const vec2f &uv,
                 bool no_interpolation = false, bool clamp_to_edge = false);

vec4f eval_image(const image<vec4b> &img, const vec2f &uv,
                 bool as_linear = false, bool no_interpolation = false,
                 bool clamp_to_edge = false);

vec3f eval_image(const image<vec3f> &img, const vec2f &uv,
                 bool no_interpolation = false, bool clamp_to_edge = false);

vec3f eval_image(const image<vec3b> &img, const vec2f &uv,
                 bool as_linear = false, bool no_interpolation = false,
                 bool clamp_to_edge = false);


// Conversion from/to floats.
image<vec4f> byte_to_float(const image<vec4b> &bt);

image<vec4b> float_to_byte(const image<vec4f> &fl);

image<vec3f> byte_to_float(const image<vec3b> &bt);

image<vec3b> float_to_byte(const image<vec3f> &fl);

image<float> byte_to_float(const image<byte> &bt);

image<byte> float_to_byte(const image<float> &fl);

image<vec4f> ushort_to_float(const image<vec4s> &bt);

image<vec4s> float_to_ushort(const image<vec4f> &fl);

// Conversion between linear and gamma-encoded images.
image<vec4f> srgb_to_rgb(const image<vec4f> &srgb);

image<vec4f> rgb_to_srgb(const image<vec4f> &rgb);

image<vec4f> srgb_to_rgb(const image<vec4b> &srgb);

image<vec4b> rgb_to_srgbb(const image<vec4f> &rgb);

image<vec3f> srgb_to_rgb(const image<vec3f> &srgb);

image<vec3f> rgb_to_srgb(const image<vec3f> &rgb);

image<vec3f> srgb_to_rgb(const image<vec3b> &srgb);

image<vec3b> rgb_to_srgbb(const image<vec3f> &rgb);

image<float> srgb_to_rgb(const image<float> &srgb);

image<float> rgb_to_srgb(const image<float> &rgb);

image<float> srgb_to_rgb(const image<byte> &srgb);

image<byte> rgb_to_srgbb(const image<float> &rgb);

// Conversion between number of channels.
image<vec4f> rgb_to_rgba(const image<vec3f> &rgb);

image<vec3f> rgba_to_rgb(const image<vec4f> &rgba);

image<vec4b> rgb_to_rgba(const image<vec3b> &rgb);

image<vec3b> rgba_to_rgb(const image<vec4b> &rgba);

image<vec4f> red_to_rgba(const image<float> &red);

image<float> rgba_to_red(const image<vec4f> &rgba);

image<vec4b> red_to_rgba(const image<byte> &red);

image<byte> rgba_to_red(const image<vec4b> &rgba);

image<vec4f> gray_to_rgba(const image<float> &gray);

image<float> rgba_to_gray(const image<vec4f> &rgba);

image<vec4b> gray_to_rgba(const image<byte> &gray);

image<byte> rgba_to_gray(const image<vec4b> &rgba);

// Apply tone mapping
image<vec4f> tonemap_image(const image<vec4f> &hdr, float exposure,
                           bool filmic = false, bool srgb = true);

image<vec4b> tonemap_imageb(const image<vec4f> &hdr, float exposure,
                            bool filmic = false, bool srgb = true);

// Apply tone mapping using multithreading for speed
void tonemap_image_mt(image<vec4f> &ldr, const image<vec4f> &hdr,
                      float exposure, bool filmic = false, bool srgb = true);

// minimal color grading
struct colorgrade_params {
    float exposure = 0;
    vec3f tint = {1, 1, 1};
    float lincontrast = 0.5;
    float logcontrast = 0.5;
    float linsaturation = 0.5;
    bool filmic = false;
    bool srgb = true;
    float contrast = 0.5;
    float saturation = 0.5;
    float shadows = 0.5;
    float midtones = 0.5;
    float highlights = 0.5;
    vec3f shadows_color = {1, 1, 1};
    vec3f midtones_color = {1, 1, 1};
    vec3f highlights_color = {1, 1, 1};
};

// Apply color grading from a linear or srgb color to an srgb color.
vec3f colorgrade(const vec3f &rgb, bool linear, const colorgrade_params &params);

vec4f colorgrade(const vec4f &rgb, bool linear, const colorgrade_params &params);

// Color grade a linear or srgb image to an srgb image.
image<vec4f> colorgrade_image(const image<vec4f> &img, bool linear, const colorgrade_params &params);

// Color grade a linear or srgb image to an srgb image.
// Uses multithreading for speed.
void colorgrade_image_mt(image<vec4f> &corrected, const image<vec4f> &img,
                         bool linear, const colorgrade_params &params);

// determine white balance colors
vec3f compute_white_balance(const image<vec4f> &img);

// Resize an image.
image<vec4f> resize_image(const image<vec4f> &img, const vec2i &size);

image<vec4b> resize_image(const image<vec4b> &img, const vec2i &size);

// Compute the difference between two images
image<vec4f> image_difference(const image<vec4f> &a, const image<vec4f> &b, bool disply_diff);

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
inline image<T>::image() : extent{0, 0}, pixels{} {}

template<typename T>
inline image<T>::image(const vec2i &size, const T &value)
        : extent{size}, pixels((size_t) size.x * (size_t) size.y, value) {}

template<typename T>
inline image<T>::image(const vec2i &size, const T *value)
        : extent{size}, pixels(value, value + (size_t) size.x * (size_t) size.y) {}

// size
template<typename T>
inline bool image<T>::empty() const {
    return pixels.empty();
}

template<typename T>
inline vec2i image<T>::imsize() const {
    return extent;
}

template<typename T>
inline int image<T>::width() const {
    return extent.x;
}

template<typename T>
inline int image<T>::height() const {
    return extent.y;
}

template<typename T>
inline size_t image<T>::count() const {
    return pixels.size();
}

template<typename T>
inline bool image<T>::contains(const vec2i &ij) const {
    return ij.x > 0 && ij.x < extent.x && ij.y > 0 && ij.y < extent.y;
}

template<typename T>
inline void image<T>::clear() {
    extent = {0, 0};
    pixels.clear();
}

template<typename T>
inline void image<T>::resize(const vec2i &size) {
    if (size == extent) return;
    extent = size;
    pixels.resize((size_t) size.x * (size_t) size.y);
}

template<typename T>
inline void image<T>::assign(const vec2i &size, const T &value) {
    extent = size;
    pixels.assign((size_t) size.x * (size_t) size.y, value);
}

template<typename T>
inline void image<T>::shrink_to_fit() {
    pixels.shrink_to_fit();
}

template<typename T>
inline void image<T>::swap(image<T> &other) {
    std::swap(extent, other.extent);
    pixels.swap(other.pixels);
}

// element access
template<typename T>
inline T &image<T>::operator[](size_t i) {
    return pixels[i];
}

template<typename T>
inline const T &image<T>::operator[](size_t i) const {
    return pixels[i];
}

template<typename T>
inline T &image<T>::operator[](const vec2i &ij) {
    return pixels[ij.y * extent.x + ij.x];
}

template<typename T>
inline const T &image<T>::operator[](const vec2i &ij) const {
    return pixels[ij.y * extent.x + ij.x];
}

// data access
template<typename T>
inline T *image<T>::data() {
    return pixels.data();
}

template<typename T>
inline const T *image<T>::data() const {
    return pixels.data();
}

// iteration
template<typename T>
inline T *image<T>::begin() {
    return pixels.data();
}

template<typename T>
inline T *image<T>::end() {
    return pixels.data() + pixels.size();
}

template<typename T>
inline const T *image<T>::begin() const {
    return pixels.data();
}

template<typename T>
inline const T *image<T>::end() const {
    return pixels.data() + pixels.size();
}

// data access as std::vector
template<typename T>
inline std::vector <T> &image<T>::data_vector() {
    return pixels;
}

template<typename T>
inline const std::vector <T> &image<T>::data_vector() const {
    return pixels;
}

// equality
template<typename T>
inline bool operator==(const image<T> &a, const image<T> &b) {
    return a.imsize() == b.imsize() && a.pixels == b.pixels;
}

template<typename T>
inline bool operator!=(const image<T> &a, const image<T> &b) {
    return a.imsize() != b.imsize() || a.pixels != b.pixels;
}

// swap
template<typename T>
inline void swap(image<T> &a, image<T> &b) {
    a.swap(b);
}
}

#endif //BCG_GRAPHICS_BCG_IMAGE_H
