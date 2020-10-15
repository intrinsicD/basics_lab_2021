//
// Created by alex on 14.10.20.
//

#include <gtest/gtest.h>
#include <numeric>

#include "bcg_library/geometry/bcg_property.h"
#include "bcg_library/geometry/bcg_property_map_eigen.h"
#include "bcg_library/math/bcg_linalg.h"
#include "bcg_library/exts/eigen/Eigen/Core"

using namespace bcg;

TEST(TestSuiteProperty, all) {
    property_container container;
    EXPECT_TRUE(container.empty());
    EXPECT_FALSE(container.has("scalarfield"));
    EXPECT_FALSE(container.has("vectorfield"));

    property<bcg_scalar_t, 1> scalarfield;
    property<VectorS<3>, 3> vectorfield;

    EXPECT_FALSE(scalarfield);
    EXPECT_FALSE(vectorfield);

    scalarfield = container.add<bcg_scalar_t, 1>("scalarfield");

    EXPECT_TRUE(scalarfield);
    EXPECT_FALSE(vectorfield);

    vectorfield = container.add<VectorS<3>, 3>("vectorfield", zero3s);

    EXPECT_TRUE(scalarfield);
    EXPECT_TRUE(vectorfield);

    container.resize(10);

    container.push_back();
    vectorfield.back() = VectorS<3>(1, 2, 3);
    scalarfield.back() = 1;
    container.resize(container.size());

    container.push_back();

    EXPECT_EQ(vectorfield.size(), 12);
    EXPECT_EQ(scalarfield.size(), 12);

    EXPECT_EQ(vectorfield[vectorfield.size() - 2], VectorS<3>(1, 2, 3));

    property<bool, 1> conditions = container.add<bool, 1>("conditions");
    conditions[0] = true;

    auto test = container.get_or_add<VectorS<3>, 3>("normals", one3s);
    EXPECT_TRUE(container.has("normals"));
    container.remove(test);
    EXPECT_FALSE(test);
    EXPECT_FALSE(container.has("normals"));

    EXPECT_NE(vectorfield.back(), VectorS<3>(1, 2, 3));
    container.swap(vectorfield.size() - 2, vectorfield.size() - 1);
    EXPECT_EQ(vectorfield.back(), VectorS<3>(1, 2, 3));
}

TEST(TestSuiteProperty, property_container) {
    property_container container;
    EXPECT_TRUE(container.empty());
    EXPECT_FALSE(container.has("scalarfield"));
    EXPECT_FALSE(container.has("vectorfield"));

    property<bcg_scalar_t, 1> scalarfield;
    property<VectorS<3>, 3> vectorfield;

    EXPECT_FALSE(scalarfield);
    EXPECT_FALSE(vectorfield);

    scalarfield = container.add<bcg_scalar_t, 1>("scalarfield");

    EXPECT_TRUE(scalarfield);
    EXPECT_FALSE(vectorfield);

    vectorfield = container.add<VectorS<3>, 3>("vectorfield",zero3s);

    EXPECT_TRUE(scalarfield);
    EXPECT_TRUE(vectorfield);

    auto test1 = container.get_or_add<bcg_scalar_t, 1>("test1");
    EXPECT_TRUE(test1);
    EXPECT_TRUE(container.has("test1"));
    auto test2 = container.get_or_add<VectorS<4>, 4>("test2", one4s);
    EXPECT_TRUE(test2);
    EXPECT_TRUE(container.has("test2"));

    container.remove(test1);
    EXPECT_FALSE(test1);
    EXPECT_FALSE(container.has("test1"));

    container.remove(test2);
    EXPECT_FALSE(test2);
    EXPECT_FALSE(container.has("test2"));

    container.resize(12);
    Map(scalarfield).col(0).setLinSpaced(12, 0, 11);
    EXPECT_EQ(scalarfield[0], 0);
    EXPECT_EQ(scalarfield.back(), 11);

    container.swap(0, 11);
    EXPECT_EQ(scalarfield[0], 11);
    EXPECT_EQ(scalarfield.back(), 0);

    for (size_t i = 0; i < 100; ++i) {
        container.push_back();
    }
    EXPECT_EQ(scalarfield.capacity(), 192);
    container.free_unused_memory();
    EXPECT_EQ(scalarfield.capacity(), 112);
    container.reserve(192);
    EXPECT_EQ(scalarfield.capacity(), 192);

    EXPECT_FALSE(container.empty());
    EXPECT_EQ(container.size(), 112);

    EXPECT_EQ(container.num_properties(), 2);

    container.clear();
    EXPECT_EQ(scalarfield.size(), 0);
}

TEST(TestSuiteProperty, Property) {
    property_container container;
    auto scalarfield = container.get_or_add<bcg_scalar_t, 1>("scalarfield");
    auto vectorfield = container.get_or_add<VectorS<3>, 3>("vectorfield", one3s);

    EXPECT_FALSE(scalarfield.is_dirty());
    EXPECT_FALSE(vectorfield.is_dirty());
    container.resize(10);
    EXPECT_TRUE(scalarfield.is_dirty());
    EXPECT_TRUE(vectorfield.is_dirty());

    scalarfield.set_clean();
    vectorfield.set_clean();
    EXPECT_FALSE(scalarfield.is_dirty());
    EXPECT_FALSE(vectorfield.is_dirty());

    property<bool, 1> conditions;
    EXPECT_FALSE(conditions);
    conditions = container.add<bool, 1>("conditions", false);

    EXPECT_TRUE(conditions.void_ptr() == nullptr);
    EXPECT_EQ(scalarfield.dims(), 1);
    EXPECT_EQ(vectorfield.dims(), 3);
    EXPECT_EQ(conditions.dims(), 1);

    EXPECT_EQ(scalarfield.size_bytes(), 1 * 10 * sizeof(bcg_scalar_t));
    EXPECT_EQ(vectorfield.size_bytes(), 3 * 10 * sizeof(bcg_scalar_t));
    EXPECT_EQ(conditions.size_bytes(), 1 * 10 * sizeof(bool));

    Map(scalarfield) = VectorS<-1>::Ones(10);

    EXPECT_EQ(Map(scalarfield).mean(), 1);
    EXPECT_EQ((Map(scalarfield).array() - Map(scalarfield).mean()).square().sum() / scalarfield.size(), 0);

    bcg_scalar_t eps = 1e-6f;
    EXPECT_LE(Map(vectorfield).colwise().mean().mean(), 1);
    EXPECT_LE((Map(vectorfield).rowwise() - Map(vectorfield).colwise().mean()).sum() / vectorfield.size(), eps);

    property_container container2;
    auto scalarfield2 = container.add(scalarfield);

    EXPECT_EQ(scalarfield2, scalarfield);
    for (size_t i = 0; i < scalarfield.size(); ++i) {
        EXPECT_FLOAT_EQ(scalarfield[i], scalarfield2[i]);
    }

    std::vector<VectorS<3>> data(100, one3s);
    vectorfield.set(data);
    EXPECT_EQ(vectorfield.size(), 100);
    vectorfield.append(data);
    EXPECT_EQ(vectorfield.size(), 200);
}