//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_OPENGL_H
#define BCG_GRAPHICS_BCG_OPENGL_H

#include <string>
#include <array>
#include <vector>
#include "bcg_library/math/bcg_linalg.h"

namespace bcg {

// forward declaration
struct GLFWwindow;

bool init_ogl(std::string &error);

void assert_ogl_error();

bool check_ogl_error(std::string &error);

void clear_ogl_framebuffer(const vec4f &color, bool clear_depth = true);

// OpenGL texture
struct ogl_texture {
    // Texture properties
    vec2i size = {0, 0};
    int num_channels = 0;
    bool is_srgb = false;
    bool is_float = false;
    bool linear = false;
    bool mipmap = false;

    // OpenGL state
    uint texture_id = 0;

    // Disable copy construction
    ogl_texture() = default;

    ogl_texture(const ogl_texture &) = delete;

    ogl_texture &operator=(const ogl_texture &) = delete;

    // cleanup
    ~ogl_texture();
};

// set texture
void set_texture(ogl_texture *texture, const vec2i &size, int num_channels,
                 const byte *img, bool as_srgb = false, bool linear = true,
                 bool mipmap = true, bool wrap_repeat = true);

void set_texture(ogl_texture *texture, const vec2i &size, int num_channels,
                 const float *img, bool as_float = false, bool linear = true,
                 bool mipmap = true, bool wrap_repeat = true);

// check if texture is initialized
bool is_initialized(const ogl_texture *texture);

// clear texture
void clear_texture(ogl_texture *texture);

// OpenGL cubemap
struct ogl_cubemap {
    // Cubemap properties
    int size = 0;
    int num_channels = 0;
    bool is_srgb = false;
    bool is_float = false;
    bool linear = false;
    bool mipmap = false;

    // OpenGL state
    uint cubemap_id = 0;

    // Disable copy construction
    ogl_cubemap() = default;

    ogl_cubemap(const ogl_cubemap &) = delete;

    ogl_cubemap &operator=(const ogl_cubemap &) = delete;

    // cleanup
    ~ogl_cubemap();
};

// set cubemap
void set_cubemap(ogl_cubemap *cubemap, int size, int num_channels,
                 const std::array<byte *, 6> &img, bool as_srgb = false, bool linear = true,
                 bool mipmap = true);

void set_cubemap(ogl_cubemap *cubemap, int size, int num_channels,
                 const std::array<float *, 6> &img, bool as_float = false, bool linear = true,
                 bool mipmap = true);

// check if cubemap is initialized
bool is_initialized(const ogl_cubemap *cubemap);

// clear cubemap
void clear_cubemap(ogl_cubemap *cubemap);


// Opengl array/element buffer
struct ogl_arraybuffer {
    size_t capacity = 0;
    size_t num_elements = 0;
    int element_size = 0;
    bool dynamic = false;

    // OpenGL state
    uint buffer_id = 0;

    // Disable copy construction
    ogl_arraybuffer() = default;

    ogl_arraybuffer(const ogl_arraybuffer &) = delete;

    ogl_arraybuffer &operator=(const ogl_arraybuffer &) = delete;

    // Cleanup
    ~ogl_arraybuffer();
};


// set buffer
void set_arraybuffer(ogl_arraybuffer *buffer, size_t size, int esize,
                     const float *data, bool dynamic = false);

// check if buffer is initialized
bool is_initialized(const ogl_arraybuffer *buffer);

// clear buffer
void clear_arraybuffer(ogl_arraybuffer *buffer);

// set buffer
void set_arraybuffer(
        ogl_arraybuffer *buffer, const std::vector<float> &data, bool dynamic = false);

void set_arraybuffer(
        ogl_arraybuffer *buffer, const std::vector<vec2f> &data, bool dynamic = false);

void set_arraybuffer(
        ogl_arraybuffer *buffer, const std::vector<vec3f> &data, bool dynamic = false);

void set_arraybuffer(
        ogl_arraybuffer *buffer, const std::vector<vec4f> &data, bool dynamic = false);


// Opengl array/element buffer
struct ogl_elementbuffer {
    size_t capacity = 0;
    size_t num_elements = 0;
    int element_size = 0;
    bool dynamic = false;

