//
// Created by alex on 16.11.20.
//

#include "bcg_gui_ogl_buffer.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_ogl_buffer(viewer_state* state, const ogl_buffer_object &buffer){
    if (ImGui::CollapsingHeader(buffer.name.c_str())) {
        draw_label(&state->window, "handle_id: ", std::to_string(buffer.handle));
        draw_label(&state->window, "target", ogl_enum_to_string(buffer.target));
        draw_label(&state->window, "capacity", std::to_string(buffer.capacity));
        draw_label(&state->window, "num_elements", std::to_string(buffer.num_elements));
        draw_label(&state->window, "dims", std::to_string(buffer.dims));
        draw_label(&state->window, "dynamic", (buffer.dynamic ? "true" : "false"));
        draw_label(&state->window, "size_bytes_cpu", std::to_string(buffer.size_bytes));
/*        glBindVertexArray(0);
        buffer.bind();
        draw_label(&state->window, "size_bytes_gpu", std::to_string(buffer.get_buffer_size_gpu()));
        buffer.release();*/
        ImGui::Separator();
    }
}

}