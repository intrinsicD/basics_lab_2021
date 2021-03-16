//
// Created by alex on 29.10.20.
//

#include "bcg_renderer.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_property_map_eigen.h"
#include "bcg_library/math/matrix/bcg_matrix_map_eigen.h"
#include "utils/bcg_string_utils.h"
#include "geometry/mesh/bcg_mesh.h"

namespace bcg{

renderer::renderer(viewer_state *state) : system("renderer", state){

}

renderer::renderer(std::string name, viewer_state *state) : system(name, state){
    state->dispatcher.sink<event::internal::begin_frame>().connect<&renderer::on_begin_frame>(this);
    state->dispatcher.sink<event::internal::render>().connect<&renderer::on_render>(this);
    state->dispatcher.sink<event::internal::end_frame>().connect<&renderer::on_end_frame>(this);
}

void renderer::on_begin_frame(const event::internal::begin_frame &){}

void renderer::on_render(const event::internal::render &){}

void renderer::on_end_frame(const event::internal::end_frame &){

}

std::vector<Vector<float, 3>>
renderer::map_to_colors(property_container *container, const std::string &property_name, colormap::base_colormap color_map){
    std::vector<Vector<float, 3>> colors;
    auto *base_ptr = container->get_base_ptr(property_name);
    colors.resize(base_ptr->size());
    if(base_ptr->dims() == 1){
        if(base_ptr->void_ptr() == nullptr){
            VectorS<-1> values = MapConst(container->get<bool, 1>(base_ptr->name())).cast<bcg_scalar_t>();
            Map(colors) = MapConst(color_map(values, values.minCoeff(), values.maxCoeff())).cast<float>();
        }else{
            if(contains(property_name, "v_connectivity")){
                VectorS<-1> values = MapConst(container->get<halfedge_graph::vertex_connectivity , 1>(base_ptr->name())).cast<bcg_scalar_t>();
                Map(colors) = MapConst(color_map(values, values.minCoeff(), values.maxCoeff())).cast<float>();
            }else if(contains(property_name, "f_connectivity")){
                VectorS<-1> values = MapConst(container->get<halfedge_mesh::face_connectivity , 1>(base_ptr->name())).cast<bcg_scalar_t>();
                Map(colors) = MapConst(color_map(values, values.minCoeff(), values.maxCoeff())).cast<float>();
            }else{
                VectorS<-1> values = MapConst(container->get<bcg_scalar_t , 1>(base_ptr->name()));
                Map(colors) = MapConst(color_map(values, values.minCoeff(), values.maxCoeff())).cast<float>();
            }
        }
    }else if(base_ptr->dims() == 3){
        std::vector<Vector<float, 3>> colors1(base_ptr->size());
        auto values = container->get<VectorS<3>, 3>(base_ptr->name());
        if(!values){
            auto values1 = container->get<VectorI<3>, 3>(base_ptr->name());
            colors1.resize(values1.size());
            Map<3>(colors1) = MapConst(values1).cast<float>();
        }else{
            colors1.resize(values.size());
            Map<3>(colors1) = MapConst(values).cast<float>();
        }
        auto min = MapConst<3>(colors1).minCoeff();
        auto max = MapConst<3>(colors1).maxCoeff();
        colors.resize(colors1.size());
        Map<3>(colors) = (MapConst<3>(colors1).array() - min) /  (max - min);
    }
    return colors;
}


}