    // OpenGL state
    uint buffer_id = 0;

    // Disable copy construction
    ogl_elementbuffer() = default;

    ogl_elementbuffer(const ogl_elementbuffer &) = delete;

    ogl_elementbuffer &operator=(const ogl_elementbuffer &) = delete;

    // Cleanup
    ~ogl_elementbuffer();
};


// set buffer
void set_elementbuffer(ogl_elementbuffer *buffer, size_t size, const int *data,
                       bool dynamic = false);

// check if buffer is initialized
bool is_initialized(const ogl_elementbuffer *buffer);

// clear buffer
void clear_elementbuffer(ogl_elementbuffer *buffer);

// set buffer
void set_elementbuffer(ogl_elementbuffer *buffer, const std::vector<int> &points,
                       bool dynamic = false);

void set_elementbuffer(ogl_elementbuffer *buffer, const std::vector<vec2i> &lines,
                       bool dynamic = false);

void set_elementbuffer(ogl_elementbuffer *buffer,
                       const std::vector<vec3i> &triangles, bool dynamic = false);


// Opengl program
struct ogl_program {
    // program code
    std::string vertex_code = {};
    std::string fragment_code = {};

    // OpenGL state
    uint program_id = 0;
    uint vertex_id = 0;
    uint fragment_id = 0;

    // Debugging
    static inline uint bound_program_id = 0;

    // Disable copy construction
    ogl_program() = default;

    ogl_program(const ogl_program &) = delete;

    ogl_program &operator=(const ogl_program &) = delete;

    // Cleanup
    ~ogl_program();
};


// initialize program
bool set_program(ogl_program *program, const std::string &vertex,
                 const std::string &fragment, std::string &error);

bool set_program(ogl_program *program, const std::string &vertex,
                 const std::string &fragment, std::string &error, std::string &errorlog);

bool set_program(ogl_program *program, const std::string &vertex,
                 const std::string &fragment, bool exceptions = true);

bool is_initialized(const ogl_program *program);

// clear program
void clear_program(ogl_program *program);

// bind program
void bind_program(ogl_program *program);

// unbind program
void unbind_program();

// get uniform location
int get_uniform_location(const ogl_program *program, const char *name);

// set uniforms
void set_uniform(const ogl_program *program, int location, int value);

void set_uniform(const ogl_program *program, int location, const vec2i &value);

void set_uniform(const ogl_program *program, int location, const vec3i &value);

void set_uniform(const ogl_program *program, int location, const vec4i &value);

void set_uniform(const ogl_program *program, int location, float value);

void set_uniform(const ogl_program *program, int location, const vec2f &value);

void set_uniform(const ogl_program *program, int location, const vec3f &value);

void set_uniform(const ogl_program *program, int location, const vec4f &value);

void set_uniform(const ogl_program *program, int location, const mat2f &value);

void set_uniform(const ogl_program *program, int location, const mat3f &value);

void set_uniform(const ogl_program *program, int location, const mat4f &value);

template<typename T>
inline void set_uniform(const ogl_program *program, const char *name, const T &value) {
    return set_uniform(program, get_uniform_location(program, name), value);
}

// set uniform texture
void set_uniform(const ogl_program *program, int location,
                 const ogl_texture *texture, int unit);

void set_uniform(const ogl_program *program, const char *name,
                 const ogl_texture *texture, int unit);

void set_uniform(const ogl_program *program, int location, int location_on,
                 const ogl_texture *texture, int unit);

void set_uniform(const ogl_program *program, const char *name,
                 const char *name_on, const ogl_texture *texture, int unit);

// set uniform cubemap
void set_uniform(const ogl_program *program, int location,
                 const ogl_cubemap *cubemap, int unit);

void set_uniform(const ogl_program *program, const char *name,
                 const ogl_cubemap *cubemap, int unit);

void set_uniform(ogl_program *program, int location, int location_on,
                 const ogl_cubemap *cubemap, int unit);

void set_uniform(ogl_program *program, const char *name, const char *name_on,
                 const ogl_cubemap *cubemap, int unit);


struct ogl_framebuffer {
    // framebuffer properties
    vec2i size = {0, 0};

