//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_UI_UTILITY_H
#define BCG_GRAPHICS_BCG_UI_UTILITY_H

// -----------------------------------------------------------------------------
// USER INTERFACE UTILITIES
// -----------------------------------------------------------------------------
namespace bcg {

// Computes the image uv coordinates corresponding to the view parameters.
// Returns negative coordinates if out of the image.
inline vec2i image_coords(const vec2f &mouse_pos, const vec2f &center,
                          float scale, const vec2i &txt_size);

// Center image and autofit. Returns center and scale.
inline std::pair<vec2f, float> camera_imview(const vec2f &center, float scale,
                                             const vec2i &imsize, const vec2i &winsize, bool zoom_to_fit);

// Turntable for UI navigation. Returns from and to.
inline std::pair <vec3f, vec3f> camera_turntable(const vec3f &from, const vec3f &to,
                                                 const vec3f &up, const vec2f &rotate, float dolly, const vec2f &pan);

// Turntable for UI navigation. Returns frame and focus.
inline std::pair<frame3f, float> camera_turntable(const frame3f &frame, float focus,
                                                  const vec2f &rotate, float dolly, const vec2f &pan);

// FPS camera for UI navigation for a frame parametrization. Returns frame.
inline frame3f camera_fpscam(const frame3f &frame, const vec3f &transl, const vec2f &rotate);

// Computes the image uv coordinates corresponding to the view parameters.
// Returns negative coordinates if out of the image.
[[deprecated]] inline vec2i get_image_coords(const vec2f &mouse_pos,
                                             const vec2f &center, float scale, const vec2i &txt_size);

// Center image and autofit.
[[deprecated]] inline void update_imview(vec2f &center, float &scale,
                                         const vec2i &imsize, const vec2i &winsize, bool zoom_to_fit);

// Turntable for UI navigation.
[[deprecated]] inline void update_turntable(vec3f &from, vec3f &to,
                                            const vec3f &up, const vec2f &rotate, float dolly, const vec2f &pan);

// Turntable for UI navigation.
[[deprecated]] inline void update_turntable(frame3f &frame, float &focus,
                                            const vec2f &rotate, float dolly, const vec2f &pan);

// FPS camera for UI navigation for a frame parametrization.
[[deprecated]] inline void update_fpscam(
        frame3f &frame, const vec3f &transl, const vec2f &rotate);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

// Computes the image uv coordinates corresponding to the view parameters.
// Returns negative coordinates if out of the image.
inline vec2i image_coords(const vec2f &mouse_pos, const vec2f &center,
                          float scale, const vec2i &txt_size) {
    auto xyf = (mouse_pos - center) / scale;
    return vec2i{(int) round(xyf.x + txt_size.x / 2.0f),
                 (int) round(xyf.y + txt_size.y / 2.0f)};
}

// Center image and autofit. Returns center and scale.
inline std::pair<vec2f, float> camera_imview(const vec2f &center, float scale,
                                             const vec2i &imsize, const vec2i &winsize, bool zoom_to_fit) {
    if (zoom_to_fit) {
        return {{(float) winsize.x / 2, (float) winsize.y / 2},
                min(winsize.x / (float) imsize.x, winsize.y / (float) imsize.y)};
    } else {
        return {
                {(winsize.x >= imsize.x * scale) ? (float) winsize.x / 2 : center.x,
                 (winsize.y >= imsize.y * scale) ? (float) winsize.y / 2 : center.y},
                scale};
    }
}

// Turntable for UI navigation. Returns from and to.
inline std::pair <vec3f, vec3f> camera_turntable(const vec3f &from_, const vec3f &to_,
                                                 const vec3f &up, const vec2f &rotate, float dolly, const vec2f &pan) {
    // copy values
    auto from = from_, to = to_;

    // rotate if necessary
    if (rotate != zero2f) {
        auto z = normalize(to - from);
        auto lz = length(to - from);
        auto phi = atan2(z.z, z.x) + rotate.x;
        auto theta = acos(z.y) + rotate.y;
        theta = clamp(theta, 0.001f, pif - 0.001f);
        auto nz = vec3f{sin(theta) * cos(phi) * lz, cos(theta) * lz,
                        sin(theta) * sin(phi) * lz};
        from = to - nz;
    }

    // dolly if necessary
    if (dolly != 0) {
        auto z = normalize(to - from);
        auto lz = max(0.001f, length(to - from) * (1 + dolly));
        z *= lz;
        from = to - z;
    }

    // pan if necessary
    if (pan != zero2f) {
        auto z = normalize(to - from);
        auto x = normalize(cross(up, z));
        auto y = normalize(cross(z, x));
        auto t = vec3f{pan.x * x.x + pan.y * y.x, pan.x * x.y + pan.y * y.y,
                       pan.x * x.z + pan.y * y.z};
        from += t;
        to += t;
    }

    // done
    return {from, to};
}

// Turntable for UI navigation. Returns frame and focus.
inline std::pair<frame3f, float> camera_turntable(const frame3f &frame_, float focus,
                                                  const vec2f &rotate, float dolly, const vec2f &pan) {
    // copy values
    auto frame = frame_;

    // rotate if necessary
    if (rotate != zero2f) {
        auto phi = atan2(frame.z.z, frame.z.x) + rotate.x;
        auto theta = acos(frame.z.y) + rotate.y;
        theta = clamp(theta, 0.001f, pif - 0.001f);
        auto new_z = vec3f{
                sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi)};
        auto new_center = frame.o - frame.z * focus;
        auto new_o = new_center + new_z * focus;
        frame = lookat_frame(new_o, new_center, {0, 1, 0});
        focus = length(new_o - new_center);
    }

