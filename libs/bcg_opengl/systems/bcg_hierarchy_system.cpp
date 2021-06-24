//
// Created by alex on 19.11.20.
//

#include "bcg_hierarchy_system.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_hierarchy.h"
#include "components/bcg_component_transform_world_space.h"

namespace bcg {

hierarchy_system::hierarchy_system(viewer_state *state) : system("hierarchy_system", state) {
    state->dispatcher.sink<event::hierarchy::set_parent>().connect<&hierarchy_system::on_set_parent>(this);
    state->dispatcher.sink<event::hierarchy::add_child>().connect<&hierarchy_system::on_add_child>(this);
    state->dispatcher.sink<event::hierarchy::remove_child>().connect<&hierarchy_system::on_remove_child>(this);
    state->dispatcher.sink<event::internal::destroy>().connect<&hierarchy_system::on_destroy>(this);
    state->dispatcher.sink<event::internal::update>().connect<&hierarchy_system::on_update>(this);
}

void hierarchy_system::on_set_parent(const event::hierarchy::set_parent &event) {
    if (!state->scene.valid(event.id)) return;
    if (!state->scene.has<entity_hierarchy>(event.id)) {
        state->scene().emplace<entity_hierarchy>(event.id);
    }
    auto &hierarchy = state->scene.get<entity_hierarchy>(event.id);
    hierarchy.parent = event.parent_id;

    auto &model = state->scene.get<world_space_transform>(event.id);
    if (state->scene.valid(hierarchy.parent) && state->scene.has<entity_hierarchy>(hierarchy.parent)) {
        auto &parent_hierarchy = state->scene.get<entity_hierarchy>(hierarchy.parent);
        hierarchy.accumulated_model = parent_hierarchy.accumulated_model * model;
    }
}

void hierarchy_system::on_add_child(const event::hierarchy::add_child &event) {
    if(!state->scene.valid(event.id)) return;
    if(!state->scene.has<entity_hierarchy>(event.id)){
        state->scene().emplace<entity_hierarchy>(event.id);
    }
    auto &hierarchy = state->scene.get<entity_hierarchy>(event.id);
    hierarchy.children[event.child_id] = event.child_id;
    state->dispatcher.trigger<event::hierarchy::set_parent>(event.child_id, event.id);
}

void hierarchy_system::on_remove_child(const event::hierarchy::remove_child &event) {
    if(!state->scene.valid(event.id)) return;
    if(!state->scene.has<entity_hierarchy>(event.id)){
        state->scene().emplace<entity_hierarchy>(event.id);
    }
    auto &hierarchy = state->scene.get<entity_hierarchy>(event.id);
    auto iter = hierarchy.children.find(event.child_id);
    if(iter != hierarchy.children.end()){
        if(state->scene.has<entity_hierarchy>(event.child_id)){
            auto &child_hierarchy = state->scene.get<entity_hierarchy>(event.child_id);
            if(child_hierarchy.parent == event.id){
                state->dispatcher.trigger<event::hierarchy::set_parent>(event.child_id, entt::null);
            }
            hierarchy.children.erase(iter);
        }
    }
}

void hierarchy_system::update_accumulated_model(entt::entity id){
    auto &hierarchy = state->scene.get<entity_hierarchy>(id);
    auto &model = state->scene.get<world_space_transform>(id);
    hierarchy.accumulated_model = model;
    if(hierarchy.parent == entt::null){
        hierarchy.accumulated_model = model;
    }else{
        auto &parent_hierarchy = state->scene.get<entity_hierarchy>(hierarchy.parent);
        hierarchy.accumulated_model = parent_hierarchy.accumulated_model * model;
    }
    for(const auto item : hierarchy.children){
        update_accumulated_model(item.second);
    }
}

void hierarchy_system::on_update(const event::internal::update &){
    auto view = state->scene().view<entity_hierarchy, Transform>();
    for(const auto id : view){
        auto &hierarchy = state->scene.get<entity_hierarchy>(id);
        if(hierarchy.parent == entt::null){
            update_accumulated_model(id);
        }
    }
}

void hierarchy_system::on_destroy(const event::internal::destroy &event){
    if(state->scene.valid(event.id) && state->scene.has<entity_hierarchy>(event.id)){
        auto hierarchy = state->scene.get<entity_hierarchy>(event.id);
        if(!hierarchy.children.empty()){
            for(auto &child : hierarchy.children){
                state->dispatcher.trigger<event::internal::destroy>(child.second);
                state->scene().destroy(child.second);
            }
        }
        if(state->scene.valid(hierarchy.parent)){
            state->dispatcher.trigger<event::hierarchy::remove_child>(hierarchy.parent, event.id);
        }
    }
}

}