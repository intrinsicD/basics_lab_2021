//
// Created by alex on 15.06.21.
//

#include "bcg_jaw_component.h"

namespace bcg {

std::vector<std::string> jaw_type_names() {
    std::vector<std::string> names(static_cast<int>(JawType::__last__));
    names[static_cast<int>(JawType::maxilla)] = "maxilla";
    names[static_cast<int>(JawType::mandible)] = "mandible";
    names[static_cast<int>(JawType::undefined)] = "undefined";
    return names;
}

std::vector<std::string> jaw_age_type_names() {
    std::vector<std::string> names(static_cast<int>(JawType::__last__));
    names[static_cast<int>(JawAge::primary)] = "primary";
    names[static_cast<int>(JawAge::permament)] = "permament";
    names[static_cast<int>(JawAge::undefined)] = "undefined";
    return names;
}

bool jaw_component::has_teeth(int fdi_number) const {
    return teeth.find(fdi_number) != teeth.end();
}

std::vector<int> jaw_component::get_incisors() const {
    std::vector<int> subset;
    for(const auto &item : teeth){
        auto fid = item.first;
        auto number = fid % 10;
        if(number == 1 || number == 2){
            subset.push_back(fid);
        }
    }
    return subset;
}

std::vector<int> jaw_component::get_canines() const {
    std::vector<int> subset;
    for(const auto &item : teeth){
        auto fid = item.first;
        auto number = fid % 10;
        if(number == 3){
            subset.push_back(fid);
        }
    }
    return subset;
}

std::vector<int> jaw_component::get_premolars() const {
    std::vector<int> subset;
    if(age == JawAge::primary) return subset;
    for(const auto &item : teeth){
        auto fid = item.first;
        auto number = fid % 10;
        if(number == 4 || number == 5){
            subset.push_back(fid);
        }
    }
    return subset;
}

std::vector<int> jaw_component::get_molars() const {
    std::vector<int> subset;
    for(const auto &item : teeth){
        auto fid = item.first;
        auto number = fid % 10;
        if(age == JawAge::primary){
            if(number == 4 || number == 5){
                subset.push_back(fid);
            }
        }else if(age == JawAge::permament){
            if(number == 6 || number == 7 || number == 8){
                subset.push_back(fid);
            }
        }
    }
    return subset;
}

//----------------------------------------------------------------------------------------------------------------------

jaw_component jaw_factory::make_primary_maxilla() {
    jaw_component component;
    component.type = JawType::maxilla;
    component.age = JawAge::primary;
    component.max_number_teeth = 10;
    return component;
}

jaw_component jaw_factory::make_primary_mandible() {
    jaw_component component;
    component.type = JawType::mandible;
    component.age = JawAge::primary;
    component.max_number_teeth = 10;
    return component;
}

jaw_component jaw_factory::make_permanent_maxilla() {
    jaw_component component;
    component.type = JawType::maxilla;
    component.age = JawAge::permament;
    component.max_number_teeth = 16;
    return component;
}

jaw_component jaw_factory::make_permanent_mandible() {
    jaw_component component;
    component.type = JawType::mandible;
    component.age = JawAge::permament;
    component.max_number_teeth = 16;
    return component;
}

}