    // pan if necessary
    if (dolly != 0) {
        auto c = frame.o - frame.z * focus;
        focus = max(focus * (1 + dolly), 0.001f);
        frame.o = c + frame.z * focus;
    }

    // pan if necessary
    if (pan != zero2f) {
        frame.o += frame.x * pan.x + frame.y * pan.y;
    }

    // done
    return {frame, focus};
}

// FPS camera for UI navigation for a frame parametrization. Returns frame.
inline frame3f camera_fpscam(
        const frame3f &frame, const vec3f &transl, const vec2f &rotate) {
    // https://gamedev.stackexchange.com/questions/30644/how-to-keep-my-quaternion-using-fps-camera-from-tilting-and-messing-up
    auto y = vec3f{0, 1, 0};
    auto z = orthonormalize(frame.z, y);
    auto x = cross(y, z);

    auto rot = rotation_frame(vec3f{1, 0, 0}, rotate.y) *
               frame3f{frame.x, frame.y, frame.z, vec3f{0, 0, 0}} *
               rotation_frame(vec3f{0, 1, 0}, rotate.x);
    auto pos = frame.o + transl.x * x + transl.y * y + transl.z * z;

    return {rot.x, rot.y, rot.z, pos};
}

// Computes the image uv coordinates corresponding to the view parameters.
// Returns negative coordinates if out of the image.
inline vec2i get_image_coords(const vec2f &mouse_pos, const vec2f &center,
                              float scale, const vec2i &txt_size) {
    auto xyf = (mouse_pos - center) / scale;
    return vec2i{(int) round(xyf.x + txt_size.x / 2.0f),
                 (int) round(xyf.y + txt_size.y / 2.0f)};
}

// Center image and autofit.
inline void update_imview(vec2f &center, float &scale, const vec2i &imsize,
                          const vec2i &winsize, bool zoom_to_fit) {
    if (zoom_to_fit) {
        scale = min(winsize.x / (float) imsize.x, winsize.y / (float) imsize.y);
        center = {(float) winsize.x / 2, (float) winsize.y / 2};
    } else {
        if (winsize.x >= imsize.x * scale) center.x = (float) winsize.x / 2;
        if (winsize.y >= imsize.y * scale) center.y = (float) winsize.y / 2;
    }
}

// Turntable for UI navigation.
inline void update_turntable(vec3f &from, vec3f &to, vec3f &up,
                             const vec2f &rotate, float dolly, const vec2f &pan) {
    // rotate if necessary
    if (rotate != zero2f) {
        auto z = normalize(to - from);
        auto lz = length(to - from);
        auto phi = atan2(z.z, z.x) + rotate.x;
        auto theta = acos(z.y) + rotate.y;
        theta = clamp(theta, 0.001f, pif - 0.001f);
        auto nz = vec3f{sin(theta) * cos(phi) * lz, cos(theta) * lz,
                        sin(theta) * sin(phi) * lz};
        from = to - nz;
    }

    // dolly if necessary
    if (dolly != 0) {
        auto z = normalize(to - from);
        auto lz = max(0.001f, length(to - from) * (1 + dolly));
        z *= lz;
        from = to - z;
    }

    // pan if necessary
    if (pan != zero2f) {
        auto z = normalize(to - from);
        auto x = normalize(cross(up, z));
        auto y = normalize(cross(z, x));
        auto t = vec3f{pan.x * x.x + pan.y * y.x, pan.x * x.y + pan.y * y.y,
                       pan.x * x.z + pan.y * y.z};
        from += t;
        to += t;
    }
}

// Turntable for UI navigation.
inline void update_turntable(frame3f &frame, float &focus, const vec2f &rotate,
                             float dolly, const vec2f &pan) {
    // rotate if necessary
    if (rotate != zero2f) {
        auto phi = atan2(frame.z.z, frame.z.x) + rotate.x;
        auto theta = acos(frame.z.y) + rotate.y;
        theta = clamp(theta, 0.001f, pif - 0.001f);
        auto new_z = vec3f{
                sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi)};
        auto new_center = frame.o - frame.z * focus;
        auto new_o = new_center + new_z * focus;
        frame = lookat_frame(new_o, new_center, {0, 1, 0});
        focus = length(new_o - new_center);
    }

    // pan if necessary
    if (dolly != 0) {
        auto c = frame.o - frame.z * focus;
        focus = max(focus * (1 + dolly), 0.001f);
        frame.o = c + frame.z * focus;
    }

    // pan if necessary
    if (pan != zero2f) {
        frame.o += frame.x * pan.x + frame.y * pan.y;
    }
}

// FPS camera for UI navigation for a frame parametrization.
inline void update_fpscam(
        frame3f &frame, const vec3f &transl, const vec2f &rotate) {
    // https://gamedev.stackexchange.com/questions/30644/how-to-keep-my-quaternion-using-fps-camera-from-tilting-and-messing-up
    auto y = vec3f{0, 1, 0};
    auto z = orthonormalize(frame.z, y);
    auto x = cross(y, z);

    auto rot = rotation_frame(vec3f{1, 0, 0}, rotate.y) *
               frame3f{frame.x, frame.y, frame.z, vec3f{0, 0, 0}} *
               rotation_frame(vec3f{0, 1, 0}, rotate.x);
    auto pos = frame.o + transl.x * x + transl.y * y + transl.z * z;

    frame = {rot.x, rot.y, rot.z, pos};
}

}  // namespace bcg


#endif //BCG_GRAPHICS_BCG_UI_UTILITY_H
