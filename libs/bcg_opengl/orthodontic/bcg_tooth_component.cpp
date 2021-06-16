//
// Created by alex on 15.06.21.
//

#include "bcg_tooth_component.h"
#include <cassert>

namespace bcg {

std::vector<std::string> tooth_type_names() {
    std::vector<std::string> names(static_cast<int>(ToothType::__last__));
    names[static_cast<int>(ToothType::incisor)] = "incisor";
    names[static_cast<int>(ToothType::canine)] = "canine";
    names[static_cast<int>(ToothType::premolar)] = "premolar";
    names[static_cast<int>(ToothType::molar)] = "molar";
    names[static_cast<int>(ToothType::undefined)] = "undefined";
    return names;
}

std::vector<std::string> tooth_quadrant_names() {
    std::vector<std::string> names(static_cast<int>(ToothQuadrant::__last__));
    names[static_cast<int>(ToothQuadrant::upper_right)] = "upper_right";
    names[static_cast<int>(ToothQuadrant::upper_left)] = "upper_left";
    names[static_cast<int>(ToothQuadrant::lower_right)] = "lower_right";
    names[static_cast<int>(ToothQuadrant::lower_left)] = "lower_left";
    names[static_cast<int>(ToothQuadrant::undefined)] = "undefined";
    return names;
}

//----------------------------------------------------------------------------------------------------------------------

tooth_component permanent_tooth_factory::make_11() {
    return make(11);
}

tooth_component permanent_tooth_factory::make_12() {
    return make(12);
}

tooth_component permanent_tooth_factory::make_13() {
    return make(13);
}

tooth_component permanent_tooth_factory::make_14() {
    return make(14);
}

tooth_component permanent_tooth_factory::make_15() {
    return make(15);
}

tooth_component permanent_tooth_factory::make_16() {
    return make(16);
}

tooth_component permanent_tooth_factory::make_17() {
    return make(17);
}

tooth_component permanent_tooth_factory::make_18() {
    return make(18);
}

tooth_component permanent_tooth_factory::make_21() {
    return make(21);
}

tooth_component permanent_tooth_factory::make_22() {
    return make(22);
}

tooth_component permanent_tooth_factory::make_23() {
    return make(23);
}

tooth_component permanent_tooth_factory::make_24() {
    return make(24);
}

tooth_component permanent_tooth_factory::make_25() {
    return make(25);
}

tooth_component permanent_tooth_factory::make_26() {
    return make(26);
}

tooth_component permanent_tooth_factory::make_27() {
    return make(27);
}

tooth_component permanent_tooth_factory::make_28() {
    return make(28);
}

tooth_component permanent_tooth_factory::make_31() {
    return make(31);
}

tooth_component permanent_tooth_factory::make_32() {
    return make(32);
}

tooth_component permanent_tooth_factory::make_33() {
    return make(33);
}

tooth_component permanent_tooth_factory::make_34() {
    return make(34);
}

tooth_component permanent_tooth_factory::make_35() {
    return make(35);
}

tooth_component permanent_tooth_factory::make_36() {
    return make(36);
}

tooth_component permanent_tooth_factory::make_37() {
    return make(37);
}

tooth_component permanent_tooth_factory::make_38() {
    return make(38);
}

tooth_component permanent_tooth_factory::make_41() {
    return make(41);
}

tooth_component permanent_tooth_factory::make_42() {
    return make(42);
}

tooth_component permanent_tooth_factory::make_43() {
    return make(43);
}

tooth_component permanent_tooth_factory::make_44() {
    return make(44);
}

tooth_component permanent_tooth_factory::make_45() {
    return make(45);
}

tooth_component permanent_tooth_factory::make_46() {
    return make(46);
}

tooth_component permanent_tooth_factory::make_47() {
    return make(47);
}

tooth_component permanent_tooth_factory::make_48() {
    return make(48);
}

tooth_component permanent_tooth_factory::make(int fdi_number) {
    int number = fdi_number % 10;
    auto quadrant = static_cast<ToothQuadrant>((fdi_number - number) / 10);
    tooth_component component = make(quadrant, number);
    assert(component.fdi_number == fdi_number);
    return component;
}

tooth_component permanent_tooth_factory::make(ToothQuadrant quadrant, int number) {
    tooth_component component;
    component.quadrant = quadrant;
    component.number = number;
    component.fdi_number = static_cast<int>(quadrant) * 10 + number;
    if (component.number == 1 || component.number == 2) {
        component.type = ToothType::incisor;
    } else if (component.number == 3) {
        component.type = ToothType::canine;
    } else if (component.number == 4 || component.number == 5) {
        component.type = ToothType::premolar;
    } else if (component.number == 6 || component.number == 7 || component.number == 8) {
        component.type = ToothType::molar;
    } else {
        component.type = ToothType::undefined;
    }

    assert(number > 0);
    assert(number < 9);
    assert(static_cast<int>(quadrant) < static_cast<int>(ToothQuadrant::__last__));
    assert(static_cast<int>(component.type) < static_cast<int>(ToothType::__last__));
    assert(component.fdi_number > 0);
    assert(component.fdi_number <= 48);
    return component;
}

//----------------------------------------------------------------------------------------------------------------------
tooth_component primary_tooth_factory::make_51() {
    return make(51);
}

tooth_component primary_tooth_factory::make_52() {
    return make(52);
}

tooth_component primary_tooth_factory::make_53() {
    return make(53);
}

tooth_component primary_tooth_factory::make_54() {
    return make(54);
}

tooth_component primary_tooth_factory::make_55() {
    return make(55);
}

tooth_component primary_tooth_factory::make_61() {
    return make(61);
}

tooth_component primary_tooth_factory::make_62() {
    return make(62);
}

tooth_component primary_tooth_factory::make_63() {
    return make(63);
}

tooth_component primary_tooth_factory::make_64() {
    return make(64);
}

tooth_component primary_tooth_factory::make_65() {
    return make(65);
}

tooth_component primary_tooth_factory::make_71() {
    return make(71);
}

tooth_component primary_tooth_factory::make_72() {
    return make(72);
}

tooth_component primary_tooth_factory::make_73() {
    return make(73);
}

tooth_component primary_tooth_factory::make_74() {
    return make(74);
}

tooth_component primary_tooth_factory::make_75() {
    return make(75);
}

tooth_component primary_tooth_factory::make_81() {
    return make(81);
}

tooth_component primary_tooth_factory::make_82() {
    return make(82);
}

tooth_component primary_tooth_factory::make_83() {
    return make(83);
}

tooth_component primary_tooth_factory::make_84() {
    return make(84);
}

tooth_component primary_tooth_factory::make_85() {
    return make(85);
}

tooth_component primary_tooth_factory::make(int fdi_number) {
    assert(fdi_number <= 85);
    int number = fdi_number % 10;
    assert(number != 0);
    int q = (fdi_number - number) / 10;
    ToothQuadrant quadrant;
    switch (q) {
        case 5: {
            quadrant = ToothQuadrant::upper_right;
            break;
        }
        case 6: {
            quadrant = ToothQuadrant::upper_left;
            break;
        }
        case 7: {
            quadrant = ToothQuadrant::lower_left;
            break;
        }
        case 8: {
            quadrant = ToothQuadrant::lower_right;
            break;
        }
    }
    assert(static_cast<int>(quadrant) < static_cast<int>(ToothQuadrant::__last__));
    tooth_component component = make(quadrant, number);
    assert(component.fdi_number == fdi_number);
    return component;
}

tooth_component primary_tooth_factory::make(ToothQuadrant quadrant, int number) {
    tooth_component component;
    component.quadrant = quadrant;
    component.number = number;
    int q;
    switch (quadrant) {
        case ToothQuadrant::upper_right: {
            q = 5;
            break;
        }
        case ToothQuadrant::upper_left: {
            q = 6;
            break;
        }
        case ToothQuadrant::lower_left: {
            q = 7;
            break;
        }
        case ToothQuadrant::lower_right: {
            q = 8;
            break;
        }

    }
    component.fdi_number = q * 10 + number;
    if (component.number == 1 || component.number == 2) {
        component.type = ToothType::incisor;
    } else if (component.number == 3) {
        component.type = ToothType::canine;
    } else if (component.number == 4 || component.number == 5) {
        component.type = ToothType::molar;
    } else {
        component.type = ToothType::undefined;
    }

    assert(number > 0);
    assert(number < 6);
    assert(static_cast<int>(quadrant) < static_cast<int>(ToothQuadrant::__last__));
    assert(static_cast<int>(component.type) < static_cast<int>(ToothType::__last__));
    assert(component.fdi_number > 0);
    assert(component.fdi_number <= 85);
    return component;
}

}