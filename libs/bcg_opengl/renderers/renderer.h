//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_RENDERER_H
#define BCG_GRAPHICS_RENDERER_H

#include <unordered_map>
#include <vector>
#include <string>

namespace bcg{

struct viewer_state;

struct renderer{
    explicit renderer(viewer_state *state);

    virtual ~renderer() = default;

    virtual void draw() = 0;

    void enqueue(size_t id){
        entities_to_draw.emplace_back(id);
    };

protected:
    std::unordered_map<std::string, unsigned int> programs;
    std::vector<size_t> entities_to_draw;
};

}

#endif //BCG_GRAPHICS_RENDERER_H
