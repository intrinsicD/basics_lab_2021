//
// Created by alex on 22.03.21.
//

#include "bcg_gui_exporting.h"
#include "viewer/bcg_entity_info.h"
#include "viewer/bcg_viewer_state.h"
#include "utils/bcg_path.h"
#include "utils/bcg_string_utils.h"
#include "geometry/mesh/bcg_meshio.h"
#include "geometry/point_cloud/bcg_point_cloudio.h"

namespace bcg {

void gui_exporting(viewer_state *state) {
    static std::vector<std::string> mesh_formats = meshio::formats();
    static std::vector<std::string> point_cloud_formats = point_cloudio::formats();
    static std::string filter;
    static bool save = true;
    if (state->scene.valid(state->picker.entity_id)) {
        auto &info = state->scene.get<entity_info>(state->picker.entity_id);
        static bool clicked_mesh = false;
        static bool clicked_point_cloud = false;
        static int mesh_format_selected = 0;
        static int point_cloud_format_selected = 0;
        if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
            if(ImGui::Button("export mesh")){
                ImGui::OpenPopup("export mesh");
                clicked_mesh = true;
                filter = join(join("*.", mesh_formats), ";");
            }
        }
        if(ImGui::Button("export point cloud")){
            ImGui::OpenPopup("export point cloud");
            clicked_point_cloud = true;
            filter = join(join("*.", point_cloud_formats), ";");
        }
        if(clicked_mesh){
            draw_combobox(&state->window, "mesh formats", mesh_format_selected, mesh_formats);
        }
        if(clicked_point_cloud){
            draw_combobox(&state->window, "point cloud formats", point_cloud_format_selected, point_cloud_formats);
        }
        std::string output_path;
        if(draw_filedialog(&state->window, "export mesh", output_path, save, path_dirname(info.filename),
                        path_filename(info.filename), filter)){
            auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
            output_path = replace_extension(output_path, mesh_formats[mesh_format_selected]);
            meshio io(output_path, meshio_flags());
            io.write(mesh);
        }
        if(draw_filedialog(&state->window, "export point cloud", output_path, save, path_dirname(info.filename),
                           path_filename(info.filename), filter)){
            auto *vertices = state->get_vertices(state->picker.entity_id);
            output_path = replace_extension(output_path, point_cloud_formats[point_cloud_format_selected]);
            point_cloudio io(output_path, point_cloudio_flags());
            io.write(vertices);
        }
    }


}

}