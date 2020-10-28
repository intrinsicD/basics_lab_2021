//
// Created by alex on 28.10.20.
//

#include <iostream>
#include "bcg_mesh_system.h"
#include "bcg_viewer_state.h"

namespace bcg{

mesh_system::mesh_system(viewer_state *state) : system("mesh_system", state){
    state->dispatcher.sink<event::file_drop>().connect<&mesh_system::on_file_drop>(this);
}

void mesh_system::on_file_drop(const event::file_drop &event){
    std::cout << "files to read:\n";
    for(const auto &item : event.filenames){
        std::cout << item << "\n";
    }
}

}