    // opengl state
    uint framebuffer_id = 0;
    uint renderbuffer_id = 0;

    // Denugging
    static inline uint bound_framebuffer_id = 0;

    // Disable copy construction
    ogl_framebuffer() = default;

    ogl_framebuffer(const ogl_framebuffer &) = delete;

    ogl_framebuffer &operator=(const ogl_framebuffer &) = delete;

    // Cleanup
    ~ogl_framebuffer();
};

void set_framebuffer(ogl_framebuffer *framebuffer, const vec2i &size);

void set_framebuffer_texture(const ogl_framebuffer *framebuffer,
                             const ogl_texture *texture, uint mipmap_level = 0);

void set_framebuffer_texture(const ogl_framebuffer *framebuffer,
                             const ogl_cubemap *cubemap, uint face, uint mipmap_level = 0);

void bind_framebuffer(const ogl_framebuffer *target);

void unbind_framebuffer();

void clear_framebuffer(ogl_framebuffer *target);

// Opengl draw elements
enum struct ogl_element_type {
    points,
    lines,
    line_strip,
    triangles,
    triangle_strip,
    triangle_fan,
};

// Opengl shape
struct ogl_shape {
    // OpenGL objects
    std::vector<ogl_arraybuffer *> vertex_buffers = {};
    ogl_elementbuffer *index_buffer = new ogl_elementbuffer{};
    ogl_element_type elements = ogl_element_type::triangles;
    size_t num_instances = 0;

    // OpenGl state
    uint shape_id = 0;

    // Disable copy construction
    ogl_shape() = default;

    ogl_shape(const ogl_shape &) = delete;

    ogl_shape &operator=(const ogl_shape &) = delete;

    // Cleanup
    ~ogl_shape();
};

// set vertex buffer
void set_vertex_buffer(
        ogl_shape *shape, const std::vector<float> &values, int location);

void set_vertex_buffer(
        ogl_shape *shape, const std::vector <vec2f> &values, int location);

void set_vertex_buffer(
        ogl_shape *shape, const std::vector <vec3f> &values, int location);

void set_vertex_buffer(
        ogl_shape *shape, const std::vector <vec4f> &values, int location);

// set vertex buffer with constant value
void set_vertex_buffer(ogl_shape *shape, float attribute, int location);

void set_vertex_buffer(ogl_shape *shape, const vec2f &attribute, int location);

void set_vertex_buffer(ogl_shape *shape, const vec3f &attribute, int location);

void set_vertex_buffer(ogl_shape *shape, const vec4f &attribute, int location);

// set vertex buffer instance
void set_instance_buffer(ogl_shape *shape, int location, bool is_instance);

// set element buffer
void set_index_buffer(ogl_shape *shape, const std::vector<int> &indices);

void set_index_buffer(ogl_shape *shape, const std::vector<vec2i> &indices);

void set_index_buffer(ogl_shape *shape, const std::vector<vec3i> &indices);

// check if shape is initialized
bool is_initialized(const ogl_shape *shape);

// clear buffer
void clear_shape(ogl_shape *shape);

// bind shape
void bind_shape(const ogl_shape *shape);

// draw shape
void draw_shape(const ogl_shape *shape);

// init common shapes
void set_cube_shape(ogl_shape *shape);

void set_quad_shape(ogl_shape *shape);

}


#endif //BCG_GRAPHICS_BCG_OPENGL_H
