//
// Created by alex on 22.03.21.
//

#include "bcg_gui_exporting.h"
#include "viewer/bcg_entity_info.h"
#include "viewer/bcg_viewer_state.h"
#include "utils/bcg_path.h"
#include "utils/bcg_string_utils.h"
#include "geometry/mesh/bcg_meshio.h"

namespace bcg {

void gui_exporting(viewer_state *state) {

    static std::string filter = join(join("*.", meshio::formats()), ";");
    static bool save = true;
    if (state->scene.valid(state->picker.entity_id)) {
        auto &info = state->scene.get<entity_info>(state->picker.entity_id);

        if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
            if(ImGui::Button("export mesh")){
                ImGui::OpenPopup("export mesh");
            }
        }
        std::string output_path;
        if(draw_filedialog(&state->window, "export mesh", output_path, save, path_dirname(info.filename),
                        path_filename(info.filename), filter)){
            auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
            meshio io(output_path, meshio_flags());
            io.write(mesh);
        }
    }


}

}