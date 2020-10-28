//
// Created by alex on 28.10.20.
//

#include "bcg_viewer_state.h"

#include <utility>
#include "exts/glad/glad.h"

namespace bcg{

bool viewer_systems::has(const std::string &name) const {
    return systems.find(name) != systems.end();
}

system &viewer_systems::operator[](const std::string &name) {
    return systems[name];
}

const system &viewer_systems::operator[](const std::string &name) const {
    return systems.at(name);
}

void viewer_shaders::load(std::string name,
          std::string vertex_shader_file,
          std::string fragment_shader_file,
          std::string *geometry_shader_file,
          std::string *tess_control_shader_file,
          std::string *tess_eval_shader_file){
    glsl_program program(std::move(name));
    std::vector<glsl_shader> shaders;
    shaders.push_back(load_shader(std::move(vertex_shader_file), GL_VERTEX_SHADER));
    shaders.push_back(load_shader(std::move(fragment_shader_file), GL_FRAGMENT_SHADER));
    if (geometry_shader_file) {
        shaders.push_back(load_shader(*geometry_shader_file, GL_GEOMETRY_SHADER));
    }
    if (tess_control_shader_file) {
        shaders.push_back(load_shader(*tess_control_shader_file, GL_TESS_CONTROL_SHADER));
    }
    if (tess_eval_shader_file) {
        shaders.push_back(load_shader(*tess_eval_shader_file, GL_TESS_EVALUATION_SHADER));
    }

    program.build(shaders);

    for (const auto &shader : shaders) {
        watcher.watch(shader.filename, [shader, program]() {
            glsl_shader new_shader = shader;
            program.detach(shader);
            new_shader.compile_file(shader.filename.c_str(), shader.type);
            if (!program.is_valid()) return;
            program.attach(new_shader);
            if (program.link()) return;
        });
    }
    programs[name] = program;
}

glsl_shader viewer_shaders::load_shader(std::string filename, unsigned int type) const{
    glsl_shader shader;
    shader.compile_file(filename.c_str(), type);
    return shader;
}

glsl_program &viewer_shaders::operator[](const std::string &name) {
    return programs[name];
}

const glsl_program &viewer_shaders::operator[](const std::string &name) const {
    return programs.at(name);
}

}