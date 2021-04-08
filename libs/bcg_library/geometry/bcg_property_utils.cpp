//
// Created by alex on 01.04.21.
//

#include "bcg_property_utils.h"
#include "bcg_property_map_eigen.h"

namespace bcg{

property<bcg_scalar_t, 1> invert(property_container *container, property<bcg_scalar_t, 1> property){
    return scale(container, property, -1);
}

property<bcg_scalar_t, 1> abs(property_container *container, property<bcg_scalar_t, 1> property){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_abs");
    Map(result) = MapConst(property).cwiseAbs();
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> gaussian(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t sigma){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_gaussian");
    Map(result) = (-MapConst(property).array().square().array() / (2.0 * sigma * sigma)).exp();
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> shift(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t value){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_shifted");
    Map(result) = MapConst(property).array() + value;
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> scale(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t value){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_shifted");
    Map(result) = MapConst(property).array() * value;
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> sum(property_container *container, property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property1.name() + "+" + property2.name());
    Map(result) = MapConst(property1) + MapConst(property2);
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> prod(property_container *container, property<bcg_scalar_t, 1> property1, property<bcg_scalar_t, 1> property2){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property1.name() + "-" + property2.name());
    Map(result) = MapConst(property1).array() - MapConst(property2).array();
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> clampthreshold_min(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t min){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_threshold_min");
    Map(result) = (MapConst(property).array() < min).select(min, MapConst(property));
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> clampthreshold_max(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t max){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_threshold_max");
    Map(result) = (MapConst(property).array() > max).select(max, MapConst(property));
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> clamp(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t min, bcg_scalar_t max){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_clamped");
    Map(result) = (MapConst(property).array() < min).select(min, MapConst(property));
    Map(result) = (MapConst(result).array() > max).select(max, MapConst(result));
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> classify_min(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t min){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_classify_min");
    Map(result) = (MapConst(property).array() < min).select(VectorS<-1>::Ones(result.size()), bcg_scalar_t(0.0));
    result.set_dirty();
    return result;
}

property<bcg_scalar_t, 1> classify_max(property_container *container, property<bcg_scalar_t, 1> property, bcg_scalar_t max){
    auto result = container->get_or_add<bcg_scalar_t, 1>(property.name() + "_classify_max");
    Map(result) = (MapConst(property).array() > max).select(VectorS<-1>::Ones(result.size()), bcg_scalar_t(0.0));
    result.set_dirty();
    return result;
}

property<VectorS<3>, 3>  invert(property_container *container, property<VectorS<3>, 3> property){
    auto result = container->get_or_add<VectorS<3>, 3>(property.name() + "_inverted");
    Map(result) = -MapConst(property).array();
    result.set_dirty();
    return result;
}

property<VectorS<3>, 3>  abs(property_container *container, property<VectorS<3>, 3>  property){
    auto result = container->get_or_add<VectorS<3>, 3>(property.name() + "_abs");
    Map(result) = MapConst(property).cwiseAbs();
    result.set_dirty();
    return result;
}

property<VectorS<3>, 3>  shift(property_container *container, property<VectorS<3>, 3>  property, const VectorS<3> &value){
    auto result = container->get_or_add<VectorS<3>, 3>(property.name() + "_shifted");
    Map(result) = MapConst(property).rowwise()  + value.transpose();
    result.set_dirty();
    return result;
}

property<VectorS<3>, 3>  scale(property_container *container, property<VectorS<3>, 3>  property, const VectorS<3> &value){
    auto result = container->get_or_add<VectorS<3>, 3>(property.name() + "_shifted");
    Map(result) = MapConst(property).array().rowwise() * value.transpose().array();
    result.set_dirty();
    return result;
}

property<VectorS<3>, 3> sum(property_container *container, property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2){
    auto result = container->get_or_add<VectorS<3>, 3>(property1.name() + "+" + property2.name());
    Map(result) = MapConst(property1) + MapConst(property2);
    result.set_dirty();
    return result;
}

property<VectorS<3>, 3> prod(property_container *container, property<VectorS<3>, 3> property1, property<VectorS<3>, 3> property2){
    auto result = container->get_or_add<VectorS<3>, 3>(property1.name() + "*" + property2.name());
    Map(result) = MapConst(property1).array() * MapConst(property2).array();
    result.set_dirty();
    return result;
}

}