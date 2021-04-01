//
// Created by alex on 01.04.21.
//

#include "bcg_property_utils.h"
#include "bcg_property_map_eigen.h"

namespace bcg{

property<bcg_scalar_t, 1> invert(property<bcg_scalar_t, 1> property){
    return scale(property, -1);
}

property<bcg_scalar_t, 1> abs(property<bcg_scalar_t, 1> property){
    Map(property) = MapConst(property).cwiseAbs();
    property.set_dirty();
    return property;
}

property<bcg_scalar_t, 1> gaussian(property<bcg_scalar_t, 1> property, bcg_scalar_t sigma){
    Map(property) = (-MapConst(property).array().square().array() / (2.0 * sigma * sigma)).exp();
    property.set_dirty();
    return property;
}

property<bcg_scalar_t, 1> shift(property<bcg_scalar_t, 1> property, bcg_scalar_t value){
    Map(property).array() += value;
    property.set_dirty();
    return property;
}

property<bcg_scalar_t, 1> scale(property<bcg_scalar_t, 1> property, bcg_scalar_t value){
    Map(property) *= value;
    property.set_dirty();
    return property;
}

property<bcg_scalar_t, 1> sum(property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2){
    Map(property1) += MapConst(property2);
    property1.set_dirty();
    return property1;
}

property<bcg_scalar_t, 1> prod(property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2){
    Map(property1).array() *= MapConst(property2).array();
    property1.set_dirty();
    return property1;
}

property<VectorS<3>, 3>  invert(property<VectorS<3>, 3> property){
    Map(property).array() *= -1;
    property.set_dirty();
    return property;
}

property<VectorS<3>, 3>  abs(property<VectorS<3>, 3>  property){
    Map(property) = MapConst(property).cwiseAbs();
    property.set_dirty();
    return property;
}

property<VectorS<3>, 3>  shift(property<VectorS<3>, 3>  property, const VectorS<3> &value){
    Map(property).rowwise() += value.transpose();
    property.set_dirty();
    return property;
}

property<VectorS<3>, 3>  scale(property<VectorS<3>, 3>  property, const VectorS<3> &value){
    Map(property) = MapConst(property).array().rowwise() * value.transpose().array();
    property.set_dirty();
    return property;
}

property<VectorS<3>, 3> sum(property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2){
    Map(property1) += MapConst(property2);
    property1.set_dirty();
    return property1;
}

property<VectorS<3>, 3> prod(property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2){
    Map(property1).array() *= MapConst(property2).array();
    property1.set_dirty();
    return property1;
}

}