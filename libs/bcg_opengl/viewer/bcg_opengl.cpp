//
// Created by alex on 16.10.20.
//

#include <cassert>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>
#include "bcg_opengl.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "math/vector/bcg_vector_map_eigen.h"

#ifdef _WIN32
#undef near
#undef far
#endif

namespace bcg {

bool init_glad(std::string &error) {
    if (!gladLoadGL()) {
        error = "Cannot initialize OpenGL context.";
        return false;
    }
    return true;
}


GLenum _assert_ogl_error() {
    auto error_code = glGetError();
    if (error_code != GL_NO_ERROR) {
        std::string error = "";
        switch (error_code) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM\n"
                        "An unacceptable value is specified for an enumerated argument. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE\n"
                        "A numeric argument is out of range. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION\n"
                        "The specified operation is not allowed in the current state. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW\n"
                        "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW\n"
                        "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY\n"
                        "There is not enough memory left to execute the command. \n"
                        "The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n"
                        "Results of a GL operation are undefined only if GL_OUT_OF_MEMORY has occurred.\n"
                        "In all other cases, the command generating the error is ignored and has no effect on the GL state or frame buffer contents";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION\n"
                        "The framebuffer object is not complete. \n"
                        "The offending command is ignored and has no other side effect than to set the error flag.";
                break;
        }
        printf("\n    OPENGL ERROR: %s\n\n", error.c_str());
    }
    return error_code;
}

void assert_ogl_error() { assert(_assert_ogl_error() == GL_NO_ERROR); }

void ogl_version(int &major, int &minor) {
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    assert_ogl_error();
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    assert_ogl_error();
}

std::string ogl_version_string() {
    const unsigned char *version = glGetString(GL_VERSION);
    assert_ogl_error();
    if (version) {
        return std::string(reinterpret_cast<const char *>(version));
    } else {
        int versionMajor = 0, versionMinor = 0;
#ifdef GL_VERSION_3_0
        ogl_version(versionMajor, versionMinor);
#endif
        if (versionMajor > 0 && versionMajor < 100) {
            return std::to_string(versionMajor) + "." + std::to_string(versionMinor);
        } else {
            return "Unknown";
        }
    }
}

std::string glsl_version_string() {
    std::string version_string = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    assert_ogl_error();
    return version_string;
}

std::string ogl_renderer_string() {
    std::string renderer_string = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    assert_ogl_error();
    return renderer_string;
}

GLenum ogl_types::GetGLType(const GLubyte *) { return GL_UNSIGNED_BYTE; }

// Returns the OpenGL type identifier that corresponds to unsigned short.
GLenum ogl_types::GetGLType(const GLushort *) { return GL_UNSIGNED_SHORT; }

// Returns the OpenGL type identifier that corresponds to float.
GLenum ogl_types::GetGLType(const GLfloat *) { return GL_FLOAT; }

// Returns the OpenGL type identifier that corresponds to byte.
GLenum ogl_types::GetGLType(const GLbyte *) { return GL_BYTE; }

// Returns the OpenGL type identifier that corresponds to short.
GLenum ogl_types::GetGLType(const GLshort *) { return GL_SHORT; }

// Returns the OpenGL type identifier that corresponds to int.
GLenum ogl_types::GetGLType(const GLint *) { return GL_INT; }

// Returns the OpenGL type identifier that corresponds to unsigned int.
GLenum ogl_types::GetGLType(const GLuint *) { return GL_UNSIGNED_INT; }

// Returns the Type that corresponds to unsigned byte.
ogl_types::Type ogl_types::GetType(const GLubyte *) { return TYPE_UBYTE; }

// Returns the Type that corresponds to unsigned short.
ogl_types::Type ogl_types::GetType(const GLushort *) { return TYPE_USHORT; }

// Returns the Type that corresponds to float.
ogl_types::Type ogl_types::GetType(const GLfloat *) { return TYPE_FLOAT; }

// Returns the Type that corresponds to byte.
ogl_types::Type ogl_types::GetType(const GLbyte *) { return TYPE_INT8; }

// Returns the Type that corresponds to short.
ogl_types::Type ogl_types::GetType(const GLshort *) { return TYPE_INT16; }

// Returns the Type that corresponds to int.
ogl_types::Type ogl_types::GetType(const GLint *) { return TYPE_INT32; }

// Returns the Type that corresponds to unsigned int.
ogl_types::Type ogl_types::GetType(const GLuint *) { return TYPE_UINT32; }

unsigned int TextureFormat(ogl_types::Type type, int numChannels) {
    assert(numChannels > 0 && numChannels <= 4);
    const GLenum internalFormats[][4] = {
            {GL_R8, GL_RG8, GL_RGB8, GL_RGBA8},
            {GL_R16, GL_RG16, GL_RGB16, GL_RGBA16},
#ifdef GL_VERSION_3_0
            {GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F},
            {GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F},
            {GL_R8I, GL_RG8I, GL_RGB8I, GL_RGBA8I},
            {GL_R8UI, GL_RG8UI, GL_RGB8UI, GL_RGBA8UI},
            {GL_R16I, GL_RG16I, GL_RGB16I, GL_RGBA16I},
            {GL_R16UI, GL_RG16UI, GL_RGB16UI, GL_RGBA16UI},
            {GL_R32I, GL_RG32I, GL_RGB32I, GL_RGBA32I},
            {GL_R32UI, GL_RG32UI, GL_RGB32UI, GL_RGBA32UI},
#else
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
            { GL_RED, GL_RG, GL_RGB, GL_RGBA },
#endif
    };
    return internalFormats[type][numChannels - 1];
}

unsigned int TextureDataFormat(ogl_types::Type type, int numChannels) {
    const GLenum formats[][4] = {
            {GL_RED, GL_RG, GL_RGB, GL_RGBA},
#ifdef GL_VERSION_3_0
            {GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_RGBA_INTEGER},
#else
            { GL_RED,         GL_RG,         GL_RGB,         GL_RGBA         },
#endif
    };
    return formats[type >= ogl_types::TYPE_INT8][numChannels - 1];
}

ogl_handle::ogl_handle() : ogl_handle(BCG_GL_INVALID_ID, "") {

}

ogl_handle::ogl_handle(unsigned int handle) : ogl_handle(handle, "") {

}

ogl_handle::ogl_handle(std::string name) : ogl_handle(BCG_GL_INVALID_ID, name) {

}

ogl_handle::ogl_handle(unsigned int handle, std::string name) : handle(handle), name(name) {

}

bool ogl_handle::is_valid() const {
    return handle != BCG_GL_INVALID_ID;
}

ogl_handle::operator bool() const {
    return is_valid();
}

ogl_handle::operator unsigned int() const {
    return handle;
}


//----------------------------------------------------------------------------------------------------------------------
// glsl shader
//----------------------------------------------------------------------------------------------------------------------


glsl_shader::glsl_shader() : ogl_handle(), type(BCG_GL_INVALID_ID), source(), filename() {

}

glsl_shader::glsl_shader(unsigned int type, std::string name) : ogl_handle(BCG_GL_INVALID_ID, name), type(type) {

}

glsl_shader::glsl_shader(unsigned int handle, unsigned int type) : ogl_handle(handle), type(type), source(),
                                                                   filename() {}

glsl_shader::glsl_shader(unsigned int handle, unsigned int type, std::string name) : ogl_handle(handle, name),
                                                                                     type(type),
                                                                                     source(), filename() {

}

void glsl_shader::create() {
    destroy();
    handle = glCreateShader(type);
    assert_ogl_error();
}

void glsl_shader::destroy() {
    if (is_valid()) {
        glDeleteShader(handle);
        assert_ogl_error();
        *this = glsl_shader();
    }
}

void glsl_shader::replace_source(int count, const char **string, const int *length) {
    glShaderSource(handle, count, string, length);
    assert_ogl_error();
}

bool glsl_shader::compile_file(const char *filename, unsigned int type) {
    return compile_file(filename, type, 0, nullptr);
}

bool glsl_shader::compile_file(const char *filename, unsigned int type, const char *prepend_source) {
    return compile_file(filename, type, 1, &prepend_source);
}

bool
glsl_shader::compile_file(const char *filename, unsigned int type, int prepend_count, const char **prepend_sources) {
    this->filename = std::string(filename);
    if (load_source(filename))
        return compile(source.data(), type, prepend_count, prepend_sources);
    return false;
}

bool glsl_shader::load_source(const char *filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);

    std::streampos begin(in.tellg());
    in.seekg(0, std::ios::end);
    std::streampos end(in.tellg());
    in.close();

    in = std::ifstream(filename, std::ios::in | std::ios::binary);

    long s = end - begin;
    if (s == 0 || !in.good()) {
        std::cerr << "Cannot compile shader file: " << std::string(filename) << "\n";
        return false;
    }

    source = std::string(s, '\0');

    in.read(&source[0], s);
    in.close();
    return true;
}

bool glsl_shader::compile(const char *source, unsigned int type) {
    return compile(source, type, 0, nullptr);
}

bool glsl_shader::compile(const char *source, unsigned int type, const char *prepend_source) {
    return compile(source, type, 1, &prepend_source);
}

bool glsl_shader::compile(const char *source, unsigned int type, int prepend_count, const char **prepend_sources) {
    if (!is_valid()) {
        this->type = type;
        create();
    }
    if (prepend_count > 0) {
        std::vector<const char *> sources(prepend_count + 1);
        for (int i = 0; i < prepend_count; i++) sources[i] = prepend_sources[i];
        sources[prepend_count] = source;
        replace_source(prepend_count + 1, sources.data(), nullptr);
    } else {
        replace_source(1, &source, nullptr);
    }
    glCompileShader(handle);
    assert_ogl_error();
    check_compile_status();
    return check_eq_shader_type(type);
}

int glsl_shader::get_compile_status() const {
    GLint comile_error(GL_FALSE);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &comile_error);
    assert_ogl_error();
    return comile_error;
}

int glsl_shader::get_info_log_length() const {
    GLint info_log_length(0);
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &info_log_length);
    assert_ogl_error();
    return info_log_length;
}

std::string glsl_shader::get_info_log(int length) const {
    std::vector<char> message(length);
    glGetShaderInfoLog(handle, length, nullptr, message.data());
    assert_ogl_error();
    return std::string(message.data());
}

void glsl_shader::check_compile_status() const {
    GLint info_log_length = get_info_log_length();

    if (info_log_length > 1) {
        std::cerr << "Cannot compile shader source:\nOpenGL Version: " + ogl_version_string() + "\n" +
                     "OpenGl Message: \n" +
                     get_info_log(info_log_length);
    }
}

bool glsl_shader::check_eq_shader_type(unsigned int type) const {
    GLint result = get_compile_status();
    if (result) {
        GLint stype;
        glGetShaderiv(handle, GL_SHADER_TYPE, &stype);
        assert_ogl_error();
        check_compile_status();
        if (stype != (GLint) type) {
            std::cerr << "Incorrect shader type.";
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// glsl program
//----------------------------------------------------------------------------------------------------------------------

glsl_program::glsl_program() : ogl_handle() {

}

glsl_program::glsl_program(std::string name) : ogl_handle(BCG_GL_INVALID_ID, name) {

}

glsl_program::glsl_program(unsigned int handle) : ogl_handle(handle) {

}

glsl_program::glsl_program(unsigned int handle, std::string name) : ogl_handle(handle, name) {

}

void glsl_program::create() {
    destroy();
    handle = glCreateProgram();
    assert_ogl_error();
}

void glsl_program::destroy() {
    if (is_valid()) {
        glDeleteProgram(handle);
        assert_ogl_error();
        handle = BCG_GL_INVALID_ID;
    }
}

void glsl_program::attach(const glsl_shader &shader) const {
    glAttachShader(handle, shader.handle);
    assert_ogl_error();
}

void glsl_program::detach(const glsl_shader &shader) const {
    glDetachShader(handle, shader.handle);
    assert_ogl_error();
}

bool glsl_program::build(const std::vector<glsl_shader> &shaders) {
    if (!is_valid()) {
        create();
    }
    for (const auto &shader:shaders) {
        attach(shader);
    }
    return link();
}


bool glsl_program::build(const glsl_shader *vertex_shader, const glsl_shader *fragment_shader,
                         const glsl_shader *geometry_shader,
                         const glsl_shader *tess_control_shader, const glsl_shader *tess_eval_shader) {
    std::vector<glsl_shader> shaders{*vertex_shader, *fragment_shader};
    if (geometry_shader) shaders.push_back(*geometry_shader);
#if GL_VERSION_4_0
    if (tess_control_shader) shaders.push_back(*tess_control_shader);
    if (tess_eval_shader) shaders.push_back(*tess_eval_shader);
#endif
    return build(shaders);
}

bool glsl_program::build_files(const char *vertex_shader_file, const char *fragment_shader_file,
                               const char *geometry_shader_file,
                               const char *tess_control_shader_file, const char *tess_eval_shader_file) {
    return build_files(vertex_shader_file, fragment_shader_file, geometry_shader_file, tess_control_shader_file,
                       tess_eval_shader_file, 0, nullptr);
}

bool glsl_program::build_files(const char *vertex_shader_file, const char *fragment_shader_file,
                               const char *geometry_shader_file,
                               const char *tess_control_shader_file, const char *tess_eval_shader_file,
                               const char *prepend_source) {
    return build_files(vertex_shader_file, fragment_shader_file, geometry_shader_file, tess_control_shader_file,
                       tess_eval_shader_file, 1, &prepend_source);
}

bool glsl_program::build_files(const char *vertex_shader_file, const char *fragment_shader_file,
                               const char *geometry_shader_file,
                               const char *tess_control_shader_file, const char *tess_eval_shader_file,
                               int prepend_count, const char **prepend_sources) {
    std::vector<glsl_shader> shaders;
    glsl_shader vs, fs, gs, tcs, tes;
    if (!vs.compile_file(vertex_shader_file, GL_VERTEX_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(vs);
    if (!fs.compile_file(fragment_shader_file, GL_FRAGMENT_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(fs);
    if (geometry_shader_file) {
        if (!gs.compile_file(geometry_shader_file, GL_GEOMETRY_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(gs);
    }
#ifdef GL_VERSION_4_0
    if (tess_control_shader_file) {
        if (!tcs.compile_file(tess_control_shader_file, GL_TESS_CONTROL_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tcs);
    }
    if (tess_eval_shader_file) {
        if (!tes.compile_file(tess_eval_shader_file, GL_TESS_EVALUATION_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tes);
    }
#endif
    return build(shaders);
}


bool glsl_program::build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                                 const char *geometry_shader_source, const char *tess_control_shader_source,
                                 const char *tess_eval_shader_source) {
    return build_sources(vertex_shader_source, fragment_shader_source, geometry_shader_source,
                         tess_control_shader_source, tess_eval_shader_source, 0, nullptr);
}

bool glsl_program::build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                                 const char *geometry_shader_source, const char *tess_control_shader_source,
                                 const char *tess_eval_shader_source, const char *prepend_source) {
    return build_sources(vertex_shader_source, fragment_shader_source, geometry_shader_source,
                         tess_control_shader_source, tess_eval_shader_source, 1, &prepend_source);
}

bool glsl_program::build_sources(const char *vertex_shader_source, const char *fragment_shader_source,
                                 const char *geometry_shader_source, const char *tess_control_shader_source,
                                 const char *tess_eval_shader_source, int prepend_count,
                                 const char **prepend_sources) {
    std::vector<glsl_shader> shaders;
    glsl_shader vs, fs, gs, tcs, tes;

    if (!vs.compile(vertex_shader_source, GL_VERTEX_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(vs);
    if (!fs.compile(fragment_shader_source, GL_FRAGMENT_SHADER, prepend_count, prepend_sources)) {
        return false;
    }
    shaders.push_back(fs);
    if (geometry_shader_source) {
        if (!gs.compile(geometry_shader_source, GL_GEOMETRY_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(gs);
    }
#ifdef GL_VERSION_4_0
    if (tess_control_shader_source) {
        if (!tcs.compile(tess_control_shader_source, GL_TESS_CONTROL_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tcs);
    }
    if (tess_eval_shader_source) {
        if (!tes.compile(tess_eval_shader_source, GL_TESS_EVALUATION_SHADER, prepend_count, prepend_sources)) {
            return false;
        }
        shaders.push_back(tes);
    }
#else
    BCG_MAYBE_UNUSED(tess_control_shader_source);
        BCG_MAYBE_UNUSED(tess_eval_shader_source);
#endif
    return build(shaders);
}

#if GL_VERSION_4_3

bool glsl_program::build_compute_file(const char *compute_shader_file) {
    if (!is_valid()) create();
    glsl_shader cs;
    if (!cs.compile_file(compute_shader_file, GL_COMPUTE_SHADER)) {
        return false;
    }
    attach(cs);
    return link();
}

#endif

bool glsl_program::link() const {
    glLinkProgram(handle);
    assert_ogl_error();
    check_link_status();
    bind();
    return get_link_status();
}

bool glsl_program::get_link_status() const {
    GLint result = GL_FALSE;
    glGetProgramiv(handle, GL_LINK_STATUS, &result);
    assert_ogl_error();
    return result == GL_TRUE;
}

GLint glsl_program::get_info_log_length() const {
    GLint info_log_length(0);
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &info_log_length);
    assert_ogl_error();
    return info_log_length;
}

std::string glsl_program::get_info_log(GLint length) const {
    std::vector<char> message(length);
    glGetProgramInfoLog(handle, length, nullptr, message.data());
    assert_ogl_error();
    return std::string(message.data());
}

void glsl_program::check_link_status() const {
    GLint info_log_length = get_info_log_length();
    if (info_log_length > 0)
        std::cerr << "Could not link program:\nOpenGl Message: \n" << get_info_log(info_log_length);
}

void glsl_program::bind() const {
    glUseProgram(handle);
    assert_ogl_error();
}

void glsl_program::release() const {
    glUseProgram(0);
    assert_ogl_error();
}

void glsl_program::print_active_attrinutes() const {
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &count);
    assert_ogl_error();

    std::cout << "Active Attributes: " << std::to_string(count) << "\n";
    for (int i = 0; i < count; i++) {
        glGetActiveAttrib(handle, (GLuint) i, bufSize, &length, &size, &type, name);
        assert_ogl_error();
        std::cout << "Attribute #" << std::to_string(i) << " Type: " << std::to_string(type) << " Name: " << name
                  << "\n";
    }
    std::cout << "\n";
}

void glsl_program::print_active_uniforms() const {
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);
    assert_ogl_error();

    std::cout << "Active Uniforms: " << std::to_string(count) << "\n";
    for (int i = 0; i < count; i++) {
        glGetActiveUniform(handle, (GLuint) i, bufSize, &length, &size, &type, name);
        assert_ogl_error();
        std::cout << "Uniform #" << std::to_string(i) << " Type: " << std::to_string(type) << " Name: " << name << "\n";
    }
    std::cout << "\n";
}

GLuint glsl_program::get_attribute_location(const char *name) const {
    GLint index = glGetAttribLocation(handle, name);
    assert_ogl_error();
    if (index < 0) return BCG_GL_INVALID_ID;
    return GLuint(index);
}

GLint glsl_program::get_uniform_location(const char *name) const {
    GLint index = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (index < 0) return BCG_GL_INVALID_ID;
    return index;
}

GLint glsl_program::get_uniform_block_location(const char *name) const {
    GLint index = glGetUniformBlockIndex(handle, name);
    assert_ogl_error();
    if (index < 0) return BCG_GL_INVALID_ID;
    return index;
}

GLint glsl_program::get_uniform_block_size(GLint index) const {
    GLint size = 0;
    glGetActiveUniformBlockiv(handle, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
    assert_ogl_error();
    return size;
}

void glsl_program::set_uniform_f(const char *name, float x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1f(id, x);
    assert_ogl_error();
}

void glsl_program::set_uniform_f(const char *name, float x, float y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2f(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_uniform_f(const char *name, float x, float y, float z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3f(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_uniform_f(const char *name, float x, float y, float z, float w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4f(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_uniform_1f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_2f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_3f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_4f(const char *name, int count, const float *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0)glUniform4fv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_i(const char *name, int x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1i(id, x);
    assert_ogl_error();
}

void glsl_program::set_uniform_i(const char *name, int x, int y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2i(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_uniform_i(const char *name, int x, int y, int z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3i(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_uniform_i(const char *name, int x, int y, int z, int w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4i(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_uniform_1i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1iv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_2i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2iv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_3i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3iv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_4i(const char *name, int count, const int *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4iv(id, count, data);
    assert_ogl_error();
}

#ifdef GL_VERSION_3_0

void glsl_program::set_uniform_u(const char *name, GLuint x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1ui(id, x);
    assert_ogl_error();
}

void glsl_program::set_uniform_u(const char *name, GLuint x, GLuint y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2ui(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_uniform_u(const char *name, GLuint x, GLuint y, GLuint z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3ui(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_uniform_u(const char *name, GLuint x, GLuint y, GLuint z, GLuint w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4ui(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_uniform_1u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1uiv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_2u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2uiv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_3u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3uiv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_4u(const char *name, int count, const GLuint *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4uiv(id, count, data);
    assert_ogl_error();
}

#endif
#ifdef GL_VERSION_4_0

void glsl_program::set_uniform_d(const char *name, double x) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1d(id, x);
    assert_ogl_error();
}

void glsl_program::set_uniform_d(const char *name, double x, double y) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2d(id, x, y);
    assert_ogl_error();
}

void glsl_program::set_uniform_d(const char *name, double x, double y, double z) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3d(id, x, y, z);
    assert_ogl_error();
}

void glsl_program::set_uniform_d(const char *name, double x, double y, double z, double w) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4d(id, x, y, z, w);
    assert_ogl_error();
}

void glsl_program::set_uniform_1d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform1dv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_2d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform2dv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_3d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform3dv(id, count, data);
    assert_ogl_error();
}

void glsl_program::set_uniform_4d(const char *name, int count, const double *data) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniform4dv(id, count, data);
    assert_ogl_error();
}

#endif

void glsl_program::set_uniform_matrix_2f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2fv(id, count, transpose, m);
    assert_ogl_error();
}

void glsl_program::set_uniform_matrix_3f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3fv(id, count, transpose, m);
    assert_ogl_error();
}

void glsl_program::set_uniform_matrix_4f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4fv(id, count, transpose, m);
    assert_ogl_error();
}

#ifdef GL_VERSION_2_1

void
glsl_program::set_uniform_matrix_2x3f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x3fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_2x4f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x4fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_3x2f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x2fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_3x4f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x4fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_4x2f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x2fv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_4x3f(const char *name, const float *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x3fv(id, count, transpose, m);
    assert_ogl_error();
}

#endif
#ifdef GL_VERSION_4_0

void
glsl_program::set_uniform_matrix_2d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_3d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_4d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_2x3d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x3dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_2x4d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix2x4dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_3x2d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x2dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_3x4d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix3x4dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_4x2d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x2dv(id, count, transpose, m);
    assert_ogl_error();
}

void
glsl_program::set_uniform_matrix_4x3d(const char *name, const double *m, int count, bool transpose) const {
    int id = glGetUniformLocation(handle, name);
    assert_ogl_error();
    if (id >= 0) glUniformMatrix4x3dv(id, count, transpose, m);
    assert_ogl_error();
}

#endif

//----------------------------------------------------------------------------------------------------------------------
// ogl texture
//----------------------------------------------------------------------------------------------------------------------

ogl_texture::ogl_texture() : ogl_texture(BCG_GL_INVALID_ID, BCG_GL_INVALID_ID, "texture") {

}

ogl_texture::ogl_texture(std::string name) : ogl_texture(BCG_GL_INVALID_ID, BCG_GL_INVALID_ID, name) {

}

ogl_texture::ogl_texture(unsigned int target, std::string name) : ogl_texture(BCG_GL_INVALID_ID, target, name) {

}

ogl_texture::ogl_texture(unsigned int handle, unsigned int target, std::string name) : ogl_handle(handle, name),
                                                                                       target(target),
                                                                                       width(-1),
                                                                                       height(-1),
                                                                                       channels(-1),
                                                                                       unit(-1),
                                                                                       mipmap_level(-1),
                                                                                       internal_format(-1),
                                                                                       format(-1),
                                                                                       type(-1),
                                                                                       border_color_rgba{0.5f, 0.5f,
                                                                                                         0.5f, 1.0f} {

}

void ogl_texture::create() {
    destroy();
    glGenTextures(1, &handle);
    assert_ogl_error();
}

void ogl_texture::destroy() {
    if (is_valid()) {
        glDeleteTextures(1, &handle);
        assert_ogl_error();
        handle = BCG_GL_INVALID_ID;
    }
}

void ogl_texture::bind() const {
    glBindTexture(target, handle);
    assert_ogl_error();
}

void ogl_texture::release() const {
    glBindTexture(target, 0);
    assert_ogl_error();
}

void ogl_texture::activate(GLint unit_) {
    unit = unit_;
    activate();
}

void ogl_texture::activate() const {
    glActiveTexture(GL_TEXTURE0 + unit);
    assert_ogl_error();
    bind();
}

void ogl_texture::set_wrap_repeat() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert_ogl_error();
}

void ogl_texture::set_wrap_mirror_repeat() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    assert_ogl_error();
}

void ogl_texture::set_wrap_clamp_to_edge() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    assert_ogl_error();
}

void ogl_texture::set_wrap_clamp_to_border() {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    assert_ogl_error();
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border_color_rgba);
    assert_ogl_error();
}

void ogl_texture::set_filter_linear() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_filter_nearest() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    assert_ogl_error();
}

void ogl_texture::generate_mipmaps() {
    glGenerateMipmap(target);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_nearest_nearest() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_linear_nearest() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_nearest_linear() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_mipmaps_filter_linear_linear() {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    assert_ogl_error();
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert_ogl_error();
}

void ogl_texture::set_image_data(GLenum target, GLint mipmap_level, GLenum internal_format, int width, int height,
                                 GLenum format, GLenum type, void *data) {
    this->target = target;
    this->mipmap_level = mipmap_level;
    this->internal_format = internal_format;
    this->format = format;
    this->type = type;
    update_data(data, width, height);
}

void ogl_texture::update_data(void *data, int w, int h) {
    bind();
    if (width != w || height != h) {
        width = w;
        height = h;
        if (target == GL_TEXTURE_1D) {
            glTexImage1D(target, mipmap_level, internal_format, width, 0, format, type, data);
        } else if (target == GL_TEXTURE_2D) {
            glTexImage2D(target, mipmap_level, internal_format, width, height, 0, format, type, data);
        }
    } else {
        if (target == GL_TEXTURE_1D) {
            glTexSubImage1D(target, mipmap_level, 0, width, format, type, data);
        } else if (target == GL_TEXTURE_2D) {
            glTexSubImage2D(target, mipmap_level, 0, 0, width, height, format, type, data);
        }
    }
    assert_ogl_error();
}

void ogl_texture::download_data(void *data) {
    bind();
    glGetTexImage(target, mipmap_level, format, type, data);
    assert_ogl_error();
    release();
}

void ogl_texture::resize(int width, int height) {
    update_data(nullptr, width, height);
}

ogl_buffer_object::ogl_buffer_object() : ogl_buffer_object(BCG_GL_INVALID_ID, "") {

}

ogl_buffer_object::ogl_buffer_object(std::string name) : ogl_buffer_object(BCG_GL_INVALID_ID, name) {

}

ogl_buffer_object::ogl_buffer_object(unsigned int target, std::string name) : ogl_buffer_object(BCG_GL_INVALID_ID,
                                                                                                target,
                                                                                                name) {

}

ogl_buffer_object::ogl_buffer_object(unsigned int handle, unsigned int target, std::string name) : ogl_handle(handle,
                                                                                                              name),
                                                                                                   target(target),
                                                                                                   capacity(0),
                                                                                                   num_elements(0),
                                                                                                   size_bytes(0),
                                                                                                   dims(0),
                                                                                                   dynamic(false) {

}

size_t ogl_buffer_object::get_buffer_size_gpu() const {
    GLint bytes = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bytes);
    assert_ogl_error();
    return static_cast<size_t>(bytes);
}

void ogl_buffer_object::create() {
    destroy();
    glGenBuffers(1, &handle);
    assert_ogl_error();
}

void ogl_buffer_object::destroy() {
    if (*this) {
        glDeleteBuffers(1, &handle);
        assert_ogl_error();
    }
}

void ogl_buffer_object::bind() const {
    glBindBuffer(target, handle);
    assert_ogl_error();
}

void ogl_buffer_object::release() const {
    glBindBuffer(target, 0);
    assert_ogl_error();
}

void ogl_buffer_object::upload(const void *data, size_t size, size_t dims_, size_t offset, bool dynamic_) {
    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        num_elements = size * dims_;
        capacity = num_elements;
        size_bytes = capacity * sizeof(bcg_index_t);
    } else {
        num_elements = size;
        capacity = num_elements * dims_;
        size_bytes = capacity * sizeof(float);
    }

    glBufferData(target, size_bytes, data, (dynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    assert_ogl_error();
    dims = dims_;
    dynamic = dynamic_;
}

void ogl_buffer_object::download(bcg_scalar_t *data, size_t size_bytes_, size_t offset_bytes) {
    glGetBufferSubData(target, offset_bytes, size_bytes_, data);
    assert_ogl_error();
}

ogl_vertex_buffer::ogl_vertex_buffer() : ogl_vertex_buffer(BCG_GL_INVALID_ID, "vbo") {

}

ogl_vertex_buffer::ogl_vertex_buffer(std::string name) : ogl_vertex_buffer(BCG_GL_INVALID_ID, name) {

}

ogl_vertex_buffer::ogl_vertex_buffer(unsigned int handle, std::string name) : ogl_buffer_object(handle,
                                                                                                GL_ARRAY_BUFFER,
                                                                                                name),
                                                                              normalized(false),
                                                                              type(GL_FLOAT) {

}

void ogl_vertex_buffer::upload(const std::vector<bcg_scalar_t> &data, size_t offset, bool dynamic) {
    Vector<float, -1> DATA = MapConst(data).cast<float>();
    upload(DATA.data(), data.size(), 1, offset, dynamic);
}

void ogl_vertex_buffer::upload(const std::vector<VectorS<2>> &data, size_t offset, bool dynamic) {
    Matrix<float, -1, 2> DATA = MapConst(data).cast<float>();
    upload(DATA.data(), data.size(), 2, offset, dynamic);
}

void ogl_vertex_buffer::upload(const std::vector<VectorS<3>> &data, size_t offset, bool dynamic) {
    Matrix<float, -1, 3> DATA = MapConst(data).cast<float>();
    upload(DATA.data(), data.size(), 3, offset, dynamic);
}

void ogl_vertex_buffer::upload(const std::vector<VectorS<4>> &data, size_t offset, bool dynamic) {
    Matrix<float, -1, 4> DATA = MapConst(data).cast<float>();
    upload(DATA.data(), data.size(), 4, offset, dynamic);
}

void ogl_vertex_buffer::upload(const void *data, size_t size, size_t dims, size_t offset, bool dynamic) {
    if (size > static_cast<size_t>(capacity) || !(*this)) {
        // reallocate buffer if needed
        num_elements = size;
        capacity = num_elements;
        size_bytes = capacity * dims * sizeof(float);
        glBufferData(target, size_bytes, data, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        assert_ogl_error();
        this->dims = dims;
        this->dynamic = dynamic;
    } else {
        // we have enough space
        glBufferSubData(target, offset * dims * sizeof(float), size * dims * sizeof(float), data);
        assert_ogl_error();
    }
}

void ogl_vertex_buffer::download(std::vector<bcg_scalar_t> &data, size_t offset) {
    size_t size_bytes = get_buffer_size_gpu() - offset * sizeof(bcg_scalar_t);
    data.resize(size_bytes / sizeof(bcg_scalar_t) / dims);
    download(data.data(), size_bytes, offset * sizeof(bcg_scalar_t));
}

void ogl_vertex_buffer::download(std::vector<VectorS<2>> &data, size_t offset) {
    size_t size_bytes = get_buffer_size_gpu() - offset * sizeof(bcg_scalar_t);
    data.resize(size_bytes / sizeof(bcg_scalar_t) / dims);
    download(&data[0][0], size_bytes, offset * sizeof(bcg_scalar_t));
}

void ogl_vertex_buffer::download(std::vector<VectorS<3>> &data, size_t offset) {
    size_t size_bytes = get_buffer_size_gpu() - offset * sizeof(bcg_scalar_t);
    data.resize(size_bytes / sizeof(bcg_scalar_t) / dims);
    download(&data[0][0], size_bytes, offset * sizeof(bcg_scalar_t));
}

void ogl_vertex_buffer::download(std::vector<VectorS<4>> &data, size_t offset) {
    size_t size_bytes = get_buffer_size_gpu() - offset * sizeof(bcg_scalar_t);
    data.resize(size_bytes / sizeof(bcg_scalar_t) / dims);
    download(&data[0][0], size_bytes, offset * sizeof(bcg_scalar_t));
}

void ogl_vertex_buffer::download(bcg_scalar_t *data, size_t size_bytes, size_t offset_bytes) {
    bind();
    glGetBufferSubData(target, offset_bytes, size_bytes, data);
    assert_ogl_error();
    release();
}

ogl_element_buffer::ogl_element_buffer() : ogl_element_buffer(BCG_GL_INVALID_ID, "ebo") {

}

ogl_element_buffer::ogl_element_buffer(std::string name) : ogl_element_buffer(BCG_GL_INVALID_ID, name) {

}

ogl_element_buffer::ogl_element_buffer(unsigned int handle, std::string name) : ogl_buffer_object(handle,
                                                                                                  GL_ELEMENT_ARRAY_BUFFER,
                                                                                                  name) {

}

void ogl_element_buffer::upload(const std::vector<bcg_index_t> &data, size_t offset, bool dynamic) {
    upload(data.data(), data.size(), 1, offset, dynamic);
}

void ogl_element_buffer::upload(const std::vector<VectorI<2>> &data, size_t offset, bool dynamic) {
    upload(data.data(), data.size(), 2, offset, dynamic);
}

void ogl_element_buffer::upload(const std::vector<VectorI<3>> &data, size_t offset, bool dynamic) {
    upload(data.data(), data.size(), 3, offset, dynamic);
}

void ogl_element_buffer::upload(const void *data, size_t size, int dims, size_t offset, bool dynamic) {
    if (size > static_cast<size_t>(capacity)) {
        // reallocate buffer if needed
        capacity = size * dims;
        num_elements = capacity;
        size_bytes = capacity * sizeof(bcg_index_t);
        glBufferData(target, size_bytes, data, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        assert_ogl_error();
        this->dims = dims;
        this->dynamic = dynamic;
    } else {
        // we have enough space
        glBufferSubData(target, offset * dims * sizeof(bcg_index_t), size * dims * sizeof(bcg_index_t), data);
        assert_ogl_error();
    }
}

void ogl_element_buffer::download(std::vector<bcg_index_t> &data, size_t offset) {
    size_t size_bytes = get_buffer_size_gpu() - offset * sizeof(bcg_scalar_t);
    data.resize(size_bytes / sizeof(bcg_scalar_t) / dims);
    download(data.data(), size_bytes, offset * sizeof(bcg_scalar_t));
}

void ogl_element_buffer::download(std::vector<VectorI<2>> &data, size_t offset) {
    size_t size_bytes = get_buffer_size_gpu() - offset * sizeof(bcg_scalar_t);
    data.resize(size_bytes / sizeof(bcg_scalar_t) / dims);
    download(&data[0][0], size_bytes, offset * sizeof(bcg_scalar_t));
}

void ogl_element_buffer::download(std::vector<VectorI<3>> &data, size_t offset) {
    size_t size_bytes = get_buffer_size_gpu() - offset * sizeof(bcg_scalar_t);
    data.resize(size_bytes / sizeof(bcg_scalar_t) / dims);
    download(&data[0][0], size_bytes, offset * sizeof(bcg_scalar_t));
}

void ogl_element_buffer::download(bcg_index_t *data, size_t size_bytes, size_t offset_bytes) {
    bind();
    glGetBufferSubData(target, offset_bytes, size_bytes, data);
    assert_ogl_error();
    release();
}

ogl_vertex_array::ogl_vertex_array() : ogl_vertex_array(BCG_GL_INVALID_ID, "vao") {

}

ogl_vertex_array::ogl_vertex_array(std::string name) : ogl_vertex_array(BCG_GL_INVALID_ID, name) {

}

ogl_vertex_array::ogl_vertex_array(unsigned int handle, std::string name) : ogl_handle(handle, name) {

}

void ogl_vertex_array::create() {
    destroy();
    glGenVertexArrays(1, &handle);
    assert_ogl_error();
}

void ogl_vertex_array::destroy() {
    if (*this) {
        glDeleteVertexArrays(1, &handle);
        assert_ogl_error();
        handle = BCG_GL_INVALID_ID;
    }
}

void ogl_vertex_array::bind() const {
    glBindVertexArray(handle);
    assert_ogl_error();
}

void ogl_vertex_array::release() const {
    glBindVertexArray(0);
    assert_ogl_error();
}

void ogl_vertex_array::enable_attribute(unsigned int index, const ogl_vertex_buffer &buffer) const {
    if (index == static_cast<unsigned int>(-1)) return;
    glVertexAttribPointer(index, buffer.dims, buffer.type, (bool) buffer.normalized, buffer.dims * sizeof(float),
                          (void *) 0);
    assert_ogl_error();
    glEnableVertexAttribArray(index);
    assert_ogl_error();
    captured_attributes[index] = buffer.name;
}

void ogl_vertex_array::disable_attribute(unsigned int index) const {
    if (index == static_cast<unsigned int>(-1)) return;
    glDisableVertexAttribArray(index);
    assert_ogl_error();
}

void ogl_vertex_array::capture_vertex_buffer(unsigned int index, const ogl_vertex_buffer &buffer) {
    enable_attribute(index, buffer);
}

ogl_renderbuffer::ogl_renderbuffer() : ogl_renderbuffer(BCG_GL_INVALID_ID, "rbo") {

}

ogl_renderbuffer::ogl_renderbuffer(std::string name) : ogl_renderbuffer(BCG_GL_INVALID_ID, name) {

}

ogl_renderbuffer::ogl_renderbuffer(unsigned int handle, std::string name) : ogl_handle(handle, name), internal_format(GL_DEPTH_COMPONENT) {

}

void ogl_renderbuffer::create() {
    destroy();
    glGenRenderbuffers(1, &handle);
    assert_ogl_error();
}

void ogl_renderbuffer::destroy() {
    if (*this) {
        glDeleteRenderbuffers(1, &handle);
        assert_ogl_error();
        handle = BCG_GL_INVALID_ID;
    }
}

void ogl_renderbuffer::bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, handle);
    assert_ogl_error();
}

void ogl_renderbuffer::release() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    assert_ogl_error();
}

void ogl_renderbuffer::storage(GLenum internal_format_, int width, int height) {
    internal_format = internal_format_;
    resize(width, height);
}

void ogl_renderbuffer::resize(int width, int height) const{
    glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
    assert_ogl_error();
}

ogl_framebuffer::ogl_framebuffer() : ogl_framebuffer(BCG_GL_INVALID_ID, "fbo") {

}

ogl_framebuffer::ogl_framebuffer(std::string name) : ogl_framebuffer(BCG_GL_INVALID_ID, name) {

}

ogl_framebuffer::ogl_framebuffer(unsigned int handle, std::string name) : ogl_handle(handle, name) {

}

void ogl_framebuffer::create() {
    destroy();
    glGenFramebuffers(1, &handle);
    assert_ogl_error();
}

void ogl_framebuffer::destroy() {
    if (*this) {
        glDeleteFramebuffers(1, &handle);
        assert_ogl_error();
        handle = BCG_GL_INVALID_ID;
    }
}

void ogl_framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    assert_ogl_error();
}

void ogl_framebuffer::release() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert_ogl_error();
}

void ogl_framebuffer::activate_textures(){
    for (size_t i = 0; i < textures.size(); ++i) {
        textures[i].activate(i);
    }
}

void ogl_framebuffer::attach_texture(const ogl_texture &texture, unsigned int attachment) {
    size_t index = attachment - GL_COLOR_ATTACHMENT0;
    while(textures.size() <= index){
        textures.push_back({});
        attachments.push_back({});
    }
    textures[index] = texture;
    attachments[index] = attachment;
    width = texture.width;
    height = texture.height;
    texture.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.target, texture.handle, 0);
    assert_ogl_error();
}

void ogl_framebuffer::attach_renderbuffer(const ogl_renderbuffer &renderbuffer, unsigned int attachment) {
    rbo = renderbuffer;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo.handle);
    assert_ogl_error();
    has_depth_buffer = true;
}

bool ogl_framebuffer::check() const {
    auto complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    assert_ogl_error();
    if (!complete) {  //Check for FBO completeness
        std::cerr << "FrameBuffer is not complete!\n";
    }
    return complete;
}

void ogl_framebuffer::oepngl_draw_buffers() {
    glDrawBuffers((GLsizei)attachments.size(), attachments.data());
    assert_ogl_error();
}

void ogl_framebuffer::copy_to_default_framebuffer() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, handle);
    assert_ogl_error();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    assert_ogl_error();
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    assert(width > 0);
    assert(height > 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    assert_ogl_error();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert_ogl_error();
}

void ogl_framebuffer::resize(int width_, int height_){
    width = width_;
    height = height_;
    for(auto &texture : textures){
        texture.resize(width, height);
    }
    rbo.resize(width, height);
}


ogl_state::ogl_state() {
    gl_blend = false;
    gl_clip_distance = false;
    gl_color_logic_op = false;
    gl_cull_face = false;
#ifdef GL_VERSION_4_3
    gl_debug_output = false;
    gl_debug_output_synchonous = false;
#endif
    gl_depth_clamp = false;
    gl_depth_test = true; //default enabled
    gl_depth_mask = true; //default enabled
    gl_dither = true; //default enabled
    gl_framebuffer_srgb = false;
    gl_line_smooth = false;
    gl_multisample = true; //default enabled
    gl_polygon_offset_fill = false;
    gl_polygon_offset_line = false;
    gl_polygon_offset_point = false;
    gl_polygon_smooth = false;
#ifdef GL_VERSION_3_1
    gl_primitive_restart = false;
#endif
#ifdef GL_VERSION_4_3
    gl_primitive_restart_fixed_index = false;
#endif
    gl_rasterizer_discard = false;
    gl_sample_alpha_to_converge = false;
    gl_sample_alpha_to_one = false;
    gl_sample_converge = false;
    gl_sample_shading = false;
    gl_sample_mask = false;
    gl_scissor_test = false;
    gl_stencil_test = false;
#ifdef GL_VERSION_3_2
    gl_texture_cube_map_seamless = false;
#endif
    gl_program_point_size = false;

    //------------------------------------------------------------------------------------------------------------------

    blend_func_sfactor = GL_ONE;
    blend_func_dfactor = GL_ZERO;
    logic_op_opcode = GL_COPY;
    cull_face_mode = GL_BACK;
    front_face_mode = GL_CCW;
    depth_range_nearVal = 0.0;
    depth_range_farVal = 1.0;
    depth_func = GL_LESS;
    polygon_offset_factor = 0.0f;
    polygon_offset_units = 0.0f;
#ifdef GL_VERSION_3_1
    primitive_restart_index = 0;
#endif
    sample_coverage_value = 1.0f;
    sample_coverage_invert = GL_FALSE;
    multisample_value = 0.0f;
    sample_mask_number = 0;
    sample_maks = 0;
    scissor_x = 0;
    scissor_y = 0;
    scissor_width = 1;
    scissor_height = 1;
    stencil_func = GL_ALWAYS;
    stencil_ref = 0;
    stencil_mask = 0xFF;
    stencil_sfail = GL_KEEP;
    stencil_dpfail = GL_KEEP;
    stencil_dppass = GL_KEEP;
    point_size_value = 5;
    polygon_mode_face = GL_FRONT_AND_BACK;
    polygon_mode = GL_FILL;
}

void ogl_state::startup() {
    //default enabled
    set_depth_test(true);
    set_depth_mask(true);
    set_depth_func(GL_LESS);
    set_dither(true);
    set_multisample(true);
    set_blend(true);
    set_blend_func_factors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    set_line_smooth(true);
    set_cull_face(true);
}

void ogl_state::set_blend(bool enabled) {
    gl_blend = enabled;
    gl_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    assert_ogl_error();
}

void ogl_state::set_blend_func_factors(GLenum sfactor, GLenum dfactor) {
    blend_func_sfactor = sfactor;
    blend_func_dfactor = dfactor;
    glBlendFunc(blend_func_sfactor, blend_func_dfactor);
    assert_ogl_error();
}

void ogl_state::set_color_logic_op(bool enabled) {
    gl_color_logic_op = enabled;
    gl_color_logic_op ? glEnable(GL_COLOR_LOGIC_OP) : glDisable(GL_COLOR_LOGIC_OP);
    assert_ogl_error();
}

void ogl_state::set_logic_op_opcode(GLenum opcode) {
    logic_op_opcode = opcode;
    glLogicOp(logic_op_opcode);
    assert_ogl_error();
}

void ogl_state::set_cull_face(bool enabled) {
    gl_cull_face = enabled;
    gl_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    assert_ogl_error();
}

void ogl_state::set_cullface_mode(GLenum mode) {
    cull_face_mode = mode;
    glCullFace(cull_face_mode);
    assert_ogl_error();
}

void ogl_state::set_frontface_mode(GLenum mode) {
    front_face_mode = mode;
    glFrontFace(front_face_mode);
    assert_ogl_error();
}

void ogl_state::set_depth_clamp(bool enabled) {
    gl_depth_clamp = enabled;
    gl_depth_clamp ? glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);
    assert_ogl_error();
}

void ogl_state::set_depth_test(bool enabled) {
    gl_depth_test = enabled;
    gl_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    assert_ogl_error();
}

void ogl_state::set_depth_mask(bool enabled) {
    gl_depth_mask = enabled;
    gl_depth_mask ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    assert_ogl_error();
}

void ogl_state::set_depth_range_values(GLdouble near, GLdouble far) {
    depth_range_nearVal = near;
    depth_range_farVal = far;
    glDepthRange(depth_range_nearVal, depth_range_farVal);
    assert_ogl_error();
}

void ogl_state::set_depth_func(GLenum func) {
    depth_func = func;
    glDepthFunc(depth_func);
    assert_ogl_error();
}

void ogl_state::set_dither(bool enabled) {
    gl_dither = enabled;
    gl_dither ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
    assert_ogl_error();
}

void ogl_state::set_framebuffer_srgb(bool enabled) {
    gl_framebuffer_srgb = enabled;
    gl_framebuffer_srgb ? glEnable(GL_FRAMEBUFFER_SRGB) : glDisable(GL_FRAMEBUFFER_SRGB);
    assert_ogl_error();
}

void ogl_state::set_line_smooth(bool enabled) {
    gl_line_smooth = enabled;
    gl_line_smooth ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
    assert_ogl_error();
}

void ogl_state::set_multisample(bool enabled) {
    gl_multisample = enabled;
    gl_multisample ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
    assert_ogl_error();
}

void ogl_state::set_multisample_value(GLfloat value) {
    multisample_value = value;
    glMinSampleShading(multisample_value);
    assert_ogl_error();
}

void ogl_state::set_polygon_offset_fill(bool enabled) {
    gl_polygon_offset_fill = enabled;
    gl_polygon_offset_fill ? glEnable(GL_POLYGON_OFFSET_FILL) : glDisable(GL_POLYGON_OFFSET_FILL);
    assert_ogl_error();
}

void ogl_state::set_polygon_offset_point(bool enabled) {
    gl_polygon_offset_point = enabled;
    gl_polygon_offset_point ? glEnable(GL_POLYGON_OFFSET_POINT) : glDisable(GL_POLYGON_OFFSET_POINT);
    assert_ogl_error();
}

void ogl_state::set_polygon_offset_line(bool enabled) {
    gl_polygon_offset_line = enabled;
    gl_polygon_offset_line ? glEnable(GL_POLYGON_OFFSET_LINE) : glDisable(GL_POLYGON_OFFSET_LINE);
    assert_ogl_error();
}

void ogl_state::set_polygon_smooth(bool enabled) {
    gl_polygon_smooth = enabled;
    gl_polygon_smooth ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);
    assert_ogl_error();
}

void ogl_state::set_polygon_offset_factor(GLfloat factor, GLfloat units) {
    polygon_offset_factor = factor;
    polygon_offset_units = units;
    glPolygonOffset(polygon_offset_factor, polygon_offset_units);
    assert_ogl_error();
}

void ogl_state::set_polygon_mode(GLenum mode) {
    polygon_mode = mode;
    glPolygonMode(polygon_mode_face, polygon_mode);
    assert_ogl_error();
}

void ogl_state::set_rasterizer_discard(bool enabled) {
    gl_rasterizer_discard = enabled;
    gl_rasterizer_discard ? glEnable(GL_RASTERIZER_DISCARD) : glDisable(GL_RASTERIZER_DISCARD);
    assert_ogl_error();
}

void ogl_state::set_alpha_to_coverage(bool enabled) {
    gl_sample_alpha_to_converge = enabled;
    gl_sample_alpha_to_converge ? glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE) : glDisable(
            GL_SAMPLE_ALPHA_TO_COVERAGE);
    assert_ogl_error();
}

void ogl_state::set_alpha_to_one(bool enabled) {
    gl_sample_alpha_to_one = enabled;
    gl_sample_alpha_to_one ? glEnable(GL_SAMPLE_ALPHA_TO_ONE) : glDisable(GL_SAMPLE_ALPHA_TO_ONE);
    assert_ogl_error();
}

void ogl_state::set_sample_converge(bool enabled) {
    gl_sample_converge = enabled;
    gl_sample_converge ? glEnable(GL_SAMPLE_COVERAGE) : glDisable(GL_SAMPLE_COVERAGE);
    assert_ogl_error();
}

void ogl_state::set_sample_converge_value(GLfloat value, GLboolean invert) {
    sample_coverage_value = value;
    sample_coverage_invert = invert;
    glSampleCoverage(sample_coverage_value, sample_coverage_invert);
    assert_ogl_error();
}

void ogl_state::set_sample_shading(bool enabled) {
    gl_sample_shading = enabled;
    gl_sample_shading ? glEnable(GL_SAMPLE_SHADING) : glDisable(GL_SAMPLE_SHADING);
    assert_ogl_error();
}

void ogl_state::set_sample_mask(bool enabled) {
    gl_sample_mask = enabled;
    gl_sample_mask ? glEnable(GL_SAMPLE_MASK) : glDisable(GL_SAMPLE_MASK);
    assert_ogl_error();
}

void ogl_state::set_sample_mask_number(GLuint number, GLbitfield masks) {
    sample_mask_number = number;
    sample_maks = masks;
    glSampleMaski(sample_mask_number, sample_maks);
    assert_ogl_error();
}

void ogl_state::set_scissor_test(bool enabled) {
    gl_scissor_test = enabled;
    gl_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    assert_ogl_error();
}

void ogl_state::set_scissor_values(GLint x, GLint y, GLsizei width, GLsizei height) {
    scissor_x = x;
    scissor_y = y;
    scissor_width = width;
    scissor_height = height;
    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
    assert_ogl_error();
}

void ogl_state::set_stencil_test(bool enabled) {
    gl_stencil_test = enabled;
    gl_stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    assert_ogl_error();
}

void ogl_state::set_stencil_func(GLenum func, GLint ref, GLuint mask) {
    stencil_func = func;
    stencil_ref = ref;
    stencil_mask = mask;
    glStencilFunc(stencil_func, stencil_ref, stencil_mask);
    assert_ogl_error();
}

void ogl_state::set_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass) {
    stencil_sfail = sfail;
    stencil_dpfail = dpfail;
    stencil_dppass = dppass;
    glStencilOp(stencil_sfail, stencil_dpfail, stencil_dppass);
    assert_ogl_error();
}

void ogl_state::set_program_point_size(bool enabled) {
    gl_program_point_size = enabled;
    gl_program_point_size ? glEnable(GL_PROGRAM_POINT_SIZE) : glDisable(GL_PROGRAM_POINT_SIZE);
    assert_ogl_error();
}

void ogl_state::set_point_size(GLfloat size) {
    point_size_value = size;
    set_program_point_size(true);
    glPointSize(point_size_value);
    assert_ogl_error();
}

#ifdef GL_VERSION_3_1

void ogl_state::set_primitive_restart(bool enabled) {
    gl_primitive_restart = enabled;
    gl_primitive_restart ? glEnable(GL_PRIMITIVE_RESTART) : glDisable(GL_PRIMITIVE_RESTART);
    assert_ogl_error();
}

void ogl_state::set_primitive_restart_index(GLuint index) {
    primitive_restart_index = index;
    glPrimitiveRestartIndex(primitive_restart_index);
    assert_ogl_error();
}

#endif

#ifdef GL_VERSION_3_2

void ogl_state::set_texture_cube_map_seamless(bool enabled) {
    gl_texture_cube_map_seamless = enabled;
    gl_texture_cube_map_seamless ? glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : glDisable(
            GL_TEXTURE_CUBE_MAP_SEAMLESS);
    assert_ogl_error();
}

#endif

#ifdef GL_VERSION_4_3

void ogl_state::set_debug_output(bool enabled) {
    gl_debug_output = enabled;
    gl_debug_output ? glEnable(GL_DEBUG_OUTPUT) : glDisable(GL_DEBUG_OUTPUT);
    assert_ogl_error();
}

void ogl_state::set_debug_output_synchonous(bool enabled) {
    gl_debug_output_synchonous = enabled;
    gl_debug_output_synchonous ? glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS) : glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    assert_ogl_error();
}

void ogl_state::set_primitive_restart_fixed_index(bool enabled) {
    gl_primitive_restart_fixed_index = enabled;
    gl_primitive_restart_fixed_index ? glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX) : glDisable(
            GL_PRIMITIVE_RESTART_FIXED_INDEX);
    assert_ogl_error();
}

#endif

#define TO_STRING(x) #x
#define PRINTBOOL(x) #x << ": "<<  (x?"true\n":"false\n")
#define PRINTVAL(x) #x << " = "<<  x << "\n"

void ogl_state::print(std::ostream &stream) const {
    stream << PRINTBOOL(gl_blend);
    stream << PRINTBOOL(gl_clip_distance);
    stream << PRINTBOOL(gl_color_logic_op);
    stream << PRINTBOOL(gl_cull_face);
#ifdef GL_VERSION_4_3
    stream << PRINTBOOL(gl_debug_output);
    stream << PRINTBOOL(gl_debug_output_synchonous);
#endif
    stream << PRINTBOOL(gl_depth_clamp);
    stream << PRINTBOOL(gl_depth_test);
    stream << PRINTBOOL(gl_depth_mask);
    stream << PRINTBOOL(gl_dither);
    stream << PRINTBOOL(gl_framebuffer_srgb);
    stream << PRINTBOOL(gl_line_smooth);
    stream << PRINTBOOL(gl_multisample);
    stream << PRINTBOOL(gl_polygon_offset_fill);
    stream << PRINTBOOL(gl_polygon_offset_line);
    stream << PRINTBOOL(gl_polygon_offset_point);
    stream << PRINTBOOL(gl_polygon_smooth);
#ifdef GL_VERSION_3_1
    stream << PRINTBOOL(gl_primitive_restart);
#endif
#ifdef GL_VERSION_4_3
    stream << PRINTBOOL(gl_primitive_restart_fixed_index);
#endif
    stream << PRINTBOOL(gl_rasterizer_discard);
    stream << PRINTBOOL(gl_sample_alpha_to_converge);
    stream << PRINTBOOL(gl_sample_alpha_to_one);
    stream << PRINTBOOL(gl_sample_converge);
    stream << PRINTBOOL(gl_sample_shading);
    stream << PRINTBOOL(gl_sample_mask);
    stream << PRINTBOOL(gl_scissor_test);
    stream << PRINTBOOL(gl_stencil_test);
#ifdef GL_VERSION_3_2
    stream << PRINTBOOL(gl_texture_cube_map_seamless);
#endif
    stream << PRINTBOOL(gl_program_point_size);
    stream << "\n";
    stream << PRINTVAL(blend_func_sfactor);
    stream << PRINTVAL(blend_func_dfactor);
    stream << PRINTVAL(logic_op_opcode);
    stream << PRINTVAL(cull_face_mode);
    stream << PRINTVAL(front_face_mode);
    stream << PRINTVAL(depth_range_nearVal);
    stream << PRINTVAL(depth_range_farVal);
    stream << PRINTVAL(depth_func);
    stream << PRINTVAL(polygon_offset_factor);
    stream << PRINTVAL(polygon_offset_units);
#ifdef GL_VERSION_3_1
    stream << PRINTVAL(primitive_restart_index);
#endif
    stream << PRINTVAL(sample_coverage_value);
    stream << PRINTVAL(sample_coverage_invert);
    stream << PRINTVAL(multisample_value);
    stream << PRINTVAL(sample_mask_number);
    stream << PRINTVAL(sample_maks);
    stream << PRINTVAL(scissor_x);
    stream << PRINTVAL(scissor_y);
    stream << PRINTVAL(scissor_width);
    stream << PRINTVAL(scissor_height);
    stream << PRINTVAL(stencil_func);
    stream << PRINTVAL(stencil_ref);
    stream << PRINTVAL(stencil_mask);
    stream << PRINTVAL(stencil_sfail);
    stream << PRINTVAL(stencil_dpfail);
    stream << PRINTVAL(stencil_dppass);
    stream << PRINTVAL(point_size_value);
    stream << PRINTVAL(polygon_mode_face);
    stream << PRINTVAL(polygon_mode);
}

std::ostream &operator<<(std::ostream &stream, const ogl_state &state) {
    stream << "opengl::GLstate: " << std::endl;
    state.print(stream);
    stream << "|====================================================|" << std::endl;
    return stream;
}


//----------------------------------------------------------------------------------------------------------------------
// glfw helper
//----------------------------------------------------------------------------------------------------------------------

std::string GlfwKeyToString(int key) {
    if (key == GLFW_KEY_SPACE) return "SPACE";
    if (key == GLFW_KEY_APOSTROPHE) return "\'";  /* ' */
    if (key == GLFW_KEY_COMMA) return ",";  /* , */
    if (key == GLFW_KEY_MINUS) return "-";  /* - */
    if (key == GLFW_KEY_PERIOD) return "!";  /* . */
    if (key == GLFW_KEY_SLASH) return "/";  /* / */
    if (key == GLFW_KEY_0) return "0";
    if (key == GLFW_KEY_1) return "1";
    if (key == GLFW_KEY_2) return "2";
    if (key == GLFW_KEY_3) return "3";
    if (key == GLFW_KEY_4) return "4";
    if (key == GLFW_KEY_5) return "5";
    if (key == GLFW_KEY_6) return "6";
    if (key == GLFW_KEY_7) return "7";
    if (key == GLFW_KEY_8) return "8";
    if (key == GLFW_KEY_9) return "9";
    if (key == GLFW_KEY_SEMICOLON) return ";";  /* ; */
    if (key == GLFW_KEY_EQUAL) return "=";  /* = */
    if (key == GLFW_KEY_A) return "A";
    if (key == GLFW_KEY_B) return "B";
    if (key == GLFW_KEY_C) return "C";
    if (key == GLFW_KEY_D) return "D";
    if (key == GLFW_KEY_E) return "E";
    if (key == GLFW_KEY_F) return "F";
    if (key == GLFW_KEY_G) return "G";
    if (key == GLFW_KEY_H) return "H";
    if (key == GLFW_KEY_I) return "I";
    if (key == GLFW_KEY_J) return "J";
    if (key == GLFW_KEY_K) return "K";
    if (key == GLFW_KEY_L) return "L";
    if (key == GLFW_KEY_M) return "M";
    if (key == GLFW_KEY_N) return "N";
    if (key == GLFW_KEY_O) return "O";
    if (key == GLFW_KEY_P) return "P";
    if (key == GLFW_KEY_Q) return "Q";
    if (key == GLFW_KEY_R) return "R";
    if (key == GLFW_KEY_S) return "S";
    if (key == GLFW_KEY_T) return "T";
    if (key == GLFW_KEY_U) return "U";
    if (key == GLFW_KEY_V) return "V";
    if (key == GLFW_KEY_W) return "W";
    if (key == GLFW_KEY_X) return "X";
    if (key == GLFW_KEY_Y) return "Y";
    if (key == GLFW_KEY_Z) return "Z";
    if (key == GLFW_KEY_LEFT_BRACKET) return "[";  /* [ */
    if (key == GLFW_KEY_BACKSLASH) return "\\";  /* \ */
    if (key == GLFW_KEY_RIGHT_BRACKET) return "]";  /* ] */
    if (key == GLFW_KEY_GRAVE_ACCENT) return "`"; /* ` */
    if (key == GLFW_KEY_WORLD_1) return ""; /* non-US #1 */
    if (key == GLFW_KEY_WORLD_2) return ""; /* non-US #2 */

    if (key == GLFW_KEY_ESCAPE) return "ESC";
    if (key == GLFW_KEY_ENTER) return "ENTER";
    if (key == GLFW_KEY_TAB) return "TAB";
    if (key == GLFW_KEY_BACKSPACE) return "BACKSPACE";
    if (key == GLFW_KEY_INSERT) return "INSERT";
    if (key == GLFW_KEY_DELETE) return "DELETE";
    if (key == GLFW_KEY_RIGHT) return "RIGHT";
    if (key == GLFW_KEY_LEFT) return "LEFT";
    if (key == GLFW_KEY_DOWN) return "DOWN";
    if (key == GLFW_KEY_UP) return "UP";
    if (key == GLFW_KEY_PAGE_UP) return "PAGE_UP";
    if (key == GLFW_KEY_PAGE_DOWN) return "PAGE_DOWN";
    if (key == GLFW_KEY_HOME) return "HOME";
    if (key == GLFW_KEY_END) return "END";
    if (key == GLFW_KEY_CAPS_LOCK) return "CAPS";
    if (key == GLFW_KEY_SCROLL_LOCK) return "SCROLL";
    if (key == GLFW_KEY_NUM_LOCK) return "NUM";
    if (key == GLFW_KEY_PRINT_SCREEN) return "PRINT";
    if (key == GLFW_KEY_PAUSE) return "PAUSE";
    if (key == GLFW_KEY_F1) return "F1";
    if (key == GLFW_KEY_F2) return "F2";
    if (key == GLFW_KEY_F3) return "F3";
    if (key == GLFW_KEY_F4) return "F4";
    if (key == GLFW_KEY_F5) return "F5";
    if (key == GLFW_KEY_F6) return "F6";
    if (key == GLFW_KEY_F7) return "F7";
    if (key == GLFW_KEY_F8) return "F8";
    if (key == GLFW_KEY_F9) return "F9";
    if (key == GLFW_KEY_F10) return "F10";
    if (key == GLFW_KEY_F11) return "F11";
    if (key == GLFW_KEY_F12) return "F12";
    if (key == GLFW_KEY_F13) return "F13";
    if (key == GLFW_KEY_F14) return "F14";
    if (key == GLFW_KEY_F15) return "F15";
    if (key == GLFW_KEY_F16) return "F16";
    if (key == GLFW_KEY_F17) return "F17";
    if (key == GLFW_KEY_F18) return "F18";
    if (key == GLFW_KEY_F19) return "F19";
    if (key == GLFW_KEY_F20) return "F20";
    if (key == GLFW_KEY_F21) return "F21";
    if (key == GLFW_KEY_F22) return "F22";
    if (key == GLFW_KEY_F23) return "F23";
    if (key == GLFW_KEY_F24) return "F24";
    if (key == GLFW_KEY_F25) return "F25";
    if (key == GLFW_KEY_KP_0) return "KP0";
    if (key == GLFW_KEY_KP_1) return "KP1";
    if (key == GLFW_KEY_KP_2) return "KP2";
    if (key == GLFW_KEY_KP_3) return "KP3";
    if (key == GLFW_KEY_KP_4) return "KP4";
    if (key == GLFW_KEY_KP_5) return "KP5";
    if (key == GLFW_KEY_KP_6) return "KP6";
    if (key == GLFW_KEY_KP_7) return "KP7";
    if (key == GLFW_KEY_KP_8) return "KP8";
    if (key == GLFW_KEY_KP_9) return "KP9";
    if (key == GLFW_KEY_KP_DECIMAL) return "";
    if (key == GLFW_KEY_KP_DIVIDE) return "KP_DIV";
    if (key == GLFW_KEY_KP_MULTIPLY) return "KP_MUL";
    if (key == GLFW_KEY_KP_SUBTRACT) return "KP_SUB";
    if (key == GLFW_KEY_KP_ADD) return "KP_ADD";
    if (key == GLFW_KEY_KP_ENTER) return "KP_ENTER";
    if (key == GLFW_KEY_KP_EQUAL) return "KP_EQ";
    if (key == GLFW_KEY_LEFT_SHIFT) return "LEFT_SHIFT";
    if (key == GLFW_KEY_LEFT_CONTROL) return "LEFT_CONTROL";
    if (key == GLFW_KEY_LEFT_ALT) return "LEFT_ALT";
    if (key == GLFW_KEY_LEFT_SUPER) return "LEFT_SUPER";
    if (key == GLFW_KEY_RIGHT_SHIFT) return "RIGHT_SHIFT";
    if (key == GLFW_KEY_RIGHT_CONTROL) return "RIGHT_CONTROL";
    if (key == GLFW_KEY_RIGHT_ALT) return "RIGHT_ALT";
    if (key == GLFW_KEY_RIGHT_SUPER) return "RIGHT_SUPER";
    if (key == GLFW_KEY_MENU) return "";
    return "";
}

GLenum string_to_ogl_enum(const std::string &name) {
    if (name == "GL_DEPTH_BUFFER_BIT") return GL_DEPTH_BUFFER_BIT;
    if (name == "GL_STENCIL_BUFFER_BIT") return GL_STENCIL_BUFFER_BIT;
    if (name == "GL_COLOR_BUFFER_BIT") return GL_COLOR_BUFFER_BIT;
    if (name == "GL_FALSE") return GL_FALSE;
    if (name == "GL_TRUE") return GL_TRUE;
    if (name == "GL_POINTS") return GL_POINTS;
    if (name == "GL_LINES") return GL_LINES;
    if (name == "GL_LINE_LOOP") return GL_LINE_LOOP;
    if (name == "GL_LINE_STRIP") return GL_LINE_STRIP;
    if (name == "GL_TRIANGLES") return GL_TRIANGLES;
    if (name == "GL_TRIANGLE_STRIP") return GL_TRIANGLE_STRIP;
    if (name == "GL_TRIANGLE_FAN") return GL_TRIANGLE_FAN;
    if (name == "GL_NEVER") return GL_NEVER;
    if (name == "GL_LESS") return GL_LESS;
    if (name == "GL_EQUAL") return GL_EQUAL;
    if (name == "GL_LEQUAL") return GL_LEQUAL;
    if (name == "GL_GREATER") return GL_GREATER;
    if (name == "GL_NOTEQUAL") return GL_NOTEQUAL;
    if (name == "GL_GEQUAL") return GL_GEQUAL;
    if (name == "GL_ALWAYS") return GL_ALWAYS;
    if (name == "GL_ZERO") return GL_ZERO;
    if (name == "GL_ONE") return GL_ONE;
    if (name == "GL_SRC_COLOR") return GL_SRC_COLOR;
    if (name == "GL_ONE_MINUS_SRC_COLOR") return GL_ONE_MINUS_SRC_COLOR;
    if (name == "GL_SRC_ALPHA") return GL_SRC_ALPHA;
    if (name == "GL_ONE_MINUS_SRC_ALPHA") return GL_ONE_MINUS_SRC_ALPHA;
    if (name == "GL_DST_ALPHA") return GL_DST_ALPHA;
    if (name == "GL_ONE_MINUS_DST_ALPHA") return GL_ONE_MINUS_DST_ALPHA;
    if (name == "GL_DST_COLOR") return GL_DST_COLOR;
    if (name == "GL_ONE_MINUS_DST_COLOR") return GL_ONE_MINUS_DST_COLOR;
    if (name == "GL_SRC_ALPHA_SATURATE") return GL_SRC_ALPHA_SATURATE;
    if (name == "GL_NONE") return GL_NONE;
    if (name == "GL_FRONT_LEFT") return GL_FRONT_LEFT;
    if (name == "GL_FRONT_RIGHT") return GL_FRONT_RIGHT;
    if (name == "GL_BACK_LEFT") return GL_BACK_LEFT;
    if (name == "GL_BACK_RIGHT") return GL_BACK_RIGHT;
    if (name == "GL_FRONT") return GL_FRONT;
    if (name == "GL_BACK") return GL_BACK;
    if (name == "GL_LEFT") return GL_LEFT;
    if (name == "GL_RIGHT") return GL_RIGHT;
    if (name == "GL_FRONT_AND_BACK") return GL_FRONT_AND_BACK;
    if (name == "GL_NO_ERROR") return GL_NO_ERROR;
    if (name == "GL_INVALID_ENUM") return GL_INVALID_ENUM;
    if (name == "GL_INVALID_VALUE") return GL_INVALID_VALUE;
    if (name == "GL_INVALID_OPERATION") return GL_INVALID_OPERATION;
    if (name == "GL_OUT_OF_MEMORY") return GL_OUT_OF_MEMORY;
    if (name == "GL_CW") return GL_CW;
    if (name == "GL_CCW") return GL_CCW;
    if (name == "GL_POINT_SIZE") return GL_POINT_SIZE;
    if (name == "GL_POINT_SIZE_RANGE") return GL_POINT_SIZE_RANGE;
    if (name == "GL_POINT_SIZE_GRANULARITY") return GL_POINT_SIZE_GRANULARITY;
    if (name == "GL_LINE_SMOOTH") return GL_LINE_SMOOTH;
    if (name == "GL_LINE_WIDTH") return GL_LINE_WIDTH;
    if (name == "GL_LINE_WIDTH_RANGE") return GL_LINE_WIDTH_RANGE;
    if (name == "GL_LINE_WIDTH_GRANULARITY") return GL_LINE_WIDTH_GRANULARITY;
    if (name == "GL_POLYGON_MODE") return GL_POLYGON_MODE;
    if (name == "GL_POLYGON_SMOOTH") return GL_POLYGON_SMOOTH;
    if (name == "GL_CULL_FACE") return GL_CULL_FACE;
    if (name == "GL_CULL_FACE_MODE") return GL_CULL_FACE_MODE;
    if (name == "GL_FRONT_FACE") return GL_FRONT_FACE;
    if (name == "GL_DEPTH_RANGE") return GL_DEPTH_RANGE;
    if (name == "GL_DEPTH_TEST") return GL_DEPTH_TEST;
    if (name == "GL_DEPTH_WRITEMASK") return GL_DEPTH_WRITEMASK;
    if (name == "GL_DEPTH_CLEAR_VALUE") return GL_DEPTH_CLEAR_VALUE;
    if (name == "GL_DEPTH_FUNC") return GL_DEPTH_FUNC;
    if (name == "GL_STENCIL_TEST") return GL_STENCIL_TEST;
    if (name == "GL_STENCIL_CLEAR_VALUE") return GL_STENCIL_CLEAR_VALUE;
    if (name == "GL_STENCIL_FUNC") return GL_STENCIL_FUNC;
    if (name == "GL_STENCIL_VALUE_MASK") return GL_STENCIL_VALUE_MASK;
    if (name == "GL_STENCIL_FAIL") return GL_STENCIL_FAIL;
    if (name == "GL_STENCIL_PASS_DEPTH_FAIL") return GL_STENCIL_PASS_DEPTH_FAIL;
    if (name == "GL_STENCIL_PASS_DEPTH_PASS") return GL_STENCIL_PASS_DEPTH_PASS;
    if (name == "GL_STENCIL_REF") return GL_STENCIL_REF;
    if (name == "GL_STENCIL_WRITEMASK") return GL_STENCIL_WRITEMASK;
    if (name == "GL_VIEWPORT") return GL_VIEWPORT;
    if (name == "GL_DITHER") return GL_DITHER;
    if (name == "GL_BLEND_DST") return GL_BLEND_DST;
    if (name == "GL_BLEND_SRC") return GL_BLEND_SRC;
    if (name == "GL_BLEND") return GL_BLEND;
    if (name == "GL_LOGIC_OP_MODE") return GL_LOGIC_OP_MODE;
    if (name == "GL_DRAW_BUFFER") return GL_DRAW_BUFFER;
    if (name == "GL_READ_BUFFER") return GL_READ_BUFFER;
    if (name == "GL_SCISSOR_BOX") return GL_SCISSOR_BOX;
    if (name == "GL_SCISSOR_TEST") return GL_SCISSOR_TEST;
    if (name == "GL_COLOR_CLEAR_VALUE") return GL_COLOR_CLEAR_VALUE;
    if (name == "GL_COLOR_WRITEMASK") return GL_COLOR_WRITEMASK;
    if (name == "GL_DOUBLEBUFFER") return GL_DOUBLEBUFFER;
    if (name == "GL_STEREO") return GL_STEREO;
    if (name == "GL_LINE_SMOOTH_HINT") return GL_LINE_SMOOTH_HINT;
    if (name == "GL_POLYGON_SMOOTH_HINT") return GL_POLYGON_SMOOTH_HINT;
    if (name == "GL_UNPACK_SWAP_BYTES") return GL_UNPACK_SWAP_BYTES;
    if (name == "GL_UNPACK_LSB_FIRST") return GL_UNPACK_LSB_FIRST;
    if (name == "GL_UNPACK_ROW_LENGTH") return GL_UNPACK_ROW_LENGTH;
    if (name == "GL_UNPACK_SKIP_ROWS") return GL_UNPACK_SKIP_ROWS;
    if (name == "GL_UNPACK_SKIP_PIXELS") return GL_UNPACK_SKIP_PIXELS;
    if (name == "GL_UNPACK_ALIGNMENT") return GL_UNPACK_ALIGNMENT;
    if (name == "GL_PACK_SWAP_BYTES") return GL_PACK_SWAP_BYTES;
    if (name == "GL_PACK_LSB_FIRST") return GL_PACK_LSB_FIRST;
    if (name == "GL_PACK_ROW_LENGTH") return GL_PACK_ROW_LENGTH;
    if (name == "GL_PACK_SKIP_ROWS") return GL_PACK_SKIP_ROWS;
    if (name == "GL_PACK_SKIP_PIXELS") return GL_PACK_SKIP_PIXELS;
    if (name == "GL_PACK_ALIGNMENT") return GL_PACK_ALIGNMENT;
    if (name == "GL_MAX_TEXTURE_SIZE") return GL_MAX_TEXTURE_SIZE;
    if (name == "GL_MAX_VIEWPORT_DIMS") return GL_MAX_VIEWPORT_DIMS;
    if (name == "GL_SUBPIXEL_BITS") return GL_SUBPIXEL_BITS;
    if (name == "GL_TEXTURE_1D") return GL_TEXTURE_1D;
    if (name == "GL_TEXTURE_2D") return GL_TEXTURE_2D;
    if (name == "GL_TEXTURE_WIDTH") return GL_TEXTURE_WIDTH;
    if (name == "GL_TEXTURE_HEIGHT") return GL_TEXTURE_HEIGHT;
    if (name == "GL_TEXTURE_BORDER_COLOR") return GL_TEXTURE_BORDER_COLOR;
    if (name == "GL_DONT_CARE") return GL_DONT_CARE;
    if (name == "GL_FASTEST") return GL_FASTEST;
    if (name == "GL_NICEST") return GL_NICEST;
    if (name == "GL_BYTE") return GL_BYTE;
    if (name == "GL_UNSIGNED_BYTE") return GL_UNSIGNED_BYTE;
    if (name == "GL_SHORT") return GL_SHORT;
    if (name == "GL_UNSIGNED_SHORT") return GL_UNSIGNED_SHORT;
    if (name == "GL_INT") return GL_INT;
    if (name == "GL_UNSIGNED_INT") return GL_UNSIGNED_INT;
    if (name == "GL_FLOAT") return GL_FLOAT;
    if (name == "GL_CLEAR") return GL_CLEAR;
    if (name == "GL_AND") return GL_AND;
    if (name == "GL_AND_REVERSE") return GL_AND_REVERSE;
    if (name == "GL_COPY") return GL_COPY;
    if (name == "GL_AND_INVERTED") return GL_AND_INVERTED;
    if (name == "GL_NOOP") return GL_NOOP;
    if (name == "GL_XOR") return GL_XOR;
    if (name == "GL_OR") return GL_OR;
    if (name == "GL_NOR") return GL_NOR;
    if (name == "GL_EQUIV") return GL_EQUIV;
    if (name == "GL_INVERT") return GL_INVERT;
    if (name == "GL_OR_REVERSE") return GL_OR_REVERSE;
    if (name == "GL_COPY_INVERTED") return GL_COPY_INVERTED;
    if (name == "GL_OR_INVERTED") return GL_OR_INVERTED;
    if (name == "GL_NAND") return GL_NAND;
    if (name == "GL_SET") return GL_SET;
    if (name == "GL_TEXTURE") return GL_TEXTURE;
    if (name == "GL_COLOR") return GL_COLOR;
    if (name == "GL_DEPTH") return GL_DEPTH;
    if (name == "GL_STENCIL") return GL_STENCIL;
    if (name == "GL_STENCIL_INDEX") return GL_STENCIL_INDEX;
    if (name == "GL_DEPTH_COMPONENT") return GL_DEPTH_COMPONENT;
    if (name == "GL_RED") return GL_RED;
    if (name == "GL_GREEN") return GL_GREEN;
    if (name == "GL_BLUE") return GL_BLUE;
    if (name == "GL_ALPHA") return GL_ALPHA;
    if (name == "GL_RGB") return GL_RGB;
    if (name == "GL_RGBA") return GL_RGBA;
    if (name == "GL_POINT") return GL_POINT;
    if (name == "GL_LINE") return GL_LINE;
    if (name == "GL_FILL") return GL_FILL;
    if (name == "GL_KEEP") return GL_KEEP;
    if (name == "GL_REPLACE") return GL_REPLACE;
    if (name == "GL_INCR") return GL_INCR;
    if (name == "GL_DECR") return GL_DECR;
    if (name == "GL_VENDOR") return GL_VENDOR;
    if (name == "GL_RENDERER") return GL_RENDERER;
    if (name == "GL_VERSION") return GL_VERSION;
    if (name == "GL_EXTENSIONS") return GL_EXTENSIONS;
    if (name == "GL_NEAREST") return GL_NEAREST;
    if (name == "GL_LINEAR") return GL_LINEAR;
    if (name == "GL_NEAREST_MIPMAP_NEAREST") return GL_NEAREST_MIPMAP_NEAREST;
    if (name == "GL_LINEAR_MIPMAP_NEAREST") return GL_LINEAR_MIPMAP_NEAREST;
    if (name == "GL_NEAREST_MIPMAP_LINEAR") return GL_NEAREST_MIPMAP_LINEAR;
    if (name == "GL_LINEAR_MIPMAP_LINEAR") return GL_LINEAR_MIPMAP_LINEAR;
    if (name == "GL_TEXTURE_MAG_FILTER") return GL_TEXTURE_MAG_FILTER;
    if (name == "GL_TEXTURE_MIN_FILTER") return GL_TEXTURE_MIN_FILTER;
    if (name == "GL_TEXTURE_WRAP_S") return GL_TEXTURE_WRAP_S;
    if (name == "GL_TEXTURE_WRAP_T") return GL_TEXTURE_WRAP_T;
    if (name == "GL_REPEAT") return GL_REPEAT;
    if (name == "GL_COLOR_LOGIC_OP") return GL_COLOR_LOGIC_OP;
    if (name == "GL_POLYGON_OFFSET_UNITS") return GL_POLYGON_OFFSET_UNITS;
    if (name == "GL_POLYGON_OFFSET_POINT") return GL_POLYGON_OFFSET_POINT;
    if (name == "GL_POLYGON_OFFSET_LINE") return GL_POLYGON_OFFSET_LINE;
    if (name == "GL_POLYGON_OFFSET_FILL") return GL_POLYGON_OFFSET_FILL;
    if (name == "GL_POLYGON_OFFSET_FACTOR") return GL_POLYGON_OFFSET_FACTOR;
    if (name == "GL_TEXTURE_BINDING_1D") return GL_TEXTURE_BINDING_1D;
    if (name == "GL_TEXTURE_BINDING_2D") return GL_TEXTURE_BINDING_2D;
    if (name == "GL_TEXTURE_INTERNAL_FORMAT") return GL_TEXTURE_INTERNAL_FORMAT;
    if (name == "GL_TEXTURE_RED_SIZE") return GL_TEXTURE_RED_SIZE;
    if (name == "GL_TEXTURE_GREEN_SIZE") return GL_TEXTURE_GREEN_SIZE;
    if (name == "GL_TEXTURE_BLUE_SIZE") return GL_TEXTURE_BLUE_SIZE;
    if (name == "GL_TEXTURE_ALPHA_SIZE") return GL_TEXTURE_ALPHA_SIZE;
    if (name == "GL_DOUBLE") return GL_DOUBLE;
    if (name == "GL_PROXY_TEXTURE_1D") return GL_PROXY_TEXTURE_1D;
    if (name == "GL_PROXY_TEXTURE_2D") return GL_PROXY_TEXTURE_2D;
    if (name == "GL_R3_G3_B2") return GL_R3_G3_B2;
    if (name == "GL_RGB4") return GL_RGB4;
    if (name == "GL_RGB5") return GL_RGB5;
    if (name == "GL_RGB8") return GL_RGB8;
    if (name == "GL_RGB10") return GL_RGB10;
    if (name == "GL_RGB12") return GL_RGB12;
    if (name == "GL_RGB16") return GL_RGB16;
    if (name == "GL_RGBA2") return GL_RGBA2;
    if (name == "GL_RGBA4") return GL_RGBA4;
    if (name == "GL_RGB5_A1") return GL_RGB5_A1;
    if (name == "GL_RGBA8") return GL_RGBA8;
    if (name == "GL_RGB10_A2") return GL_RGB10_A2;
    if (name == "GL_RGBA12") return GL_RGBA12;
    if (name == "GL_RGBA16") return GL_RGBA16;
    if (name == "GL_UNSIGNED_BYTE_3_3_2") return GL_UNSIGNED_BYTE_3_3_2;
    if (name == "GL_UNSIGNED_SHORT_4_4_4_4") return GL_UNSIGNED_SHORT_4_4_4_4;
    if (name == "GL_UNSIGNED_SHORT_5_5_5_1") return GL_UNSIGNED_SHORT_5_5_5_1;
    if (name == "GL_UNSIGNED_INT_8_8_8_8") return GL_UNSIGNED_INT_8_8_8_8;
    if (name == "GL_UNSIGNED_INT_10_10_10_2") return GL_UNSIGNED_INT_10_10_10_2;
    if (name == "GL_TEXTURE_BINDING_3D") return GL_TEXTURE_BINDING_3D;
    if (name == "GL_PACK_SKIP_IMAGES") return GL_PACK_SKIP_IMAGES;
    if (name == "GL_PACK_IMAGE_HEIGHT") return GL_PACK_IMAGE_HEIGHT;
    if (name == "GL_UNPACK_SKIP_IMAGES") return GL_UNPACK_SKIP_IMAGES;
    if (name == "GL_UNPACK_IMAGE_HEIGHT") return GL_UNPACK_IMAGE_HEIGHT;
    if (name == "GL_TEXTURE_3D") return GL_TEXTURE_3D;
    if (name == "GL_PROXY_TEXTURE_3D") return GL_PROXY_TEXTURE_3D;
    if (name == "GL_TEXTURE_DEPTH") return GL_TEXTURE_DEPTH;
    if (name == "GL_TEXTURE_WRAP_R") return GL_TEXTURE_WRAP_R;
    if (name == "GL_MAX_3D_TEXTURE_SIZE") return GL_MAX_3D_TEXTURE_SIZE;
    if (name == "GL_UNSIGNED_BYTE_2_3_3_REV") return GL_UNSIGNED_BYTE_2_3_3_REV;
    if (name == "GL_UNSIGNED_SHORT_5_6_5") return GL_UNSIGNED_SHORT_5_6_5;
    if (name == "GL_UNSIGNED_SHORT_5_6_5_REV") return GL_UNSIGNED_SHORT_5_6_5_REV;
    if (name == "GL_UNSIGNED_SHORT_4_4_4_4_REV") return GL_UNSIGNED_SHORT_4_4_4_4_REV;
    if (name == "GL_UNSIGNED_SHORT_1_5_5_5_REV") return GL_UNSIGNED_SHORT_1_5_5_5_REV;
    if (name == "GL_UNSIGNED_INT_8_8_8_8_REV") return GL_UNSIGNED_INT_8_8_8_8_REV;
    if (name == "GL_UNSIGNED_INT_2_10_10_10_REV") return GL_UNSIGNED_INT_2_10_10_10_REV;
    if (name == "GL_BGR") return GL_BGR;
    if (name == "GL_BGRA") return GL_BGRA;
    if (name == "GL_MAX_ELEMENTS_VERTICES") return GL_MAX_ELEMENTS_VERTICES;
    if (name == "GL_MAX_ELEMENTS_INDICES") return GL_MAX_ELEMENTS_INDICES;
    if (name == "GL_CLAMP_TO_EDGE") return GL_CLAMP_TO_EDGE;
    if (name == "GL_TEXTURE_MIN_LOD") return GL_TEXTURE_MIN_LOD;
    if (name == "GL_TEXTURE_MAX_LOD") return GL_TEXTURE_MAX_LOD;
    if (name == "GL_TEXTURE_BASE_LEVEL") return GL_TEXTURE_BASE_LEVEL;
    if (name == "GL_TEXTURE_MAX_LEVEL") return GL_TEXTURE_MAX_LEVEL;
    if (name == "GL_SMOOTH_POINT_SIZE_RANGE") return GL_SMOOTH_POINT_SIZE_RANGE;
    if (name == "GL_SMOOTH_POINT_SIZE_GRANULARITY") return GL_SMOOTH_POINT_SIZE_GRANULARITY;
    if (name == "GL_SMOOTH_LINE_WIDTH_RANGE") return GL_SMOOTH_LINE_WIDTH_RANGE;
    if (name == "GL_SMOOTH_LINE_WIDTH_GRANULARITY") return GL_SMOOTH_LINE_WIDTH_GRANULARITY;
    if (name == "GL_ALIASED_LINE_WIDTH_RANGE") return GL_ALIASED_LINE_WIDTH_RANGE;
    if (name == "GL_TEXTURE0") return GL_TEXTURE0;
    if (name == "GL_TEXTURE1") return GL_TEXTURE1;
    if (name == "GL_TEXTURE2") return GL_TEXTURE2;
    if (name == "GL_TEXTURE3") return GL_TEXTURE3;
    if (name == "GL_TEXTURE4") return GL_TEXTURE4;
    if (name == "GL_TEXTURE5") return GL_TEXTURE5;
    if (name == "GL_TEXTURE6") return GL_TEXTURE6;
    if (name == "GL_TEXTURE7") return GL_TEXTURE7;
    if (name == "GL_TEXTURE8") return GL_TEXTURE8;
    if (name == "GL_TEXTURE9") return GL_TEXTURE9;
    if (name == "GL_TEXTURE10") return GL_TEXTURE10;
    if (name == "GL_TEXTURE11") return GL_TEXTURE11;
    if (name == "GL_TEXTURE12") return GL_TEXTURE12;
    if (name == "GL_TEXTURE13") return GL_TEXTURE13;
    if (name == "GL_TEXTURE14") return GL_TEXTURE14;
    if (name == "GL_TEXTURE15") return GL_TEXTURE15;
    if (name == "GL_TEXTURE16") return GL_TEXTURE16;
    if (name == "GL_TEXTURE17") return GL_TEXTURE17;
    if (name == "GL_TEXTURE18") return GL_TEXTURE18;
    if (name == "GL_TEXTURE19") return GL_TEXTURE19;
    if (name == "GL_TEXTURE20") return GL_TEXTURE20;
    if (name == "GL_TEXTURE21") return GL_TEXTURE21;
    if (name == "GL_TEXTURE22") return GL_TEXTURE22;
    if (name == "GL_TEXTURE23") return GL_TEXTURE23;
    if (name == "GL_TEXTURE24") return GL_TEXTURE24;
    if (name == "GL_TEXTURE25") return GL_TEXTURE25;
    if (name == "GL_TEXTURE26") return GL_TEXTURE26;
    if (name == "GL_TEXTURE27") return GL_TEXTURE27;
    if (name == "GL_TEXTURE28") return GL_TEXTURE28;
    if (name == "GL_TEXTURE29") return GL_TEXTURE29;
    if (name == "GL_TEXTURE30") return GL_TEXTURE30;
    if (name == "GL_TEXTURE31") return GL_TEXTURE31;
    if (name == "GL_ACTIVE_TEXTURE") return GL_ACTIVE_TEXTURE;
    if (name == "GL_MULTISAMPLE") return GL_MULTISAMPLE;
    if (name == "GL_SAMPLE_ALPHA_TO_COVERAGE") return GL_SAMPLE_ALPHA_TO_COVERAGE;
    if (name == "GL_SAMPLE_ALPHA_TO_ONE") return GL_SAMPLE_ALPHA_TO_ONE;
    if (name == "GL_SAMPLE_COVERAGE") return GL_SAMPLE_COVERAGE;
    if (name == "GL_SAMPLE_BUFFERS") return GL_SAMPLE_BUFFERS;
    if (name == "GL_SAMPLES") return GL_SAMPLES;
    if (name == "GL_SAMPLE_COVERAGE_VALUE") return GL_SAMPLE_COVERAGE_VALUE;
    if (name == "GL_SAMPLE_COVERAGE_INVERT") return GL_SAMPLE_COVERAGE_INVERT;
    if (name == "GL_TEXTURE_CUBE_MAP") return GL_TEXTURE_CUBE_MAP;
    if (name == "GL_TEXTURE_BINDING_CUBE_MAP") return GL_TEXTURE_BINDING_CUBE_MAP;
    if (name == "GL_TEXTURE_CUBE_MAP_POSITIVE_X") return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    if (name == "GL_TEXTURE_CUBE_MAP_NEGATIVE_X") return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    if (name == "GL_TEXTURE_CUBE_MAP_POSITIVE_Y") return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    if (name == "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y") return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    if (name == "GL_TEXTURE_CUBE_MAP_POSITIVE_Z") return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    if (name == "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z") return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    if (name == "GL_PROXY_TEXTURE_CUBE_MAP") return GL_PROXY_TEXTURE_CUBE_MAP;
    if (name == "GL_MAX_CUBE_MAP_TEXTURE_SIZE") return GL_MAX_CUBE_MAP_TEXTURE_SIZE;
    if (name == "GL_COMPRESSED_RGB") return GL_COMPRESSED_RGB;
    if (name == "GL_COMPRESSED_RGBA") return GL_COMPRESSED_RGBA;
    if (name == "GL_TEXTURE_COMPRESSION_HINT") return GL_TEXTURE_COMPRESSION_HINT;
    if (name == "GL_TEXTURE_COMPRESSED_IMAGE_SIZE") return GL_TEXTURE_COMPRESSED_IMAGE_SIZE;
    if (name == "GL_TEXTURE_COMPRESSED") return GL_TEXTURE_COMPRESSED;
    if (name == "GL_NUM_COMPRESSED_TEXTURE_FORMATS") return GL_NUM_COMPRESSED_TEXTURE_FORMATS;
    if (name == "GL_COMPRESSED_TEXTURE_FORMATS") return GL_COMPRESSED_TEXTURE_FORMATS;
    if (name == "GL_CLAMP_TO_BORDER") return GL_CLAMP_TO_BORDER;
    if (name == "GL_BLEND_DST_RGB") return GL_BLEND_DST_RGB;
    if (name == "GL_BLEND_SRC_RGB") return GL_BLEND_SRC_RGB;
    if (name == "GL_BLEND_DST_ALPHA") return GL_BLEND_DST_ALPHA;
    if (name == "GL_BLEND_SRC_ALPHA") return GL_BLEND_SRC_ALPHA;
    if (name == "GL_POINT_FADE_THRESHOLD_SIZE") return GL_POINT_FADE_THRESHOLD_SIZE;
    if (name == "GL_DEPTH_COMPONENT16") return GL_DEPTH_COMPONENT16;
    if (name == "GL_DEPTH_COMPONENT24") return GL_DEPTH_COMPONENT24;
    if (name == "GL_DEPTH_COMPONENT32") return GL_DEPTH_COMPONENT32;
    if (name == "GL_MIRRORED_REPEAT") return GL_MIRRORED_REPEAT;
    if (name == "GL_MAX_TEXTURE_LOD_BIAS") return GL_MAX_TEXTURE_LOD_BIAS;
    if (name == "GL_TEXTURE_LOD_BIAS") return GL_TEXTURE_LOD_BIAS;
    if (name == "GL_INCR_WRAP") return GL_INCR_WRAP;
    if (name == "GL_DECR_WRAP") return GL_DECR_WRAP;
    if (name == "GL_TEXTURE_DEPTH_SIZE") return GL_TEXTURE_DEPTH_SIZE;
    if (name == "GL_TEXTURE_COMPARE_MODE") return GL_TEXTURE_COMPARE_MODE;
    if (name == "GL_TEXTURE_COMPARE_FUNC") return GL_TEXTURE_COMPARE_FUNC;
    if (name == "GL_BLEND_COLOR") return GL_BLEND_COLOR;
    if (name == "GL_BLEND_EQUATION") return GL_BLEND_EQUATION;
    if (name == "GL_CONSTANT_COLOR") return GL_CONSTANT_COLOR;
    if (name == "GL_ONE_MINUS_CONSTANT_COLOR") return GL_ONE_MINUS_CONSTANT_COLOR;
    if (name == "GL_CONSTANT_ALPHA") return GL_CONSTANT_ALPHA;
    if (name == "GL_ONE_MINUS_CONSTANT_ALPHA") return GL_ONE_MINUS_CONSTANT_ALPHA;
    if (name == "GL_FUNC_ADD") return GL_FUNC_ADD;
    if (name == "GL_FUNC_REVERSE_SUBTRACT") return GL_FUNC_REVERSE_SUBTRACT;
    if (name == "GL_FUNC_SUBTRACT") return GL_FUNC_SUBTRACT;
    if (name == "GL_MIN") return GL_MIN;
    if (name == "GL_MAX") return GL_MAX;
    if (name == "GL_BUFFER_SIZE") return GL_BUFFER_SIZE;
    if (name == "GL_BUFFER_USAGE") return GL_BUFFER_USAGE;
    if (name == "GL_QUERY_COUNTER_BITS") return GL_QUERY_COUNTER_BITS;
    if (name == "GL_CURRENT_QUERY") return GL_CURRENT_QUERY;
    if (name == "GL_QUERY_RESULT") return GL_QUERY_RESULT;
    if (name == "GL_QUERY_RESULT_AVAILABLE") return GL_QUERY_RESULT_AVAILABLE;
    if (name == "GL_ARRAY_BUFFER") return GL_ARRAY_BUFFER;
    if (name == "GL_ELEMENT_ARRAY_BUFFER") return GL_ELEMENT_ARRAY_BUFFER;
    if (name == "GL_ARRAY_BUFFER_BINDING") return GL_ARRAY_BUFFER_BINDING;
    if (name == "GL_ELEMENT_ARRAY_BUFFER_BINDING") return GL_ELEMENT_ARRAY_BUFFER_BINDING;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING") return GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING;
    if (name == "GL_READ_ONLY") return GL_READ_ONLY;
    if (name == "GL_WRITE_ONLY") return GL_WRITE_ONLY;
    if (name == "GL_READ_WRITE") return GL_READ_WRITE;
    if (name == "GL_BUFFER_ACCESS") return GL_BUFFER_ACCESS;
    if (name == "GL_BUFFER_MAPPED") return GL_BUFFER_MAPPED;
    if (name == "GL_BUFFER_MAP_POINTER") return GL_BUFFER_MAP_POINTER;
    if (name == "GL_STREAM_DRAW") return GL_STREAM_DRAW;
    if (name == "GL_STREAM_READ") return GL_STREAM_READ;
    if (name == "GL_STREAM_COPY") return GL_STREAM_COPY;
    if (name == "GL_STATIC_DRAW") return GL_STATIC_DRAW;
    if (name == "GL_STATIC_READ") return GL_STATIC_READ;
    if (name == "GL_STATIC_COPY") return GL_STATIC_COPY;
    if (name == "GL_DYNAMIC_DRAW") return GL_DYNAMIC_DRAW;
    if (name == "GL_DYNAMIC_READ") return GL_DYNAMIC_READ;
    if (name == "GL_DYNAMIC_COPY") return GL_DYNAMIC_COPY;
    if (name == "GL_SAMPLES_PASSED") return GL_SAMPLES_PASSED;
    if (name == "GL_SRC1_ALPHA") return GL_SRC1_ALPHA;
    if (name == "GL_BLEND_EQUATION_RGB") return GL_BLEND_EQUATION_RGB;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_ENABLED") return GL_VERTEX_ATTRIB_ARRAY_ENABLED;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_SIZE") return GL_VERTEX_ATTRIB_ARRAY_SIZE;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_STRIDE") return GL_VERTEX_ATTRIB_ARRAY_STRIDE;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_TYPE") return GL_VERTEX_ATTRIB_ARRAY_TYPE;
    if (name == "GL_CURRENT_VERTEX_ATTRIB") return GL_CURRENT_VERTEX_ATTRIB;
    if (name == "GL_VERTEX_PROGRAM_POINT_SIZE") return GL_VERTEX_PROGRAM_POINT_SIZE;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_POINTER") return GL_VERTEX_ATTRIB_ARRAY_POINTER;
    if (name == "GL_STENCIL_BACK_FUNC") return GL_STENCIL_BACK_FUNC;
    if (name == "GL_STENCIL_BACK_FAIL") return GL_STENCIL_BACK_FAIL;
    if (name == "GL_STENCIL_BACK_PASS_DEPTH_FAIL") return GL_STENCIL_BACK_PASS_DEPTH_FAIL;
    if (name == "GL_STENCIL_BACK_PASS_DEPTH_PASS") return GL_STENCIL_BACK_PASS_DEPTH_PASS;
    if (name == "GL_MAX_DRAW_BUFFERS") return GL_MAX_DRAW_BUFFERS;
    if (name == "GL_DRAW_BUFFER0") return GL_DRAW_BUFFER0;
    if (name == "GL_DRAW_BUFFER1") return GL_DRAW_BUFFER1;
    if (name == "GL_DRAW_BUFFER2") return GL_DRAW_BUFFER2;
    if (name == "GL_DRAW_BUFFER3") return GL_DRAW_BUFFER3;
    if (name == "GL_DRAW_BUFFER4") return GL_DRAW_BUFFER4;
    if (name == "GL_DRAW_BUFFER5") return GL_DRAW_BUFFER5;
    if (name == "GL_DRAW_BUFFER6") return GL_DRAW_BUFFER6;
    if (name == "GL_DRAW_BUFFER7") return GL_DRAW_BUFFER7;
    if (name == "GL_DRAW_BUFFER8") return GL_DRAW_BUFFER8;
    if (name == "GL_DRAW_BUFFER9") return GL_DRAW_BUFFER9;
    if (name == "GL_DRAW_BUFFER10") return GL_DRAW_BUFFER10;
    if (name == "GL_DRAW_BUFFER11") return GL_DRAW_BUFFER11;
    if (name == "GL_DRAW_BUFFER12") return GL_DRAW_BUFFER12;
    if (name == "GL_DRAW_BUFFER13") return GL_DRAW_BUFFER13;
    if (name == "GL_DRAW_BUFFER14") return GL_DRAW_BUFFER14;
    if (name == "GL_DRAW_BUFFER15") return GL_DRAW_BUFFER15;
    if (name == "GL_BLEND_EQUATION_ALPHA") return GL_BLEND_EQUATION_ALPHA;
    if (name == "GL_MAX_VERTEX_ATTRIBS") return GL_MAX_VERTEX_ATTRIBS;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED") return GL_VERTEX_ATTRIB_ARRAY_NORMALIZED;
    if (name == "GL_MAX_TEXTURE_IMAGE_UNITS") return GL_MAX_TEXTURE_IMAGE_UNITS;
    if (name == "GL_FRAGMENT_SHADER") return GL_FRAGMENT_SHADER;
    if (name == "GL_VERTEX_SHADER") return GL_VERTEX_SHADER;
    if (name == "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS") return GL_MAX_FRAGMENT_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_VERTEX_UNIFORM_COMPONENTS") return GL_MAX_VERTEX_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_VARYING_FLOATS") return GL_MAX_VARYING_FLOATS;
    if (name == "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS") return GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS;
    if (name == "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS") return GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
    if (name == "GL_SHADER_TYPE") return GL_SHADER_TYPE;
    if (name == "GL_FLOAT_VEC2") return GL_FLOAT_VEC2;
    if (name == "GL_FLOAT_VEC3") return GL_FLOAT_VEC3;
    if (name == "GL_FLOAT_VEC4") return GL_FLOAT_VEC4;
    if (name == "GL_INT_VEC2") return GL_INT_VEC2;
    if (name == "GL_INT_VEC3") return GL_INT_VEC3;
    if (name == "GL_INT_VEC4") return GL_INT_VEC4;
    if (name == "GL_BOOL") return GL_BOOL;
    if (name == "GL_BOOL_VEC2") return GL_BOOL_VEC2;
    if (name == "GL_BOOL_VEC3") return GL_BOOL_VEC3;
    if (name == "GL_BOOL_VEC4") return GL_BOOL_VEC4;
    if (name == "GL_FLOAT_MAT2") return GL_FLOAT_MAT2;
    if (name == "GL_FLOAT_MAT3") return GL_FLOAT_MAT3;
    if (name == "GL_FLOAT_MAT4") return GL_FLOAT_MAT4;
    if (name == "GL_SAMPLER_1D") return GL_SAMPLER_1D;
    if (name == "GL_SAMPLER_2D") return GL_SAMPLER_2D;
    if (name == "GL_SAMPLER_3D") return GL_SAMPLER_3D;
    if (name == "GL_SAMPLER_CUBE") return GL_SAMPLER_CUBE;
    if (name == "GL_SAMPLER_1D_SHADOW") return GL_SAMPLER_1D_SHADOW;
    if (name == "GL_SAMPLER_2D_SHADOW") return GL_SAMPLER_2D_SHADOW;
    if (name == "GL_DELETE_STATUS") return GL_DELETE_STATUS;
    if (name == "GL_COMPILE_STATUS") return GL_COMPILE_STATUS;
    if (name == "GL_LINK_STATUS") return GL_LINK_STATUS;
    if (name == "GL_VALIDATE_STATUS") return GL_VALIDATE_STATUS;
    if (name == "GL_INFO_LOG_LENGTH") return GL_INFO_LOG_LENGTH;
    if (name == "GL_ATTACHED_SHADERS") return GL_ATTACHED_SHADERS;
    if (name == "GL_ACTIVE_UNIFORMS") return GL_ACTIVE_UNIFORMS;
    if (name == "GL_ACTIVE_UNIFORM_MAX_LENGTH") return GL_ACTIVE_UNIFORM_MAX_LENGTH;
    if (name == "GL_SHADER_SOURCE_LENGTH") return GL_SHADER_SOURCE_LENGTH;
    if (name == "GL_ACTIVE_ATTRIBUTES") return GL_ACTIVE_ATTRIBUTES;
    if (name == "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH") return GL_ACTIVE_ATTRIBUTE_MAX_LENGTH;
    if (name == "GL_FRAGMENT_SHADER_DERIVATIVE_HINT") return GL_FRAGMENT_SHADER_DERIVATIVE_HINT;
    if (name == "GL_SHADING_LANGUAGE_VERSION") return GL_SHADING_LANGUAGE_VERSION;
    if (name == "GL_CURRENT_PROGRAM") return GL_CURRENT_PROGRAM;
    if (name == "GL_POINT_SPRITE_COORD_ORIGIN") return GL_POINT_SPRITE_COORD_ORIGIN;
    if (name == "GL_LOWER_LEFT") return GL_LOWER_LEFT;
    if (name == "GL_UPPER_LEFT") return GL_UPPER_LEFT;
    if (name == "GL_STENCIL_BACK_REF") return GL_STENCIL_BACK_REF;
    if (name == "GL_STENCIL_BACK_VALUE_MASK") return GL_STENCIL_BACK_VALUE_MASK;
    if (name == "GL_STENCIL_BACK_WRITEMASK") return GL_STENCIL_BACK_WRITEMASK;
    if (name == "GL_PIXEL_PACK_BUFFER") return GL_PIXEL_PACK_BUFFER;
    if (name == "GL_PIXEL_UNPACK_BUFFER") return GL_PIXEL_UNPACK_BUFFER;
    if (name == "GL_PIXEL_PACK_BUFFER_BINDING") return GL_PIXEL_PACK_BUFFER_BINDING;
    if (name == "GL_PIXEL_UNPACK_BUFFER_BINDING") return GL_PIXEL_UNPACK_BUFFER_BINDING;
    if (name == "GL_FLOAT_MAT2x3") return GL_FLOAT_MAT2x3;
    if (name == "GL_FLOAT_MAT2x4") return GL_FLOAT_MAT2x4;
    if (name == "GL_FLOAT_MAT3x2") return GL_FLOAT_MAT3x2;
    if (name == "GL_FLOAT_MAT3x4") return GL_FLOAT_MAT3x4;
    if (name == "GL_FLOAT_MAT4x2") return GL_FLOAT_MAT4x2;
    if (name == "GL_FLOAT_MAT4x3") return GL_FLOAT_MAT4x3;
    if (name == "GL_SRGB") return GL_SRGB;
    if (name == "GL_SRGB8") return GL_SRGB8;
    if (name == "GL_SRGB_ALPHA") return GL_SRGB_ALPHA;
    if (name == "GL_SRGB8_ALPHA8") return GL_SRGB8_ALPHA8;
    if (name == "GL_COMPRESSED_SRGB") return GL_COMPRESSED_SRGB;
    if (name == "GL_COMPRESSED_SRGB_ALPHA") return GL_COMPRESSED_SRGB_ALPHA;
    if (name == "GL_COMPARE_REF_TO_TEXTURE") return GL_COMPARE_REF_TO_TEXTURE;
    if (name == "GL_CLIP_DISTANCE0") return GL_CLIP_DISTANCE0;
    if (name == "GL_CLIP_DISTANCE1") return GL_CLIP_DISTANCE1;
    if (name == "GL_CLIP_DISTANCE2") return GL_CLIP_DISTANCE2;
    if (name == "GL_CLIP_DISTANCE3") return GL_CLIP_DISTANCE3;
    if (name == "GL_CLIP_DISTANCE4") return GL_CLIP_DISTANCE4;
    if (name == "GL_CLIP_DISTANCE5") return GL_CLIP_DISTANCE5;
    if (name == "GL_CLIP_DISTANCE6") return GL_CLIP_DISTANCE6;
    if (name == "GL_CLIP_DISTANCE7") return GL_CLIP_DISTANCE7;
    if (name == "GL_MAX_CLIP_DISTANCES") return GL_MAX_CLIP_DISTANCES;
    if (name == "GL_MAJOR_VERSION") return GL_MAJOR_VERSION;
    if (name == "GL_MINOR_VERSION") return GL_MINOR_VERSION;
    if (name == "GL_NUM_EXTENSIONS") return GL_NUM_EXTENSIONS;
    if (name == "GL_CONTEXT_FLAGS") return GL_CONTEXT_FLAGS;
    if (name == "GL_COMPRESSED_RED") return GL_COMPRESSED_RED;
    if (name == "GL_COMPRESSED_RG") return GL_COMPRESSED_RG;
    if (name == "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT") return GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT;
    if (name == "GL_RGBA32F") return GL_RGBA32F;
    if (name == "GL_RGB32F") return GL_RGB32F;
    if (name == "GL_RGBA16F") return GL_RGBA16F;
    if (name == "GL_RGB16F") return GL_RGB16F;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_INTEGER") return GL_VERTEX_ATTRIB_ARRAY_INTEGER;
    if (name == "GL_MAX_ARRAY_TEXTURE_LAYERS") return GL_MAX_ARRAY_TEXTURE_LAYERS;
    if (name == "GL_MIN_PROGRAM_TEXEL_OFFSET") return GL_MIN_PROGRAM_TEXEL_OFFSET;
    if (name == "GL_MAX_PROGRAM_TEXEL_OFFSET") return GL_MAX_PROGRAM_TEXEL_OFFSET;
    if (name == "GL_CLAMP_READ_COLOR") return GL_CLAMP_READ_COLOR;
    if (name == "GL_FIXED_ONLY") return GL_FIXED_ONLY;
    if (name == "GL_MAX_VARYING_COMPONENTS") return GL_MAX_VARYING_COMPONENTS;
    if (name == "GL_TEXTURE_1D_ARRAY") return GL_TEXTURE_1D_ARRAY;
    if (name == "GL_PROXY_TEXTURE_1D_ARRAY") return GL_PROXY_TEXTURE_1D_ARRAY;
    if (name == "GL_TEXTURE_2D_ARRAY") return GL_TEXTURE_2D_ARRAY;
    if (name == "GL_PROXY_TEXTURE_2D_ARRAY") return GL_PROXY_TEXTURE_2D_ARRAY;
    if (name == "GL_TEXTURE_BINDING_1D_ARRAY") return GL_TEXTURE_BINDING_1D_ARRAY;
    if (name == "GL_TEXTURE_BINDING_2D_ARRAY") return GL_TEXTURE_BINDING_2D_ARRAY;
    if (name == "GL_R11F_G11F_B10F") return GL_R11F_G11F_B10F;
    if (name == "GL_UNSIGNED_INT_10F_11F_11F_REV") return GL_UNSIGNED_INT_10F_11F_11F_REV;
    if (name == "GL_RGB9_E5") return GL_RGB9_E5;
    if (name == "GL_UNSIGNED_INT_5_9_9_9_REV") return GL_UNSIGNED_INT_5_9_9_9_REV;
    if (name == "GL_TEXTURE_SHARED_SIZE") return GL_TEXTURE_SHARED_SIZE;
    if (name == "GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH") return GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_MODE") return GL_TRANSFORM_FEEDBACK_BUFFER_MODE;
    if (name == "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS") return GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS;
    if (name == "GL_TRANSFORM_FEEDBACK_VARYINGS") return GL_TRANSFORM_FEEDBACK_VARYINGS;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_START") return GL_TRANSFORM_FEEDBACK_BUFFER_START;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_SIZE") return GL_TRANSFORM_FEEDBACK_BUFFER_SIZE;
    if (name == "GL_PRIMITIVES_GENERATED") return GL_PRIMITIVES_GENERATED;
    if (name == "GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN") return GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN;
    if (name == "GL_RASTERIZER_DISCARD") return GL_RASTERIZER_DISCARD;
    if (name == "GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS")
        return GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS;
    if (name == "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS") return GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS;
    if (name == "GL_INTERLEAVED_ATTRIBS") return GL_INTERLEAVED_ATTRIBS;
    if (name == "GL_SEPARATE_ATTRIBS") return GL_SEPARATE_ATTRIBS;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER") return GL_TRANSFORM_FEEDBACK_BUFFER;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_BINDING") return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
    if (name == "GL_RGBA32UI") return GL_RGBA32UI;
    if (name == "GL_RGB32UI") return GL_RGB32UI;
    if (name == "GL_RGBA16UI") return GL_RGBA16UI;
    if (name == "GL_RGB16UI") return GL_RGB16UI;
    if (name == "GL_RGBA8UI") return GL_RGBA8UI;
    if (name == "GL_RGB8UI") return GL_RGB8UI;
    if (name == "GL_RGBA32I") return GL_RGBA32I;
    if (name == "GL_RGB32I") return GL_RGB32I;
    if (name == "GL_RGBA16I") return GL_RGBA16I;
    if (name == "GL_RGB16I") return GL_RGB16I;
    if (name == "GL_RGBA8I") return GL_RGBA8I;
    if (name == "GL_RGB8I") return GL_RGB8I;
    if (name == "GL_RED_INTEGER") return GL_RED_INTEGER;
    if (name == "GL_GREEN_INTEGER") return GL_GREEN_INTEGER;
    if (name == "GL_BLUE_INTEGER") return GL_BLUE_INTEGER;
    if (name == "GL_RGB_INTEGER") return GL_RGB_INTEGER;
    if (name == "GL_RGBA_INTEGER") return GL_RGBA_INTEGER;
    if (name == "GL_BGR_INTEGER") return GL_BGR_INTEGER;
    if (name == "GL_BGRA_INTEGER") return GL_BGRA_INTEGER;
    if (name == "GL_SAMPLER_1D_ARRAY") return GL_SAMPLER_1D_ARRAY;
    if (name == "GL_SAMPLER_2D_ARRAY") return GL_SAMPLER_2D_ARRAY;
    if (name == "GL_SAMPLER_1D_ARRAY_SHADOW") return GL_SAMPLER_1D_ARRAY_SHADOW;
    if (name == "GL_SAMPLER_2D_ARRAY_SHADOW") return GL_SAMPLER_2D_ARRAY_SHADOW;
    if (name == "GL_SAMPLER_CUBE_SHADOW") return GL_SAMPLER_CUBE_SHADOW;
    if (name == "GL_UNSIGNED_INT_VEC2") return GL_UNSIGNED_INT_VEC2;
    if (name == "GL_UNSIGNED_INT_VEC3") return GL_UNSIGNED_INT_VEC3;
    if (name == "GL_UNSIGNED_INT_VEC4") return GL_UNSIGNED_INT_VEC4;
    if (name == "GL_INT_SAMPLER_1D") return GL_INT_SAMPLER_1D;
    if (name == "GL_INT_SAMPLER_2D") return GL_INT_SAMPLER_2D;
    if (name == "GL_INT_SAMPLER_3D") return GL_INT_SAMPLER_3D;
    if (name == "GL_INT_SAMPLER_CUBE") return GL_INT_SAMPLER_CUBE;
    if (name == "GL_INT_SAMPLER_1D_ARRAY") return GL_INT_SAMPLER_1D_ARRAY;
    if (name == "GL_INT_SAMPLER_2D_ARRAY") return GL_INT_SAMPLER_2D_ARRAY;
    if (name == "GL_UNSIGNED_INT_SAMPLER_1D") return GL_UNSIGNED_INT_SAMPLER_1D;
    if (name == "GL_UNSIGNED_INT_SAMPLER_2D") return GL_UNSIGNED_INT_SAMPLER_2D;
    if (name == "GL_UNSIGNED_INT_SAMPLER_3D") return GL_UNSIGNED_INT_SAMPLER_3D;
    if (name == "GL_UNSIGNED_INT_SAMPLER_CUBE") return GL_UNSIGNED_INT_SAMPLER_CUBE;
    if (name == "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY") return GL_UNSIGNED_INT_SAMPLER_1D_ARRAY;
    if (name == "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY") return GL_UNSIGNED_INT_SAMPLER_2D_ARRAY;
    if (name == "GL_QUERY_WAIT") return GL_QUERY_WAIT;
    if (name == "GL_QUERY_NO_WAIT") return GL_QUERY_NO_WAIT;
    if (name == "GL_QUERY_BY_REGION_WAIT") return GL_QUERY_BY_REGION_WAIT;
    if (name == "GL_QUERY_BY_REGION_NO_WAIT") return GL_QUERY_BY_REGION_NO_WAIT;
    if (name == "GL_BUFFER_ACCESS_FLAGS") return GL_BUFFER_ACCESS_FLAGS;
    if (name == "GL_BUFFER_MAP_LENGTH") return GL_BUFFER_MAP_LENGTH;
    if (name == "GL_BUFFER_MAP_OFFSET") return GL_BUFFER_MAP_OFFSET;
    if (name == "GL_DEPTH_COMPONENT32F") return GL_DEPTH_COMPONENT32F;
    if (name == "GL_DEPTH32F_STENCIL8") return GL_DEPTH32F_STENCIL8;
    if (name == "GL_FLOAT_32_UNSIGNED_INT_24_8_REV") return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
    if (name == "GL_INVALID_FRAMEBUFFER_OPERATION") return GL_INVALID_FRAMEBUFFER_OPERATION;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING") return GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE") return GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE") return GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE") return GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE") return GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE") return GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE") return GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE") return GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE;
    if (name == "GL_FRAMEBUFFER_DEFAULT") return GL_FRAMEBUFFER_DEFAULT;
    if (name == "GL_FRAMEBUFFER_UNDEFINED") return GL_FRAMEBUFFER_UNDEFINED;
    if (name == "GL_DEPTH_STENCIL_ATTACHMENT") return GL_DEPTH_STENCIL_ATTACHMENT;
    if (name == "GL_MAX_RENDERBUFFER_SIZE") return GL_MAX_RENDERBUFFER_SIZE;
    if (name == "GL_DEPTH_STENCIL") return GL_DEPTH_STENCIL;
    if (name == "GL_UNSIGNED_INT_24_8") return GL_UNSIGNED_INT_24_8;
    if (name == "GL_DEPTH24_STENCIL8") return GL_DEPTH24_STENCIL8;
    if (name == "GL_TEXTURE_STENCIL_SIZE") return GL_TEXTURE_STENCIL_SIZE;
    if (name == "GL_TEXTURE_RED_TYPE") return GL_TEXTURE_RED_TYPE;
    if (name == "GL_TEXTURE_GREEN_TYPE") return GL_TEXTURE_GREEN_TYPE;
    if (name == "GL_TEXTURE_BLUE_TYPE") return GL_TEXTURE_BLUE_TYPE;
    if (name == "GL_TEXTURE_ALPHA_TYPE") return GL_TEXTURE_ALPHA_TYPE;
    if (name == "GL_TEXTURE_DEPTH_TYPE") return GL_TEXTURE_DEPTH_TYPE;
    if (name == "GL_UNSIGNED_NORMALIZED") return GL_UNSIGNED_NORMALIZED;
    if (name == "GL_FRAMEBUFFER_BINDING") return GL_FRAMEBUFFER_BINDING;
    if (name == "GL_DRAW_FRAMEBUFFER_BINDING") return GL_DRAW_FRAMEBUFFER_BINDING;
    if (name == "GL_RENDERBUFFER_BINDING") return GL_RENDERBUFFER_BINDING;
    if (name == "GL_READ_FRAMEBUFFER") return GL_READ_FRAMEBUFFER;
    if (name == "GL_DRAW_FRAMEBUFFER") return GL_DRAW_FRAMEBUFFER;
    if (name == "GL_READ_FRAMEBUFFER_BINDING") return GL_READ_FRAMEBUFFER_BINDING;
    if (name == "GL_RENDERBUFFER_SAMPLES") return GL_RENDERBUFFER_SAMPLES;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE") return GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME") return GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL") return GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE")
        return GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER") return GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER;
    if (name == "GL_FRAMEBUFFER_COMPLETE") return GL_FRAMEBUFFER_COMPLETE;
    if (name == "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT") return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
    if (name == "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT") return GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
    if (name == "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER") return GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER;
    if (name == "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER") return GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER;
    if (name == "GL_FRAMEBUFFER_UNSUPPORTED") return GL_FRAMEBUFFER_UNSUPPORTED;
    if (name == "GL_MAX_COLOR_ATTACHMENTS") return GL_MAX_COLOR_ATTACHMENTS;
    if (name == "GL_COLOR_ATTACHMENT0") return GL_COLOR_ATTACHMENT0;
    if (name == "GL_COLOR_ATTACHMENT1") return GL_COLOR_ATTACHMENT1;
    if (name == "GL_COLOR_ATTACHMENT2") return GL_COLOR_ATTACHMENT2;
    if (name == "GL_COLOR_ATTACHMENT3") return GL_COLOR_ATTACHMENT3;
    if (name == "GL_COLOR_ATTACHMENT4") return GL_COLOR_ATTACHMENT4;
    if (name == "GL_COLOR_ATTACHMENT5") return GL_COLOR_ATTACHMENT5;
    if (name == "GL_COLOR_ATTACHMENT6") return GL_COLOR_ATTACHMENT6;
    if (name == "GL_COLOR_ATTACHMENT7") return GL_COLOR_ATTACHMENT7;
    if (name == "GL_COLOR_ATTACHMENT8") return GL_COLOR_ATTACHMENT8;
    if (name == "GL_COLOR_ATTACHMENT9") return GL_COLOR_ATTACHMENT9;
    if (name == "GL_COLOR_ATTACHMENT10") return GL_COLOR_ATTACHMENT10;
    if (name == "GL_COLOR_ATTACHMENT11") return GL_COLOR_ATTACHMENT11;
    if (name == "GL_COLOR_ATTACHMENT12") return GL_COLOR_ATTACHMENT12;
    if (name == "GL_COLOR_ATTACHMENT13") return GL_COLOR_ATTACHMENT13;
    if (name == "GL_COLOR_ATTACHMENT14") return GL_COLOR_ATTACHMENT14;
    if (name == "GL_COLOR_ATTACHMENT15") return GL_COLOR_ATTACHMENT15;
    if (name == "GL_COLOR_ATTACHMENT16") return GL_COLOR_ATTACHMENT16;
    if (name == "GL_COLOR_ATTACHMENT17") return GL_COLOR_ATTACHMENT17;
    if (name == "GL_COLOR_ATTACHMENT18") return GL_COLOR_ATTACHMENT18;
    if (name == "GL_COLOR_ATTACHMENT19") return GL_COLOR_ATTACHMENT19;
    if (name == "GL_COLOR_ATTACHMENT20") return GL_COLOR_ATTACHMENT20;
    if (name == "GL_COLOR_ATTACHMENT21") return GL_COLOR_ATTACHMENT21;
    if (name == "GL_COLOR_ATTACHMENT22") return GL_COLOR_ATTACHMENT22;
    if (name == "GL_COLOR_ATTACHMENT23") return GL_COLOR_ATTACHMENT23;
    if (name == "GL_COLOR_ATTACHMENT24") return GL_COLOR_ATTACHMENT24;
    if (name == "GL_COLOR_ATTACHMENT25") return GL_COLOR_ATTACHMENT25;
    if (name == "GL_COLOR_ATTACHMENT26") return GL_COLOR_ATTACHMENT26;
    if (name == "GL_COLOR_ATTACHMENT27") return GL_COLOR_ATTACHMENT27;
    if (name == "GL_COLOR_ATTACHMENT28") return GL_COLOR_ATTACHMENT28;
    if (name == "GL_COLOR_ATTACHMENT29") return GL_COLOR_ATTACHMENT29;
    if (name == "GL_COLOR_ATTACHMENT30") return GL_COLOR_ATTACHMENT30;
    if (name == "GL_COLOR_ATTACHMENT31") return GL_COLOR_ATTACHMENT31;
    if (name == "GL_DEPTH_ATTACHMENT") return GL_DEPTH_ATTACHMENT;
    if (name == "GL_STENCIL_ATTACHMENT") return GL_STENCIL_ATTACHMENT;
    if (name == "GL_FRAMEBUFFER") return GL_FRAMEBUFFER;
    if (name == "GL_RENDERBUFFER") return GL_RENDERBUFFER;
    if (name == "GL_RENDERBUFFER_WIDTH") return GL_RENDERBUFFER_WIDTH;
    if (name == "GL_RENDERBUFFER_HEIGHT") return GL_RENDERBUFFER_HEIGHT;
    if (name == "GL_RENDERBUFFER_INTERNAL_FORMAT") return GL_RENDERBUFFER_INTERNAL_FORMAT;
    if (name == "GL_STENCIL_INDEX1") return GL_STENCIL_INDEX1;
    if (name == "GL_STENCIL_INDEX4") return GL_STENCIL_INDEX4;
    if (name == "GL_STENCIL_INDEX8") return GL_STENCIL_INDEX8;
    if (name == "GL_STENCIL_INDEX16") return GL_STENCIL_INDEX16;
    if (name == "GL_RENDERBUFFER_RED_SIZE") return GL_RENDERBUFFER_RED_SIZE;
    if (name == "GL_RENDERBUFFER_GREEN_SIZE") return GL_RENDERBUFFER_GREEN_SIZE;
    if (name == "GL_RENDERBUFFER_BLUE_SIZE") return GL_RENDERBUFFER_BLUE_SIZE;
    if (name == "GL_RENDERBUFFER_ALPHA_SIZE") return GL_RENDERBUFFER_ALPHA_SIZE;
    if (name == "GL_RENDERBUFFER_DEPTH_SIZE") return GL_RENDERBUFFER_DEPTH_SIZE;
    if (name == "GL_RENDERBUFFER_STENCIL_SIZE") return GL_RENDERBUFFER_STENCIL_SIZE;
    if (name == "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE") return GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE;
    if (name == "GL_MAX_SAMPLES") return GL_MAX_SAMPLES;
    if (name == "GL_FRAMEBUFFER_SRGB") return GL_FRAMEBUFFER_SRGB;
    if (name == "GL_HALF_FLOAT") return GL_HALF_FLOAT;
    if (name == "GL_MAP_READ_BIT") return GL_MAP_READ_BIT;
    if (name == "GL_MAP_WRITE_BIT") return GL_MAP_WRITE_BIT;
    if (name == "GL_MAP_INVALIDATE_RANGE_BIT") return GL_MAP_INVALIDATE_RANGE_BIT;
    if (name == "GL_MAP_INVALIDATE_BUFFER_BIT") return GL_MAP_INVALIDATE_BUFFER_BIT;
    if (name == "GL_MAP_FLUSH_EXPLICIT_BIT") return GL_MAP_FLUSH_EXPLICIT_BIT;
    if (name == "GL_MAP_UNSYNCHRONIZED_BIT") return GL_MAP_UNSYNCHRONIZED_BIT;
    if (name == "GL_COMPRESSED_RED_RGTC1") return GL_COMPRESSED_RED_RGTC1;
    if (name == "GL_COMPRESSED_SIGNED_RED_RGTC1") return GL_COMPRESSED_SIGNED_RED_RGTC1;
    if (name == "GL_COMPRESSED_RG_RGTC2") return GL_COMPRESSED_RG_RGTC2;
    if (name == "GL_COMPRESSED_SIGNED_RG_RGTC2") return GL_COMPRESSED_SIGNED_RG_RGTC2;
    if (name == "GL_RG") return GL_RG;
    if (name == "GL_RG_INTEGER") return GL_RG_INTEGER;
    if (name == "GL_R8") return GL_R8;
    if (name == "GL_R16") return GL_R16;
    if (name == "GL_RG8") return GL_RG8;
    if (name == "GL_RG16") return GL_RG16;
    if (name == "GL_R16F") return GL_R16F;
    if (name == "GL_R32F") return GL_R32F;
    if (name == "GL_RG16F") return GL_RG16F;
    if (name == "GL_RG32F") return GL_RG32F;
    if (name == "GL_R8I") return GL_R8I;
    if (name == "GL_R8UI") return GL_R8UI;
    if (name == "GL_R16I") return GL_R16I;
    if (name == "GL_R16UI") return GL_R16UI;
    if (name == "GL_R32I") return GL_R32I;
    if (name == "GL_R32UI") return GL_R32UI;
    if (name == "GL_RG8I") return GL_RG8I;
    if (name == "GL_RG8UI") return GL_RG8UI;
    if (name == "GL_RG16I") return GL_RG16I;
    if (name == "GL_RG16UI") return GL_RG16UI;
    if (name == "GL_RG32I") return GL_RG32I;
    if (name == "GL_RG32UI") return GL_RG32UI;
    if (name == "GL_VERTEX_ARRAY_BINDING") return GL_VERTEX_ARRAY_BINDING;
    if (name == "GL_SAMPLER_2D_RECT") return GL_SAMPLER_2D_RECT;
    if (name == "GL_SAMPLER_2D_RECT_SHADOW") return GL_SAMPLER_2D_RECT_SHADOW;
    if (name == "GL_SAMPLER_BUFFER") return GL_SAMPLER_BUFFER;
    if (name == "GL_INT_SAMPLER_2D_RECT") return GL_INT_SAMPLER_2D_RECT;
    if (name == "GL_INT_SAMPLER_BUFFER") return GL_INT_SAMPLER_BUFFER;
    if (name == "GL_UNSIGNED_INT_SAMPLER_2D_RECT") return GL_UNSIGNED_INT_SAMPLER_2D_RECT;
    if (name == "GL_UNSIGNED_INT_SAMPLER_BUFFER") return GL_UNSIGNED_INT_SAMPLER_BUFFER;
    if (name == "GL_TEXTURE_BUFFER") return GL_TEXTURE_BUFFER;
    if (name == "GL_MAX_TEXTURE_BUFFER_SIZE") return GL_MAX_TEXTURE_BUFFER_SIZE;
    if (name == "GL_TEXTURE_BINDING_BUFFER") return GL_TEXTURE_BINDING_BUFFER;
    if (name == "GL_TEXTURE_BUFFER_DATA_STORE_BINDING") return GL_TEXTURE_BUFFER_DATA_STORE_BINDING;
    if (name == "GL_TEXTURE_RECTANGLE") return GL_TEXTURE_RECTANGLE;
    if (name == "GL_TEXTURE_BINDING_RECTANGLE") return GL_TEXTURE_BINDING_RECTANGLE;
    if (name == "GL_PROXY_TEXTURE_RECTANGLE") return GL_PROXY_TEXTURE_RECTANGLE;
    if (name == "GL_MAX_RECTANGLE_TEXTURE_SIZE") return GL_MAX_RECTANGLE_TEXTURE_SIZE;
    if (name == "GL_R8_SNORM") return GL_R8_SNORM;
    if (name == "GL_RG8_SNORM") return GL_RG8_SNORM;
    if (name == "GL_RGB8_SNORM") return GL_RGB8_SNORM;
    if (name == "GL_RGBA8_SNORM") return GL_RGBA8_SNORM;
    if (name == "GL_R16_SNORM") return GL_R16_SNORM;
    if (name == "GL_RG16_SNORM") return GL_RG16_SNORM;
    if (name == "GL_RGB16_SNORM") return GL_RGB16_SNORM;
    if (name == "GL_RGBA16_SNORM") return GL_RGBA16_SNORM;
    if (name == "GL_SIGNED_NORMALIZED") return GL_SIGNED_NORMALIZED;
    if (name == "GL_PRIMITIVE_RESTART") return GL_PRIMITIVE_RESTART;
    if (name == "GL_PRIMITIVE_RESTART_INDEX") return GL_PRIMITIVE_RESTART_INDEX;
    if (name == "GL_COPY_READ_BUFFER") return GL_COPY_READ_BUFFER;
    if (name == "GL_COPY_WRITE_BUFFER") return GL_COPY_WRITE_BUFFER;
    if (name == "GL_UNIFORM_BUFFER") return GL_UNIFORM_BUFFER;
    if (name == "GL_UNIFORM_BUFFER_BINDING") return GL_UNIFORM_BUFFER_BINDING;
    if (name == "GL_UNIFORM_BUFFER_START") return GL_UNIFORM_BUFFER_START;
    if (name == "GL_UNIFORM_BUFFER_SIZE") return GL_UNIFORM_BUFFER_SIZE;
    if (name == "GL_MAX_VERTEX_UNIFORM_BLOCKS") return GL_MAX_VERTEX_UNIFORM_BLOCKS;
    if (name == "GL_MAX_GEOMETRY_UNIFORM_BLOCKS") return GL_MAX_GEOMETRY_UNIFORM_BLOCKS;
    if (name == "GL_MAX_FRAGMENT_UNIFORM_BLOCKS") return GL_MAX_FRAGMENT_UNIFORM_BLOCKS;
    if (name == "GL_MAX_COMBINED_UNIFORM_BLOCKS") return GL_MAX_COMBINED_UNIFORM_BLOCKS;
    if (name == "GL_MAX_UNIFORM_BUFFER_BINDINGS") return GL_MAX_UNIFORM_BUFFER_BINDINGS;
    if (name == "GL_MAX_UNIFORM_BLOCK_SIZE") return GL_MAX_UNIFORM_BLOCK_SIZE;
    if (name == "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS") return GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS") return GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS") return GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS;
    if (name == "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT") return GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT;
    if (name == "GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH") return GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH;
    if (name == "GL_ACTIVE_UNIFORM_BLOCKS") return GL_ACTIVE_UNIFORM_BLOCKS;
    if (name == "GL_UNIFORM_TYPE") return GL_UNIFORM_TYPE;
    if (name == "GL_UNIFORM_SIZE") return GL_UNIFORM_SIZE;
    if (name == "GL_UNIFORM_NAME_LENGTH") return GL_UNIFORM_NAME_LENGTH;
    if (name == "GL_UNIFORM_BLOCK_INDEX") return GL_UNIFORM_BLOCK_INDEX;
    if (name == "GL_UNIFORM_OFFSET") return GL_UNIFORM_OFFSET;
    if (name == "GL_UNIFORM_ARRAY_STRIDE") return GL_UNIFORM_ARRAY_STRIDE;
    if (name == "GL_UNIFORM_MATRIX_STRIDE") return GL_UNIFORM_MATRIX_STRIDE;
    if (name == "GL_UNIFORM_IS_ROW_MAJOR") return GL_UNIFORM_IS_ROW_MAJOR;
    if (name == "GL_UNIFORM_BLOCK_BINDING") return GL_UNIFORM_BLOCK_BINDING;
    if (name == "GL_UNIFORM_BLOCK_DATA_SIZE") return GL_UNIFORM_BLOCK_DATA_SIZE;
    if (name == "GL_UNIFORM_BLOCK_NAME_LENGTH") return GL_UNIFORM_BLOCK_NAME_LENGTH;
    if (name == "GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS") return GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS;
    if (name == "GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES") return GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES;
    if (name == "GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER") return GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER;
    if (name == "GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER") return GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER;
    if (name == "GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER") return GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER;
    if (name == "GL_INVALID_INDEX") return GL_INVALID_INDEX;
    if (name == "GL_CONTEXT_CORE_PROFILE_BIT") return GL_CONTEXT_CORE_PROFILE_BIT;
    if (name == "GL_CONTEXT_COMPATIBILITY_PROFILE_BIT") return GL_CONTEXT_COMPATIBILITY_PROFILE_BIT;
    if (name == "GL_LINES_ADJACENCY") return GL_LINES_ADJACENCY;
    if (name == "GL_LINE_STRIP_ADJACENCY") return GL_LINE_STRIP_ADJACENCY;
    if (name == "GL_TRIANGLES_ADJACENCY") return GL_TRIANGLES_ADJACENCY;
    if (name == "GL_TRIANGLE_STRIP_ADJACENCY") return GL_TRIANGLE_STRIP_ADJACENCY;
    if (name == "GL_PROGRAM_POINT_SIZE") return GL_PROGRAM_POINT_SIZE;
    if (name == "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS") return GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS;
    if (name == "GL_FRAMEBUFFER_ATTACHMENT_LAYERED") return GL_FRAMEBUFFER_ATTACHMENT_LAYERED;
    if (name == "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS") return GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS;
    if (name == "GL_GEOMETRY_SHADER") return GL_GEOMETRY_SHADER;
    if (name == "GL_GEOMETRY_VERTICES_OUT") return GL_GEOMETRY_VERTICES_OUT;
    if (name == "GL_GEOMETRY_INPUT_TYPE") return GL_GEOMETRY_INPUT_TYPE;
    if (name == "GL_GEOMETRY_OUTPUT_TYPE") return GL_GEOMETRY_OUTPUT_TYPE;
    if (name == "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS") return GL_MAX_GEOMETRY_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_GEOMETRY_OUTPUT_VERTICES") return GL_MAX_GEOMETRY_OUTPUT_VERTICES;
    if (name == "GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS") return GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS;
    if (name == "GL_MAX_VERTEX_OUTPUT_COMPONENTS") return GL_MAX_VERTEX_OUTPUT_COMPONENTS;
    if (name == "GL_MAX_GEOMETRY_INPUT_COMPONENTS") return GL_MAX_GEOMETRY_INPUT_COMPONENTS;
    if (name == "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS") return GL_MAX_GEOMETRY_OUTPUT_COMPONENTS;
    if (name == "GL_MAX_FRAGMENT_INPUT_COMPONENTS") return GL_MAX_FRAGMENT_INPUT_COMPONENTS;
    if (name == "GL_CONTEXT_PROFILE_MASK") return GL_CONTEXT_PROFILE_MASK;
    if (name == "GL_DEPTH_CLAMP") return GL_DEPTH_CLAMP;
    if (name == "GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION") return GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION;
    if (name == "GL_FIRST_VERTEX_CONVENTION") return GL_FIRST_VERTEX_CONVENTION;
    if (name == "GL_LAST_VERTEX_CONVENTION") return GL_LAST_VERTEX_CONVENTION;
    if (name == "GL_PROVOKING_VERTEX") return GL_PROVOKING_VERTEX;
    if (name == "GL_TEXTURE_CUBE_MAP_SEAMLESS") return GL_TEXTURE_CUBE_MAP_SEAMLESS;
    if (name == "GL_MAX_SERVER_WAIT_TIMEOUT") return GL_MAX_SERVER_WAIT_TIMEOUT;
    if (name == "GL_OBJECT_TYPE") return GL_OBJECT_TYPE;
    if (name == "GL_SYNC_CONDITION") return GL_SYNC_CONDITION;
    if (name == "GL_SYNC_STATUS") return GL_SYNC_STATUS;
    if (name == "GL_SYNC_FLAGS") return GL_SYNC_FLAGS;
    if (name == "GL_SYNC_FENCE") return GL_SYNC_FENCE;
    if (name == "GL_SYNC_GPU_COMMANDS_COMPLETE") return GL_SYNC_GPU_COMMANDS_COMPLETE;
    if (name == "GL_UNSIGNALED") return GL_UNSIGNALED;
    if (name == "GL_SIGNALED") return GL_SIGNALED;
    if (name == "GL_ALREADY_SIGNALED") return GL_ALREADY_SIGNALED;
    if (name == "GL_TIMEOUT_EXPIRED") return GL_TIMEOUT_EXPIRED;
    if (name == "GL_CONDITION_SATISFIED") return GL_CONDITION_SATISFIED;
    if (name == "GL_WAIT_FAILED") return GL_WAIT_FAILED;
    if (name == "GL_TIMEOUT_IGNORED") return static_cast<GLenum>(GL_TIMEOUT_IGNORED);
    if (name == "GL_SYNC_FLUSH_COMMANDS_BIT") return GL_SYNC_FLUSH_COMMANDS_BIT;
    if (name == "GL_SAMPLE_POSITION") return GL_SAMPLE_POSITION;
    if (name == "GL_SAMPLE_MASK") return GL_SAMPLE_MASK;
    if (name == "GL_SAMPLE_MASK_VALUE") return GL_SAMPLE_MASK_VALUE;
    if (name == "GL_MAX_SAMPLE_MASK_WORDS") return GL_MAX_SAMPLE_MASK_WORDS;
    if (name == "GL_TEXTURE_2D_MULTISAMPLE") return GL_TEXTURE_2D_MULTISAMPLE;
    if (name == "GL_PROXY_TEXTURE_2D_MULTISAMPLE") return GL_PROXY_TEXTURE_2D_MULTISAMPLE;
    if (name == "GL_TEXTURE_2D_MULTISAMPLE_ARRAY") return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY") return GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_TEXTURE_BINDING_2D_MULTISAMPLE") return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
    if (name == "GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY") return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_TEXTURE_SAMPLES") return GL_TEXTURE_SAMPLES;
    if (name == "GL_TEXTURE_FIXED_SAMPLE_LOCATIONS") return GL_TEXTURE_FIXED_SAMPLE_LOCATIONS;
    if (name == "GL_SAMPLER_2D_MULTISAMPLE") return GL_SAMPLER_2D_MULTISAMPLE;
    if (name == "GL_INT_SAMPLER_2D_MULTISAMPLE") return GL_INT_SAMPLER_2D_MULTISAMPLE;
    if (name == "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE") return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE;
    if (name == "GL_SAMPLER_2D_MULTISAMPLE_ARRAY") return GL_SAMPLER_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY") return GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY") return GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_MAX_COLOR_TEXTURE_SAMPLES") return GL_MAX_COLOR_TEXTURE_SAMPLES;
    if (name == "GL_MAX_DEPTH_TEXTURE_SAMPLES") return GL_MAX_DEPTH_TEXTURE_SAMPLES;
    if (name == "GL_MAX_INTEGER_SAMPLES") return GL_MAX_INTEGER_SAMPLES;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_DIVISOR") return GL_VERTEX_ATTRIB_ARRAY_DIVISOR;
    if (name == "GL_SRC1_COLOR") return GL_SRC1_COLOR;
    if (name == "GL_ONE_MINUS_SRC1_COLOR") return GL_ONE_MINUS_SRC1_COLOR;
    if (name == "GL_ONE_MINUS_SRC1_ALPHA") return GL_ONE_MINUS_SRC1_ALPHA;
    if (name == "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS") return GL_MAX_DUAL_SOURCE_DRAW_BUFFERS;
    if (name == "GL_ANY_SAMPLES_PASSED") return GL_ANY_SAMPLES_PASSED;
    if (name == "GL_SAMPLER_BINDING") return GL_SAMPLER_BINDING;
    if (name == "GL_RGB10_A2UI") return GL_RGB10_A2UI;
    if (name == "GL_TEXTURE_SWIZZLE_R") return GL_TEXTURE_SWIZZLE_R;
    if (name == "GL_TEXTURE_SWIZZLE_G") return GL_TEXTURE_SWIZZLE_G;
    if (name == "GL_TEXTURE_SWIZZLE_B") return GL_TEXTURE_SWIZZLE_B;
    if (name == "GL_TEXTURE_SWIZZLE_A") return GL_TEXTURE_SWIZZLE_A;
    if (name == "GL_TEXTURE_SWIZZLE_RGBA") return GL_TEXTURE_SWIZZLE_RGBA;
    if (name == "GL_TIME_ELAPSED") return GL_TIME_ELAPSED;
    if (name == "GL_TIMESTAMP") return GL_TIMESTAMP;
    if (name == "GL_INT_2_10_10_10_REV") return GL_INT_2_10_10_10_REV;
    if (name == "GL_SAMPLE_SHADING") return GL_SAMPLE_SHADING;
    if (name == "GL_MIN_SAMPLE_SHADING_VALUE") return GL_MIN_SAMPLE_SHADING_VALUE;
    if (name == "GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET") return GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET;
    if (name == "GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET") return GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET;
    if (name == "GL_TEXTURE_CUBE_MAP_ARRAY") return GL_TEXTURE_CUBE_MAP_ARRAY;
    if (name == "GL_TEXTURE_BINDING_CUBE_MAP_ARRAY") return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
    if (name == "GL_PROXY_TEXTURE_CUBE_MAP_ARRAY") return GL_PROXY_TEXTURE_CUBE_MAP_ARRAY;
    if (name == "GL_SAMPLER_CUBE_MAP_ARRAY") return GL_SAMPLER_CUBE_MAP_ARRAY;
    if (name == "GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW") return GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW;
    if (name == "GL_INT_SAMPLER_CUBE_MAP_ARRAY") return GL_INT_SAMPLER_CUBE_MAP_ARRAY;
    if (name == "GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY") return GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY;
    if (name == "GL_DRAW_INDIRECT_BUFFER") return GL_DRAW_INDIRECT_BUFFER;
    if (name == "GL_DRAW_INDIRECT_BUFFER_BINDING") return GL_DRAW_INDIRECT_BUFFER_BINDING;
    if (name == "GL_GEOMETRY_SHADER_INVOCATIONS") return GL_GEOMETRY_SHADER_INVOCATIONS;
    if (name == "GL_MAX_GEOMETRY_SHADER_INVOCATIONS") return GL_MAX_GEOMETRY_SHADER_INVOCATIONS;
    if (name == "GL_MIN_FRAGMENT_INTERPOLATION_OFFSET") return GL_MIN_FRAGMENT_INTERPOLATION_OFFSET;
    if (name == "GL_MAX_FRAGMENT_INTERPOLATION_OFFSET") return GL_MAX_FRAGMENT_INTERPOLATION_OFFSET;
    if (name == "GL_FRAGMENT_INTERPOLATION_OFFSET_BITS") return GL_FRAGMENT_INTERPOLATION_OFFSET_BITS;
    if (name == "GL_MAX_VERTEX_STREAMS") return GL_MAX_VERTEX_STREAMS;
    if (name == "GL_DOUBLE_VEC2") return GL_DOUBLE_VEC2;
    if (name == "GL_DOUBLE_VEC3") return GL_DOUBLE_VEC3;
    if (name == "GL_DOUBLE_VEC4") return GL_DOUBLE_VEC4;
    if (name == "GL_DOUBLE_MAT2") return GL_DOUBLE_MAT2;
    if (name == "GL_DOUBLE_MAT3") return GL_DOUBLE_MAT3;
    if (name == "GL_DOUBLE_MAT4") return GL_DOUBLE_MAT4;
    if (name == "GL_DOUBLE_MAT2x3") return GL_DOUBLE_MAT2x3;
    if (name == "GL_DOUBLE_MAT2x4") return GL_DOUBLE_MAT2x4;
    if (name == "GL_DOUBLE_MAT3x2") return GL_DOUBLE_MAT3x2;
    if (name == "GL_DOUBLE_MAT3x4") return GL_DOUBLE_MAT3x4;
    if (name == "GL_DOUBLE_MAT4x2") return GL_DOUBLE_MAT4x2;
    if (name == "GL_DOUBLE_MAT4x3") return GL_DOUBLE_MAT4x3;
    if (name == "GL_ACTIVE_SUBROUTINES") return GL_ACTIVE_SUBROUTINES;
    if (name == "GL_ACTIVE_SUBROUTINE_UNIFORMS") return GL_ACTIVE_SUBROUTINE_UNIFORMS;
    if (name == "GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS") return GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS;
    if (name == "GL_ACTIVE_SUBROUTINE_MAX_LENGTH") return GL_ACTIVE_SUBROUTINE_MAX_LENGTH;
    if (name == "GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH") return GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH;
    if (name == "GL_MAX_SUBROUTINES") return GL_MAX_SUBROUTINES;
    if (name == "GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS") return GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS;
    if (name == "GL_NUM_COMPATIBLE_SUBROUTINES") return GL_NUM_COMPATIBLE_SUBROUTINES;
    if (name == "GL_COMPATIBLE_SUBROUTINES") return GL_COMPATIBLE_SUBROUTINES;
    if (name == "GL_PATCHES") return GL_PATCHES;
    if (name == "GL_PATCH_VERTICES") return GL_PATCH_VERTICES;
    if (name == "GL_PATCH_DEFAULT_INNER_LEVEL") return GL_PATCH_DEFAULT_INNER_LEVEL;
    if (name == "GL_PATCH_DEFAULT_OUTER_LEVEL") return GL_PATCH_DEFAULT_OUTER_LEVEL;
    if (name == "GL_TESS_CONTROL_OUTPUT_VERTICES") return GL_TESS_CONTROL_OUTPUT_VERTICES;
    if (name == "GL_TESS_GEN_MODE") return GL_TESS_GEN_MODE;
    if (name == "GL_TESS_GEN_SPACING") return GL_TESS_GEN_SPACING;
    if (name == "GL_TESS_GEN_VERTEX_ORDER") return GL_TESS_GEN_VERTEX_ORDER;
    if (name == "GL_TESS_GEN_POINT_MODE") return GL_TESS_GEN_POINT_MODE;
    if (name == "GL_ISOLINES") return GL_ISOLINES;
    if (name == "GL_QUADS") return GL_QUADS;
    if (name == "GL_FRACTIONAL_ODD") return GL_FRACTIONAL_ODD;
    if (name == "GL_FRACTIONAL_EVEN") return GL_FRACTIONAL_EVEN;
    if (name == "GL_MAX_PATCH_VERTICES") return GL_MAX_PATCH_VERTICES;
    if (name == "GL_MAX_TESS_GEN_LEVEL") return GL_MAX_TESS_GEN_LEVEL;
    if (name == "GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS") return GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS") return GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS") return GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS;
    if (name == "GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS") return GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS;
    if (name == "GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS") return GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS;
    if (name == "GL_MAX_TESS_PATCH_COMPONENTS") return GL_MAX_TESS_PATCH_COMPONENTS;
    if (name == "GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS") return GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS;
    if (name == "GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS") return GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS;
    if (name == "GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS") return GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS;
    if (name == "GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS") return GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS;
    if (name == "GL_MAX_TESS_CONTROL_INPUT_COMPONENTS") return GL_MAX_TESS_CONTROL_INPUT_COMPONENTS;
    if (name == "GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS") return GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS;
    if (name == "GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS")
        return GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS")
        return GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS;
    if (name == "GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER")
        return GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER;
    if (name == "GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER")
        return GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER;
    if (name == "GL_TESS_EVALUATION_SHADER") return GL_TESS_EVALUATION_SHADER;
    if (name == "GL_TESS_CONTROL_SHADER") return GL_TESS_CONTROL_SHADER;
    if (name == "GL_TRANSFORM_FEEDBACK") return GL_TRANSFORM_FEEDBACK;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED") return GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE") return GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE;
    if (name == "GL_TRANSFORM_FEEDBACK_BINDING") return GL_TRANSFORM_FEEDBACK_BINDING;
    if (name == "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS") return GL_MAX_TRANSFORM_FEEDBACK_BUFFERS;
    if (name == "GL_FIXED") return GL_FIXED;
    if (name == "GL_IMPLEMENTATION_COLOR_READ_TYPE") return GL_IMPLEMENTATION_COLOR_READ_TYPE;
    if (name == "GL_IMPLEMENTATION_COLOR_READ_FORMAT") return GL_IMPLEMENTATION_COLOR_READ_FORMAT;
    if (name == "GL_LOW_FLOAT") return GL_LOW_FLOAT;
    if (name == "GL_MEDIUM_FLOAT") return GL_MEDIUM_FLOAT;
    if (name == "GL_HIGH_FLOAT") return GL_HIGH_FLOAT;
    if (name == "GL_LOW_INT") return GL_LOW_INT;
    if (name == "GL_MEDIUM_INT") return GL_MEDIUM_INT;
    if (name == "GL_HIGH_INT") return GL_HIGH_INT;
    if (name == "GL_SHADER_COMPILER") return GL_SHADER_COMPILER;
    if (name == "GL_SHADER_BINARY_FORMATS") return GL_SHADER_BINARY_FORMATS;
    if (name == "GL_NUM_SHADER_BINARY_FORMATS") return GL_NUM_SHADER_BINARY_FORMATS;
    if (name == "GL_MAX_VERTEX_UNIFORM_VECTORS") return GL_MAX_VERTEX_UNIFORM_VECTORS;
    if (name == "GL_MAX_VARYING_VECTORS") return GL_MAX_VARYING_VECTORS;
    if (name == "GL_MAX_FRAGMENT_UNIFORM_VECTORS") return GL_MAX_FRAGMENT_UNIFORM_VECTORS;
    if (name == "GL_RGB565") return GL_RGB565;
    if (name == "GL_PROGRAM_BINARY_RETRIEVABLE_HINT") return GL_PROGRAM_BINARY_RETRIEVABLE_HINT;
    if (name == "GL_PROGRAM_BINARY_LENGTH") return GL_PROGRAM_BINARY_LENGTH;
    if (name == "GL_NUM_PROGRAM_BINARY_FORMATS") return GL_NUM_PROGRAM_BINARY_FORMATS;
    if (name == "GL_PROGRAM_BINARY_FORMATS") return GL_PROGRAM_BINARY_FORMATS;
    if (name == "GL_VERTEX_SHADER_BIT") return GL_VERTEX_SHADER_BIT;
    if (name == "GL_FRAGMENT_SHADER_BIT") return GL_FRAGMENT_SHADER_BIT;
    if (name == "GL_GEOMETRY_SHADER_BIT") return GL_GEOMETRY_SHADER_BIT;
    if (name == "GL_TESS_CONTROL_SHADER_BIT") return GL_TESS_CONTROL_SHADER_BIT;
    if (name == "GL_TESS_EVALUATION_SHADER_BIT") return GL_TESS_EVALUATION_SHADER_BIT;
    if (name == "GL_ALL_SHADER_BITS") return GL_ALL_SHADER_BITS;
    if (name == "GL_PROGRAM_SEPARABLE") return GL_PROGRAM_SEPARABLE;
    if (name == "GL_ACTIVE_PROGRAM") return GL_ACTIVE_PROGRAM;
    if (name == "GL_PROGRAM_PIPELINE_BINDING") return GL_PROGRAM_PIPELINE_BINDING;
    if (name == "GL_MAX_VIEWPORTS") return GL_MAX_VIEWPORTS;
    if (name == "GL_VIEWPORT_SUBPIXEL_BITS") return GL_VIEWPORT_SUBPIXEL_BITS;
    if (name == "GL_VIEWPORT_BOUNDS_RANGE") return GL_VIEWPORT_BOUNDS_RANGE;
    if (name == "GL_LAYER_PROVOKING_VERTEX") return GL_LAYER_PROVOKING_VERTEX;
    if (name == "GL_VIEWPORT_INDEX_PROVOKING_VERTEX") return GL_VIEWPORT_INDEX_PROVOKING_VERTEX;
    if (name == "GL_UNDEFINED_VERTEX") return GL_UNDEFINED_VERTEX;
    if (name == "GL_COPY_READ_BUFFER_BINDING") return GL_COPY_READ_BUFFER_BINDING;
    if (name == "GL_COPY_WRITE_BUFFER_BINDING") return GL_COPY_WRITE_BUFFER_BINDING;
    if (name == "GL_TRANSFORM_FEEDBACK_ACTIVE") return GL_TRANSFORM_FEEDBACK_ACTIVE;
    if (name == "GL_TRANSFORM_FEEDBACK_PAUSED") return GL_TRANSFORM_FEEDBACK_PAUSED;
    if (name == "GL_UNPACK_COMPRESSED_BLOCK_WIDTH") return GL_UNPACK_COMPRESSED_BLOCK_WIDTH;
    if (name == "GL_UNPACK_COMPRESSED_BLOCK_HEIGHT") return GL_UNPACK_COMPRESSED_BLOCK_HEIGHT;
    if (name == "GL_UNPACK_COMPRESSED_BLOCK_DEPTH") return GL_UNPACK_COMPRESSED_BLOCK_DEPTH;
    if (name == "GL_UNPACK_COMPRESSED_BLOCK_SIZE") return GL_UNPACK_COMPRESSED_BLOCK_SIZE;
    if (name == "GL_PACK_COMPRESSED_BLOCK_WIDTH") return GL_PACK_COMPRESSED_BLOCK_WIDTH;
    if (name == "GL_PACK_COMPRESSED_BLOCK_HEIGHT") return GL_PACK_COMPRESSED_BLOCK_HEIGHT;
    if (name == "GL_PACK_COMPRESSED_BLOCK_DEPTH") return GL_PACK_COMPRESSED_BLOCK_DEPTH;
    if (name == "GL_PACK_COMPRESSED_BLOCK_SIZE") return GL_PACK_COMPRESSED_BLOCK_SIZE;
    if (name == "GL_NUM_SAMPLE_COUNTS") return GL_NUM_SAMPLE_COUNTS;
    if (name == "GL_MIN_MAP_BUFFER_ALIGNMENT") return GL_MIN_MAP_BUFFER_ALIGNMENT;
    if (name == "GL_ATOMIC_COUNTER_BUFFER") return GL_ATOMIC_COUNTER_BUFFER;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_BINDING") return GL_ATOMIC_COUNTER_BUFFER_BINDING;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_START") return GL_ATOMIC_COUNTER_BUFFER_START;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_SIZE") return GL_ATOMIC_COUNTER_BUFFER_SIZE;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE") return GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS")
        return GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES")
        return GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER")
        return GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER")
        return GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER")
        return GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER")
        return GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER")
        return GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER;
    if (name == "GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS") return GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS") return GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS") return GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS") return GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS") return GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS") return GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_MAX_VERTEX_ATOMIC_COUNTERS") return GL_MAX_VERTEX_ATOMIC_COUNTERS;
    if (name == "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS") return GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS;
    if (name == "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS") return GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS;
    if (name == "GL_MAX_GEOMETRY_ATOMIC_COUNTERS") return GL_MAX_GEOMETRY_ATOMIC_COUNTERS;
    if (name == "GL_MAX_FRAGMENT_ATOMIC_COUNTERS") return GL_MAX_FRAGMENT_ATOMIC_COUNTERS;
    if (name == "GL_MAX_COMBINED_ATOMIC_COUNTERS") return GL_MAX_COMBINED_ATOMIC_COUNTERS;
    if (name == "GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE") return GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE;
    if (name == "GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS") return GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS;
    if (name == "GL_ACTIVE_ATOMIC_COUNTER_BUFFERS") return GL_ACTIVE_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX") return GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX;
    if (name == "GL_UNSIGNED_INT_ATOMIC_COUNTER") return GL_UNSIGNED_INT_ATOMIC_COUNTER;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT") return GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
    if (name == "GL_ELEMENT_ARRAY_BARRIER_BIT") return GL_ELEMENT_ARRAY_BARRIER_BIT;
    if (name == "GL_UNIFORM_BARRIER_BIT") return GL_UNIFORM_BARRIER_BIT;
    if (name == "GL_TEXTURE_FETCH_BARRIER_BIT") return GL_TEXTURE_FETCH_BARRIER_BIT;
    if (name == "GL_SHADER_IMAGE_ACCESS_BARRIER_BIT") return GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
    if (name == "GL_COMMAND_BARRIER_BIT") return GL_COMMAND_BARRIER_BIT;
    if (name == "GL_PIXEL_BUFFER_BARRIER_BIT") return GL_PIXEL_BUFFER_BARRIER_BIT;
    if (name == "GL_TEXTURE_UPDATE_BARRIER_BIT") return GL_TEXTURE_UPDATE_BARRIER_BIT;
    if (name == "GL_BUFFER_UPDATE_BARRIER_BIT") return GL_BUFFER_UPDATE_BARRIER_BIT;
    if (name == "GL_FRAMEBUFFER_BARRIER_BIT") return GL_FRAMEBUFFER_BARRIER_BIT;
    if (name == "GL_TRANSFORM_FEEDBACK_BARRIER_BIT") return GL_TRANSFORM_FEEDBACK_BARRIER_BIT;
    if (name == "GL_ATOMIC_COUNTER_BARRIER_BIT") return GL_ATOMIC_COUNTER_BARRIER_BIT;
    if (name == "GL_ALL_BARRIER_BITS") return GL_ALL_BARRIER_BITS;
    if (name == "GL_MAX_IMAGE_UNITS") return GL_MAX_IMAGE_UNITS;
    if (name == "GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS")
        return GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS;
    if (name == "GL_IMAGE_BINDING_NAME") return GL_IMAGE_BINDING_NAME;
    if (name == "GL_IMAGE_BINDING_LEVEL") return GL_IMAGE_BINDING_LEVEL;
    if (name == "GL_IMAGE_BINDING_LAYERED") return GL_IMAGE_BINDING_LAYERED;
    if (name == "GL_IMAGE_BINDING_LAYER") return GL_IMAGE_BINDING_LAYER;
    if (name == "GL_IMAGE_BINDING_ACCESS") return GL_IMAGE_BINDING_ACCESS;
    if (name == "GL_IMAGE_1D") return GL_IMAGE_1D;
    if (name == "GL_IMAGE_2D") return GL_IMAGE_2D;
    if (name == "GL_IMAGE_3D") return GL_IMAGE_3D;
    if (name == "GL_IMAGE_2D_RECT") return GL_IMAGE_2D_RECT;
    if (name == "GL_IMAGE_CUBE") return GL_IMAGE_CUBE;
    if (name == "GL_IMAGE_BUFFER") return GL_IMAGE_BUFFER;
    if (name == "GL_IMAGE_1D_ARRAY") return GL_IMAGE_1D_ARRAY;
    if (name == "GL_IMAGE_2D_ARRAY") return GL_IMAGE_2D_ARRAY;
    if (name == "GL_IMAGE_CUBE_MAP_ARRAY") return GL_IMAGE_CUBE_MAP_ARRAY;
    if (name == "GL_IMAGE_2D_MULTISAMPLE") return GL_IMAGE_2D_MULTISAMPLE;
    if (name == "GL_IMAGE_2D_MULTISAMPLE_ARRAY") return GL_IMAGE_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_INT_IMAGE_1D") return GL_INT_IMAGE_1D;
    if (name == "GL_INT_IMAGE_2D") return GL_INT_IMAGE_2D;
    if (name == "GL_INT_IMAGE_3D") return GL_INT_IMAGE_3D;
    if (name == "GL_INT_IMAGE_2D_RECT") return GL_INT_IMAGE_2D_RECT;
    if (name == "GL_INT_IMAGE_CUBE") return GL_INT_IMAGE_CUBE;
    if (name == "GL_INT_IMAGE_BUFFER") return GL_INT_IMAGE_BUFFER;
    if (name == "GL_INT_IMAGE_1D_ARRAY") return GL_INT_IMAGE_1D_ARRAY;
    if (name == "GL_INT_IMAGE_2D_ARRAY") return GL_INT_IMAGE_2D_ARRAY;
    if (name == "GL_INT_IMAGE_CUBE_MAP_ARRAY") return GL_INT_IMAGE_CUBE_MAP_ARRAY;
    if (name == "GL_INT_IMAGE_2D_MULTISAMPLE") return GL_INT_IMAGE_2D_MULTISAMPLE;
    if (name == "GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY") return GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_UNSIGNED_INT_IMAGE_1D") return GL_UNSIGNED_INT_IMAGE_1D;
    if (name == "GL_UNSIGNED_INT_IMAGE_2D") return GL_UNSIGNED_INT_IMAGE_2D;
    if (name == "GL_UNSIGNED_INT_IMAGE_3D") return GL_UNSIGNED_INT_IMAGE_3D;
    if (name == "GL_UNSIGNED_INT_IMAGE_2D_RECT") return GL_UNSIGNED_INT_IMAGE_2D_RECT;
    if (name == "GL_UNSIGNED_INT_IMAGE_CUBE") return GL_UNSIGNED_INT_IMAGE_CUBE;
    if (name == "GL_UNSIGNED_INT_IMAGE_BUFFER") return GL_UNSIGNED_INT_IMAGE_BUFFER;
    if (name == "GL_UNSIGNED_INT_IMAGE_1D_ARRAY") return GL_UNSIGNED_INT_IMAGE_1D_ARRAY;
    if (name == "GL_UNSIGNED_INT_IMAGE_2D_ARRAY") return GL_UNSIGNED_INT_IMAGE_2D_ARRAY;
    if (name == "GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY") return GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY;
    if (name == "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE") return GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE;
    if (name == "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY") return GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY;
    if (name == "GL_MAX_IMAGE_SAMPLES") return GL_MAX_IMAGE_SAMPLES;
    if (name == "GL_IMAGE_BINDING_FORMAT") return GL_IMAGE_BINDING_FORMAT;
    if (name == "GL_IMAGE_FORMAT_COMPATIBILITY_TYPE") return GL_IMAGE_FORMAT_COMPATIBILITY_TYPE;
    if (name == "GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE") return GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE;
    if (name == "GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS") return GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS;
    if (name == "GL_MAX_VERTEX_IMAGE_UNIFORMS") return GL_MAX_VERTEX_IMAGE_UNIFORMS;
    if (name == "GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS") return GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS;
    if (name == "GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS") return GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS;
    if (name == "GL_MAX_GEOMETRY_IMAGE_UNIFORMS") return GL_MAX_GEOMETRY_IMAGE_UNIFORMS;
    if (name == "GL_MAX_FRAGMENT_IMAGE_UNIFORMS") return GL_MAX_FRAGMENT_IMAGE_UNIFORMS;
    if (name == "GL_MAX_COMBINED_IMAGE_UNIFORMS") return GL_MAX_COMBINED_IMAGE_UNIFORMS;
    if (name == "GL_COMPRESSED_RGBA_BPTC_UNORM") return GL_COMPRESSED_RGBA_BPTC_UNORM;
    if (name == "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM") return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
    if (name == "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT") return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
    if (name == "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT") return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
    if (name == "GL_TEXTURE_IMMUTABLE_FORMAT") return GL_TEXTURE_IMMUTABLE_FORMAT;
    if (name == "GL_NUM_SHADING_LANGUAGE_VERSIONS") return GL_NUM_SHADING_LANGUAGE_VERSIONS;
    if (name == "GL_VERTEX_ATTRIB_ARRAY_LONG") return GL_VERTEX_ATTRIB_ARRAY_LONG;
    if (name == "GL_COMPRESSED_RGB8_ETC2") return GL_COMPRESSED_RGB8_ETC2;
    if (name == "GL_COMPRESSED_SRGB8_ETC2") return GL_COMPRESSED_SRGB8_ETC2;
    if (name == "GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2") return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
    if (name == "GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2") return GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
    if (name == "GL_COMPRESSED_RGBA8_ETC2_EAC") return GL_COMPRESSED_RGBA8_ETC2_EAC;
    if (name == "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC") return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
    if (name == "GL_COMPRESSED_R11_EAC") return GL_COMPRESSED_R11_EAC;
    if (name == "GL_COMPRESSED_SIGNED_R11_EAC") return GL_COMPRESSED_SIGNED_R11_EAC;
    if (name == "GL_COMPRESSED_RG11_EAC") return GL_COMPRESSED_RG11_EAC;
    if (name == "GL_COMPRESSED_SIGNED_RG11_EAC") return GL_COMPRESSED_SIGNED_RG11_EAC;
    if (name == "GL_PRIMITIVE_RESTART_FIXED_INDEX") return GL_PRIMITIVE_RESTART_FIXED_INDEX;
    if (name == "GL_ANY_SAMPLES_PASSED_CONSERVATIVE") return GL_ANY_SAMPLES_PASSED_CONSERVATIVE;
    if (name == "GL_MAX_ELEMENT_INDEX") return GL_MAX_ELEMENT_INDEX;
    if (name == "GL_COMPUTE_SHADER") return GL_COMPUTE_SHADER;
    if (name == "GL_MAX_COMPUTE_UNIFORM_BLOCKS") return GL_MAX_COMPUTE_UNIFORM_BLOCKS;
    if (name == "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS") return GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS;
    if (name == "GL_MAX_COMPUTE_IMAGE_UNIFORMS") return GL_MAX_COMPUTE_IMAGE_UNIFORMS;
    if (name == "GL_MAX_COMPUTE_SHARED_MEMORY_SIZE") return GL_MAX_COMPUTE_SHARED_MEMORY_SIZE;
    if (name == "GL_MAX_COMPUTE_UNIFORM_COMPONENTS") return GL_MAX_COMPUTE_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS") return GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS;
    if (name == "GL_MAX_COMPUTE_ATOMIC_COUNTERS") return GL_MAX_COMPUTE_ATOMIC_COUNTERS;
    if (name == "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS") return GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS;
    if (name == "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS") return GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS;
    if (name == "GL_MAX_COMPUTE_WORK_GROUP_COUNT") return GL_MAX_COMPUTE_WORK_GROUP_COUNT;
    if (name == "GL_MAX_COMPUTE_WORK_GROUP_SIZE") return GL_MAX_COMPUTE_WORK_GROUP_SIZE;
    if (name == "GL_COMPUTE_WORK_GROUP_SIZE") return GL_COMPUTE_WORK_GROUP_SIZE;
    if (name == "GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER") return GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER")
        return GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER;
    if (name == "GL_DISPATCH_INDIRECT_BUFFER") return GL_DISPATCH_INDIRECT_BUFFER;
    if (name == "GL_DISPATCH_INDIRECT_BUFFER_BINDING") return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
    if (name == "GL_COMPUTE_SHADER_BIT") return GL_COMPUTE_SHADER_BIT;
    if (name == "GL_DEBUG_OUTPUT_SYNCHRONOUS") return GL_DEBUG_OUTPUT_SYNCHRONOUS;
    if (name == "GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH") return GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH;
    if (name == "GL_DEBUG_CALLBACK_FUNCTION") return GL_DEBUG_CALLBACK_FUNCTION;
    if (name == "GL_DEBUG_CALLBACK_USER_PARAM") return GL_DEBUG_CALLBACK_USER_PARAM;
    if (name == "GL_DEBUG_SOURCE_API") return GL_DEBUG_SOURCE_API;
    if (name == "GL_DEBUG_SOURCE_WINDOW_SYSTEM") return GL_DEBUG_SOURCE_WINDOW_SYSTEM;
    if (name == "GL_DEBUG_SOURCE_SHADER_COMPILER") return GL_DEBUG_SOURCE_SHADER_COMPILER;
    if (name == "GL_DEBUG_SOURCE_THIRD_PARTY") return GL_DEBUG_SOURCE_THIRD_PARTY;
    if (name == "GL_DEBUG_SOURCE_APPLICATION") return GL_DEBUG_SOURCE_APPLICATION;
    if (name == "GL_DEBUG_SOURCE_OTHER") return GL_DEBUG_SOURCE_OTHER;
    if (name == "GL_DEBUG_TYPE_ERROR") return GL_DEBUG_TYPE_ERROR;
    if (name == "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR") return GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR;
    if (name == "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR") return GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR;
    if (name == "GL_DEBUG_TYPE_PORTABILITY") return GL_DEBUG_TYPE_PORTABILITY;
    if (name == "GL_DEBUG_TYPE_PERFORMANCE") return GL_DEBUG_TYPE_PERFORMANCE;
    if (name == "GL_DEBUG_TYPE_OTHER") return GL_DEBUG_TYPE_OTHER;
    if (name == "GL_MAX_DEBUG_MESSAGE_LENGTH") return GL_MAX_DEBUG_MESSAGE_LENGTH;
    if (name == "GL_MAX_DEBUG_LOGGED_MESSAGES") return GL_MAX_DEBUG_LOGGED_MESSAGES;
    if (name == "GL_DEBUG_LOGGED_MESSAGES") return GL_DEBUG_LOGGED_MESSAGES;
    if (name == "GL_DEBUG_SEVERITY_HIGH") return GL_DEBUG_SEVERITY_HIGH;
    if (name == "GL_DEBUG_SEVERITY_MEDIUM") return GL_DEBUG_SEVERITY_MEDIUM;
    if (name == "GL_DEBUG_SEVERITY_LOW") return GL_DEBUG_SEVERITY_LOW;
    if (name == "GL_DEBUG_TYPE_MARKER") return GL_DEBUG_TYPE_MARKER;
    if (name == "GL_DEBUG_TYPE_PUSH_GROUP") return GL_DEBUG_TYPE_PUSH_GROUP;
    if (name == "GL_DEBUG_TYPE_POP_GROUP") return GL_DEBUG_TYPE_POP_GROUP;
    if (name == "GL_DEBUG_SEVERITY_NOTIFICATION") return GL_DEBUG_SEVERITY_NOTIFICATION;
    if (name == "GL_MAX_DEBUG_GROUP_STACK_DEPTH") return GL_MAX_DEBUG_GROUP_STACK_DEPTH;
    if (name == "GL_DEBUG_GROUP_STACK_DEPTH") return GL_DEBUG_GROUP_STACK_DEPTH;
    if (name == "GL_BUFFER") return GL_BUFFER;
    if (name == "GL_SHADER") return GL_SHADER;
    if (name == "GL_PROGRAM") return GL_PROGRAM;
    if (name == "GL_VERTEX_ARRAY") return GL_VERTEX_ARRAY;
    if (name == "GL_QUERY") return GL_QUERY;
    if (name == "GL_PROGRAM_PIPELINE") return GL_PROGRAM_PIPELINE;
    if (name == "GL_SAMPLER") return GL_SAMPLER;
    if (name == "GL_MAX_LABEL_LENGTH") return GL_MAX_LABEL_LENGTH;
    if (name == "GL_DEBUG_OUTPUT") return GL_DEBUG_OUTPUT;
    if (name == "GL_CONTEXT_FLAG_DEBUG_BIT") return GL_CONTEXT_FLAG_DEBUG_BIT;
    if (name == "GL_MAX_UNIFORM_LOCATIONS") return GL_MAX_UNIFORM_LOCATIONS;
    if (name == "GL_FRAMEBUFFER_DEFAULT_WIDTH") return GL_FRAMEBUFFER_DEFAULT_WIDTH;
    if (name == "GL_FRAMEBUFFER_DEFAULT_HEIGHT") return GL_FRAMEBUFFER_DEFAULT_HEIGHT;
    if (name == "GL_FRAMEBUFFER_DEFAULT_LAYERS") return GL_FRAMEBUFFER_DEFAULT_LAYERS;
    if (name == "GL_FRAMEBUFFER_DEFAULT_SAMPLES") return GL_FRAMEBUFFER_DEFAULT_SAMPLES;
    if (name == "GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS") return GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS;
    if (name == "GL_MAX_FRAMEBUFFER_WIDTH") return GL_MAX_FRAMEBUFFER_WIDTH;
    if (name == "GL_MAX_FRAMEBUFFER_HEIGHT") return GL_MAX_FRAMEBUFFER_HEIGHT;
    if (name == "GL_MAX_FRAMEBUFFER_LAYERS") return GL_MAX_FRAMEBUFFER_LAYERS;
    if (name == "GL_MAX_FRAMEBUFFER_SAMPLES") return GL_MAX_FRAMEBUFFER_SAMPLES;
    if (name == "GL_INTERNALFORMAT_SUPPORTED") return GL_INTERNALFORMAT_SUPPORTED;
    if (name == "GL_INTERNALFORMAT_PREFERRED") return GL_INTERNALFORMAT_PREFERRED;
    if (name == "GL_INTERNALFORMAT_RED_SIZE") return GL_INTERNALFORMAT_RED_SIZE;
    if (name == "GL_INTERNALFORMAT_GREEN_SIZE") return GL_INTERNALFORMAT_GREEN_SIZE;
    if (name == "GL_INTERNALFORMAT_BLUE_SIZE") return GL_INTERNALFORMAT_BLUE_SIZE;
    if (name == "GL_INTERNALFORMAT_ALPHA_SIZE") return GL_INTERNALFORMAT_ALPHA_SIZE;
    if (name == "GL_INTERNALFORMAT_DEPTH_SIZE") return GL_INTERNALFORMAT_DEPTH_SIZE;
    if (name == "GL_INTERNALFORMAT_STENCIL_SIZE") return GL_INTERNALFORMAT_STENCIL_SIZE;
    if (name == "GL_INTERNALFORMAT_SHARED_SIZE") return GL_INTERNALFORMAT_SHARED_SIZE;
    if (name == "GL_INTERNALFORMAT_RED_TYPE") return GL_INTERNALFORMAT_RED_TYPE;
    if (name == "GL_INTERNALFORMAT_GREEN_TYPE") return GL_INTERNALFORMAT_GREEN_TYPE;
    if (name == "GL_INTERNALFORMAT_BLUE_TYPE") return GL_INTERNALFORMAT_BLUE_TYPE;
    if (name == "GL_INTERNALFORMAT_ALPHA_TYPE") return GL_INTERNALFORMAT_ALPHA_TYPE;
    if (name == "GL_INTERNALFORMAT_DEPTH_TYPE") return GL_INTERNALFORMAT_DEPTH_TYPE;
    if (name == "GL_INTERNALFORMAT_STENCIL_TYPE") return GL_INTERNALFORMAT_STENCIL_TYPE;
    if (name == "GL_MAX_WIDTH") return GL_MAX_WIDTH;
    if (name == "GL_MAX_HEIGHT") return GL_MAX_HEIGHT;
    if (name == "GL_MAX_DEPTH") return GL_MAX_DEPTH;
    if (name == "GL_MAX_LAYERS") return GL_MAX_LAYERS;
    if (name == "GL_MAX_COMBINED_DIMENSIONS") return GL_MAX_COMBINED_DIMENSIONS;
    if (name == "GL_COLOR_COMPONENTS") return GL_COLOR_COMPONENTS;
    if (name == "GL_DEPTH_COMPONENTS") return GL_DEPTH_COMPONENTS;
    if (name == "GL_STENCIL_COMPONENTS") return GL_STENCIL_COMPONENTS;
    if (name == "GL_COLOR_RENDERABLE") return GL_COLOR_RENDERABLE;
    if (name == "GL_DEPTH_RENDERABLE") return GL_DEPTH_RENDERABLE;
    if (name == "GL_STENCIL_RENDERABLE") return GL_STENCIL_RENDERABLE;
    if (name == "GL_FRAMEBUFFER_RENDERABLE") return GL_FRAMEBUFFER_RENDERABLE;
    if (name == "GL_FRAMEBUFFER_RENDERABLE_LAYERED") return GL_FRAMEBUFFER_RENDERABLE_LAYERED;
    if (name == "GL_FRAMEBUFFER_BLEND") return GL_FRAMEBUFFER_BLEND;
    if (name == "GL_READ_PIXELS") return GL_READ_PIXELS;
    if (name == "GL_READ_PIXELS_FORMAT") return GL_READ_PIXELS_FORMAT;
    if (name == "GL_READ_PIXELS_TYPE") return GL_READ_PIXELS_TYPE;
    if (name == "GL_TEXTURE_IMAGE_FORMAT") return GL_TEXTURE_IMAGE_FORMAT;
    if (name == "GL_TEXTURE_IMAGE_TYPE") return GL_TEXTURE_IMAGE_TYPE;
    if (name == "GL_GET_TEXTURE_IMAGE_FORMAT") return GL_GET_TEXTURE_IMAGE_FORMAT;
    if (name == "GL_GET_TEXTURE_IMAGE_TYPE") return GL_GET_TEXTURE_IMAGE_TYPE;
    if (name == "GL_MIPMAP") return GL_MIPMAP;
    if (name == "GL_MANUAL_GENERATE_MIPMAP") return GL_MANUAL_GENERATE_MIPMAP;
    if (name == "GL_AUTO_GENERATE_MIPMAP") return GL_AUTO_GENERATE_MIPMAP;
    if (name == "GL_COLOR_ENCODING") return GL_COLOR_ENCODING;
    if (name == "GL_SRGB_READ") return GL_SRGB_READ;
    if (name == "GL_SRGB_WRITE") return GL_SRGB_WRITE;
    if (name == "GL_FILTER") return GL_FILTER;
    if (name == "GL_VERTEX_TEXTURE") return GL_VERTEX_TEXTURE;
    if (name == "GL_TESS_CONTROL_TEXTURE") return GL_TESS_CONTROL_TEXTURE;
    if (name == "GL_TESS_EVALUATION_TEXTURE") return GL_TESS_EVALUATION_TEXTURE;
    if (name == "GL_GEOMETRY_TEXTURE") return GL_GEOMETRY_TEXTURE;
    if (name == "GL_FRAGMENT_TEXTURE") return GL_FRAGMENT_TEXTURE;
    if (name == "GL_COMPUTE_TEXTURE") return GL_COMPUTE_TEXTURE;
    if (name == "GL_TEXTURE_SHADOW") return GL_TEXTURE_SHADOW;
    if (name == "GL_TEXTURE_GATHER") return GL_TEXTURE_GATHER;
    if (name == "GL_TEXTURE_GATHER_SHADOW") return GL_TEXTURE_GATHER_SHADOW;
    if (name == "GL_SHADER_IMAGE_LOAD") return GL_SHADER_IMAGE_LOAD;
    if (name == "GL_SHADER_IMAGE_STORE") return GL_SHADER_IMAGE_STORE;
    if (name == "GL_SHADER_IMAGE_ATOMIC") return GL_SHADER_IMAGE_ATOMIC;
    if (name == "GL_IMAGE_TEXEL_SIZE") return GL_IMAGE_TEXEL_SIZE;
    if (name == "GL_IMAGE_COMPATIBILITY_CLASS") return GL_IMAGE_COMPATIBILITY_CLASS;
    if (name == "GL_IMAGE_PIXEL_FORMAT") return GL_IMAGE_PIXEL_FORMAT;
    if (name == "GL_IMAGE_PIXEL_TYPE") return GL_IMAGE_PIXEL_TYPE;
    if (name == "GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST") return GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST;
    if (name == "GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST") return GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST;
    if (name == "GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE") return GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE;
    if (name == "GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE") return GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE;
    if (name == "GL_TEXTURE_COMPRESSED_BLOCK_WIDTH") return GL_TEXTURE_COMPRESSED_BLOCK_WIDTH;
    if (name == "GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT") return GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT;
    if (name == "GL_TEXTURE_COMPRESSED_BLOCK_SIZE") return GL_TEXTURE_COMPRESSED_BLOCK_SIZE;
    if (name == "GL_CLEAR_BUFFER") return GL_CLEAR_BUFFER;
    if (name == "GL_TEXTURE_VIEW") return GL_TEXTURE_VIEW;
    if (name == "GL_VIEW_COMPATIBILITY_CLASS") return GL_VIEW_COMPATIBILITY_CLASS;
    if (name == "GL_FULL_SUPPORT") return GL_FULL_SUPPORT;
    if (name == "GL_CAVEAT_SUPPORT") return GL_CAVEAT_SUPPORT;
    if (name == "GL_IMAGE_CLASS_4_X_32") return GL_IMAGE_CLASS_4_X_32;
    if (name == "GL_IMAGE_CLASS_2_X_32") return GL_IMAGE_CLASS_2_X_32;
    if (name == "GL_IMAGE_CLASS_1_X_32") return GL_IMAGE_CLASS_1_X_32;
    if (name == "GL_IMAGE_CLASS_4_X_16") return GL_IMAGE_CLASS_4_X_16;
    if (name == "GL_IMAGE_CLASS_2_X_16") return GL_IMAGE_CLASS_2_X_16;
    if (name == "GL_IMAGE_CLASS_1_X_16") return GL_IMAGE_CLASS_1_X_16;
    if (name == "GL_IMAGE_CLASS_4_X_8") return GL_IMAGE_CLASS_4_X_8;
    if (name == "GL_IMAGE_CLASS_2_X_8") return GL_IMAGE_CLASS_2_X_8;
    if (name == "GL_IMAGE_CLASS_1_X_8") return GL_IMAGE_CLASS_1_X_8;
    if (name == "GL_IMAGE_CLASS_11_11_10") return GL_IMAGE_CLASS_11_11_10;
    if (name == "GL_IMAGE_CLASS_10_10_10_2") return GL_IMAGE_CLASS_10_10_10_2;
    if (name == "GL_VIEW_CLASS_128_BITS") return GL_VIEW_CLASS_128_BITS;
    if (name == "GL_VIEW_CLASS_96_BITS") return GL_VIEW_CLASS_96_BITS;
    if (name == "GL_VIEW_CLASS_64_BITS") return GL_VIEW_CLASS_64_BITS;
    if (name == "GL_VIEW_CLASS_48_BITS") return GL_VIEW_CLASS_48_BITS;
    if (name == "GL_VIEW_CLASS_32_BITS") return GL_VIEW_CLASS_32_BITS;
    if (name == "GL_VIEW_CLASS_24_BITS") return GL_VIEW_CLASS_24_BITS;
    if (name == "GL_VIEW_CLASS_16_BITS") return GL_VIEW_CLASS_16_BITS;
    if (name == "GL_VIEW_CLASS_8_BITS") return GL_VIEW_CLASS_8_BITS;
    if (name == "GL_VIEW_CLASS_S3TC_DXT1_RGB") return GL_VIEW_CLASS_S3TC_DXT1_RGB;
    if (name == "GL_VIEW_CLASS_S3TC_DXT1_RGBA") return GL_VIEW_CLASS_S3TC_DXT1_RGBA;
    if (name == "GL_VIEW_CLASS_S3TC_DXT3_RGBA") return GL_VIEW_CLASS_S3TC_DXT3_RGBA;
    if (name == "GL_VIEW_CLASS_S3TC_DXT5_RGBA") return GL_VIEW_CLASS_S3TC_DXT5_RGBA;
    if (name == "GL_VIEW_CLASS_RGTC1_RED") return GL_VIEW_CLASS_RGTC1_RED;
    if (name == "GL_VIEW_CLASS_RGTC2_RG") return GL_VIEW_CLASS_RGTC2_RG;
    if (name == "GL_VIEW_CLASS_BPTC_UNORM") return GL_VIEW_CLASS_BPTC_UNORM;
    if (name == "GL_VIEW_CLASS_BPTC_FLOAT") return GL_VIEW_CLASS_BPTC_FLOAT;
    if (name == "GL_UNIFORM") return GL_UNIFORM;
    if (name == "GL_UNIFORM_BLOCK") return GL_UNIFORM_BLOCK;
    if (name == "GL_PROGRAM_INPUT") return GL_PROGRAM_INPUT;
    if (name == "GL_PROGRAM_OUTPUT") return GL_PROGRAM_OUTPUT;
    if (name == "GL_BUFFER_VARIABLE") return GL_BUFFER_VARIABLE;
    if (name == "GL_SHADER_STORAGE_BLOCK") return GL_SHADER_STORAGE_BLOCK;
    if (name == "GL_VERTEX_SUBROUTINE") return GL_VERTEX_SUBROUTINE;
    if (name == "GL_TESS_CONTROL_SUBROUTINE") return GL_TESS_CONTROL_SUBROUTINE;
    if (name == "GL_TESS_EVALUATION_SUBROUTINE") return GL_TESS_EVALUATION_SUBROUTINE;
    if (name == "GL_GEOMETRY_SUBROUTINE") return GL_GEOMETRY_SUBROUTINE;
    if (name == "GL_FRAGMENT_SUBROUTINE") return GL_FRAGMENT_SUBROUTINE;
    if (name == "GL_COMPUTE_SUBROUTINE") return GL_COMPUTE_SUBROUTINE;
    if (name == "GL_VERTEX_SUBROUTINE_UNIFORM") return GL_VERTEX_SUBROUTINE_UNIFORM;
    if (name == "GL_TESS_CONTROL_SUBROUTINE_UNIFORM") return GL_TESS_CONTROL_SUBROUTINE_UNIFORM;
    if (name == "GL_TESS_EVALUATION_SUBROUTINE_UNIFORM") return GL_TESS_EVALUATION_SUBROUTINE_UNIFORM;
    if (name == "GL_GEOMETRY_SUBROUTINE_UNIFORM") return GL_GEOMETRY_SUBROUTINE_UNIFORM;
    if (name == "GL_FRAGMENT_SUBROUTINE_UNIFORM") return GL_FRAGMENT_SUBROUTINE_UNIFORM;
    if (name == "GL_COMPUTE_SUBROUTINE_UNIFORM") return GL_COMPUTE_SUBROUTINE_UNIFORM;
    if (name == "GL_TRANSFORM_FEEDBACK_VARYING") return GL_TRANSFORM_FEEDBACK_VARYING;
    if (name == "GL_ACTIVE_RESOURCES") return GL_ACTIVE_RESOURCES;
    if (name == "GL_MAX_NAME_LENGTH") return GL_MAX_NAME_LENGTH;
    if (name == "GL_MAX_NUM_ACTIVE_VARIABLES") return GL_MAX_NUM_ACTIVE_VARIABLES;
    if (name == "GL_MAX_NUM_COMPATIBLE_SUBROUTINES") return GL_MAX_NUM_COMPATIBLE_SUBROUTINES;
    if (name == "GL_NAME_LENGTH") return GL_NAME_LENGTH;
    if (name == "GL_TYPE") return GL_TYPE;
    if (name == "GL_ARRAY_SIZE") return GL_ARRAY_SIZE;
    if (name == "GL_OFFSET") return GL_OFFSET;
    if (name == "GL_BLOCK_INDEX") return GL_BLOCK_INDEX;
    if (name == "GL_ARRAY_STRIDE") return GL_ARRAY_STRIDE;
    if (name == "GL_MATRIX_STRIDE") return GL_MATRIX_STRIDE;
    if (name == "GL_IS_ROW_MAJOR") return GL_IS_ROW_MAJOR;
    if (name == "GL_ATOMIC_COUNTER_BUFFER_INDEX") return GL_ATOMIC_COUNTER_BUFFER_INDEX;
    if (name == "GL_BUFFER_BINDING") return GL_BUFFER_BINDING;
    if (name == "GL_BUFFER_DATA_SIZE") return GL_BUFFER_DATA_SIZE;
    if (name == "GL_NUM_ACTIVE_VARIABLES") return GL_NUM_ACTIVE_VARIABLES;
    if (name == "GL_ACTIVE_VARIABLES") return GL_ACTIVE_VARIABLES;
    if (name == "GL_REFERENCED_BY_VERTEX_SHADER") return GL_REFERENCED_BY_VERTEX_SHADER;
    if (name == "GL_REFERENCED_BY_TESS_CONTROL_SHADER") return GL_REFERENCED_BY_TESS_CONTROL_SHADER;
    if (name == "GL_REFERENCED_BY_TESS_EVALUATION_SHADER") return GL_REFERENCED_BY_TESS_EVALUATION_SHADER;
    if (name == "GL_REFERENCED_BY_GEOMETRY_SHADER") return GL_REFERENCED_BY_GEOMETRY_SHADER;
    if (name == "GL_REFERENCED_BY_FRAGMENT_SHADER") return GL_REFERENCED_BY_FRAGMENT_SHADER;
    if (name == "GL_REFERENCED_BY_COMPUTE_SHADER") return GL_REFERENCED_BY_COMPUTE_SHADER;
    if (name == "GL_TOP_LEVEL_ARRAY_SIZE") return GL_TOP_LEVEL_ARRAY_SIZE;
    if (name == "GL_TOP_LEVEL_ARRAY_STRIDE") return GL_TOP_LEVEL_ARRAY_STRIDE;
    if (name == "GL_LOCATION") return GL_LOCATION;
    if (name == "GL_LOCATION_INDEX") return GL_LOCATION_INDEX;
    if (name == "GL_IS_PER_PATCH") return GL_IS_PER_PATCH;
    if (name == "GL_SHADER_STORAGE_BUFFER") return GL_SHADER_STORAGE_BUFFER;
    if (name == "GL_SHADER_STORAGE_BUFFER_BINDING") return GL_SHADER_STORAGE_BUFFER_BINDING;
    if (name == "GL_SHADER_STORAGE_BUFFER_START") return GL_SHADER_STORAGE_BUFFER_START;
    if (name == "GL_SHADER_STORAGE_BUFFER_SIZE") return GL_SHADER_STORAGE_BUFFER_SIZE;
    if (name == "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS") return GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS;
    if (name == "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS") return GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS;
    if (name == "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS") return GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS;
    if (name == "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS") return GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS;
    if (name == "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS") return GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS;
    if (name == "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS") return GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS;
    if (name == "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS") return GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS;
    if (name == "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS") return GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS;
    if (name == "GL_MAX_SHADER_STORAGE_BLOCK_SIZE") return GL_MAX_SHADER_STORAGE_BLOCK_SIZE;
    if (name == "GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT") return GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
    if (name == "GL_SHADER_STORAGE_BARRIER_BIT") return GL_SHADER_STORAGE_BARRIER_BIT;
    if (name == "GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES") return GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES;
    if (name == "GL_DEPTH_STENCIL_TEXTURE_MODE") return GL_DEPTH_STENCIL_TEXTURE_MODE;
    if (name == "GL_TEXTURE_BUFFER_OFFSET") return GL_TEXTURE_BUFFER_OFFSET;
    if (name == "GL_TEXTURE_BUFFER_SIZE") return GL_TEXTURE_BUFFER_SIZE;
    if (name == "GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT") return GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT;
    if (name == "GL_TEXTURE_VIEW_MIN_LEVEL") return GL_TEXTURE_VIEW_MIN_LEVEL;
    if (name == "GL_TEXTURE_VIEW_NUM_LEVELS") return GL_TEXTURE_VIEW_NUM_LEVELS;
    if (name == "GL_TEXTURE_VIEW_MIN_LAYER") return GL_TEXTURE_VIEW_MIN_LAYER;
    if (name == "GL_TEXTURE_VIEW_NUM_LAYERS") return GL_TEXTURE_VIEW_NUM_LAYERS;
    if (name == "GL_TEXTURE_IMMUTABLE_LEVELS") return GL_TEXTURE_IMMUTABLE_LEVELS;
    if (name == "GL_VERTEX_ATTRIB_BINDING") return GL_VERTEX_ATTRIB_BINDING;
    if (name == "GL_VERTEX_ATTRIB_RELATIVE_OFFSET") return GL_VERTEX_ATTRIB_RELATIVE_OFFSET;
    if (name == "GL_VERTEX_BINDING_DIVISOR") return GL_VERTEX_BINDING_DIVISOR;
    if (name == "GL_VERTEX_BINDING_OFFSET") return GL_VERTEX_BINDING_OFFSET;
    if (name == "GL_VERTEX_BINDING_STRIDE") return GL_VERTEX_BINDING_STRIDE;
    if (name == "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET") return GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET;
    if (name == "GL_MAX_VERTEX_ATTRIB_BINDINGS") return GL_MAX_VERTEX_ATTRIB_BINDINGS;
    if (name == "GL_VERTEX_BINDING_BUFFER") return GL_VERTEX_BINDING_BUFFER;
    if (name == "GL_DISPLAY_LIST") return GL_DISPLAY_LIST;
    if (name == "GL_STACK_UNDERFLOW") return GL_STACK_UNDERFLOW;
    if (name == "GL_STACK_OVERFLOW") return GL_STACK_OVERFLOW;
    if (name == "GL_MAX_VERTEX_ATTRIB_STRIDE") return GL_MAX_VERTEX_ATTRIB_STRIDE;
    if (name == "GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED") return GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED;
    if (name == "GL_TEXTURE_BUFFER_BINDING") return GL_TEXTURE_BUFFER_BINDING;
    if (name == "GL_MAP_PERSISTENT_BIT") return GL_MAP_PERSISTENT_BIT;
    if (name == "GL_MAP_COHERENT_BIT") return GL_MAP_COHERENT_BIT;
    if (name == "GL_DYNAMIC_STORAGE_BIT") return GL_DYNAMIC_STORAGE_BIT;
    if (name == "GL_CLIENT_STORAGE_BIT") return GL_CLIENT_STORAGE_BIT;
    if (name == "GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT") return GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT;
    if (name == "GL_BUFFER_IMMUTABLE_STORAGE") return GL_BUFFER_IMMUTABLE_STORAGE;
    if (name == "GL_BUFFER_STORAGE_FLAGS") return GL_BUFFER_STORAGE_FLAGS;
    if (name == "GL_CLEAR_TEXTURE") return GL_CLEAR_TEXTURE;
    if (name == "GL_LOCATION_COMPONENT") return GL_LOCATION_COMPONENT;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_INDEX") return GL_TRANSFORM_FEEDBACK_BUFFER_INDEX;
    if (name == "GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE") return GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE;
    if (name == "GL_QUERY_BUFFER") return GL_QUERY_BUFFER;
    if (name == "GL_QUERY_BUFFER_BARRIER_BIT") return GL_QUERY_BUFFER_BARRIER_BIT;
    if (name == "GL_QUERY_BUFFER_BINDING") return GL_QUERY_BUFFER_BINDING;
    if (name == "GL_QUERY_RESULT_NO_WAIT") return GL_QUERY_RESULT_NO_WAIT;
    if (name == "GL_MIRROR_CLAMP_TO_EDGE") return GL_MIRROR_CLAMP_TO_EDGE;
    if (name == "GL_CONTEXT_LOST") return GL_CONTEXT_LOST;
    if (name == "GL_NEGATIVE_ONE_TO_ONE") return GL_NEGATIVE_ONE_TO_ONE;
    if (name == "GL_ZERO_TO_ONE") return GL_ZERO_TO_ONE;
    if (name == "GL_CLIP_ORIGIN") return GL_CLIP_ORIGIN;
    if (name == "GL_CLIP_DEPTH_MODE") return GL_CLIP_DEPTH_MODE;
    if (name == "GL_QUERY_WAIT_INVERTED") return GL_QUERY_WAIT_INVERTED;
    if (name == "GL_QUERY_NO_WAIT_INVERTED") return GL_QUERY_NO_WAIT_INVERTED;
    if (name == "GL_QUERY_BY_REGION_WAIT_INVERTED") return GL_QUERY_BY_REGION_WAIT_INVERTED;
    if (name == "GL_QUERY_BY_REGION_NO_WAIT_INVERTED") return GL_QUERY_BY_REGION_NO_WAIT_INVERTED;
    if (name == "GL_MAX_CULL_DISTANCES") return GL_MAX_CULL_DISTANCES;
    if (name == "GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES") return GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES;
    if (name == "GL_TEXTURE_TARGET") return GL_TEXTURE_TARGET;
    if (name == "GL_QUERY_TARGET") return GL_QUERY_TARGET;
    if (name == "GL_GUILTY_CONTEXT_RESET") return GL_GUILTY_CONTEXT_RESET;
    if (name == "GL_INNOCENT_CONTEXT_RESET") return GL_INNOCENT_CONTEXT_RESET;
    if (name == "GL_UNKNOWN_CONTEXT_RESET") return GL_UNKNOWN_CONTEXT_RESET;
    if (name == "GL_RESET_NOTIFICATION_STRATEGY") return GL_RESET_NOTIFICATION_STRATEGY;
    if (name == "GL_LOSE_CONTEXT_ON_RESET") return GL_LOSE_CONTEXT_ON_RESET;
    if (name == "GL_NO_RESET_NOTIFICATION") return GL_NO_RESET_NOTIFICATION;
    if (name == "GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT") return GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT;
    if (name == "GL_CONTEXT_RELEASE_BEHAVIOR") return GL_CONTEXT_RELEASE_BEHAVIOR;
    if (name == "GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH") return GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH;
    if (name == "GL_SHADER_BINARY_FORMAT_SPIR_V") return GL_SHADER_BINARY_FORMAT_SPIR_V;
    if (name == "GL_SPIR_V_BINARY") return GL_SPIR_V_BINARY;
    if (name == "GL_PARAMETER_BUFFER") return GL_PARAMETER_BUFFER;
    if (name == "GL_PARAMETER_BUFFER_BINDING") return GL_PARAMETER_BUFFER_BINDING;
    if (name == "GL_CONTEXT_FLAG_NO_ERROR_BIT") return GL_CONTEXT_FLAG_NO_ERROR_BIT;
    if (name == "GL_VERTICES_SUBMITTED") return GL_VERTICES_SUBMITTED;
    if (name == "GL_PRIMITIVES_SUBMITTED") return GL_PRIMITIVES_SUBMITTED;
    if (name == "GL_VERTEX_SHADER_INVOCATIONS") return GL_VERTEX_SHADER_INVOCATIONS;
    if (name == "GL_TESS_CONTROL_SHADER_PATCHES") return GL_TESS_CONTROL_SHADER_PATCHES;
    if (name == "GL_TESS_EVALUATION_SHADER_INVOCATIONS") return GL_TESS_EVALUATION_SHADER_INVOCATIONS;
    if (name == "GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED") return GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED;
    if (name == "GL_FRAGMENT_SHADER_INVOCATIONS") return GL_FRAGMENT_SHADER_INVOCATIONS;
    if (name == "GL_COMPUTE_SHADER_INVOCATIONS") return GL_COMPUTE_SHADER_INVOCATIONS;
    if (name == "GL_CLIPPING_INPUT_PRIMITIVES") return GL_CLIPPING_INPUT_PRIMITIVES;
    if (name == "GL_CLIPPING_OUTPUT_PRIMITIVES") return GL_CLIPPING_OUTPUT_PRIMITIVES;
    if (name == "GL_POLYGON_OFFSET_CLAMP") return GL_POLYGON_OFFSET_CLAMP;
    if (name == "GL_SPIR_V_EXTENSIONS") return GL_SPIR_V_EXTENSIONS;
    if (name == "GL_NUM_SPIR_V_EXTENSIONS") return GL_NUM_SPIR_V_EXTENSIONS;
    if (name == "GL_TEXTURE_MAX_ANISOTROPY") return GL_TEXTURE_MAX_ANISOTROPY;
    if (name == "GL_MAX_TEXTURE_MAX_ANISOTROPY") return GL_MAX_TEXTURE_MAX_ANISOTROPY;
    if (name == "GL_TRANSFORM_FEEDBACK_OVERFLOW") return GL_TRANSFORM_FEEDBACK_OVERFLOW;
    if (name == "GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW") return GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW;
    if (name == "GL_VERSION_1_0") return GL_VERSION_1_0;
    return 0;
}

std::string ogl_enum_to_string(size_t value) {
    if (value == GL_DEPTH_BUFFER_BIT) return "GL_DEPTH_BUFFER_BIT";
    if (value == GL_STENCIL_BUFFER_BIT) return "GL_STENCIL_BUFFER_BIT";
    if (value == GL_COLOR_BUFFER_BIT) return "GL_COLOR_BUFFER_BIT";
    if (value == GL_FALSE) return "GL_FALSE";
    if (value == GL_TRUE) return "GL_TRUE";
    if (value == GL_POINTS) return "GL_POINTS";
    if (value == GL_LINES) return "GL_LINES";
    if (value == GL_LINE_LOOP) return "GL_LINE_LOOP";
    if (value == GL_LINE_STRIP) return "GL_LINE_STRIP";
    if (value == GL_TRIANGLES) return "GL_TRIANGLES";
    if (value == GL_TRIANGLE_STRIP) return "GL_TRIANGLE_STRIP";
    if (value == GL_TRIANGLE_FAN) return "GL_TRIANGLE_FAN";
    if (value == GL_NEVER) return "GL_NEVER";
    if (value == GL_LESS) return "GL_LESS";
    if (value == GL_EQUAL) return "GL_EQUAL";
    if (value == GL_LEQUAL) return "GL_LEQUAL";
    if (value == GL_GREATER) return "GL_GREATER";
    if (value == GL_NOTEQUAL) return "GL_NOTEQUAL";
    if (value == GL_GEQUAL) return "GL_GEQUAL";
    if (value == GL_ALWAYS) return "GL_ALWAYS";
    if (value == GL_ZERO) return "GL_ZERO";
    if (value == GL_ONE) return "GL_ONE";
    if (value == GL_SRC_COLOR) return "GL_SRC_COLOR";
    if (value == GL_ONE_MINUS_SRC_COLOR) return "GL_ONE_MINUS_SRC_COLOR";
    if (value == GL_SRC_ALPHA) return "GL_SRC_ALPHA";
    if (value == GL_ONE_MINUS_SRC_ALPHA) return "GL_ONE_MINUS_SRC_ALPHA";
    if (value == GL_DST_ALPHA) return "GL_DST_ALPHA";
    if (value == GL_ONE_MINUS_DST_ALPHA) return "GL_ONE_MINUS_DST_ALPHA";
    if (value == GL_DST_COLOR) return "GL_DST_COLOR";
    if (value == GL_ONE_MINUS_DST_COLOR) return "GL_ONE_MINUS_DST_COLOR";
    if (value == GL_SRC_ALPHA_SATURATE) return "GL_SRC_ALPHA_SATURATE";
    if (value == GL_NONE) return "GL_NONE";
    if (value == GL_FRONT_LEFT) return "GL_FRONT_LEFT";
    if (value == GL_FRONT_RIGHT) return "GL_FRONT_RIGHT";
    if (value == GL_BACK_LEFT) return "GL_BACK_LEFT";
    if (value == GL_BACK_RIGHT) return "GL_BACK_RIGHT";
    if (value == GL_FRONT) return "GL_FRONT";
    if (value == GL_BACK) return "GL_BACK";
    if (value == GL_LEFT) return "GL_LEFT";
    if (value == GL_RIGHT) return "GL_RIGHT";
    if (value == GL_FRONT_AND_BACK) return "GL_FRONT_AND_BACK";
    if (value == GL_NO_ERROR) return "GL_NO_ERROR";
    if (value == GL_INVALID_ENUM) return "GL_INVALID_ENUM";
    if (value == GL_INVALID_VALUE) return "GL_INVALID_VALUE";
    if (value == GL_INVALID_OPERATION) return "GL_INVALID_OPERATION";
    if (value == GL_OUT_OF_MEMORY) return "GL_OUT_OF_MEMORY";
    if (value == GL_CW) return "GL_CW";
    if (value == GL_CCW) return "GL_CCW";
    if (value == GL_POINT_SIZE) return "GL_POINT_SIZE";
    if (value == GL_POINT_SIZE_RANGE) return "GL_POINT_SIZE_RANGE";
    if (value == GL_POINT_SIZE_GRANULARITY) return "GL_POINT_SIZE_GRANULARITY";
    if (value == GL_LINE_SMOOTH) return "GL_LINE_SMOOTH";
    if (value == GL_LINE_WIDTH) return "GL_LINE_WIDTH";
    if (value == GL_LINE_WIDTH_RANGE) return "GL_LINE_WIDTH_RANGE";
    if (value == GL_LINE_WIDTH_GRANULARITY) return "GL_LINE_WIDTH_GRANULARITY";
    if (value == GL_POLYGON_MODE) return "GL_POLYGON_MODE";
    if (value == GL_POLYGON_SMOOTH) return "GL_POLYGON_SMOOTH";
    if (value == GL_CULL_FACE) return "GL_CULL_FACE";
    if (value == GL_CULL_FACE_MODE) return "GL_CULL_FACE_MODE";
    if (value == GL_FRONT_FACE) return "GL_FRONT_FACE";
    if (value == GL_DEPTH_RANGE) return "GL_DEPTH_RANGE";
    if (value == GL_DEPTH_TEST) return "GL_DEPTH_TEST";
    if (value == GL_DEPTH_WRITEMASK) return "GL_DEPTH_WRITEMASK";
    if (value == GL_DEPTH_CLEAR_VALUE) return "GL_DEPTH_CLEAR_VALUE";
    if (value == GL_DEPTH_FUNC) return "GL_DEPTH_FUNC";
    if (value == GL_STENCIL_TEST) return "GL_STENCIL_TEST";
    if (value == GL_STENCIL_CLEAR_VALUE) return "GL_STENCIL_CLEAR_VALUE";
    if (value == GL_STENCIL_FUNC) return "GL_STENCIL_FUNC";
    if (value == GL_STENCIL_VALUE_MASK) return "GL_STENCIL_VALUE_MASK";
    if (value == GL_STENCIL_FAIL) return "GL_STENCIL_FAIL";
    if (value == GL_STENCIL_PASS_DEPTH_FAIL) return "GL_STENCIL_PASS_DEPTH_FAIL";
    if (value == GL_STENCIL_PASS_DEPTH_PASS) return "GL_STENCIL_PASS_DEPTH_PASS";
    if (value == GL_STENCIL_REF) return "GL_STENCIL_REF";
    if (value == GL_STENCIL_WRITEMASK) return "GL_STENCIL_WRITEMASK";
    if (value == GL_VIEWPORT) return "GL_VIEWPORT";
    if (value == GL_DITHER) return "GL_DITHER";
    if (value == GL_BLEND_DST) return "GL_BLEND_DST";
    if (value == GL_BLEND_SRC) return "GL_BLEND_SRC";
    if (value == GL_BLEND) return "GL_BLEND";
    if (value == GL_LOGIC_OP_MODE) return "GL_LOGIC_OP_MODE";
    if (value == GL_DRAW_BUFFER) return "GL_DRAW_BUFFER";
    if (value == GL_READ_BUFFER) return "GL_READ_BUFFER";
    if (value == GL_SCISSOR_BOX) return "GL_SCISSOR_BOX";
    if (value == GL_SCISSOR_TEST) return "GL_SCISSOR_TEST";
    if (value == GL_COLOR_CLEAR_VALUE) return "GL_COLOR_CLEAR_VALUE";
    if (value == GL_COLOR_WRITEMASK) return "GL_COLOR_WRITEMASK";
    if (value == GL_DOUBLEBUFFER) return "GL_DOUBLEBUFFER";
    if (value == GL_STEREO) return "GL_STEREO";
    if (value == GL_LINE_SMOOTH_HINT) return "GL_LINE_SMOOTH_HINT";
    if (value == GL_POLYGON_SMOOTH_HINT) return "GL_POLYGON_SMOOTH_HINT";
    if (value == GL_UNPACK_SWAP_BYTES) return "GL_UNPACK_SWAP_BYTES";
    if (value == GL_UNPACK_LSB_FIRST) return "GL_UNPACK_LSB_FIRST";
    if (value == GL_UNPACK_ROW_LENGTH) return "GL_UNPACK_ROW_LENGTH";
    if (value == GL_UNPACK_SKIP_ROWS) return "GL_UNPACK_SKIP_ROWS";
    if (value == GL_UNPACK_SKIP_PIXELS) return "GL_UNPACK_SKIP_PIXELS";
    if (value == GL_UNPACK_ALIGNMENT) return "GL_UNPACK_ALIGNMENT";
    if (value == GL_PACK_SWAP_BYTES) return "GL_PACK_SWAP_BYTES";
    if (value == GL_PACK_LSB_FIRST) return "GL_PACK_LSB_FIRST";
    if (value == GL_PACK_ROW_LENGTH) return "GL_PACK_ROW_LENGTH";
    if (value == GL_PACK_SKIP_ROWS) return "GL_PACK_SKIP_ROWS";
    if (value == GL_PACK_SKIP_PIXELS) return "GL_PACK_SKIP_PIXELS";
    if (value == GL_PACK_ALIGNMENT) return "GL_PACK_ALIGNMENT";
    if (value == GL_MAX_TEXTURE_SIZE) return "GL_MAX_TEXTURE_SIZE";
    if (value == GL_MAX_VIEWPORT_DIMS) return "GL_MAX_VIEWPORT_DIMS";
    if (value == GL_SUBPIXEL_BITS) return "GL_SUBPIXEL_BITS";
    if (value == GL_TEXTURE_1D) return "GL_TEXTURE_1D";
    if (value == GL_TEXTURE_2D) return "GL_TEXTURE_2D";
    if (value == GL_TEXTURE_WIDTH) return "GL_TEXTURE_WIDTH";
    if (value == GL_TEXTURE_HEIGHT) return "GL_TEXTURE_HEIGHT";
    if (value == GL_TEXTURE_BORDER_COLOR) return "GL_TEXTURE_BORDER_COLOR";
    if (value == GL_DONT_CARE) return "GL_DONT_CARE";
    if (value == GL_FASTEST) return "GL_FASTEST";
    if (value == GL_NICEST) return "GL_NICEST";
    if (value == GL_BYTE) return "GL_BYTE";
    if (value == GL_UNSIGNED_BYTE) return "GL_UNSIGNED_BYTE";
    if (value == GL_SHORT) return "GL_SHORT";
    if (value == GL_UNSIGNED_SHORT) return "GL_UNSIGNED_SHORT";
    if (value == GL_INT) return "GL_INT";
    if (value == GL_UNSIGNED_INT) return "GL_UNSIGNED_INT";
    if (value == GL_FLOAT) return "GL_FLOAT";
    if (value == GL_CLEAR) return "GL_CLEAR";
    if (value == GL_AND) return "GL_AND";
    if (value == GL_AND_REVERSE) return "GL_AND_REVERSE";
    if (value == GL_COPY) return "GL_COPY";
    if (value == GL_AND_INVERTED) return "GL_AND_INVERTED";
    if (value == GL_NOOP) return "GL_NOOP";
    if (value == GL_XOR) return "GL_XOR";
    if (value == GL_OR) return "GL_OR";
    if (value == GL_NOR) return "GL_NOR";
    if (value == GL_EQUIV) return "GL_EQUIV";
    if (value == GL_INVERT) return "GL_INVERT";
    if (value == GL_OR_REVERSE) return "GL_OR_REVERSE";
    if (value == GL_COPY_INVERTED) return "GL_COPY_INVERTED";
    if (value == GL_OR_INVERTED) return "GL_OR_INVERTED";
    if (value == GL_NAND) return "GL_NAND";
    if (value == GL_SET) return "GL_SET";
    if (value == GL_TEXTURE) return "GL_TEXTURE";
    if (value == GL_COLOR) return "GL_COLOR";
    if (value == GL_DEPTH) return "GL_DEPTH";
    if (value == GL_STENCIL) return "GL_STENCIL";
    if (value == GL_STENCIL_INDEX) return "GL_STENCIL_INDEX";
    if (value == GL_DEPTH_COMPONENT) return "GL_DEPTH_COMPONENT";
    if (value == GL_RED) return "GL_RED";
    if (value == GL_GREEN) return "GL_GREEN";
    if (value == GL_BLUE) return "GL_BLUE";
    if (value == GL_ALPHA) return "GL_ALPHA";
    if (value == GL_RGB) return "GL_RGB";
    if (value == GL_RGBA) return "GL_RGBA";
    if (value == GL_POINT) return "GL_POINT";
    if (value == GL_LINE) return "GL_LINE";
    if (value == GL_FILL) return "GL_FILL";
    if (value == GL_KEEP) return "GL_KEEP";
    if (value == GL_REPLACE) return "GL_REPLACE";
    if (value == GL_INCR) return "GL_INCR";
    if (value == GL_DECR) return "GL_DECR";
    if (value == GL_VENDOR) return "GL_VENDOR";
    if (value == GL_RENDERER) return "GL_RENDERER";
    if (value == GL_VERSION) return "GL_VERSION";
    if (value == GL_EXTENSIONS) return "GL_EXTENSIONS";
    if (value == GL_NEAREST) return "GL_NEAREST";
    if (value == GL_LINEAR) return "GL_LINEAR";
    if (value == GL_NEAREST_MIPMAP_NEAREST) return "GL_NEAREST_MIPMAP_NEAREST";
    if (value == GL_LINEAR_MIPMAP_NEAREST) return "GL_LINEAR_MIPMAP_NEAREST";
    if (value == GL_NEAREST_MIPMAP_LINEAR) return "GL_NEAREST_MIPMAP_LINEAR";
    if (value == GL_LINEAR_MIPMAP_LINEAR) return "GL_LINEAR_MIPMAP_LINEAR";
    if (value == GL_TEXTURE_MAG_FILTER) return "GL_TEXTURE_MAG_FILTER";
    if (value == GL_TEXTURE_MIN_FILTER) return "GL_TEXTURE_MIN_FILTER";
    if (value == GL_TEXTURE_WRAP_S) return "GL_TEXTURE_WRAP_S";
    if (value == GL_TEXTURE_WRAP_T) return "GL_TEXTURE_WRAP_T";
    if (value == GL_REPEAT) return "GL_REPEAT";
    if (value == GL_COLOR_LOGIC_OP) return "GL_COLOR_LOGIC_OP";
    if (value == GL_POLYGON_OFFSET_UNITS) return "GL_POLYGON_OFFSET_UNITS";
    if (value == GL_POLYGON_OFFSET_POINT) return "GL_POLYGON_OFFSET_POINT";
    if (value == GL_POLYGON_OFFSET_LINE) return "GL_POLYGON_OFFSET_LINE";
    if (value == GL_POLYGON_OFFSET_FILL) return "GL_POLYGON_OFFSET_FILL";
    if (value == GL_POLYGON_OFFSET_FACTOR) return "GL_POLYGON_OFFSET_FACTOR";
    if (value == GL_TEXTURE_BINDING_1D) return "GL_TEXTURE_BINDING_1D";
    if (value == GL_TEXTURE_BINDING_2D) return "GL_TEXTURE_BINDING_2D";
    if (value == GL_TEXTURE_INTERNAL_FORMAT) return "GL_TEXTURE_INTERNAL_FORMAT";
    if (value == GL_TEXTURE_RED_SIZE) return "GL_TEXTURE_RED_SIZE";
    if (value == GL_TEXTURE_GREEN_SIZE) return "GL_TEXTURE_GREEN_SIZE";
    if (value == GL_TEXTURE_BLUE_SIZE) return "GL_TEXTURE_BLUE_SIZE";
    if (value == GL_TEXTURE_ALPHA_SIZE) return "GL_TEXTURE_ALPHA_SIZE";
    if (value == GL_DOUBLE) return "GL_DOUBLE";
    if (value == GL_PROXY_TEXTURE_1D) return "GL_PROXY_TEXTURE_1D";
    if (value == GL_PROXY_TEXTURE_2D) return "GL_PROXY_TEXTURE_2D";
    if (value == GL_R3_G3_B2) return "GL_R3_G3_B2";
    if (value == GL_RGB4) return "GL_RGB4";
    if (value == GL_RGB5) return "GL_RGB5";
    if (value == GL_RGB8) return "GL_RGB8";
    if (value == GL_RGB10) return "GL_RGB10";
    if (value == GL_RGB12) return "GL_RGB12";
    if (value == GL_RGB16) return "GL_RGB16";
    if (value == GL_RGBA2) return "GL_RGBA2";
    if (value == GL_RGBA4) return "GL_RGBA4";
    if (value == GL_RGB5_A1) return "GL_RGB5_A1";
    if (value == GL_RGBA8) return "GL_RGBA8";
    if (value == GL_RGB10_A2) return "GL_RGB10_A2";
    if (value == GL_RGBA12) return "GL_RGBA12";
    if (value == GL_RGBA16) return "GL_RGBA16";
    if (value == GL_UNSIGNED_BYTE_3_3_2) return "GL_UNSIGNED_BYTE_3_3_2";
    if (value == GL_UNSIGNED_SHORT_4_4_4_4) return "GL_UNSIGNED_SHORT_4_4_4_4";
    if (value == GL_UNSIGNED_SHORT_5_5_5_1) return "GL_UNSIGNED_SHORT_5_5_5_1";
    if (value == GL_UNSIGNED_INT_8_8_8_8) return "GL_UNSIGNED_INT_8_8_8_8";
    if (value == GL_UNSIGNED_INT_10_10_10_2) return "GL_UNSIGNED_INT_10_10_10_2";
    if (value == GL_TEXTURE_BINDING_3D) return "GL_TEXTURE_BINDING_3D";
    if (value == GL_PACK_SKIP_IMAGES) return "GL_PACK_SKIP_IMAGES";
    if (value == GL_PACK_IMAGE_HEIGHT) return "GL_PACK_IMAGE_HEIGHT";
    if (value == GL_UNPACK_SKIP_IMAGES) return "GL_UNPACK_SKIP_IMAGES";
    if (value == GL_UNPACK_IMAGE_HEIGHT) return "GL_UNPACK_IMAGE_HEIGHT";
    if (value == GL_TEXTURE_3D) return "GL_TEXTURE_3D";
    if (value == GL_PROXY_TEXTURE_3D) return "GL_PROXY_TEXTURE_3D";
    if (value == GL_TEXTURE_DEPTH) return "GL_TEXTURE_DEPTH";
    if (value == GL_TEXTURE_WRAP_R) return "GL_TEXTURE_WRAP_R";
    if (value == GL_MAX_3D_TEXTURE_SIZE) return "GL_MAX_3D_TEXTURE_SIZE";
    if (value == GL_UNSIGNED_BYTE_2_3_3_REV) return "GL_UNSIGNED_BYTE_2_3_3_REV";
    if (value == GL_UNSIGNED_SHORT_5_6_5) return "GL_UNSIGNED_SHORT_5_6_5";
    if (value == GL_UNSIGNED_SHORT_5_6_5_REV) return "GL_UNSIGNED_SHORT_5_6_5_REV";
    if (value == GL_UNSIGNED_SHORT_4_4_4_4_REV) return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
    if (value == GL_UNSIGNED_SHORT_1_5_5_5_REV) return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
    if (value == GL_UNSIGNED_INT_8_8_8_8_REV) return "GL_UNSIGNED_INT_8_8_8_8_REV";
    if (value == GL_UNSIGNED_INT_2_10_10_10_REV) return "GL_UNSIGNED_INT_2_10_10_10_REV";
    if (value == GL_BGR) return "GL_BGR";
    if (value == GL_BGRA) return "GL_BGRA";
    if (value == GL_MAX_ELEMENTS_VERTICES) return "GL_MAX_ELEMENTS_VERTICES";
    if (value == GL_MAX_ELEMENTS_INDICES) return "GL_MAX_ELEMENTS_INDICES";
    if (value == GL_CLAMP_TO_EDGE) return "GL_CLAMP_TO_EDGE";
    if (value == GL_TEXTURE_MIN_LOD) return "GL_TEXTURE_MIN_LOD";
    if (value == GL_TEXTURE_MAX_LOD) return "GL_TEXTURE_MAX_LOD";
    if (value == GL_TEXTURE_BASE_LEVEL) return "GL_TEXTURE_BASE_LEVEL";
    if (value == GL_TEXTURE_MAX_LEVEL) return "GL_TEXTURE_MAX_LEVEL";
    if (value == GL_SMOOTH_POINT_SIZE_RANGE) return "GL_SMOOTH_POINT_SIZE_RANGE";
    if (value == GL_SMOOTH_POINT_SIZE_GRANULARITY) return "GL_SMOOTH_POINT_SIZE_GRANULARITY";
    if (value == GL_SMOOTH_LINE_WIDTH_RANGE) return "GL_SMOOTH_LINE_WIDTH_RANGE";
    if (value == GL_SMOOTH_LINE_WIDTH_GRANULARITY) return "GL_SMOOTH_LINE_WIDTH_GRANULARITY";
    if (value == GL_ALIASED_LINE_WIDTH_RANGE) return "GL_ALIASED_LINE_WIDTH_RANGE";
    if (value == GL_TEXTURE0) return "GL_TEXTURE0";
    if (value == GL_TEXTURE1) return "GL_TEXTURE1";
    if (value == GL_TEXTURE2) return "GL_TEXTURE2";
    if (value == GL_TEXTURE3) return "GL_TEXTURE3";
    if (value == GL_TEXTURE4) return "GL_TEXTURE4";
    if (value == GL_TEXTURE5) return "GL_TEXTURE5";
    if (value == GL_TEXTURE6) return "GL_TEXTURE6";
    if (value == GL_TEXTURE7) return "GL_TEXTURE7";
    if (value == GL_TEXTURE8) return "GL_TEXTURE8";
    if (value == GL_TEXTURE9) return "GL_TEXTURE9";
    if (value == GL_TEXTURE10) return "GL_TEXTURE10";
    if (value == GL_TEXTURE11) return "GL_TEXTURE11";
    if (value == GL_TEXTURE12) return "GL_TEXTURE12";
    if (value == GL_TEXTURE13) return "GL_TEXTURE13";
    if (value == GL_TEXTURE14) return "GL_TEXTURE14";
    if (value == GL_TEXTURE15) return "GL_TEXTURE15";
    if (value == GL_TEXTURE16) return "GL_TEXTURE16";
    if (value == GL_TEXTURE17) return "GL_TEXTURE17";
    if (value == GL_TEXTURE18) return "GL_TEXTURE18";
    if (value == GL_TEXTURE19) return "GL_TEXTURE19";
    if (value == GL_TEXTURE20) return "GL_TEXTURE20";
    if (value == GL_TEXTURE21) return "GL_TEXTURE21";
    if (value == GL_TEXTURE22) return "GL_TEXTURE22";
    if (value == GL_TEXTURE23) return "GL_TEXTURE23";
    if (value == GL_TEXTURE24) return "GL_TEXTURE24";
    if (value == GL_TEXTURE25) return "GL_TEXTURE25";
    if (value == GL_TEXTURE26) return "GL_TEXTURE26";
    if (value == GL_TEXTURE27) return "GL_TEXTURE27";
    if (value == GL_TEXTURE28) return "GL_TEXTURE28";
    if (value == GL_TEXTURE29) return "GL_TEXTURE29";
    if (value == GL_TEXTURE30) return "GL_TEXTURE30";
    if (value == GL_TEXTURE31) return "GL_TEXTURE31";
    if (value == GL_ACTIVE_TEXTURE) return "GL_ACTIVE_TEXTURE";
    if (value == GL_MULTISAMPLE) return "GL_MULTISAMPLE";
    if (value == GL_SAMPLE_ALPHA_TO_COVERAGE) return "GL_SAMPLE_ALPHA_TO_COVERAGE";
    if (value == GL_SAMPLE_ALPHA_TO_ONE) return "GL_SAMPLE_ALPHA_TO_ONE";
    if (value == GL_SAMPLE_COVERAGE) return "GL_SAMPLE_COVERAGE";
    if (value == GL_SAMPLE_BUFFERS) return "GL_SAMPLE_BUFFERS";
    if (value == GL_SAMPLES) return "GL_SAMPLES";
    if (value == GL_SAMPLE_COVERAGE_VALUE) return "GL_SAMPLE_COVERAGE_VALUE";
    if (value == GL_SAMPLE_COVERAGE_INVERT) return "GL_SAMPLE_COVERAGE_INVERT";
    if (value == GL_TEXTURE_CUBE_MAP) return "GL_TEXTURE_CUBE_MAP";
    if (value == GL_TEXTURE_BINDING_CUBE_MAP) return "GL_TEXTURE_BINDING_CUBE_MAP";
    if (value == GL_TEXTURE_CUBE_MAP_POSITIVE_X) return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
    if (value == GL_TEXTURE_CUBE_MAP_NEGATIVE_X) return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
    if (value == GL_TEXTURE_CUBE_MAP_POSITIVE_Y) return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
    if (value == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y) return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
    if (value == GL_TEXTURE_CUBE_MAP_POSITIVE_Z) return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
    if (value == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
    if (value == GL_PROXY_TEXTURE_CUBE_MAP) return "GL_PROXY_TEXTURE_CUBE_MAP";
    if (value == GL_MAX_CUBE_MAP_TEXTURE_SIZE) return "GL_MAX_CUBE_MAP_TEXTURE_SIZE";
    if (value == GL_COMPRESSED_RGB) return "GL_COMPRESSED_RGB";
    if (value == GL_COMPRESSED_RGBA) return "GL_COMPRESSED_RGBA";
    if (value == GL_TEXTURE_COMPRESSION_HINT) return "GL_TEXTURE_COMPRESSION_HINT";
    if (value == GL_TEXTURE_COMPRESSED_IMAGE_SIZE) return "GL_TEXTURE_COMPRESSED_IMAGE_SIZE";
    if (value == GL_TEXTURE_COMPRESSED) return "GL_TEXTURE_COMPRESSED";
    if (value == GL_NUM_COMPRESSED_TEXTURE_FORMATS) return "GL_NUM_COMPRESSED_TEXTURE_FORMATS";
    if (value == GL_COMPRESSED_TEXTURE_FORMATS) return "GL_COMPRESSED_TEXTURE_FORMATS";
    if (value == GL_CLAMP_TO_BORDER) return "GL_CLAMP_TO_BORDER";
    if (value == GL_BLEND_DST_RGB) return "GL_BLEND_DST_RGB";
    if (value == GL_BLEND_SRC_RGB) return "GL_BLEND_SRC_RGB";
    if (value == GL_BLEND_DST_ALPHA) return "GL_BLEND_DST_ALPHA";
    if (value == GL_BLEND_SRC_ALPHA) return "GL_BLEND_SRC_ALPHA";
    if (value == GL_POINT_FADE_THRESHOLD_SIZE) return "GL_POINT_FADE_THRESHOLD_SIZE";
    if (value == GL_DEPTH_COMPONENT16) return "GL_DEPTH_COMPONENT16";
    if (value == GL_DEPTH_COMPONENT24) return "GL_DEPTH_COMPONENT24";
    if (value == GL_DEPTH_COMPONENT32) return "GL_DEPTH_COMPONENT32";
    if (value == GL_MIRRORED_REPEAT) return "GL_MIRRORED_REPEAT";
    if (value == GL_MAX_TEXTURE_LOD_BIAS) return "GL_MAX_TEXTURE_LOD_BIAS";
    if (value == GL_TEXTURE_LOD_BIAS) return "GL_TEXTURE_LOD_BIAS";
    if (value == GL_INCR_WRAP) return "GL_INCR_WRAP";
    if (value == GL_DECR_WRAP) return "GL_DECR_WRAP";
    if (value == GL_TEXTURE_DEPTH_SIZE) return "GL_TEXTURE_DEPTH_SIZE";
    if (value == GL_TEXTURE_COMPARE_MODE) return "GL_TEXTURE_COMPARE_MODE";
    if (value == GL_TEXTURE_COMPARE_FUNC) return "GL_TEXTURE_COMPARE_FUNC";
    if (value == GL_BLEND_COLOR) return "GL_BLEND_COLOR";
    if (value == GL_BLEND_EQUATION) return "GL_BLEND_EQUATION";
    if (value == GL_CONSTANT_COLOR) return "GL_CONSTANT_COLOR";
    if (value == GL_ONE_MINUS_CONSTANT_COLOR) return "GL_ONE_MINUS_CONSTANT_COLOR";
    if (value == GL_CONSTANT_ALPHA) return "GL_CONSTANT_ALPHA";
    if (value == GL_ONE_MINUS_CONSTANT_ALPHA) return "GL_ONE_MINUS_CONSTANT_ALPHA";
    if (value == GL_FUNC_ADD) return "GL_FUNC_ADD";
    if (value == GL_FUNC_REVERSE_SUBTRACT) return "GL_FUNC_REVERSE_SUBTRACT";
    if (value == GL_FUNC_SUBTRACT) return "GL_FUNC_SUBTRACT";
    if (value == GL_MIN) return "GL_MIN";
    if (value == GL_MAX) return "GL_MAX";
    if (value == GL_BUFFER_SIZE) return "GL_BUFFER_SIZE";
    if (value == GL_BUFFER_USAGE) return "GL_BUFFER_USAGE";
    if (value == GL_QUERY_COUNTER_BITS) return "GL_QUERY_COUNTER_BITS";
    if (value == GL_CURRENT_QUERY) return "GL_CURRENT_QUERY";
    if (value == GL_QUERY_RESULT) return "GL_QUERY_RESULT";
    if (value == GL_QUERY_RESULT_AVAILABLE) return "GL_QUERY_RESULT_AVAILABLE";
    if (value == GL_ARRAY_BUFFER) return "GL_ARRAY_BUFFER";
    if (value == GL_ELEMENT_ARRAY_BUFFER) return "GL_ELEMENT_ARRAY_BUFFER";
    if (value == GL_ARRAY_BUFFER_BINDING) return "GL_ARRAY_BUFFER_BINDING";
    if (value == GL_ELEMENT_ARRAY_BUFFER_BINDING) return "GL_ELEMENT_ARRAY_BUFFER_BINDING";
    if (value == GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING) return "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING";
    if (value == GL_READ_ONLY) return "GL_READ_ONLY";
    if (value == GL_WRITE_ONLY) return "GL_WRITE_ONLY";
    if (value == GL_READ_WRITE) return "GL_READ_WRITE";
    if (value == GL_BUFFER_ACCESS) return "GL_BUFFER_ACCESS";
    if (value == GL_BUFFER_MAPPED) return "GL_BUFFER_MAPPED";
    if (value == GL_BUFFER_MAP_POINTER) return "GL_BUFFER_MAP_POINTER";
    if (value == GL_STREAM_DRAW) return "GL_STREAM_DRAW";
    if (value == GL_STREAM_READ) return "GL_STREAM_READ";
    if (value == GL_STREAM_COPY) return "GL_STREAM_COPY";
    if (value == GL_STATIC_DRAW) return "GL_STATIC_DRAW";
    if (value == GL_STATIC_READ) return "GL_STATIC_READ";
    if (value == GL_STATIC_COPY) return "GL_STATIC_COPY";
    if (value == GL_DYNAMIC_DRAW) return "GL_DYNAMIC_DRAW";
    if (value == GL_DYNAMIC_READ) return "GL_DYNAMIC_READ";
    if (value == GL_DYNAMIC_COPY) return "GL_DYNAMIC_COPY";
    if (value == GL_SAMPLES_PASSED) return "GL_SAMPLES_PASSED";
    if (value == GL_SRC1_ALPHA) return "GL_SRC1_ALPHA";
    if (value == GL_BLEND_EQUATION_RGB) return "GL_BLEND_EQUATION_RGB";
    if (value == GL_VERTEX_ATTRIB_ARRAY_ENABLED) return "GL_VERTEX_ATTRIB_ARRAY_ENABLED";
    if (value == GL_VERTEX_ATTRIB_ARRAY_SIZE) return "GL_VERTEX_ATTRIB_ARRAY_SIZE";
    if (value == GL_VERTEX_ATTRIB_ARRAY_STRIDE) return "GL_VERTEX_ATTRIB_ARRAY_STRIDE";
    if (value == GL_VERTEX_ATTRIB_ARRAY_TYPE) return "GL_VERTEX_ATTRIB_ARRAY_TYPE";
    if (value == GL_CURRENT_VERTEX_ATTRIB) return "GL_CURRENT_VERTEX_ATTRIB";
    if (value == GL_VERTEX_PROGRAM_POINT_SIZE) return "GL_VERTEX_PROGRAM_POINT_SIZE";
    if (value == GL_VERTEX_ATTRIB_ARRAY_POINTER) return "GL_VERTEX_ATTRIB_ARRAY_POINTER";
    if (value == GL_STENCIL_BACK_FUNC) return "GL_STENCIL_BACK_FUNC";
    if (value == GL_STENCIL_BACK_FAIL) return "GL_STENCIL_BACK_FAIL";
    if (value == GL_STENCIL_BACK_PASS_DEPTH_FAIL) return "GL_STENCIL_BACK_PASS_DEPTH_FAIL";
    if (value == GL_STENCIL_BACK_PASS_DEPTH_PASS) return "GL_STENCIL_BACK_PASS_DEPTH_PASS";
    if (value == GL_MAX_DRAW_BUFFERS) return "GL_MAX_DRAW_BUFFERS";
    if (value == GL_DRAW_BUFFER0) return "GL_DRAW_BUFFER0";
    if (value == GL_DRAW_BUFFER1) return "GL_DRAW_BUFFER1";
    if (value == GL_DRAW_BUFFER2) return "GL_DRAW_BUFFER2";
    if (value == GL_DRAW_BUFFER3) return "GL_DRAW_BUFFER3";
    if (value == GL_DRAW_BUFFER4) return "GL_DRAW_BUFFER4";
    if (value == GL_DRAW_BUFFER5) return "GL_DRAW_BUFFER5";
    if (value == GL_DRAW_BUFFER6) return "GL_DRAW_BUFFER6";
    if (value == GL_DRAW_BUFFER7) return "GL_DRAW_BUFFER7";
    if (value == GL_DRAW_BUFFER8) return "GL_DRAW_BUFFER8";
    if (value == GL_DRAW_BUFFER9) return "GL_DRAW_BUFFER9";
    if (value == GL_DRAW_BUFFER10) return "GL_DRAW_BUFFER10";
    if (value == GL_DRAW_BUFFER11) return "GL_DRAW_BUFFER11";
    if (value == GL_DRAW_BUFFER12) return "GL_DRAW_BUFFER12";
    if (value == GL_DRAW_BUFFER13) return "GL_DRAW_BUFFER13";
    if (value == GL_DRAW_BUFFER14) return "GL_DRAW_BUFFER14";
    if (value == GL_DRAW_BUFFER15) return "GL_DRAW_BUFFER15";
    if (value == GL_BLEND_EQUATION_ALPHA) return "GL_BLEND_EQUATION_ALPHA";
    if (value == GL_MAX_VERTEX_ATTRIBS) return "GL_MAX_VERTEX_ATTRIBS";
    if (value == GL_VERTEX_ATTRIB_ARRAY_NORMALIZED) return "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED";
    if (value == GL_MAX_TEXTURE_IMAGE_UNITS) return "GL_MAX_TEXTURE_IMAGE_UNITS";
    if (value == GL_FRAGMENT_SHADER) return "GL_FRAGMENT_SHADER";
    if (value == GL_VERTEX_SHADER) return "GL_VERTEX_SHADER";
    if (value == GL_MAX_FRAGMENT_UNIFORM_COMPONENTS) return "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS";
    if (value == GL_MAX_VERTEX_UNIFORM_COMPONENTS) return "GL_MAX_VERTEX_UNIFORM_COMPONENTS";
    if (value == GL_MAX_VARYING_FLOATS) return "GL_MAX_VARYING_FLOATS";
    if (value == GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS) return "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS";
    if (value == GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) return "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS";
    if (value == GL_SHADER_TYPE) return "GL_SHADER_TYPE";
    if (value == GL_FLOAT_VEC2) return "GL_FLOAT_VEC2";
    if (value == GL_FLOAT_VEC3) return "GL_FLOAT_VEC3";
    if (value == GL_FLOAT_VEC4) return "GL_FLOAT_VEC4";
    if (value == GL_INT_VEC2) return "GL_INT_VEC2";
    if (value == GL_INT_VEC3) return "GL_INT_VEC3";
    if (value == GL_INT_VEC4) return "GL_INT_VEC4";
    if (value == GL_BOOL) return "GL_BOOL";
    if (value == GL_BOOL_VEC2) return "GL_BOOL_VEC2";
    if (value == GL_BOOL_VEC3) return "GL_BOOL_VEC3";
    if (value == GL_BOOL_VEC4) return "GL_BOOL_VEC4";
    if (value == GL_FLOAT_MAT2) return "GL_FLOAT_MAT2";
    if (value == GL_FLOAT_MAT3) return "GL_FLOAT_MAT3";
    if (value == GL_FLOAT_MAT4) return "GL_FLOAT_MAT4";
    if (value == GL_SAMPLER_1D) return "GL_SAMPLER_1D";
    if (value == GL_SAMPLER_2D) return "GL_SAMPLER_2D";
    if (value == GL_SAMPLER_3D) return "GL_SAMPLER_3D";
    if (value == GL_SAMPLER_CUBE) return "GL_SAMPLER_CUBE";
    if (value == GL_SAMPLER_1D_SHADOW) return "GL_SAMPLER_1D_SHADOW";
    if (value == GL_SAMPLER_2D_SHADOW) return "GL_SAMPLER_2D_SHADOW";
    if (value == GL_DELETE_STATUS) return "GL_DELETE_STATUS";
    if (value == GL_COMPILE_STATUS) return "GL_COMPILE_STATUS";
    if (value == GL_LINK_STATUS) return "GL_LINK_STATUS";
    if (value == GL_VALIDATE_STATUS) return "GL_VALIDATE_STATUS";
    if (value == GL_INFO_LOG_LENGTH) return "GL_INFO_LOG_LENGTH";
    if (value == GL_ATTACHED_SHADERS) return "GL_ATTACHED_SHADERS";
    if (value == GL_ACTIVE_UNIFORMS) return "GL_ACTIVE_UNIFORMS";
    if (value == GL_ACTIVE_UNIFORM_MAX_LENGTH) return "GL_ACTIVE_UNIFORM_MAX_LENGTH";
    if (value == GL_SHADER_SOURCE_LENGTH) return "GL_SHADER_SOURCE_LENGTH";
    if (value == GL_ACTIVE_ATTRIBUTES) return "GL_ACTIVE_ATTRIBUTES";
    if (value == GL_ACTIVE_ATTRIBUTE_MAX_LENGTH) return "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH";
    if (value == GL_FRAGMENT_SHADER_DERIVATIVE_HINT) return "GL_FRAGMENT_SHADER_DERIVATIVE_HINT";
    if (value == GL_SHADING_LANGUAGE_VERSION) return "GL_SHADING_LANGUAGE_VERSION";
    if (value == GL_CURRENT_PROGRAM) return "GL_CURRENT_PROGRAM";
    if (value == GL_POINT_SPRITE_COORD_ORIGIN) return "GL_POINT_SPRITE_COORD_ORIGIN";
    if (value == GL_LOWER_LEFT) return "GL_LOWER_LEFT";
    if (value == GL_UPPER_LEFT) return "GL_UPPER_LEFT";
    if (value == GL_STENCIL_BACK_REF) return "GL_STENCIL_BACK_REF";
    if (value == GL_STENCIL_BACK_VALUE_MASK) return "GL_STENCIL_BACK_VALUE_MASK";
    if (value == GL_STENCIL_BACK_WRITEMASK) return "GL_STENCIL_BACK_WRITEMASK";
    if (value == GL_PIXEL_PACK_BUFFER) return "GL_PIXEL_PACK_BUFFER";
    if (value == GL_PIXEL_UNPACK_BUFFER) return "GL_PIXEL_UNPACK_BUFFER";
    if (value == GL_PIXEL_PACK_BUFFER_BINDING) return "GL_PIXEL_PACK_BUFFER_BINDING";
    if (value == GL_PIXEL_UNPACK_BUFFER_BINDING) return "GL_PIXEL_UNPACK_BUFFER_BINDING";
    if (value == GL_FLOAT_MAT2x3) return "GL_FLOAT_MAT2x3";
    if (value == GL_FLOAT_MAT2x4) return "GL_FLOAT_MAT2x4";
    if (value == GL_FLOAT_MAT3x2) return "GL_FLOAT_MAT3x2";
    if (value == GL_FLOAT_MAT3x4) return "GL_FLOAT_MAT3x4";
    if (value == GL_FLOAT_MAT4x2) return "GL_FLOAT_MAT4x2";
    if (value == GL_FLOAT_MAT4x3) return "GL_FLOAT_MAT4x3";
    if (value == GL_SRGB) return "GL_SRGB";
    if (value == GL_SRGB8) return "GL_SRGB8";
    if (value == GL_SRGB_ALPHA) return "GL_SRGB_ALPHA";
    if (value == GL_SRGB8_ALPHA8) return "GL_SRGB8_ALPHA8";
    if (value == GL_COMPRESSED_SRGB) return "GL_COMPRESSED_SRGB";
    if (value == GL_COMPRESSED_SRGB_ALPHA) return "GL_COMPRESSED_SRGB_ALPHA";
    if (value == GL_COMPARE_REF_TO_TEXTURE) return "GL_COMPARE_REF_TO_TEXTURE";
    if (value == GL_CLIP_DISTANCE0) return "GL_CLIP_DISTANCE0";
    if (value == GL_CLIP_DISTANCE1) return "GL_CLIP_DISTANCE1";
    if (value == GL_CLIP_DISTANCE2) return "GL_CLIP_DISTANCE2";
    if (value == GL_CLIP_DISTANCE3) return "GL_CLIP_DISTANCE3";
    if (value == GL_CLIP_DISTANCE4) return "GL_CLIP_DISTANCE4";
    if (value == GL_CLIP_DISTANCE5) return "GL_CLIP_DISTANCE5";
    if (value == GL_CLIP_DISTANCE6) return "GL_CLIP_DISTANCE6";
    if (value == GL_CLIP_DISTANCE7) return "GL_CLIP_DISTANCE7";
    if (value == GL_MAX_CLIP_DISTANCES) return "GL_MAX_CLIP_DISTANCES";
    if (value == GL_MAJOR_VERSION) return "GL_MAJOR_VERSION";
    if (value == GL_MINOR_VERSION) return "GL_MINOR_VERSION";
    if (value == GL_NUM_EXTENSIONS) return "GL_NUM_EXTENSIONS";
    if (value == GL_CONTEXT_FLAGS) return "GL_CONTEXT_FLAGS";
    if (value == GL_COMPRESSED_RED) return "GL_COMPRESSED_RED";
    if (value == GL_COMPRESSED_RG) return "GL_COMPRESSED_RG";
    if (value == GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) return "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT";
    if (value == GL_RGBA32F) return "GL_RGBA32F";
    if (value == GL_RGB32F) return "GL_RGB32F";
    if (value == GL_RGBA16F) return "GL_RGBA16F";
    if (value == GL_RGB16F) return "GL_RGB16F";
    if (value == GL_VERTEX_ATTRIB_ARRAY_INTEGER) return "GL_VERTEX_ATTRIB_ARRAY_INTEGER";
    if (value == GL_MAX_ARRAY_TEXTURE_LAYERS) return "GL_MAX_ARRAY_TEXTURE_LAYERS";
    if (value == GL_MIN_PROGRAM_TEXEL_OFFSET) return "GL_MIN_PROGRAM_TEXEL_OFFSET";
    if (value == GL_MAX_PROGRAM_TEXEL_OFFSET) return "GL_MAX_PROGRAM_TEXEL_OFFSET";
    if (value == GL_CLAMP_READ_COLOR) return "GL_CLAMP_READ_COLOR";
    if (value == GL_FIXED_ONLY) return "GL_FIXED_ONLY";
    if (value == GL_MAX_VARYING_COMPONENTS) return "GL_MAX_VARYING_COMPONENTS";
    if (value == GL_TEXTURE_1D_ARRAY) return "GL_TEXTURE_1D_ARRAY";
    if (value == GL_PROXY_TEXTURE_1D_ARRAY) return "GL_PROXY_TEXTURE_1D_ARRAY";
    if (value == GL_TEXTURE_2D_ARRAY) return "GL_TEXTURE_2D_ARRAY";
    if (value == GL_PROXY_TEXTURE_2D_ARRAY) return "GL_PROXY_TEXTURE_2D_ARRAY";
    if (value == GL_TEXTURE_BINDING_1D_ARRAY) return "GL_TEXTURE_BINDING_1D_ARRAY";
    if (value == GL_TEXTURE_BINDING_2D_ARRAY) return "GL_TEXTURE_BINDING_2D_ARRAY";
    if (value == GL_R11F_G11F_B10F) return "GL_R11F_G11F_B10F";
    if (value == GL_UNSIGNED_INT_10F_11F_11F_REV) return "GL_UNSIGNED_INT_10F_11F_11F_REV";
    if (value == GL_RGB9_E5) return "GL_RGB9_E5";
    if (value == GL_UNSIGNED_INT_5_9_9_9_REV) return "GL_UNSIGNED_INT_5_9_9_9_REV";
    if (value == GL_TEXTURE_SHARED_SIZE) return "GL_TEXTURE_SHARED_SIZE";
    if (value == GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH) return "GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_MODE) return "GL_TRANSFORM_FEEDBACK_BUFFER_MODE";
    if (value == GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS) return "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS";
    if (value == GL_TRANSFORM_FEEDBACK_VARYINGS) return "GL_TRANSFORM_FEEDBACK_VARYINGS";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_START) return "GL_TRANSFORM_FEEDBACK_BUFFER_START";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_SIZE) return "GL_TRANSFORM_FEEDBACK_BUFFER_SIZE";
    if (value == GL_PRIMITIVES_GENERATED) return "GL_PRIMITIVES_GENERATED";
    if (value == GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN) return "GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN";
    if (value == GL_RASTERIZER_DISCARD) return "GL_RASTERIZER_DISCARD";
    if (value == GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS)
        return "GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS";
    if (value == GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS) return "GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS";
    if (value == GL_INTERLEAVED_ATTRIBS) return "GL_INTERLEAVED_ATTRIBS";
    if (value == GL_SEPARATE_ATTRIBS) return "GL_SEPARATE_ATTRIBS";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER) return "GL_TRANSFORM_FEEDBACK_BUFFER";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_BINDING) return "GL_TRANSFORM_FEEDBACK_BUFFER_BINDING";
    if (value == GL_RGBA32UI) return "GL_RGBA32UI";
    if (value == GL_RGB32UI) return "GL_RGB32UI";
    if (value == GL_RGBA16UI) return "GL_RGBA16UI";
    if (value == GL_RGB16UI) return "GL_RGB16UI";
    if (value == GL_RGBA8UI) return "GL_RGBA8UI";
    if (value == GL_RGB8UI) return "GL_RGB8UI";
    if (value == GL_RGBA32I) return "GL_RGBA32I";
    if (value == GL_RGB32I) return "GL_RGB32I";
    if (value == GL_RGBA16I) return "GL_RGBA16I";
    if (value == GL_RGB16I) return "GL_RGB16I";
    if (value == GL_RGBA8I) return "GL_RGBA8I";
    if (value == GL_RGB8I) return "GL_RGB8I";
    if (value == GL_RED_INTEGER) return "GL_RED_INTEGER";
    if (value == GL_GREEN_INTEGER) return "GL_GREEN_INTEGER";
    if (value == GL_BLUE_INTEGER) return "GL_BLUE_INTEGER";
    if (value == GL_RGB_INTEGER) return "GL_RGB_INTEGER";
    if (value == GL_RGBA_INTEGER) return "GL_RGBA_INTEGER";
    if (value == GL_BGR_INTEGER) return "GL_BGR_INTEGER";
    if (value == GL_BGRA_INTEGER) return "GL_BGRA_INTEGER";
    if (value == GL_SAMPLER_1D_ARRAY) return "GL_SAMPLER_1D_ARRAY";
    if (value == GL_SAMPLER_2D_ARRAY) return "GL_SAMPLER_2D_ARRAY";
    if (value == GL_SAMPLER_1D_ARRAY_SHADOW) return "GL_SAMPLER_1D_ARRAY_SHADOW";
    if (value == GL_SAMPLER_2D_ARRAY_SHADOW) return "GL_SAMPLER_2D_ARRAY_SHADOW";
    if (value == GL_SAMPLER_CUBE_SHADOW) return "GL_SAMPLER_CUBE_SHADOW";
    if (value == GL_UNSIGNED_INT_VEC2) return "GL_UNSIGNED_INT_VEC2";
    if (value == GL_UNSIGNED_INT_VEC3) return "GL_UNSIGNED_INT_VEC3";
    if (value == GL_UNSIGNED_INT_VEC4) return "GL_UNSIGNED_INT_VEC4";
    if (value == GL_INT_SAMPLER_1D) return "GL_INT_SAMPLER_1D";
    if (value == GL_INT_SAMPLER_2D) return "GL_INT_SAMPLER_2D";
    if (value == GL_INT_SAMPLER_3D) return "GL_INT_SAMPLER_3D";
    if (value == GL_INT_SAMPLER_CUBE) return "GL_INT_SAMPLER_CUBE";
    if (value == GL_INT_SAMPLER_1D_ARRAY) return "GL_INT_SAMPLER_1D_ARRAY";
    if (value == GL_INT_SAMPLER_2D_ARRAY) return "GL_INT_SAMPLER_2D_ARRAY";
    if (value == GL_UNSIGNED_INT_SAMPLER_1D) return "GL_UNSIGNED_INT_SAMPLER_1D";
    if (value == GL_UNSIGNED_INT_SAMPLER_2D) return "GL_UNSIGNED_INT_SAMPLER_2D";
    if (value == GL_UNSIGNED_INT_SAMPLER_3D) return "GL_UNSIGNED_INT_SAMPLER_3D";
    if (value == GL_UNSIGNED_INT_SAMPLER_CUBE) return "GL_UNSIGNED_INT_SAMPLER_CUBE";
    if (value == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY) return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
    if (value == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY) return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
    if (value == GL_QUERY_WAIT) return "GL_QUERY_WAIT";
    if (value == GL_QUERY_NO_WAIT) return "GL_QUERY_NO_WAIT";
    if (value == GL_QUERY_BY_REGION_WAIT) return "GL_QUERY_BY_REGION_WAIT";
    if (value == GL_QUERY_BY_REGION_NO_WAIT) return "GL_QUERY_BY_REGION_NO_WAIT";
    if (value == GL_BUFFER_ACCESS_FLAGS) return "GL_BUFFER_ACCESS_FLAGS";
    if (value == GL_BUFFER_MAP_LENGTH) return "GL_BUFFER_MAP_LENGTH";
    if (value == GL_BUFFER_MAP_OFFSET) return "GL_BUFFER_MAP_OFFSET";
    if (value == GL_DEPTH_COMPONENT32F) return "GL_DEPTH_COMPONENT32F";
    if (value == GL_DEPTH32F_STENCIL8) return "GL_DEPTH32F_STENCIL8";
    if (value == GL_FLOAT_32_UNSIGNED_INT_24_8_REV) return "GL_FLOAT_32_UNSIGNED_INT_24_8_REV";
    if (value == GL_INVALID_FRAMEBUFFER_OPERATION) return "GL_INVALID_FRAMEBUFFER_OPERATION";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING) return "GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE) return "GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE) return "GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE) return "GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE) return "GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE) return "GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE) return "GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE) return "GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE";
    if (value == GL_FRAMEBUFFER_DEFAULT) return "GL_FRAMEBUFFER_DEFAULT";
    if (value == GL_FRAMEBUFFER_UNDEFINED) return "GL_FRAMEBUFFER_UNDEFINED";
    if (value == GL_DEPTH_STENCIL_ATTACHMENT) return "GL_DEPTH_STENCIL_ATTACHMENT";
    if (value == GL_MAX_RENDERBUFFER_SIZE) return "GL_MAX_RENDERBUFFER_SIZE";
    if (value == GL_DEPTH_STENCIL) return "GL_DEPTH_STENCIL";
    if (value == GL_UNSIGNED_INT_24_8) return "GL_UNSIGNED_INT_24_8";
    if (value == GL_DEPTH24_STENCIL8) return "GL_DEPTH24_STENCIL8";
    if (value == GL_TEXTURE_STENCIL_SIZE) return "GL_TEXTURE_STENCIL_SIZE";
    if (value == GL_TEXTURE_RED_TYPE) return "GL_TEXTURE_RED_TYPE";
    if (value == GL_TEXTURE_GREEN_TYPE) return "GL_TEXTURE_GREEN_TYPE";
    if (value == GL_TEXTURE_BLUE_TYPE) return "GL_TEXTURE_BLUE_TYPE";
    if (value == GL_TEXTURE_ALPHA_TYPE) return "GL_TEXTURE_ALPHA_TYPE";
    if (value == GL_TEXTURE_DEPTH_TYPE) return "GL_TEXTURE_DEPTH_TYPE";
    if (value == GL_UNSIGNED_NORMALIZED) return "GL_UNSIGNED_NORMALIZED";
    if (value == GL_FRAMEBUFFER_BINDING) return "GL_FRAMEBUFFER_BINDING";
    if (value == GL_DRAW_FRAMEBUFFER_BINDING) return "GL_DRAW_FRAMEBUFFER_BINDING";
    if (value == GL_RENDERBUFFER_BINDING) return "GL_RENDERBUFFER_BINDING";
    if (value == GL_READ_FRAMEBUFFER) return "GL_READ_FRAMEBUFFER";
    if (value == GL_DRAW_FRAMEBUFFER) return "GL_DRAW_FRAMEBUFFER";
    if (value == GL_READ_FRAMEBUFFER_BINDING) return "GL_READ_FRAMEBUFFER_BINDING";
    if (value == GL_RENDERBUFFER_SAMPLES) return "GL_RENDERBUFFER_SAMPLES";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE) return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME) return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL) return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE)
        return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER) return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER";
    if (value == GL_FRAMEBUFFER_COMPLETE) return "GL_FRAMEBUFFER_COMPLETE";
    if (value == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    if (value == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    if (value == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    if (value == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    if (value == GL_FRAMEBUFFER_UNSUPPORTED) return "GL_FRAMEBUFFER_UNSUPPORTED";
    if (value == GL_MAX_COLOR_ATTACHMENTS) return "GL_MAX_COLOR_ATTACHMENTS";
    if (value == GL_COLOR_ATTACHMENT0) return "GL_COLOR_ATTACHMENT0";
    if (value == GL_COLOR_ATTACHMENT1) return "GL_COLOR_ATTACHMENT1";
    if (value == GL_COLOR_ATTACHMENT2) return "GL_COLOR_ATTACHMENT2";
    if (value == GL_COLOR_ATTACHMENT3) return "GL_COLOR_ATTACHMENT3";
    if (value == GL_COLOR_ATTACHMENT4) return "GL_COLOR_ATTACHMENT4";
    if (value == GL_COLOR_ATTACHMENT5) return "GL_COLOR_ATTACHMENT5";
    if (value == GL_COLOR_ATTACHMENT6) return "GL_COLOR_ATTACHMENT6";
    if (value == GL_COLOR_ATTACHMENT7) return "GL_COLOR_ATTACHMENT7";
    if (value == GL_COLOR_ATTACHMENT8) return "GL_COLOR_ATTACHMENT8";
    if (value == GL_COLOR_ATTACHMENT9) return "GL_COLOR_ATTACHMENT9";
    if (value == GL_COLOR_ATTACHMENT10) return "GL_COLOR_ATTACHMENT10";
    if (value == GL_COLOR_ATTACHMENT11) return "GL_COLOR_ATTACHMENT11";
    if (value == GL_COLOR_ATTACHMENT12) return "GL_COLOR_ATTACHMENT12";
    if (value == GL_COLOR_ATTACHMENT13) return "GL_COLOR_ATTACHMENT13";
    if (value == GL_COLOR_ATTACHMENT14) return "GL_COLOR_ATTACHMENT14";
    if (value == GL_COLOR_ATTACHMENT15) return "GL_COLOR_ATTACHMENT15";
    if (value == GL_COLOR_ATTACHMENT16) return "GL_COLOR_ATTACHMENT16";
    if (value == GL_COLOR_ATTACHMENT17) return "GL_COLOR_ATTACHMENT17";
    if (value == GL_COLOR_ATTACHMENT18) return "GL_COLOR_ATTACHMENT18";
    if (value == GL_COLOR_ATTACHMENT19) return "GL_COLOR_ATTACHMENT19";
    if (value == GL_COLOR_ATTACHMENT20) return "GL_COLOR_ATTACHMENT20";
    if (value == GL_COLOR_ATTACHMENT21) return "GL_COLOR_ATTACHMENT21";
    if (value == GL_COLOR_ATTACHMENT22) return "GL_COLOR_ATTACHMENT22";
    if (value == GL_COLOR_ATTACHMENT23) return "GL_COLOR_ATTACHMENT23";
    if (value == GL_COLOR_ATTACHMENT24) return "GL_COLOR_ATTACHMENT24";
    if (value == GL_COLOR_ATTACHMENT25) return "GL_COLOR_ATTACHMENT25";
    if (value == GL_COLOR_ATTACHMENT26) return "GL_COLOR_ATTACHMENT26";
    if (value == GL_COLOR_ATTACHMENT27) return "GL_COLOR_ATTACHMENT27";
    if (value == GL_COLOR_ATTACHMENT28) return "GL_COLOR_ATTACHMENT28";
    if (value == GL_COLOR_ATTACHMENT29) return "GL_COLOR_ATTACHMENT29";
    if (value == GL_COLOR_ATTACHMENT30) return "GL_COLOR_ATTACHMENT30";
    if (value == GL_COLOR_ATTACHMENT31) return "GL_COLOR_ATTACHMENT31";
    if (value == GL_DEPTH_ATTACHMENT) return "GL_DEPTH_ATTACHMENT";
    if (value == GL_STENCIL_ATTACHMENT) return "GL_STENCIL_ATTACHMENT";
    if (value == GL_FRAMEBUFFER) return "GL_FRAMEBUFFER";
    if (value == GL_RENDERBUFFER) return "GL_RENDERBUFFER";
    if (value == GL_RENDERBUFFER_WIDTH) return "GL_RENDERBUFFER_WIDTH";
    if (value == GL_RENDERBUFFER_HEIGHT) return "GL_RENDERBUFFER_HEIGHT";
    if (value == GL_RENDERBUFFER_INTERNAL_FORMAT) return "GL_RENDERBUFFER_INTERNAL_FORMAT";
    if (value == GL_STENCIL_INDEX1) return "GL_STENCIL_INDEX1";
    if (value == GL_STENCIL_INDEX4) return "GL_STENCIL_INDEX4";
    if (value == GL_STENCIL_INDEX8) return "GL_STENCIL_INDEX8";
    if (value == GL_STENCIL_INDEX16) return "GL_STENCIL_INDEX16";
    if (value == GL_RENDERBUFFER_RED_SIZE) return "GL_RENDERBUFFER_RED_SIZE";
    if (value == GL_RENDERBUFFER_GREEN_SIZE) return "GL_RENDERBUFFER_GREEN_SIZE";
    if (value == GL_RENDERBUFFER_BLUE_SIZE) return "GL_RENDERBUFFER_BLUE_SIZE";
    if (value == GL_RENDERBUFFER_ALPHA_SIZE) return "GL_RENDERBUFFER_ALPHA_SIZE";
    if (value == GL_RENDERBUFFER_DEPTH_SIZE) return "GL_RENDERBUFFER_DEPTH_SIZE";
    if (value == GL_RENDERBUFFER_STENCIL_SIZE) return "GL_RENDERBUFFER_STENCIL_SIZE";
    if (value == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    if (value == GL_MAX_SAMPLES) return "GL_MAX_SAMPLES";
    if (value == GL_FRAMEBUFFER_SRGB) return "GL_FRAMEBUFFER_SRGB";
    if (value == GL_HALF_FLOAT) return "GL_HALF_FLOAT";
    if (value == GL_MAP_READ_BIT) return "GL_MAP_READ_BIT";
    if (value == GL_MAP_WRITE_BIT) return "GL_MAP_WRITE_BIT";
    if (value == GL_MAP_INVALIDATE_RANGE_BIT) return "GL_MAP_INVALIDATE_RANGE_BIT";
    if (value == GL_MAP_INVALIDATE_BUFFER_BIT) return "GL_MAP_INVALIDATE_BUFFER_BIT";
    if (value == GL_MAP_FLUSH_EXPLICIT_BIT) return "GL_MAP_FLUSH_EXPLICIT_BIT";
    if (value == GL_MAP_UNSYNCHRONIZED_BIT) return "GL_MAP_UNSYNCHRONIZED_BIT";
    if (value == GL_COMPRESSED_RED_RGTC1) return "GL_COMPRESSED_RED_RGTC1";
    if (value == GL_COMPRESSED_SIGNED_RED_RGTC1) return "GL_COMPRESSED_SIGNED_RED_RGTC1";
    if (value == GL_COMPRESSED_RG_RGTC2) return "GL_COMPRESSED_RG_RGTC2";
    if (value == GL_COMPRESSED_SIGNED_RG_RGTC2) return "GL_COMPRESSED_SIGNED_RG_RGTC2";
    if (value == GL_RG) return "GL_RG";
    if (value == GL_RG_INTEGER) return "GL_RG_INTEGER";
    if (value == GL_R8) return "GL_R8";
    if (value == GL_R16) return "GL_R16";
    if (value == GL_RG8) return "GL_RG8";
    if (value == GL_RG16) return "GL_RG16";
    if (value == GL_R16F) return "GL_R16F";
    if (value == GL_R32F) return "GL_R32F";
    if (value == GL_RG16F) return "GL_RG16F";
    if (value == GL_RG32F) return "GL_RG32F";
    if (value == GL_R8I) return "GL_R8I";
    if (value == GL_R8UI) return "GL_R8UI";
    if (value == GL_R16I) return "GL_R16I";
    if (value == GL_R16UI) return "GL_R16UI";
    if (value == GL_R32I) return "GL_R32I";
    if (value == GL_R32UI) return "GL_R32UI";
    if (value == GL_RG8I) return "GL_RG8I";
    if (value == GL_RG8UI) return "GL_RG8UI";
    if (value == GL_RG16I) return "GL_RG16I";
    if (value == GL_RG16UI) return "GL_RG16UI";
    if (value == GL_RG32I) return "GL_RG32I";
    if (value == GL_RG32UI) return "GL_RG32UI";
    if (value == GL_VERTEX_ARRAY_BINDING) return "GL_VERTEX_ARRAY_BINDING";
    if (value == GL_SAMPLER_2D_RECT) return "GL_SAMPLER_2D_RECT";
    if (value == GL_SAMPLER_2D_RECT_SHADOW) return "GL_SAMPLER_2D_RECT_SHADOW";
    if (value == GL_SAMPLER_BUFFER) return "GL_SAMPLER_BUFFER";
    if (value == GL_INT_SAMPLER_2D_RECT) return "GL_INT_SAMPLER_2D_RECT";
    if (value == GL_INT_SAMPLER_BUFFER) return "GL_INT_SAMPLER_BUFFER";
    if (value == GL_UNSIGNED_INT_SAMPLER_2D_RECT) return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
    if (value == GL_UNSIGNED_INT_SAMPLER_BUFFER) return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
    if (value == GL_TEXTURE_BUFFER) return "GL_TEXTURE_BUFFER";
    if (value == GL_MAX_TEXTURE_BUFFER_SIZE) return "GL_MAX_TEXTURE_BUFFER_SIZE";
    if (value == GL_TEXTURE_BINDING_BUFFER) return "GL_TEXTURE_BINDING_BUFFER";
    if (value == GL_TEXTURE_BUFFER_DATA_STORE_BINDING) return "GL_TEXTURE_BUFFER_DATA_STORE_BINDING";
    if (value == GL_TEXTURE_RECTANGLE) return "GL_TEXTURE_RECTANGLE";
    if (value == GL_TEXTURE_BINDING_RECTANGLE) return "GL_TEXTURE_BINDING_RECTANGLE";
    if (value == GL_PROXY_TEXTURE_RECTANGLE) return "GL_PROXY_TEXTURE_RECTANGLE";
    if (value == GL_MAX_RECTANGLE_TEXTURE_SIZE) return "GL_MAX_RECTANGLE_TEXTURE_SIZE";
    if (value == GL_R8_SNORM) return "GL_R8_SNORM";
    if (value == GL_RG8_SNORM) return "GL_RG8_SNORM";
    if (value == GL_RGB8_SNORM) return "GL_RGB8_SNORM";
    if (value == GL_RGBA8_SNORM) return "GL_RGBA8_SNORM";
    if (value == GL_R16_SNORM) return "GL_R16_SNORM";
    if (value == GL_RG16_SNORM) return "GL_RG16_SNORM";
    if (value == GL_RGB16_SNORM) return "GL_RGB16_SNORM";
    if (value == GL_RGBA16_SNORM) return "GL_RGBA16_SNORM";
    if (value == GL_SIGNED_NORMALIZED) return "GL_SIGNED_NORMALIZED";
    if (value == GL_PRIMITIVE_RESTART) return "GL_PRIMITIVE_RESTART";
    if (value == GL_PRIMITIVE_RESTART_INDEX) return "GL_PRIMITIVE_RESTART_INDEX";
    if (value == GL_COPY_READ_BUFFER) return "GL_COPY_READ_BUFFER";
    if (value == GL_COPY_WRITE_BUFFER) return "GL_COPY_WRITE_BUFFER";
    if (value == GL_UNIFORM_BUFFER) return "GL_UNIFORM_BUFFER";
    if (value == GL_UNIFORM_BUFFER_BINDING) return "GL_UNIFORM_BUFFER_BINDING";
    if (value == GL_UNIFORM_BUFFER_START) return "GL_UNIFORM_BUFFER_START";
    if (value == GL_UNIFORM_BUFFER_SIZE) return "GL_UNIFORM_BUFFER_SIZE";
    if (value == GL_MAX_VERTEX_UNIFORM_BLOCKS) return "GL_MAX_VERTEX_UNIFORM_BLOCKS";
    if (value == GL_MAX_GEOMETRY_UNIFORM_BLOCKS) return "GL_MAX_GEOMETRY_UNIFORM_BLOCKS";
    if (value == GL_MAX_FRAGMENT_UNIFORM_BLOCKS) return "GL_MAX_FRAGMENT_UNIFORM_BLOCKS";
    if (value == GL_MAX_COMBINED_UNIFORM_BLOCKS) return "GL_MAX_COMBINED_UNIFORM_BLOCKS";
    if (value == GL_MAX_UNIFORM_BUFFER_BINDINGS) return "GL_MAX_UNIFORM_BUFFER_BINDINGS";
    if (value == GL_MAX_UNIFORM_BLOCK_SIZE) return "GL_MAX_UNIFORM_BLOCK_SIZE";
    if (value == GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS) return "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS";
    if (value == GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS) return "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS";
    if (value == GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS) return "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS";
    if (value == GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT) return "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT";
    if (value == GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH) return "GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH";
    if (value == GL_ACTIVE_UNIFORM_BLOCKS) return "GL_ACTIVE_UNIFORM_BLOCKS";
    if (value == GL_UNIFORM_TYPE) return "GL_UNIFORM_TYPE";
    if (value == GL_UNIFORM_SIZE) return "GL_UNIFORM_SIZE";
    if (value == GL_UNIFORM_NAME_LENGTH) return "GL_UNIFORM_NAME_LENGTH";
    if (value == GL_UNIFORM_BLOCK_INDEX) return "GL_UNIFORM_BLOCK_INDEX";
    if (value == GL_UNIFORM_OFFSET) return "GL_UNIFORM_OFFSET";
    if (value == GL_UNIFORM_ARRAY_STRIDE) return "GL_UNIFORM_ARRAY_STRIDE";
    if (value == GL_UNIFORM_MATRIX_STRIDE) return "GL_UNIFORM_MATRIX_STRIDE";
    if (value == GL_UNIFORM_IS_ROW_MAJOR) return "GL_UNIFORM_IS_ROW_MAJOR";
    if (value == GL_UNIFORM_BLOCK_BINDING) return "GL_UNIFORM_BLOCK_BINDING";
    if (value == GL_UNIFORM_BLOCK_DATA_SIZE) return "GL_UNIFORM_BLOCK_DATA_SIZE";
    if (value == GL_UNIFORM_BLOCK_NAME_LENGTH) return "GL_UNIFORM_BLOCK_NAME_LENGTH";
    if (value == GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS) return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS";
    if (value == GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES) return "GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES";
    if (value == GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER) return "GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER";
    if (value == GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER)
        return "GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER";
    if (value == GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER)
        return "GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER";
    if (value == GL_INVALID_INDEX) return "GL_INVALID_INDEX";
    if (value == GL_CONTEXT_CORE_PROFILE_BIT) return "GL_CONTEXT_CORE_PROFILE_BIT";
    if (value == GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) return "GL_CONTEXT_COMPATIBILITY_PROFILE_BIT";
    if (value == GL_LINES_ADJACENCY) return "GL_LINES_ADJACENCY";
    if (value == GL_LINE_STRIP_ADJACENCY) return "GL_LINE_STRIP_ADJACENCY";
    if (value == GL_TRIANGLES_ADJACENCY) return "GL_TRIANGLES_ADJACENCY";
    if (value == GL_TRIANGLE_STRIP_ADJACENCY) return "GL_TRIANGLE_STRIP_ADJACENCY";
    if (value == GL_PROGRAM_POINT_SIZE) return "GL_PROGRAM_POINT_SIZE";
    if (value == GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS) return "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS";
    if (value == GL_FRAMEBUFFER_ATTACHMENT_LAYERED) return "GL_FRAMEBUFFER_ATTACHMENT_LAYERED";
    if (value == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
    if (value == GL_GEOMETRY_SHADER) return "GL_GEOMETRY_SHADER";
    if (value == GL_GEOMETRY_VERTICES_OUT) return "GL_GEOMETRY_VERTICES_OUT";
    if (value == GL_GEOMETRY_INPUT_TYPE) return "GL_GEOMETRY_INPUT_TYPE";
    if (value == GL_GEOMETRY_OUTPUT_TYPE) return "GL_GEOMETRY_OUTPUT_TYPE";
    if (value == GL_MAX_GEOMETRY_UNIFORM_COMPONENTS) return "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS";
    if (value == GL_MAX_GEOMETRY_OUTPUT_VERTICES) return "GL_MAX_GEOMETRY_OUTPUT_VERTICES";
    if (value == GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS) return "GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS";
    if (value == GL_MAX_VERTEX_OUTPUT_COMPONENTS) return "GL_MAX_VERTEX_OUTPUT_COMPONENTS";
    if (value == GL_MAX_GEOMETRY_INPUT_COMPONENTS) return "GL_MAX_GEOMETRY_INPUT_COMPONENTS";
    if (value == GL_MAX_GEOMETRY_OUTPUT_COMPONENTS) return "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS";
    if (value == GL_MAX_FRAGMENT_INPUT_COMPONENTS) return "GL_MAX_FRAGMENT_INPUT_COMPONENTS";
    if (value == GL_CONTEXT_PROFILE_MASK) return "GL_CONTEXT_PROFILE_MASK";
    if (value == GL_DEPTH_CLAMP) return "GL_DEPTH_CLAMP";
    if (value == GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION) return "GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION";
    if (value == GL_FIRST_VERTEX_CONVENTION) return "GL_FIRST_VERTEX_CONVENTION";
    if (value == GL_LAST_VERTEX_CONVENTION) return "GL_LAST_VERTEX_CONVENTION";
    if (value == GL_PROVOKING_VERTEX) return "GL_PROVOKING_VERTEX";
    if (value == GL_TEXTURE_CUBE_MAP_SEAMLESS) return "GL_TEXTURE_CUBE_MAP_SEAMLESS";
    if (value == GL_MAX_SERVER_WAIT_TIMEOUT) return "GL_MAX_SERVER_WAIT_TIMEOUT";
    if (value == GL_OBJECT_TYPE) return "GL_OBJECT_TYPE";
    if (value == GL_SYNC_CONDITION) return "GL_SYNC_CONDITION";
    if (value == GL_SYNC_STATUS) return "GL_SYNC_STATUS";
    if (value == GL_SYNC_FLAGS) return "GL_SYNC_FLAGS";
    if (value == GL_SYNC_FENCE) return "GL_SYNC_FENCE";
    if (value == GL_SYNC_GPU_COMMANDS_COMPLETE) return "GL_SYNC_GPU_COMMANDS_COMPLETE";
    if (value == GL_UNSIGNALED) return "GL_UNSIGNALED";
    if (value == GL_SIGNALED) return "GL_SIGNALED";
    if (value == GL_ALREADY_SIGNALED) return "GL_ALREADY_SIGNALED";
    if (value == GL_TIMEOUT_EXPIRED) return "GL_TIMEOUT_EXPIRED";
    if (value == GL_CONDITION_SATISFIED) return "GL_CONDITION_SATISFIED";
    if (value == GL_WAIT_FAILED) return "GL_WAIT_FAILED";
    if (value == GL_TIMEOUT_IGNORED) return "GL_TIMEOUT_IGNORED";
    if (value == GL_SYNC_FLUSH_COMMANDS_BIT) return "GL_SYNC_FLUSH_COMMANDS_BIT";
    if (value == GL_SAMPLE_POSITION) return "GL_SAMPLE_POSITION";
    if (value == GL_SAMPLE_MASK) return "GL_SAMPLE_MASK";
    if (value == GL_SAMPLE_MASK_VALUE) return "GL_SAMPLE_MASK_VALUE";
    if (value == GL_MAX_SAMPLE_MASK_WORDS) return "GL_MAX_SAMPLE_MASK_WORDS";
    if (value == GL_TEXTURE_2D_MULTISAMPLE) return "GL_TEXTURE_2D_MULTISAMPLE";
    if (value == GL_PROXY_TEXTURE_2D_MULTISAMPLE) return "GL_PROXY_TEXTURE_2D_MULTISAMPLE";
    if (value == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) return "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";
    if (value == GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY) return "GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY";
    if (value == GL_TEXTURE_BINDING_2D_MULTISAMPLE) return "GL_TEXTURE_BINDING_2D_MULTISAMPLE";
    if (value == GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY) return "GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY";
    if (value == GL_TEXTURE_SAMPLES) return "GL_TEXTURE_SAMPLES";
    if (value == GL_TEXTURE_FIXED_SAMPLE_LOCATIONS) return "GL_TEXTURE_FIXED_SAMPLE_LOCATIONS";
    if (value == GL_SAMPLER_2D_MULTISAMPLE) return "GL_SAMPLER_2D_MULTISAMPLE";
    if (value == GL_INT_SAMPLER_2D_MULTISAMPLE) return "GL_INT_SAMPLER_2D_MULTISAMPLE";
    if (value == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE) return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
    if (value == GL_SAMPLER_2D_MULTISAMPLE_ARRAY) return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
    if (value == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
    if (value == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
    if (value == GL_MAX_COLOR_TEXTURE_SAMPLES) return "GL_MAX_COLOR_TEXTURE_SAMPLES";
    if (value == GL_MAX_DEPTH_TEXTURE_SAMPLES) return "GL_MAX_DEPTH_TEXTURE_SAMPLES";
    if (value == GL_MAX_INTEGER_SAMPLES) return "GL_MAX_INTEGER_SAMPLES";
    if (value == GL_VERTEX_ATTRIB_ARRAY_DIVISOR) return "GL_VERTEX_ATTRIB_ARRAY_DIVISOR";
    if (value == GL_SRC1_COLOR) return "GL_SRC1_COLOR";
    if (value == GL_ONE_MINUS_SRC1_COLOR) return "GL_ONE_MINUS_SRC1_COLOR";
    if (value == GL_ONE_MINUS_SRC1_ALPHA) return "GL_ONE_MINUS_SRC1_ALPHA";
    if (value == GL_MAX_DUAL_SOURCE_DRAW_BUFFERS) return "GL_MAX_DUAL_SOURCE_DRAW_BUFFERS";
    if (value == GL_ANY_SAMPLES_PASSED) return "GL_ANY_SAMPLES_PASSED";
    if (value == GL_SAMPLER_BINDING) return "GL_SAMPLER_BINDING";
    if (value == GL_RGB10_A2UI) return "GL_RGB10_A2UI";
    if (value == GL_TEXTURE_SWIZZLE_R) return "GL_TEXTURE_SWIZZLE_R";
    if (value == GL_TEXTURE_SWIZZLE_G) return "GL_TEXTURE_SWIZZLE_G";
    if (value == GL_TEXTURE_SWIZZLE_B) return "GL_TEXTURE_SWIZZLE_B";
    if (value == GL_TEXTURE_SWIZZLE_A) return "GL_TEXTURE_SWIZZLE_A";
    if (value == GL_TEXTURE_SWIZZLE_RGBA) return "GL_TEXTURE_SWIZZLE_RGBA";
    if (value == GL_TIME_ELAPSED) return "GL_TIME_ELAPSED";
    if (value == GL_TIMESTAMP) return "GL_TIMESTAMP";
    if (value == GL_INT_2_10_10_10_REV) return "GL_INT_2_10_10_10_REV";
    if (value == GL_SAMPLE_SHADING) return "GL_SAMPLE_SHADING";
    if (value == GL_MIN_SAMPLE_SHADING_VALUE) return "GL_MIN_SAMPLE_SHADING_VALUE";
    if (value == GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET) return "GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET";
    if (value == GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET) return "GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET";
    if (value == GL_TEXTURE_CUBE_MAP_ARRAY) return "GL_TEXTURE_CUBE_MAP_ARRAY";
    if (value == GL_TEXTURE_BINDING_CUBE_MAP_ARRAY) return "GL_TEXTURE_BINDING_CUBE_MAP_ARRAY";
    if (value == GL_PROXY_TEXTURE_CUBE_MAP_ARRAY) return "GL_PROXY_TEXTURE_CUBE_MAP_ARRAY";
    if (value == GL_SAMPLER_CUBE_MAP_ARRAY) return "GL_SAMPLER_CUBE_MAP_ARRAY";
    if (value == GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW) return "GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW";
    if (value == GL_INT_SAMPLER_CUBE_MAP_ARRAY) return "GL_INT_SAMPLER_CUBE_MAP_ARRAY";
    if (value == GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY) return "GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY";
    if (value == GL_DRAW_INDIRECT_BUFFER) return "GL_DRAW_INDIRECT_BUFFER";
    if (value == GL_DRAW_INDIRECT_BUFFER_BINDING) return "GL_DRAW_INDIRECT_BUFFER_BINDING";
    if (value == GL_GEOMETRY_SHADER_INVOCATIONS) return "GL_GEOMETRY_SHADER_INVOCATIONS";
    if (value == GL_MAX_GEOMETRY_SHADER_INVOCATIONS) return "GL_MAX_GEOMETRY_SHADER_INVOCATIONS";
    if (value == GL_MIN_FRAGMENT_INTERPOLATION_OFFSET) return "GL_MIN_FRAGMENT_INTERPOLATION_OFFSET";
    if (value == GL_MAX_FRAGMENT_INTERPOLATION_OFFSET) return "GL_MAX_FRAGMENT_INTERPOLATION_OFFSET";
    if (value == GL_FRAGMENT_INTERPOLATION_OFFSET_BITS) return "GL_FRAGMENT_INTERPOLATION_OFFSET_BITS";
    if (value == GL_MAX_VERTEX_STREAMS) return "GL_MAX_VERTEX_STREAMS";
    if (value == GL_DOUBLE_VEC2) return "GL_DOUBLE_VEC2";
    if (value == GL_DOUBLE_VEC3) return "GL_DOUBLE_VEC3";
    if (value == GL_DOUBLE_VEC4) return "GL_DOUBLE_VEC4";
    if (value == GL_DOUBLE_MAT2) return "GL_DOUBLE_MAT2";
    if (value == GL_DOUBLE_MAT3) return "GL_DOUBLE_MAT3";
    if (value == GL_DOUBLE_MAT4) return "GL_DOUBLE_MAT4";
    if (value == GL_DOUBLE_MAT2x3) return "GL_DOUBLE_MAT2x3";
    if (value == GL_DOUBLE_MAT2x4) return "GL_DOUBLE_MAT2x4";
    if (value == GL_DOUBLE_MAT3x2) return "GL_DOUBLE_MAT3x2";
    if (value == GL_DOUBLE_MAT3x4) return "GL_DOUBLE_MAT3x4";
    if (value == GL_DOUBLE_MAT4x2) return "GL_DOUBLE_MAT4x2";
    if (value == GL_DOUBLE_MAT4x3) return "GL_DOUBLE_MAT4x3";
    if (value == GL_ACTIVE_SUBROUTINES) return "GL_ACTIVE_SUBROUTINES";
    if (value == GL_ACTIVE_SUBROUTINE_UNIFORMS) return "GL_ACTIVE_SUBROUTINE_UNIFORMS";
    if (value == GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS) return "GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS";
    if (value == GL_ACTIVE_SUBROUTINE_MAX_LENGTH) return "GL_ACTIVE_SUBROUTINE_MAX_LENGTH";
    if (value == GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH) return "GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH";
    if (value == GL_MAX_SUBROUTINES) return "GL_MAX_SUBROUTINES";
    if (value == GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS) return "GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS";
    if (value == GL_NUM_COMPATIBLE_SUBROUTINES) return "GL_NUM_COMPATIBLE_SUBROUTINES";
    if (value == GL_COMPATIBLE_SUBROUTINES) return "GL_COMPATIBLE_SUBROUTINES";
    if (value == GL_PATCHES) return "GL_PATCHES";
    if (value == GL_PATCH_VERTICES) return "GL_PATCH_VERTICES";
    if (value == GL_PATCH_DEFAULT_INNER_LEVEL) return "GL_PATCH_DEFAULT_INNER_LEVEL";
    if (value == GL_PATCH_DEFAULT_OUTER_LEVEL) return "GL_PATCH_DEFAULT_OUTER_LEVEL";
    if (value == GL_TESS_CONTROL_OUTPUT_VERTICES) return "GL_TESS_CONTROL_OUTPUT_VERTICES";
    if (value == GL_TESS_GEN_MODE) return "GL_TESS_GEN_MODE";
    if (value == GL_TESS_GEN_SPACING) return "GL_TESS_GEN_SPACING";
    if (value == GL_TESS_GEN_VERTEX_ORDER) return "GL_TESS_GEN_VERTEX_ORDER";
    if (value == GL_TESS_GEN_POINT_MODE) return "GL_TESS_GEN_POINT_MODE";
    if (value == GL_ISOLINES) return "GL_ISOLINES";
    if (value == GL_QUADS) return "GL_QUADS";
    if (value == GL_FRACTIONAL_ODD) return "GL_FRACTIONAL_ODD";
    if (value == GL_FRACTIONAL_EVEN) return "GL_FRACTIONAL_EVEN";
    if (value == GL_MAX_PATCH_VERTICES) return "GL_MAX_PATCH_VERTICES";
    if (value == GL_MAX_TESS_GEN_LEVEL) return "GL_MAX_TESS_GEN_LEVEL";
    if (value == GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS) return "GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS";
    if (value == GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS) return "GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS";
    if (value == GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS) return "GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS";
    if (value == GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS) return "GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS";
    if (value == GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS) return "GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS";
    if (value == GL_MAX_TESS_PATCH_COMPONENTS) return "GL_MAX_TESS_PATCH_COMPONENTS";
    if (value == GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS) return "GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS";
    if (value == GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS) return "GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS";
    if (value == GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS) return "GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS";
    if (value == GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS) return "GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS";
    if (value == GL_MAX_TESS_CONTROL_INPUT_COMPONENTS) return "GL_MAX_TESS_CONTROL_INPUT_COMPONENTS";
    if (value == GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS) return "GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS";
    if (value == GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS)
        return "GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS";
    if (value == GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS)
        return "GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS";
    if (value == GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER)
        return "GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER";
    if (value == GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER)
        return "GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER";
    if (value == GL_TESS_EVALUATION_SHADER) return "GL_TESS_EVALUATION_SHADER";
    if (value == GL_TESS_CONTROL_SHADER) return "GL_TESS_CONTROL_SHADER";
    if (value == GL_TRANSFORM_FEEDBACK) return "GL_TRANSFORM_FEEDBACK";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED) return "GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE) return "GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE";
    if (value == GL_TRANSFORM_FEEDBACK_BINDING) return "GL_TRANSFORM_FEEDBACK_BINDING";
    if (value == GL_MAX_TRANSFORM_FEEDBACK_BUFFERS) return "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS";
    if (value == GL_FIXED) return "GL_FIXED";
    if (value == GL_IMPLEMENTATION_COLOR_READ_TYPE) return "GL_IMPLEMENTATION_COLOR_READ_TYPE";
    if (value == GL_IMPLEMENTATION_COLOR_READ_FORMAT) return "GL_IMPLEMENTATION_COLOR_READ_FORMAT";
    if (value == GL_LOW_FLOAT) return "GL_LOW_FLOAT";
    if (value == GL_MEDIUM_FLOAT) return "GL_MEDIUM_FLOAT";
    if (value == GL_HIGH_FLOAT) return "GL_HIGH_FLOAT";
    if (value == GL_LOW_INT) return "GL_LOW_INT";
    if (value == GL_MEDIUM_INT) return "GL_MEDIUM_INT";
    if (value == GL_HIGH_INT) return "GL_HIGH_INT";
    if (value == GL_SHADER_COMPILER) return "GL_SHADER_COMPILER";
    if (value == GL_SHADER_BINARY_FORMATS) return "GL_SHADER_BINARY_FORMATS";
    if (value == GL_NUM_SHADER_BINARY_FORMATS) return "GL_NUM_SHADER_BINARY_FORMATS";
    if (value == GL_MAX_VERTEX_UNIFORM_VECTORS) return "GL_MAX_VERTEX_UNIFORM_VECTORS";
    if (value == GL_MAX_VARYING_VECTORS) return "GL_MAX_VARYING_VECTORS";
    if (value == GL_MAX_FRAGMENT_UNIFORM_VECTORS) return "GL_MAX_FRAGMENT_UNIFORM_VECTORS";
    if (value == GL_RGB565) return "GL_RGB565";
    if (value == GL_PROGRAM_BINARY_RETRIEVABLE_HINT) return "GL_PROGRAM_BINARY_RETRIEVABLE_HINT";
    if (value == GL_PROGRAM_BINARY_LENGTH) return "GL_PROGRAM_BINARY_LENGTH";
    if (value == GL_NUM_PROGRAM_BINARY_FORMATS) return "GL_NUM_PROGRAM_BINARY_FORMATS";
    if (value == GL_PROGRAM_BINARY_FORMATS) return "GL_PROGRAM_BINARY_FORMATS";
    if (value == GL_VERTEX_SHADER_BIT) return "GL_VERTEX_SHADER_BIT";
    if (value == GL_FRAGMENT_SHADER_BIT) return "GL_FRAGMENT_SHADER_BIT";
    if (value == GL_GEOMETRY_SHADER_BIT) return "GL_GEOMETRY_SHADER_BIT";
    if (value == GL_TESS_CONTROL_SHADER_BIT) return "GL_TESS_CONTROL_SHADER_BIT";
    if (value == GL_TESS_EVALUATION_SHADER_BIT) return "GL_TESS_EVALUATION_SHADER_BIT";
    if (value == GL_ALL_SHADER_BITS) return "GL_ALL_SHADER_BITS";
    if (value == GL_PROGRAM_SEPARABLE) return "GL_PROGRAM_SEPARABLE";
    if (value == GL_ACTIVE_PROGRAM) return "GL_ACTIVE_PROGRAM";
    if (value == GL_PROGRAM_PIPELINE_BINDING) return "GL_PROGRAM_PIPELINE_BINDING";
    if (value == GL_MAX_VIEWPORTS) return "GL_MAX_VIEWPORTS";
    if (value == GL_VIEWPORT_SUBPIXEL_BITS) return "GL_VIEWPORT_SUBPIXEL_BITS";
    if (value == GL_VIEWPORT_BOUNDS_RANGE) return "GL_VIEWPORT_BOUNDS_RANGE";
    if (value == GL_LAYER_PROVOKING_VERTEX) return "GL_LAYER_PROVOKING_VERTEX";
    if (value == GL_VIEWPORT_INDEX_PROVOKING_VERTEX) return "GL_VIEWPORT_INDEX_PROVOKING_VERTEX";
    if (value == GL_UNDEFINED_VERTEX) return "GL_UNDEFINED_VERTEX";
    if (value == GL_COPY_READ_BUFFER_BINDING) return "GL_COPY_READ_BUFFER_BINDING";
    if (value == GL_COPY_WRITE_BUFFER_BINDING) return "GL_COPY_WRITE_BUFFER_BINDING";
    if (value == GL_TRANSFORM_FEEDBACK_ACTIVE) return "GL_TRANSFORM_FEEDBACK_ACTIVE";
    if (value == GL_TRANSFORM_FEEDBACK_PAUSED) return "GL_TRANSFORM_FEEDBACK_PAUSED";
    if (value == GL_UNPACK_COMPRESSED_BLOCK_WIDTH) return "GL_UNPACK_COMPRESSED_BLOCK_WIDTH";
    if (value == GL_UNPACK_COMPRESSED_BLOCK_HEIGHT) return "GL_UNPACK_COMPRESSED_BLOCK_HEIGHT";
    if (value == GL_UNPACK_COMPRESSED_BLOCK_DEPTH) return "GL_UNPACK_COMPRESSED_BLOCK_DEPTH";
    if (value == GL_UNPACK_COMPRESSED_BLOCK_SIZE) return "GL_UNPACK_COMPRESSED_BLOCK_SIZE";
    if (value == GL_PACK_COMPRESSED_BLOCK_WIDTH) return "GL_PACK_COMPRESSED_BLOCK_WIDTH";
    if (value == GL_PACK_COMPRESSED_BLOCK_HEIGHT) return "GL_PACK_COMPRESSED_BLOCK_HEIGHT";
    if (value == GL_PACK_COMPRESSED_BLOCK_DEPTH) return "GL_PACK_COMPRESSED_BLOCK_DEPTH";
    if (value == GL_PACK_COMPRESSED_BLOCK_SIZE) return "GL_PACK_COMPRESSED_BLOCK_SIZE";
    if (value == GL_NUM_SAMPLE_COUNTS) return "GL_NUM_SAMPLE_COUNTS";
    if (value == GL_MIN_MAP_BUFFER_ALIGNMENT) return "GL_MIN_MAP_BUFFER_ALIGNMENT";
    if (value == GL_ATOMIC_COUNTER_BUFFER) return "GL_ATOMIC_COUNTER_BUFFER";
    if (value == GL_ATOMIC_COUNTER_BUFFER_BINDING) return "GL_ATOMIC_COUNTER_BUFFER_BINDING";
    if (value == GL_ATOMIC_COUNTER_BUFFER_START) return "GL_ATOMIC_COUNTER_BUFFER_START";
    if (value == GL_ATOMIC_COUNTER_BUFFER_SIZE) return "GL_ATOMIC_COUNTER_BUFFER_SIZE";
    if (value == GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE) return "GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE";
    if (value == GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS)
        return "GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS";
    if (value == GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES)
        return "GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES";
    if (value == GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER)
        return "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER";
    if (value == GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER)
        return "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER";
    if (value == GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER)
        return "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER";
    if (value == GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER)
        return "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER";
    if (value == GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER)
        return "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER";
    if (value == GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS) return "GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS) return "GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS) return "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS) return "GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS) return "GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS) return "GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_MAX_VERTEX_ATOMIC_COUNTERS) return "GL_MAX_VERTEX_ATOMIC_COUNTERS";
    if (value == GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS) return "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS";
    if (value == GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS) return "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS";
    if (value == GL_MAX_GEOMETRY_ATOMIC_COUNTERS) return "GL_MAX_GEOMETRY_ATOMIC_COUNTERS";
    if (value == GL_MAX_FRAGMENT_ATOMIC_COUNTERS) return "GL_MAX_FRAGMENT_ATOMIC_COUNTERS";
    if (value == GL_MAX_COMBINED_ATOMIC_COUNTERS) return "GL_MAX_COMBINED_ATOMIC_COUNTERS";
    if (value == GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE) return "GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE";
    if (value == GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS) return "GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS";
    if (value == GL_ACTIVE_ATOMIC_COUNTER_BUFFERS) return "GL_ACTIVE_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX) return "GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX";
    if (value == GL_UNSIGNED_INT_ATOMIC_COUNTER) return "GL_UNSIGNED_INT_ATOMIC_COUNTER";
    if (value == GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT) return "GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT";
    if (value == GL_ELEMENT_ARRAY_BARRIER_BIT) return "GL_ELEMENT_ARRAY_BARRIER_BIT";
    if (value == GL_UNIFORM_BARRIER_BIT) return "GL_UNIFORM_BARRIER_BIT";
    if (value == GL_TEXTURE_FETCH_BARRIER_BIT) return "GL_TEXTURE_FETCH_BARRIER_BIT";
    if (value == GL_SHADER_IMAGE_ACCESS_BARRIER_BIT) return "GL_SHADER_IMAGE_ACCESS_BARRIER_BIT";
    if (value == GL_COMMAND_BARRIER_BIT) return "GL_COMMAND_BARRIER_BIT";
    if (value == GL_PIXEL_BUFFER_BARRIER_BIT) return "GL_PIXEL_BUFFER_BARRIER_BIT";
    if (value == GL_TEXTURE_UPDATE_BARRIER_BIT) return "GL_TEXTURE_UPDATE_BARRIER_BIT";
    if (value == GL_BUFFER_UPDATE_BARRIER_BIT) return "GL_BUFFER_UPDATE_BARRIER_BIT";
    if (value == GL_FRAMEBUFFER_BARRIER_BIT) return "GL_FRAMEBUFFER_BARRIER_BIT";
    if (value == GL_TRANSFORM_FEEDBACK_BARRIER_BIT) return "GL_TRANSFORM_FEEDBACK_BARRIER_BIT";
    if (value == GL_ATOMIC_COUNTER_BARRIER_BIT) return "GL_ATOMIC_COUNTER_BARRIER_BIT";
    if (value == GL_ALL_BARRIER_BITS) return "GL_ALL_BARRIER_BITS";
    if (value == GL_MAX_IMAGE_UNITS) return "GL_MAX_IMAGE_UNITS";
    if (value == GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS)
        return "GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS";
    if (value == GL_IMAGE_BINDING_NAME) return "GL_IMAGE_BINDING_NAME";
    if (value == GL_IMAGE_BINDING_LEVEL) return "GL_IMAGE_BINDING_LEVEL";
    if (value == GL_IMAGE_BINDING_LAYERED) return "GL_IMAGE_BINDING_LAYERED";
    if (value == GL_IMAGE_BINDING_LAYER) return "GL_IMAGE_BINDING_LAYER";
    if (value == GL_IMAGE_BINDING_ACCESS) return "GL_IMAGE_BINDING_ACCESS";
    if (value == GL_IMAGE_1D) return "GL_IMAGE_1D";
    if (value == GL_IMAGE_2D) return "GL_IMAGE_2D";
    if (value == GL_IMAGE_3D) return "GL_IMAGE_3D";
    if (value == GL_IMAGE_2D_RECT) return "GL_IMAGE_2D_RECT";
    if (value == GL_IMAGE_CUBE) return "GL_IMAGE_CUBE";
    if (value == GL_IMAGE_BUFFER) return "GL_IMAGE_BUFFER";
    if (value == GL_IMAGE_1D_ARRAY) return "GL_IMAGE_1D_ARRAY";
    if (value == GL_IMAGE_2D_ARRAY) return "GL_IMAGE_2D_ARRAY";
    if (value == GL_IMAGE_CUBE_MAP_ARRAY) return "GL_IMAGE_CUBE_MAP_ARRAY";
    if (value == GL_IMAGE_2D_MULTISAMPLE) return "GL_IMAGE_2D_MULTISAMPLE";
    if (value == GL_IMAGE_2D_MULTISAMPLE_ARRAY) return "GL_IMAGE_2D_MULTISAMPLE_ARRAY";
    if (value == GL_INT_IMAGE_1D) return "GL_INT_IMAGE_1D";
    if (value == GL_INT_IMAGE_2D) return "GL_INT_IMAGE_2D";
    if (value == GL_INT_IMAGE_3D) return "GL_INT_IMAGE_3D";
    if (value == GL_INT_IMAGE_2D_RECT) return "GL_INT_IMAGE_2D_RECT";
    if (value == GL_INT_IMAGE_CUBE) return "GL_INT_IMAGE_CUBE";
    if (value == GL_INT_IMAGE_BUFFER) return "GL_INT_IMAGE_BUFFER";
    if (value == GL_INT_IMAGE_1D_ARRAY) return "GL_INT_IMAGE_1D_ARRAY";
    if (value == GL_INT_IMAGE_2D_ARRAY) return "GL_INT_IMAGE_2D_ARRAY";
    if (value == GL_INT_IMAGE_CUBE_MAP_ARRAY) return "GL_INT_IMAGE_CUBE_MAP_ARRAY";
    if (value == GL_INT_IMAGE_2D_MULTISAMPLE) return "GL_INT_IMAGE_2D_MULTISAMPLE";
    if (value == GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY) return "GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
    if (value == GL_UNSIGNED_INT_IMAGE_1D) return "GL_UNSIGNED_INT_IMAGE_1D";
    if (value == GL_UNSIGNED_INT_IMAGE_2D) return "GL_UNSIGNED_INT_IMAGE_2D";
    if (value == GL_UNSIGNED_INT_IMAGE_3D) return "GL_UNSIGNED_INT_IMAGE_3D";
    if (value == GL_UNSIGNED_INT_IMAGE_2D_RECT) return "GL_UNSIGNED_INT_IMAGE_2D_RECT";
    if (value == GL_UNSIGNED_INT_IMAGE_CUBE) return "GL_UNSIGNED_INT_IMAGE_CUBE";
    if (value == GL_UNSIGNED_INT_IMAGE_BUFFER) return "GL_UNSIGNED_INT_IMAGE_BUFFER";
    if (value == GL_UNSIGNED_INT_IMAGE_1D_ARRAY) return "GL_UNSIGNED_INT_IMAGE_1D_ARRAY";
    if (value == GL_UNSIGNED_INT_IMAGE_2D_ARRAY) return "GL_UNSIGNED_INT_IMAGE_2D_ARRAY";
    if (value == GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY) return "GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY";
    if (value == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE) return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE";
    if (value == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY) return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
    if (value == GL_MAX_IMAGE_SAMPLES) return "GL_MAX_IMAGE_SAMPLES";
    if (value == GL_IMAGE_BINDING_FORMAT) return "GL_IMAGE_BINDING_FORMAT";
    if (value == GL_IMAGE_FORMAT_COMPATIBILITY_TYPE) return "GL_IMAGE_FORMAT_COMPATIBILITY_TYPE";
    if (value == GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE) return "GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE";
    if (value == GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS) return "GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS";
    if (value == GL_MAX_VERTEX_IMAGE_UNIFORMS) return "GL_MAX_VERTEX_IMAGE_UNIFORMS";
    if (value == GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS) return "GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS";
    if (value == GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS) return "GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS";
    if (value == GL_MAX_GEOMETRY_IMAGE_UNIFORMS) return "GL_MAX_GEOMETRY_IMAGE_UNIFORMS";
    if (value == GL_MAX_FRAGMENT_IMAGE_UNIFORMS) return "GL_MAX_FRAGMENT_IMAGE_UNIFORMS";
    if (value == GL_MAX_COMBINED_IMAGE_UNIFORMS) return "GL_MAX_COMBINED_IMAGE_UNIFORMS";
    if (value == GL_COMPRESSED_RGBA_BPTC_UNORM) return "GL_COMPRESSED_RGBA_BPTC_UNORM";
    if (value == GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM) return "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
    if (value == GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT) return "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
    if (value == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT) return "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
    if (value == GL_TEXTURE_IMMUTABLE_FORMAT) return "GL_TEXTURE_IMMUTABLE_FORMAT";
    if (value == GL_NUM_SHADING_LANGUAGE_VERSIONS) return "GL_NUM_SHADING_LANGUAGE_VERSIONS";
    if (value == GL_VERTEX_ATTRIB_ARRAY_LONG) return "GL_VERTEX_ATTRIB_ARRAY_LONG";
    if (value == GL_COMPRESSED_RGB8_ETC2) return "GL_COMPRESSED_RGB8_ETC2";
    if (value == GL_COMPRESSED_SRGB8_ETC2) return "GL_COMPRESSED_SRGB8_ETC2";
    if (value == GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2) return "GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
    if (value == GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2) return "GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
    if (value == GL_COMPRESSED_RGBA8_ETC2_EAC) return "GL_COMPRESSED_RGBA8_ETC2_EAC";
    if (value == GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC) return "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";
    if (value == GL_COMPRESSED_R11_EAC) return "GL_COMPRESSED_R11_EAC";
    if (value == GL_COMPRESSED_SIGNED_R11_EAC) return "GL_COMPRESSED_SIGNED_R11_EAC";
    if (value == GL_COMPRESSED_RG11_EAC) return "GL_COMPRESSED_RG11_EAC";
    if (value == GL_COMPRESSED_SIGNED_RG11_EAC) return "GL_COMPRESSED_SIGNED_RG11_EAC";
    if (value == GL_PRIMITIVE_RESTART_FIXED_INDEX) return "GL_PRIMITIVE_RESTART_FIXED_INDEX";
    if (value == GL_ANY_SAMPLES_PASSED_CONSERVATIVE) return "GL_ANY_SAMPLES_PASSED_CONSERVATIVE";
    if (value == GL_MAX_ELEMENT_INDEX) return "GL_MAX_ELEMENT_INDEX";
    if (value == GL_COMPUTE_SHADER) return "GL_COMPUTE_SHADER";
    if (value == GL_MAX_COMPUTE_UNIFORM_BLOCKS) return "GL_MAX_COMPUTE_UNIFORM_BLOCKS";
    if (value == GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS) return "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS";
    if (value == GL_MAX_COMPUTE_IMAGE_UNIFORMS) return "GL_MAX_COMPUTE_IMAGE_UNIFORMS";
    if (value == GL_MAX_COMPUTE_SHARED_MEMORY_SIZE) return "GL_MAX_COMPUTE_SHARED_MEMORY_SIZE";
    if (value == GL_MAX_COMPUTE_UNIFORM_COMPONENTS) return "GL_MAX_COMPUTE_UNIFORM_COMPONENTS";
    if (value == GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS) return "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS";
    if (value == GL_MAX_COMPUTE_ATOMIC_COUNTERS) return "GL_MAX_COMPUTE_ATOMIC_COUNTERS";
    if (value == GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS) return "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS";
    if (value == GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS) return "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS";
    if (value == GL_MAX_COMPUTE_WORK_GROUP_COUNT) return "GL_MAX_COMPUTE_WORK_GROUP_COUNT";
    if (value == GL_MAX_COMPUTE_WORK_GROUP_SIZE) return "GL_MAX_COMPUTE_WORK_GROUP_SIZE";
    if (value == GL_COMPUTE_WORK_GROUP_SIZE) return "GL_COMPUTE_WORK_GROUP_SIZE";
    if (value == GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER) return "GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER";
    if (value == GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER)
        return "GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER";
    if (value == GL_DISPATCH_INDIRECT_BUFFER) return "GL_DISPATCH_INDIRECT_BUFFER";
    if (value == GL_DISPATCH_INDIRECT_BUFFER_BINDING) return "GL_DISPATCH_INDIRECT_BUFFER_BINDING";
    if (value == GL_COMPUTE_SHADER_BIT) return "GL_COMPUTE_SHADER_BIT";
    if (value == GL_DEBUG_OUTPUT_SYNCHRONOUS) return "GL_DEBUG_OUTPUT_SYNCHRONOUS";
    if (value == GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH) return "GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH";
    if (value == GL_DEBUG_CALLBACK_FUNCTION) return "GL_DEBUG_CALLBACK_FUNCTION";
    if (value == GL_DEBUG_CALLBACK_USER_PARAM) return "GL_DEBUG_CALLBACK_USER_PARAM";
    if (value == GL_DEBUG_SOURCE_API) return "GL_DEBUG_SOURCE_API";
    if (value == GL_DEBUG_SOURCE_WINDOW_SYSTEM) return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
    if (value == GL_DEBUG_SOURCE_SHADER_COMPILER) return "GL_DEBUG_SOURCE_SHADER_COMPILER";
    if (value == GL_DEBUG_SOURCE_THIRD_PARTY) return "GL_DEBUG_SOURCE_THIRD_PARTY";
    if (value == GL_DEBUG_SOURCE_APPLICATION) return "GL_DEBUG_SOURCE_APPLICATION";
    if (value == GL_DEBUG_SOURCE_OTHER) return "GL_DEBUG_SOURCE_OTHER";
    if (value == GL_DEBUG_TYPE_ERROR) return "GL_DEBUG_TYPE_ERROR";
    if (value == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
    if (value == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
    if (value == GL_DEBUG_TYPE_PORTABILITY) return "GL_DEBUG_TYPE_PORTABILITY";
    if (value == GL_DEBUG_TYPE_PERFORMANCE) return "GL_DEBUG_TYPE_PERFORMANCE";
    if (value == GL_DEBUG_TYPE_OTHER) return "GL_DEBUG_TYPE_OTHER";
    if (value == GL_MAX_DEBUG_MESSAGE_LENGTH) return "GL_MAX_DEBUG_MESSAGE_LENGTH";
    if (value == GL_MAX_DEBUG_LOGGED_MESSAGES) return "GL_MAX_DEBUG_LOGGED_MESSAGES";
    if (value == GL_DEBUG_LOGGED_MESSAGES) return "GL_DEBUG_LOGGED_MESSAGES";
    if (value == GL_DEBUG_SEVERITY_HIGH) return "GL_DEBUG_SEVERITY_HIGH";
    if (value == GL_DEBUG_SEVERITY_MEDIUM) return "GL_DEBUG_SEVERITY_MEDIUM";
    if (value == GL_DEBUG_SEVERITY_LOW) return "GL_DEBUG_SEVERITY_LOW";
    if (value == GL_DEBUG_TYPE_MARKER) return "GL_DEBUG_TYPE_MARKER";
    if (value == GL_DEBUG_TYPE_PUSH_GROUP) return "GL_DEBUG_TYPE_PUSH_GROUP";
    if (value == GL_DEBUG_TYPE_POP_GROUP) return "GL_DEBUG_TYPE_POP_GROUP";
    if (value == GL_DEBUG_SEVERITY_NOTIFICATION) return "GL_DEBUG_SEVERITY_NOTIFICATION";
    if (value == GL_MAX_DEBUG_GROUP_STACK_DEPTH) return "GL_MAX_DEBUG_GROUP_STACK_DEPTH";
    if (value == GL_DEBUG_GROUP_STACK_DEPTH) return "GL_DEBUG_GROUP_STACK_DEPTH";
    if (value == GL_BUFFER) return "GL_BUFFER";
    if (value == GL_SHADER) return "GL_SHADER";
    if (value == GL_PROGRAM) return "GL_PROGRAM";
    if (value == GL_VERTEX_ARRAY) return "GL_VERTEX_ARRAY";
    if (value == GL_QUERY) return "GL_QUERY";
    if (value == GL_PROGRAM_PIPELINE) return "GL_PROGRAM_PIPELINE";
    if (value == GL_SAMPLER) return "GL_SAMPLER";
    if (value == GL_MAX_LABEL_LENGTH) return "GL_MAX_LABEL_LENGTH";
    if (value == GL_DEBUG_OUTPUT) return "GL_DEBUG_OUTPUT";
    if (value == GL_CONTEXT_FLAG_DEBUG_BIT) return "GL_CONTEXT_FLAG_DEBUG_BIT";
    if (value == GL_MAX_UNIFORM_LOCATIONS) return "GL_MAX_UNIFORM_LOCATIONS";
    if (value == GL_FRAMEBUFFER_DEFAULT_WIDTH) return "GL_FRAMEBUFFER_DEFAULT_WIDTH";
    if (value == GL_FRAMEBUFFER_DEFAULT_HEIGHT) return "GL_FRAMEBUFFER_DEFAULT_HEIGHT";
    if (value == GL_FRAMEBUFFER_DEFAULT_LAYERS) return "GL_FRAMEBUFFER_DEFAULT_LAYERS";
    if (value == GL_FRAMEBUFFER_DEFAULT_SAMPLES) return "GL_FRAMEBUFFER_DEFAULT_SAMPLES";
    if (value == GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS) return "GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS";
    if (value == GL_MAX_FRAMEBUFFER_WIDTH) return "GL_MAX_FRAMEBUFFER_WIDTH";
    if (value == GL_MAX_FRAMEBUFFER_HEIGHT) return "GL_MAX_FRAMEBUFFER_HEIGHT";
    if (value == GL_MAX_FRAMEBUFFER_LAYERS) return "GL_MAX_FRAMEBUFFER_LAYERS";
    if (value == GL_MAX_FRAMEBUFFER_SAMPLES) return "GL_MAX_FRAMEBUFFER_SAMPLES";
    if (value == GL_INTERNALFORMAT_SUPPORTED) return "GL_INTERNALFORMAT_SUPPORTED";
    if (value == GL_INTERNALFORMAT_PREFERRED) return "GL_INTERNALFORMAT_PREFERRED";
    if (value == GL_INTERNALFORMAT_RED_SIZE) return "GL_INTERNALFORMAT_RED_SIZE";
    if (value == GL_INTERNALFORMAT_GREEN_SIZE) return "GL_INTERNALFORMAT_GREEN_SIZE";
    if (value == GL_INTERNALFORMAT_BLUE_SIZE) return "GL_INTERNALFORMAT_BLUE_SIZE";
    if (value == GL_INTERNALFORMAT_ALPHA_SIZE) return "GL_INTERNALFORMAT_ALPHA_SIZE";
    if (value == GL_INTERNALFORMAT_DEPTH_SIZE) return "GL_INTERNALFORMAT_DEPTH_SIZE";
    if (value == GL_INTERNALFORMAT_STENCIL_SIZE) return "GL_INTERNALFORMAT_STENCIL_SIZE";
    if (value == GL_INTERNALFORMAT_SHARED_SIZE) return "GL_INTERNALFORMAT_SHARED_SIZE";
    if (value == GL_INTERNALFORMAT_RED_TYPE) return "GL_INTERNALFORMAT_RED_TYPE";
    if (value == GL_INTERNALFORMAT_GREEN_TYPE) return "GL_INTERNALFORMAT_GREEN_TYPE";
    if (value == GL_INTERNALFORMAT_BLUE_TYPE) return "GL_INTERNALFORMAT_BLUE_TYPE";
    if (value == GL_INTERNALFORMAT_ALPHA_TYPE) return "GL_INTERNALFORMAT_ALPHA_TYPE";
    if (value == GL_INTERNALFORMAT_DEPTH_TYPE) return "GL_INTERNALFORMAT_DEPTH_TYPE";
    if (value == GL_INTERNALFORMAT_STENCIL_TYPE) return "GL_INTERNALFORMAT_STENCIL_TYPE";
    if (value == GL_MAX_WIDTH) return "GL_MAX_WIDTH";
    if (value == GL_MAX_HEIGHT) return "GL_MAX_HEIGHT";
    if (value == GL_MAX_DEPTH) return "GL_MAX_DEPTH";
    if (value == GL_MAX_LAYERS) return "GL_MAX_LAYERS";
    if (value == GL_MAX_COMBINED_DIMENSIONS) return "GL_MAX_COMBINED_DIMENSIONS";
    if (value == GL_COLOR_COMPONENTS) return "GL_COLOR_COMPONENTS";
    if (value == GL_DEPTH_COMPONENTS) return "GL_DEPTH_COMPONENTS";
    if (value == GL_STENCIL_COMPONENTS) return "GL_STENCIL_COMPONENTS";
    if (value == GL_COLOR_RENDERABLE) return "GL_COLOR_RENDERABLE";
    if (value == GL_DEPTH_RENDERABLE) return "GL_DEPTH_RENDERABLE";
    if (value == GL_STENCIL_RENDERABLE) return "GL_STENCIL_RENDERABLE";
    if (value == GL_FRAMEBUFFER_RENDERABLE) return "GL_FRAMEBUFFER_RENDERABLE";
    if (value == GL_FRAMEBUFFER_RENDERABLE_LAYERED) return "GL_FRAMEBUFFER_RENDERABLE_LAYERED";
    if (value == GL_FRAMEBUFFER_BLEND) return "GL_FRAMEBUFFER_BLEND";
    if (value == GL_READ_PIXELS) return "GL_READ_PIXELS";
    if (value == GL_READ_PIXELS_FORMAT) return "GL_READ_PIXELS_FORMAT";
    if (value == GL_READ_PIXELS_TYPE) return "GL_READ_PIXELS_TYPE";
    if (value == GL_TEXTURE_IMAGE_FORMAT) return "GL_TEXTURE_IMAGE_FORMAT";
    if (value == GL_TEXTURE_IMAGE_TYPE) return "GL_TEXTURE_IMAGE_TYPE";
    if (value == GL_GET_TEXTURE_IMAGE_FORMAT) return "GL_GET_TEXTURE_IMAGE_FORMAT";
    if (value == GL_GET_TEXTURE_IMAGE_TYPE) return "GL_GET_TEXTURE_IMAGE_TYPE";
    if (value == GL_MIPMAP) return "GL_MIPMAP";
    if (value == GL_MANUAL_GENERATE_MIPMAP) return "GL_MANUAL_GENERATE_MIPMAP";
    if (value == GL_AUTO_GENERATE_MIPMAP) return "GL_AUTO_GENERATE_MIPMAP";
    if (value == GL_COLOR_ENCODING) return "GL_COLOR_ENCODING";
    if (value == GL_SRGB_READ) return "GL_SRGB_READ";
    if (value == GL_SRGB_WRITE) return "GL_SRGB_WRITE";
    if (value == GL_FILTER) return "GL_FILTER";
    if (value == GL_VERTEX_TEXTURE) return "GL_VERTEX_TEXTURE";
    if (value == GL_TESS_CONTROL_TEXTURE) return "GL_TESS_CONTROL_TEXTURE";
    if (value == GL_TESS_EVALUATION_TEXTURE) return "GL_TESS_EVALUATION_TEXTURE";
    if (value == GL_GEOMETRY_TEXTURE) return "GL_GEOMETRY_TEXTURE";
    if (value == GL_FRAGMENT_TEXTURE) return "GL_FRAGMENT_TEXTURE";
    if (value == GL_COMPUTE_TEXTURE) return "GL_COMPUTE_TEXTURE";
    if (value == GL_TEXTURE_SHADOW) return "GL_TEXTURE_SHADOW";
    if (value == GL_TEXTURE_GATHER) return "GL_TEXTURE_GATHER";
    if (value == GL_TEXTURE_GATHER_SHADOW) return "GL_TEXTURE_GATHER_SHADOW";
    if (value == GL_SHADER_IMAGE_LOAD) return "GL_SHADER_IMAGE_LOAD";
    if (value == GL_SHADER_IMAGE_STORE) return "GL_SHADER_IMAGE_STORE";
    if (value == GL_SHADER_IMAGE_ATOMIC) return "GL_SHADER_IMAGE_ATOMIC";
    if (value == GL_IMAGE_TEXEL_SIZE) return "GL_IMAGE_TEXEL_SIZE";
    if (value == GL_IMAGE_COMPATIBILITY_CLASS) return "GL_IMAGE_COMPATIBILITY_CLASS";
    if (value == GL_IMAGE_PIXEL_FORMAT) return "GL_IMAGE_PIXEL_FORMAT";
    if (value == GL_IMAGE_PIXEL_TYPE) return "GL_IMAGE_PIXEL_TYPE";
    if (value == GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST) return "GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST";
    if (value == GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST) return "GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST";
    if (value == GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE) return "GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE";
    if (value == GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE) return "GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE";
    if (value == GL_TEXTURE_COMPRESSED_BLOCK_WIDTH) return "GL_TEXTURE_COMPRESSED_BLOCK_WIDTH";
    if (value == GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT) return "GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT";
    if (value == GL_TEXTURE_COMPRESSED_BLOCK_SIZE) return "GL_TEXTURE_COMPRESSED_BLOCK_SIZE";
    if (value == GL_CLEAR_BUFFER) return "GL_CLEAR_BUFFER";
    if (value == GL_TEXTURE_VIEW) return "GL_TEXTURE_VIEW";
    if (value == GL_VIEW_COMPATIBILITY_CLASS) return "GL_VIEW_COMPATIBILITY_CLASS";
    if (value == GL_FULL_SUPPORT) return "GL_FULL_SUPPORT";
    if (value == GL_CAVEAT_SUPPORT) return "GL_CAVEAT_SUPPORT";
    if (value == GL_IMAGE_CLASS_4_X_32) return "GL_IMAGE_CLASS_4_X_32";
    if (value == GL_IMAGE_CLASS_2_X_32) return "GL_IMAGE_CLASS_2_X_32";
    if (value == GL_IMAGE_CLASS_1_X_32) return "GL_IMAGE_CLASS_1_X_32";
    if (value == GL_IMAGE_CLASS_4_X_16) return "GL_IMAGE_CLASS_4_X_16";
    if (value == GL_IMAGE_CLASS_2_X_16) return "GL_IMAGE_CLASS_2_X_16";
    if (value == GL_IMAGE_CLASS_1_X_16) return "GL_IMAGE_CLASS_1_X_16";
    if (value == GL_IMAGE_CLASS_4_X_8) return "GL_IMAGE_CLASS_4_X_8";
    if (value == GL_IMAGE_CLASS_2_X_8) return "GL_IMAGE_CLASS_2_X_8";
    if (value == GL_IMAGE_CLASS_1_X_8) return "GL_IMAGE_CLASS_1_X_8";
    if (value == GL_IMAGE_CLASS_11_11_10) return "GL_IMAGE_CLASS_11_11_10";
    if (value == GL_IMAGE_CLASS_10_10_10_2) return "GL_IMAGE_CLASS_10_10_10_2";
    if (value == GL_VIEW_CLASS_128_BITS) return "GL_VIEW_CLASS_128_BITS";
    if (value == GL_VIEW_CLASS_96_BITS) return "GL_VIEW_CLASS_96_BITS";
    if (value == GL_VIEW_CLASS_64_BITS) return "GL_VIEW_CLASS_64_BITS";
    if (value == GL_VIEW_CLASS_48_BITS) return "GL_VIEW_CLASS_48_BITS";
    if (value == GL_VIEW_CLASS_32_BITS) return "GL_VIEW_CLASS_32_BITS";
    if (value == GL_VIEW_CLASS_24_BITS) return "GL_VIEW_CLASS_24_BITS";
    if (value == GL_VIEW_CLASS_16_BITS) return "GL_VIEW_CLASS_16_BITS";
    if (value == GL_VIEW_CLASS_8_BITS) return "GL_VIEW_CLASS_8_BITS";
    if (value == GL_VIEW_CLASS_S3TC_DXT1_RGB) return "GL_VIEW_CLASS_S3TC_DXT1_RGB";
    if (value == GL_VIEW_CLASS_S3TC_DXT1_RGBA) return "GL_VIEW_CLASS_S3TC_DXT1_RGBA";
    if (value == GL_VIEW_CLASS_S3TC_DXT3_RGBA) return "GL_VIEW_CLASS_S3TC_DXT3_RGBA";
    if (value == GL_VIEW_CLASS_S3TC_DXT5_RGBA) return "GL_VIEW_CLASS_S3TC_DXT5_RGBA";
    if (value == GL_VIEW_CLASS_RGTC1_RED) return "GL_VIEW_CLASS_RGTC1_RED";
    if (value == GL_VIEW_CLASS_RGTC2_RG) return "GL_VIEW_CLASS_RGTC2_RG";
    if (value == GL_VIEW_CLASS_BPTC_UNORM) return "GL_VIEW_CLASS_BPTC_UNORM";
    if (value == GL_VIEW_CLASS_BPTC_FLOAT) return "GL_VIEW_CLASS_BPTC_FLOAT";
    if (value == GL_UNIFORM) return "GL_UNIFORM";
    if (value == GL_UNIFORM_BLOCK) return "GL_UNIFORM_BLOCK";
    if (value == GL_PROGRAM_INPUT) return "GL_PROGRAM_INPUT";
    if (value == GL_PROGRAM_OUTPUT) return "GL_PROGRAM_OUTPUT";
    if (value == GL_BUFFER_VARIABLE) return "GL_BUFFER_VARIABLE";
    if (value == GL_SHADER_STORAGE_BLOCK) return "GL_SHADER_STORAGE_BLOCK";
    if (value == GL_VERTEX_SUBROUTINE) return "GL_VERTEX_SUBROUTINE";
    if (value == GL_TESS_CONTROL_SUBROUTINE) return "GL_TESS_CONTROL_SUBROUTINE";
    if (value == GL_TESS_EVALUATION_SUBROUTINE) return "GL_TESS_EVALUATION_SUBROUTINE";
    if (value == GL_GEOMETRY_SUBROUTINE) return "GL_GEOMETRY_SUBROUTINE";
    if (value == GL_FRAGMENT_SUBROUTINE) return "GL_FRAGMENT_SUBROUTINE";
    if (value == GL_COMPUTE_SUBROUTINE) return "GL_COMPUTE_SUBROUTINE";
    if (value == GL_VERTEX_SUBROUTINE_UNIFORM) return "GL_VERTEX_SUBROUTINE_UNIFORM";
    if (value == GL_TESS_CONTROL_SUBROUTINE_UNIFORM) return "GL_TESS_CONTROL_SUBROUTINE_UNIFORM";
    if (value == GL_TESS_EVALUATION_SUBROUTINE_UNIFORM) return "GL_TESS_EVALUATION_SUBROUTINE_UNIFORM";
    if (value == GL_GEOMETRY_SUBROUTINE_UNIFORM) return "GL_GEOMETRY_SUBROUTINE_UNIFORM";
    if (value == GL_FRAGMENT_SUBROUTINE_UNIFORM) return "GL_FRAGMENT_SUBROUTINE_UNIFORM";
    if (value == GL_COMPUTE_SUBROUTINE_UNIFORM) return "GL_COMPUTE_SUBROUTINE_UNIFORM";
    if (value == GL_TRANSFORM_FEEDBACK_VARYING) return "GL_TRANSFORM_FEEDBACK_VARYING";
    if (value == GL_ACTIVE_RESOURCES) return "GL_ACTIVE_RESOURCES";
    if (value == GL_MAX_NAME_LENGTH) return "GL_MAX_NAME_LENGTH";
    if (value == GL_MAX_NUM_ACTIVE_VARIABLES) return "GL_MAX_NUM_ACTIVE_VARIABLES";
    if (value == GL_MAX_NUM_COMPATIBLE_SUBROUTINES) return "GL_MAX_NUM_COMPATIBLE_SUBROUTINES";
    if (value == GL_NAME_LENGTH) return "GL_NAME_LENGTH";
    if (value == GL_TYPE) return "GL_TYPE";
    if (value == GL_ARRAY_SIZE) return "GL_ARRAY_SIZE";
    if (value == GL_OFFSET) return "GL_OFFSET";
    if (value == GL_BLOCK_INDEX) return "GL_BLOCK_INDEX";
    if (value == GL_ARRAY_STRIDE) return "GL_ARRAY_STRIDE";
    if (value == GL_MATRIX_STRIDE) return "GL_MATRIX_STRIDE";
    if (value == GL_IS_ROW_MAJOR) return "GL_IS_ROW_MAJOR";
    if (value == GL_ATOMIC_COUNTER_BUFFER_INDEX) return "GL_ATOMIC_COUNTER_BUFFER_INDEX";
    if (value == GL_BUFFER_BINDING) return "GL_BUFFER_BINDING";
    if (value == GL_BUFFER_DATA_SIZE) return "GL_BUFFER_DATA_SIZE";
    if (value == GL_NUM_ACTIVE_VARIABLES) return "GL_NUM_ACTIVE_VARIABLES";
    if (value == GL_ACTIVE_VARIABLES) return "GL_ACTIVE_VARIABLES";
    if (value == GL_REFERENCED_BY_VERTEX_SHADER) return "GL_REFERENCED_BY_VERTEX_SHADER";
    if (value == GL_REFERENCED_BY_TESS_CONTROL_SHADER) return "GL_REFERENCED_BY_TESS_CONTROL_SHADER";
    if (value == GL_REFERENCED_BY_TESS_EVALUATION_SHADER) return "GL_REFERENCED_BY_TESS_EVALUATION_SHADER";
    if (value == GL_REFERENCED_BY_GEOMETRY_SHADER) return "GL_REFERENCED_BY_GEOMETRY_SHADER";
    if (value == GL_REFERENCED_BY_FRAGMENT_SHADER) return "GL_REFERENCED_BY_FRAGMENT_SHADER";
    if (value == GL_REFERENCED_BY_COMPUTE_SHADER) return "GL_REFERENCED_BY_COMPUTE_SHADER";
    if (value == GL_TOP_LEVEL_ARRAY_SIZE) return "GL_TOP_LEVEL_ARRAY_SIZE";
    if (value == GL_TOP_LEVEL_ARRAY_STRIDE) return "GL_TOP_LEVEL_ARRAY_STRIDE";
    if (value == GL_LOCATION) return "GL_LOCATION";
    if (value == GL_LOCATION_INDEX) return "GL_LOCATION_INDEX";
    if (value == GL_IS_PER_PATCH) return "GL_IS_PER_PATCH";
    if (value == GL_SHADER_STORAGE_BUFFER) return "GL_SHADER_STORAGE_BUFFER";
    if (value == GL_SHADER_STORAGE_BUFFER_BINDING) return "GL_SHADER_STORAGE_BUFFER_BINDING";
    if (value == GL_SHADER_STORAGE_BUFFER_START) return "GL_SHADER_STORAGE_BUFFER_START";
    if (value == GL_SHADER_STORAGE_BUFFER_SIZE) return "GL_SHADER_STORAGE_BUFFER_SIZE";
    if (value == GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS) return "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS";
    if (value == GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS) return "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS";
    if (value == GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS) return "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS";
    if (value == GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS) return "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS";
    if (value == GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS) return "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS";
    if (value == GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS) return "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS";
    if (value == GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS) return "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS";
    if (value == GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS) return "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS";
    if (value == GL_MAX_SHADER_STORAGE_BLOCK_SIZE) return "GL_MAX_SHADER_STORAGE_BLOCK_SIZE";
    if (value == GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT) return "GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT";
    if (value == GL_SHADER_STORAGE_BARRIER_BIT) return "GL_SHADER_STORAGE_BARRIER_BIT";
    if (value == GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES) return "GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES";
    if (value == GL_DEPTH_STENCIL_TEXTURE_MODE) return "GL_DEPTH_STENCIL_TEXTURE_MODE";
    if (value == GL_TEXTURE_BUFFER_OFFSET) return "GL_TEXTURE_BUFFER_OFFSET";
    if (value == GL_TEXTURE_BUFFER_SIZE) return "GL_TEXTURE_BUFFER_SIZE";
    if (value == GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT) return "GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT";
    if (value == GL_TEXTURE_VIEW_MIN_LEVEL) return "GL_TEXTURE_VIEW_MIN_LEVEL";
    if (value == GL_TEXTURE_VIEW_NUM_LEVELS) return "GL_TEXTURE_VIEW_NUM_LEVELS";
    if (value == GL_TEXTURE_VIEW_MIN_LAYER) return "GL_TEXTURE_VIEW_MIN_LAYER";
    if (value == GL_TEXTURE_VIEW_NUM_LAYERS) return "GL_TEXTURE_VIEW_NUM_LAYERS";
    if (value == GL_TEXTURE_IMMUTABLE_LEVELS) return "GL_TEXTURE_IMMUTABLE_LEVELS";
    if (value == GL_VERTEX_ATTRIB_BINDING) return "GL_VERTEX_ATTRIB_BINDING";
    if (value == GL_VERTEX_ATTRIB_RELATIVE_OFFSET) return "GL_VERTEX_ATTRIB_RELATIVE_OFFSET";
    if (value == GL_VERTEX_BINDING_DIVISOR) return "GL_VERTEX_BINDING_DIVISOR";
    if (value == GL_VERTEX_BINDING_OFFSET) return "GL_VERTEX_BINDING_OFFSET";
    if (value == GL_VERTEX_BINDING_STRIDE) return "GL_VERTEX_BINDING_STRIDE";
    if (value == GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET) return "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET";
    if (value == GL_MAX_VERTEX_ATTRIB_BINDINGS) return "GL_MAX_VERTEX_ATTRIB_BINDINGS";
    if (value == GL_VERTEX_BINDING_BUFFER) return "GL_VERTEX_BINDING_BUFFER";
    if (value == GL_DISPLAY_LIST) return "GL_DISPLAY_LIST";
    if (value == GL_STACK_UNDERFLOW) return "GL_STACK_UNDERFLOW";
    if (value == GL_STACK_OVERFLOW) return "GL_STACK_OVERFLOW";
    if (value == GL_MAX_VERTEX_ATTRIB_STRIDE) return "GL_MAX_VERTEX_ATTRIB_STRIDE";
    if (value == GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED) return "GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED";
    if (value == GL_TEXTURE_BUFFER_BINDING) return "GL_TEXTURE_BUFFER_BINDING";
    if (value == GL_MAP_PERSISTENT_BIT) return "GL_MAP_PERSISTENT_BIT";
    if (value == GL_MAP_COHERENT_BIT) return "GL_MAP_COHERENT_BIT";
    if (value == GL_DYNAMIC_STORAGE_BIT) return "GL_DYNAMIC_STORAGE_BIT";
    if (value == GL_CLIENT_STORAGE_BIT) return "GL_CLIENT_STORAGE_BIT";
    if (value == GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT) return "GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT";
    if (value == GL_BUFFER_IMMUTABLE_STORAGE) return "GL_BUFFER_IMMUTABLE_STORAGE";
    if (value == GL_BUFFER_STORAGE_FLAGS) return "GL_BUFFER_STORAGE_FLAGS";
    if (value == GL_CLEAR_TEXTURE) return "GL_CLEAR_TEXTURE";
    if (value == GL_LOCATION_COMPONENT) return "GL_LOCATION_COMPONENT";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_INDEX) return "GL_TRANSFORM_FEEDBACK_BUFFER_INDEX";
    if (value == GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE) return "GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE";
    if (value == GL_QUERY_BUFFER) return "GL_QUERY_BUFFER";
    if (value == GL_QUERY_BUFFER_BARRIER_BIT) return "GL_QUERY_BUFFER_BARRIER_BIT";
    if (value == GL_QUERY_BUFFER_BINDING) return "GL_QUERY_BUFFER_BINDING";
    if (value == GL_QUERY_RESULT_NO_WAIT) return "GL_QUERY_RESULT_NO_WAIT";
    if (value == GL_MIRROR_CLAMP_TO_EDGE) return "GL_MIRROR_CLAMP_TO_EDGE";
    if (value == GL_CONTEXT_LOST) return "GL_CONTEXT_LOST";
    if (value == GL_NEGATIVE_ONE_TO_ONE) return "GL_NEGATIVE_ONE_TO_ONE";
    if (value == GL_ZERO_TO_ONE) return "GL_ZERO_TO_ONE";
    if (value == GL_CLIP_ORIGIN) return "GL_CLIP_ORIGIN";
    if (value == GL_CLIP_DEPTH_MODE) return "GL_CLIP_DEPTH_MODE";
    if (value == GL_QUERY_WAIT_INVERTED) return "GL_QUERY_WAIT_INVERTED";
    if (value == GL_QUERY_NO_WAIT_INVERTED) return "GL_QUERY_NO_WAIT_INVERTED";
    if (value == GL_QUERY_BY_REGION_WAIT_INVERTED) return "GL_QUERY_BY_REGION_WAIT_INVERTED";
    if (value == GL_QUERY_BY_REGION_NO_WAIT_INVERTED) return "GL_QUERY_BY_REGION_NO_WAIT_INVERTED";
    if (value == GL_MAX_CULL_DISTANCES) return "GL_MAX_CULL_DISTANCES";
    if (value == GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES) return "GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES";
    if (value == GL_TEXTURE_TARGET) return "GL_TEXTURE_TARGET";
    if (value == GL_QUERY_TARGET) return "GL_QUERY_TARGET";
    if (value == GL_GUILTY_CONTEXT_RESET) return "GL_GUILTY_CONTEXT_RESET";
    if (value == GL_INNOCENT_CONTEXT_RESET) return "GL_INNOCENT_CONTEXT_RESET";
    if (value == GL_UNKNOWN_CONTEXT_RESET) return "GL_UNKNOWN_CONTEXT_RESET";
    if (value == GL_RESET_NOTIFICATION_STRATEGY) return "GL_RESET_NOTIFICATION_STRATEGY";
    if (value == GL_LOSE_CONTEXT_ON_RESET) return "GL_LOSE_CONTEXT_ON_RESET";
    if (value == GL_NO_RESET_NOTIFICATION) return "GL_NO_RESET_NOTIFICATION";
    if (value == GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT) return "GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT";
    if (value == GL_CONTEXT_RELEASE_BEHAVIOR) return "GL_CONTEXT_RELEASE_BEHAVIOR";
    if (value == GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH) return "GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH";
    if (value == GL_SHADER_BINARY_FORMAT_SPIR_V) return "GL_SHADER_BINARY_FORMAT_SPIR_V";
    if (value == GL_SPIR_V_BINARY) return "GL_SPIR_V_BINARY";
    if (value == GL_PARAMETER_BUFFER) return "GL_PARAMETER_BUFFER";
    if (value == GL_PARAMETER_BUFFER_BINDING) return "GL_PARAMETER_BUFFER_BINDING";
    if (value == GL_CONTEXT_FLAG_NO_ERROR_BIT) return "GL_CONTEXT_FLAG_NO_ERROR_BIT";
    if (value == GL_VERTICES_SUBMITTED) return "GL_VERTICES_SUBMITTED";
    if (value == GL_PRIMITIVES_SUBMITTED) return "GL_PRIMITIVES_SUBMITTED";
    if (value == GL_VERTEX_SHADER_INVOCATIONS) return "GL_VERTEX_SHADER_INVOCATIONS";
    if (value == GL_TESS_CONTROL_SHADER_PATCHES) return "GL_TESS_CONTROL_SHADER_PATCHES";
    if (value == GL_TESS_EVALUATION_SHADER_INVOCATIONS) return "GL_TESS_EVALUATION_SHADER_INVOCATIONS";
    if (value == GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED) return "GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED";
    if (value == GL_FRAGMENT_SHADER_INVOCATIONS) return "GL_FRAGMENT_SHADER_INVOCATIONS";
    if (value == GL_COMPUTE_SHADER_INVOCATIONS) return "GL_COMPUTE_SHADER_INVOCATIONS";
    if (value == GL_CLIPPING_INPUT_PRIMITIVES) return "GL_CLIPPING_INPUT_PRIMITIVES";
    if (value == GL_CLIPPING_OUTPUT_PRIMITIVES) return "GL_CLIPPING_OUTPUT_PRIMITIVES";
    if (value == GL_POLYGON_OFFSET_CLAMP) return "GL_POLYGON_OFFSET_CLAMP";
    if (value == GL_SPIR_V_EXTENSIONS) return "GL_SPIR_V_EXTENSIONS";
    if (value == GL_NUM_SPIR_V_EXTENSIONS) return "GL_NUM_SPIR_V_EXTENSIONS";
    if (value == GL_TEXTURE_MAX_ANISOTROPY) return "GL_TEXTURE_MAX_ANISOTROPY";
    if (value == GL_MAX_TEXTURE_MAX_ANISOTROPY) return "GL_MAX_TEXTURE_MAX_ANISOTROPY";
    if (value == GL_TRANSFORM_FEEDBACK_OVERFLOW) return "GL_TRANSFORM_FEEDBACK_OVERFLOW";
    if (value == GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW) return "GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW";
    if (value == GL_VERSION_1_0) return "GL_VERSION_1_0";
    return "UNKNOWN_VALUE";
}
}