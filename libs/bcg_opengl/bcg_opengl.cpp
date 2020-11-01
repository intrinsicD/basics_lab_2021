//
// Created by alex on 16.10.20.
//

#include <cassert>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iostream>
#include "bcg_opengl.h"

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

ogl_handle::operator unsigned int() const{
    return handle;
}


//----------------------------------------------------------------------------------------------------------------------
// glsl shader
//----------------------------------------------------------------------------------------------------------------------


glsl_shader::glsl_shader() : ogl_handle(), type(BCG_GL_INVALID_ID), source(), filename() {

}

glsl_shader::glsl_shader(unsigned int type, std::string name) : ogl_handle(BCG_GL_INVALID_ID, name), type(type){

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

glsl_program::glsl_program(std::string name) : ogl_handle(BCG_GL_INVALID_ID, name){

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

void ogl_texture::activate(GLint unit) {
    unit = unit;
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
    release();
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
                                                                                                BCG_GL_INVALID_ID,
                                                                                                name) {

}

ogl_buffer_object::ogl_buffer_object(unsigned int handle, unsigned int target, std::string name) : ogl_handle(handle,
                                                                                                              name),
                                                                                                   target(target),
                                                                                                   capacity(0),
                                                                                                   num_elements(0),
                                                                                                   dims(0),
                                                                                                   dynamic(false) {

}

size_t ogl_buffer_object::get_buffer_size_gpu() const {
    bind();
    GLint bytes = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bytes);
    assert_ogl_error();
    release();
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

ogl_vertex_buffer::ogl_vertex_buffer() : ogl_buffer_object(GL_ARRAY_BUFFER, "vbo") {

}

ogl_vertex_buffer::ogl_vertex_buffer(std::string name) : ogl_buffer_object(GL_ARRAY_BUFFER, name) {

}

ogl_vertex_buffer::ogl_vertex_buffer(unsigned int handle, std::string name) : ogl_buffer_object(handle, GL_ARRAY_BUFFER,
                                                                                                name) {

}

void ogl_vertex_buffer::upload(const std::vector<bcg_scalar_t> &data, size_t offset, bool dynamic) {
    upload(data.data(), data.size(), 1, offset, dynamic);
}

void ogl_vertex_buffer::upload(const std::vector<VectorS<2>> &data, size_t offset, bool dynamic) {
    upload(data.data(), data.size(), 2, offset, dynamic);
}

void ogl_vertex_buffer::upload(const std::vector<VectorS<3>> &data, size_t offset, bool dynamic) {
    upload(data.data(), data.size(), 3, offset, dynamic);
}

void ogl_vertex_buffer::upload(const std::vector<VectorS<4>> &data, size_t offset, bool dynamic) {
    upload(data.data(), data.size(), 4, offset, dynamic);
}

void ogl_vertex_buffer::upload(const void *data, size_t size, int dims, size_t offset, bool dynamic) {
    if (size > capacity || !(*this)) {
        // reallocate buffer if needed
        destroy();
        create();
        bind();
        glBufferData(target, size * sizeof(bcg_scalar_t), nullptr, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        assert_ogl_error();
        capacity = size;
        num_elements = capacity / dims;
        this->dims = dims;
        this->dynamic = dynamic;
    } else {
        // we have enough space
        bind();
    }

    glBufferSubData(target, offset * sizeof(bcg_scalar_t), size * sizeof(bcg_scalar_t), data);
    assert_ogl_error();
    release();
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

ogl_element_buffer::ogl_element_buffer() : ogl_buffer_object(GL_ARRAY_BUFFER, "ebo") {

}

ogl_element_buffer::ogl_element_buffer(std::string name) : ogl_buffer_object(GL_ELEMENT_ARRAY_BUFFER, name) {

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
    if (size > capacity) {
        // reallocate buffer if needed
        destroy();
        create();
        bind();
        glBufferData(target, size * sizeof(bcg_index_t), nullptr, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        assert_ogl_error();
        capacity = size;
        num_elements = capacity / dims;
        this->dims = dims;
        this->dynamic = dynamic;
    } else {
        // we have enough space
        bind();
    }

    glBufferSubData(target, offset * sizeof(bcg_index_t), size * sizeof(bcg_index_t), data);
    assert_ogl_error();
    release();
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

ogl_vertex_array::ogl_vertex_array() : ogl_handle("vao") {

}

ogl_vertex_array::ogl_vertex_array(std::string name) : ogl_handle(name) {

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

ogl_vertex_buffer ogl_vertex_array::get_vertex_buffer(std::string name) {
    auto iter = std::find_if(vertex_buffers.begin(), vertex_buffers.end(), [name](const ogl_vertex_buffer &vbo) {
        return vbo.name == name;
    });
    if (iter != vertex_buffers.end()) {
        return *iter;
    }
    return ogl_vertex_buffer();
}

void ogl_vertex_array::add_vertex_buffer(const ogl_vertex_buffer &vertex_buffer) {
    auto vbo = get_vertex_buffer(vertex_buffer.name);
    if (!vbo) {
        bind();
        vertex_buffer.bind();
        release();
        vertex_buffer.release();
        vertex_buffers.push_back(vertex_buffer);
    }
}

void ogl_vertex_array::set_element_buffer(const ogl_element_buffer &buffer) {
    element_buffer = buffer;
    if(!element_buffer){
        element_buffer.create();
    }
    bind();
    element_buffer.bind();
    release();
}

ogl_renderbuffer::ogl_renderbuffer() : ogl_renderbuffer(BCG_GL_INVALID_ID, "rbo") {

}

ogl_renderbuffer::ogl_renderbuffer(std::string name) : ogl_renderbuffer(BCG_GL_INVALID_ID, name) {

}

ogl_renderbuffer::ogl_renderbuffer(unsigned int handle, std::string name) : ogl_handle(handle, name) {

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

void ogl_renderbuffer::storage(GLenum internal_format, int width, int height) const {
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
    glBindBuffer(GL_FRAMEBUFFER, handle);
    assert_ogl_error();
}

void ogl_framebuffer::release() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert_ogl_error();
}

void ogl_framebuffer::attach_texture(const ogl_texture &texture, unsigned int attachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.target, texture.handle, 0);
    assert_ogl_error();
}

void ogl_framebuffer::attach_renderbuffer(const ogl_renderbuffer &renderbuffer, unsigned int attachment) {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.handle);
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

void ogl_framebuffer::oepngl_draw_buffers(){
    std::vector<unsigned int> attachments;
    for(size_t i = 0; i < textures.size(); ++i){
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(attachments.size(), attachments.data());
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
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    assert_ogl_error();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert_ogl_error();
}


ogl_state::ogl_state(){
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
    point_size_value = 1;
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

void ogl_state::set_blend(bool enabled){
    gl_blend = enabled;
    gl_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    assert_ogl_error();
}

void ogl_state::set_blend_func_factors(GLenum sfactor, GLenum dfactor){
    blend_func_sfactor = sfactor;
    blend_func_dfactor = dfactor;
    glBlendFunc(blend_func_sfactor, blend_func_dfactor);
    assert_ogl_error();
}

void ogl_state::set_color_logic_op(bool enabled){
    gl_color_logic_op = enabled;
    gl_color_logic_op ? glEnable(GL_COLOR_LOGIC_OP) : glDisable(GL_COLOR_LOGIC_OP);
    assert_ogl_error();
}
void ogl_state::set_logic_op_opcode(GLenum opcode){
    logic_op_opcode = opcode;
    glLogicOp(logic_op_opcode);
    assert_ogl_error();
}

void ogl_state::set_cull_face(bool enabled){
    gl_cull_face = enabled;
    gl_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    assert_ogl_error();
}
void ogl_state::set_cullface_mode(GLenum mode){
    cull_face_mode = mode;
    glCullFace(cull_face_mode);
    assert_ogl_error();
}
void ogl_state::set_frontface_mode(GLenum mode){
    front_face_mode = mode;
    glFrontFace(front_face_mode);
    assert_ogl_error();
}

void ogl_state::set_depth_clamp(bool enabled){
    gl_depth_clamp = enabled;
    gl_depth_clamp ? glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);
    assert_ogl_error();
}
void ogl_state::set_depth_test(bool enabled){
    gl_depth_test = enabled;
    gl_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    assert_ogl_error();
}
void ogl_state::set_depth_mask(bool enabled){
    gl_depth_mask = enabled;
    gl_depth_mask ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    assert_ogl_error();
}
void ogl_state::set_depth_range_values(GLdouble near, GLdouble far){
    depth_range_nearVal = near;
    depth_range_farVal = far;
    glDepthRange(depth_range_nearVal, depth_range_farVal);
    assert_ogl_error();
}
void ogl_state::set_depth_func(GLenum func){
    depth_func = func;
    glDepthFunc(depth_func);
    assert_ogl_error();
}

void ogl_state::set_dither(bool enabled){
    gl_dither = enabled;
    gl_dither ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
    assert_ogl_error();
}

void ogl_state::set_framebuffer_srgb(bool enabled){
    gl_framebuffer_srgb = enabled;
    gl_framebuffer_srgb ? glEnable(GL_FRAMEBUFFER_SRGB) : glDisable(GL_FRAMEBUFFER_SRGB);
    assert_ogl_error();
}

void ogl_state::set_line_smooth(bool enabled){
    gl_line_smooth = enabled;
    gl_line_smooth ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
    assert_ogl_error();
}

void ogl_state::set_multisample(bool enabled){
    gl_multisample = enabled;
    gl_multisample ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
    assert_ogl_error();
}
void ogl_state::set_multisample_value(GLfloat value){
    multisample_value = value;
    glMinSampleShading(multisample_value);
    assert_ogl_error();
}

void ogl_state::set_polygon_offset_fill(bool enabled){
    gl_polygon_offset_fill = enabled;
    gl_polygon_offset_fill ? glEnable(GL_POLYGON_OFFSET_FILL) : glDisable(GL_POLYGON_OFFSET_FILL);
    assert_ogl_error();
}
void ogl_state::set_polygon_offset_point(bool enabled){
    gl_polygon_offset_point = enabled;
    gl_polygon_offset_point ? glEnable(GL_POLYGON_OFFSET_POINT) : glDisable(GL_POLYGON_OFFSET_POINT);
    assert_ogl_error();
}
void ogl_state::set_polygon_offset_line(bool enabled){
    gl_polygon_offset_line = enabled;
    gl_polygon_offset_line ? glEnable(GL_POLYGON_OFFSET_LINE) : glDisable(GL_POLYGON_OFFSET_LINE);
    assert_ogl_error();
}
void ogl_state::set_polygon_smooth(bool enabled){
    gl_polygon_smooth = enabled;
    gl_polygon_smooth ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);
    assert_ogl_error();
}
void ogl_state::set_polygon_offset_factor(GLfloat factor, GLfloat units){
    polygon_offset_factor = factor;
    polygon_offset_units = units;
    glPolygonOffset(polygon_offset_factor, polygon_offset_units);
    assert_ogl_error();
}
void ogl_state::set_polygon_mode(GLenum mode){
    polygon_mode = mode;
    glPolygonMode(polygon_mode_face, polygon_mode);
    assert_ogl_error();
}

void ogl_state::set_rasterizer_discard(bool enabled){
    gl_rasterizer_discard = enabled;
    gl_rasterizer_discard ? glEnable(GL_RASTERIZER_DISCARD) : glDisable(GL_RASTERIZER_DISCARD);
    assert_ogl_error();
}

void ogl_state::set_alpha_to_coverage(bool enabled){
    gl_sample_alpha_to_converge = enabled;
    gl_sample_alpha_to_converge ? glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE) : glDisable(
            GL_SAMPLE_ALPHA_TO_COVERAGE);
    assert_ogl_error();
}
void ogl_state::set_alpha_to_one(bool enabled){
    gl_sample_alpha_to_one = enabled;
    gl_sample_alpha_to_one ? glEnable(GL_SAMPLE_ALPHA_TO_ONE) : glDisable(GL_SAMPLE_ALPHA_TO_ONE);
    assert_ogl_error();
}

void ogl_state::set_sample_converge(bool enabled){
    gl_sample_converge = enabled;
    gl_sample_converge ? glEnable(GL_SAMPLE_COVERAGE) : glDisable(GL_SAMPLE_COVERAGE);
    assert_ogl_error();
}
void ogl_state::set_sample_converge_value(GLfloat value, GLboolean invert){
    sample_coverage_value = value;
    sample_coverage_invert = invert;
    glSampleCoverage(sample_coverage_value, sample_coverage_invert);
    assert_ogl_error();
}
void ogl_state::set_sample_shading(bool enabled){
    gl_sample_shading = enabled;
    gl_sample_shading ? glEnable(GL_SAMPLE_SHADING) : glDisable(GL_SAMPLE_SHADING);
    assert_ogl_error();
}
void ogl_state::set_sample_mask(bool enabled){
    gl_sample_mask = enabled;
    gl_sample_mask ? glEnable(GL_SAMPLE_MASK) : glDisable(GL_SAMPLE_MASK);
    assert_ogl_error();
}
void ogl_state::set_sample_mask_number(GLuint number, GLbitfield masks){
    sample_mask_number = number;
    sample_maks = masks;
    glSampleMaski(sample_mask_number, sample_maks);
    assert_ogl_error();
}

void ogl_state::set_scissor_test(bool enabled){
    gl_scissor_test = enabled;
    gl_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
    assert_ogl_error();
}
void ogl_state::set_scissor_values(GLint x, GLint y, GLsizei width, GLsizei height){
    scissor_x = x;
    scissor_y = y;
    scissor_width = width;
    scissor_height = height;
    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
    assert_ogl_error();
}

void ogl_state::set_stencil_test(bool enabled){
    gl_stencil_test = enabled;
    gl_stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    assert_ogl_error();
}
void ogl_state::set_stencil_func(GLenum func, GLint ref, GLuint mask){
    stencil_func = func;
    stencil_ref = ref;
    stencil_mask = mask;
    glStencilFunc(stencil_func, stencil_ref, stencil_mask);
    assert_ogl_error();
}
void ogl_state::set_stencil_op(GLenum sfail, GLenum dpfail, GLenum dppass){
    stencil_sfail = sfail;
    stencil_dpfail = dpfail;
    stencil_dppass = dppass;
    glStencilOp(stencil_sfail, stencil_dpfail, stencil_dppass);
    assert_ogl_error();
}

void ogl_state::set_program_point_size(bool enabled){
    gl_program_point_size = enabled;
    gl_program_point_size ? glEnable(GL_PROGRAM_POINT_SIZE) : glDisable(GL_PROGRAM_POINT_SIZE);
    assert_ogl_error();
}
void ogl_state::set_point_size(GLuint size){
    point_size_value = size;
    glPointSize(point_size_value);
    assert_ogl_error();
}

#ifdef GL_VERSION_3_1
void ogl_state::set_primitive_restart(bool enabled){
    gl_primitive_restart = enabled;
    gl_primitive_restart ? glEnable(GL_PRIMITIVE_RESTART) : glDisable(GL_PRIMITIVE_RESTART);
    assert_ogl_error();
}

void ogl_state::set_primitive_restart_index(GLuint index){
    primitive_restart_index = index;
    glPrimitiveRestartIndex(primitive_restart_index);
    assert_ogl_error();
}
#endif

#ifdef GL_VERSION_3_2
void ogl_state::set_texture_cube_map_seamless(bool enabled){
    gl_texture_cube_map_seamless = enabled;
    gl_texture_cube_map_seamless ? glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : glDisable(
            GL_TEXTURE_CUBE_MAP_SEAMLESS);
    assert_ogl_error();
}
#endif

#ifdef GL_VERSION_4_3
void ogl_state::set_debug_output(bool enabled){
    gl_debug_output = enabled;
    gl_debug_output ? glEnable(GL_DEBUG_OUTPUT) : glDisable(GL_DEBUG_OUTPUT);
    assert_ogl_error();
}
void ogl_state::set_debug_output_synchonous(bool enabled){
    gl_debug_output_synchonous = enabled;
    gl_debug_output_synchonous ? glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS) : glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    assert_ogl_error();
}

void ogl_state::set_primitive_restart_fixed_index(bool enabled){
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


}