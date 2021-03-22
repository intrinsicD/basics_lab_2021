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
        static std::string output_path;
        static bool file_exists = false;
        std::string filename_mesh = info.filename;
        std::string filename_point_cloud = info.filename;
        filename_mesh = replace_extension(filename_mesh, mesh_formats[mesh_format_selected]);
        filename_point_cloud = replace_extension(filename_point_cloud, point_cloud_formats[point_cloud_format_selected]);
        if(draw_filedialog(&state->window, "export mesh", output_path, save, path_dirname(filename_mesh),
                        path_filename(filename_mesh), filter, file_exists) || file_exists){
            bool replace = false;
            if(file_exists){
                //TODO this is not a good solution, maybe put this into draw_filedialog?
                ImGui::OpenPopup("file exists");
                if(ImGui::BeginPopupModal("file exists")){
                    std::string filename = path_filename(output_path);
                    if(draw_textinput(&state->window, "filename", filename)){
                        output_path = path_join(path_dirname(output_path), filename);
                    }
                    if (ImGui::Button("Rename")) {
                        replace = true;
                        file_exists = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Replace")) {
                        replace = true;
                        file_exists = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel")) {
                        replace = false;
                        file_exists = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            if(!file_exists || replace){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                meshio io(output_path, meshio_flags());
                io.write(mesh);
            }
        }
        if(draw_filedialog(&state->window, "export point cloud", output_path, save, path_dirname(info.filename),
                           path_filename(info.filename), filter, file_exists)){
            auto *vertices = state->get_vertices(state->picker.entity_id);
            point_cloudio io(filename_point_cloud, point_cloudio_flags());
            io.write(vertices);
        }
    }


}

}