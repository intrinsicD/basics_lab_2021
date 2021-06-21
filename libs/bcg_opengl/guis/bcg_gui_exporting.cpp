//
// Created by alex on 22.03.21.
//

#include "bcg_gui_exporting.h"
#include "components/bcg_component_entity_info.h"
#include "viewer/bcg_viewer_state.h"
#include "utils/bcg_path.h"
#include "utils/bcg_string_utils.h"
#include "geometry/mesh/bcg_meshio.h"
#include "geometry/point_cloud/bcg_point_cloudio.h"

namespace bcg {

void gui_exporting(viewer_state *state) {
    gui_exporting_mesh(state);
    gui_exporting_point_cloud(state);
}

void gui_exporting_mesh(viewer_state *state) {
    static std::vector<std::string> mesh_formats = meshio::formats();
    static std::string filter;
    static bool save = true;
    if (state->scene.valid(state->picker.entity_id)) {
        auto &info = state->scene.get<entity_info>(state->picker.entity_id);
        static int mesh_format_selected = -1;
        std::string filename_mesh = info.filename;
        if (mesh_format_selected < 0) {
            auto formats = join(".", mesh_formats);
            auto extension = path_extension(filename_mesh);
            for (int i = 0; i < static_cast<int>(formats.size()); ++i) {
                if (formats[i] == extension) {
                    mesh_format_selected = i;
                    break;
                }
            }
            if (mesh_format_selected < 0) {
                mesh_format_selected = 0;
            }
        }
        if (state->scene.has<halfedge_mesh>(state->picker.entity_id)) {
            if (ImGui::Button("export mesh")) {
                ImGui::OpenPopup("export mesh");
                filter = join(join("*.", mesh_formats), ";");
            }

            ImGui::SameLine();
            draw_combobox(&state->window, "mesh formats", mesh_format_selected, mesh_formats);

            static std::string output_path;
            static bool file_exists = false;

            filename_mesh = replace_extension(filename_mesh, mesh_formats[mesh_format_selected]);
            if (draw_filedialog(&state->window, "export mesh", output_path, save, path_dirname(filename_mesh),
                                path_filename(filename_mesh), filter, file_exists)) {
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                meshio io(output_path, meshio_flags());
                io.write(mesh);
            }
        }
    }
}

void gui_exporting_point_cloud(viewer_state *state) {
    static std::vector<std::string> point_cloud_formats = point_cloudio::formats();
    static std::string filter;
    static bool save = true;
    if (state->scene.valid(state->picker.entity_id)) {
        auto &info = state->scene.get<entity_info>(state->picker.entity_id);
        static int point_cloud_format_selected = -1;
        std::string filename_point_cloud = info.filename;
        if (point_cloud_format_selected < 0) {
            auto formats = join(".", point_cloud_formats);
            auto extension = path_extension(filename_point_cloud);
            for (int i = 0; i < static_cast<int>(formats.size()); ++i) {
                if (formats[i] == extension) {
                    point_cloud_format_selected = i;
                    break;
                }
            }
            if (point_cloud_format_selected < 0) {
                point_cloud_format_selected = 0;
            }
        }
        if (ImGui::Button("export point cloud")) {
            ImGui::OpenPopup("export point cloud");
            filter = join(join("*.", point_cloud_formats), ";");
        }

        ImGui::SameLine();
        draw_combobox(&state->window, "point cloud formats", point_cloud_format_selected, point_cloud_formats);

        static std::string output_path;
        static bool file_exists = false;

        filename_point_cloud = replace_extension(filename_point_cloud,
                                                 point_cloud_formats[point_cloud_format_selected]);
        if (draw_filedialog(&state->window, "export point cloud", output_path, save, path_dirname(filename_point_cloud),
                            path_filename(filename_point_cloud), filter, file_exists)) {
            auto *vertices = state->get_vertices(state->picker.entity_id);
            point_cloudio io(output_path, point_cloudio_flags());
            io.write(vertices);
        }
    }
}

}