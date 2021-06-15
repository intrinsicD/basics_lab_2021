//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_TOOTH_COMPONENT_H
#define BCG_GRAPHICS_BCG_TOOTH_COMPONENT_H

#include <vector>
#include <string>

namespace bcg {

enum class ToothType {
    undefined,
    incisor,
    canine,
    premolar,
    molar,
    __last__
};

enum class ToothQuadrant {
    undefined,
    upper_right,
    upper_left,
    lower_right,
    lower_left,
    __last__
};

std::vector<std::string> tooth_type_names();

std::vector<std::string> tooth_quadrant_names();

struct tooth_component {
    int fdi_number = 0;
    int number = 0;
    ToothType type = ToothType::__last__;
    ToothQuadrant quadrant = ToothQuadrant::__last__;
};

struct permanent_tooth_factory {
    tooth_component make_11();

    tooth_component make_12();

    tooth_component make_13();

    tooth_component make_14();

    tooth_component make_15();

    tooth_component make_16();

    tooth_component make_17();

    tooth_component make_18();

    tooth_component make_21();

    tooth_component make_22();

    tooth_component make_23();

    tooth_component make_24();

    tooth_component make_25();

    tooth_component make_26();

    tooth_component make_27();

    tooth_component make_28();

    tooth_component make_31();

    tooth_component make_32();

    tooth_component make_33();

    tooth_component make_34();

    tooth_component make_35();

    tooth_component make_36();

    tooth_component make_37();

    tooth_component make_38();

    tooth_component make_41();

    tooth_component make_42();

    tooth_component make_43();

    tooth_component make_44();

    tooth_component make_45();

    tooth_component make_46();

    tooth_component make_47();

    tooth_component make_48();

    tooth_component make(int fdi_number);

    tooth_component make(ToothQuadrant quadrant, int number);
};

struct primary_tooth_factory {
    tooth_component make_51();

    tooth_component make_52();

    tooth_component make_53();

    tooth_component make_54();

    tooth_component make_55();

    tooth_component make_61();

    tooth_component make_62();

    tooth_component make_63();

    tooth_component make_64();

    tooth_component make_65();

    tooth_component make_71();

    tooth_component make_72();

    tooth_component make_73();

    tooth_component make_74();

    tooth_component make_75();

    tooth_component make_81();

    tooth_component make_82();

    tooth_component make_83();

    tooth_component make_84();

    tooth_component make_85();

    tooth_component make(int fdi_number);

    tooth_component make(ToothQuadrant quadrant, int number);
};

}

#endif //BCG_GRAPHICS_BCG_TOOTH_COMPONENT_H
