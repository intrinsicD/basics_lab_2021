//
// Created by alex on 14.10.20.
//

#include <gtest/gtest.h>
#include "bcg_library/geometry/bcg_point_cloudio.h"

#ifdef _WIN32
static std::string test_data_path = "..\\..\\tests\\data\\";
#else
static std::string test_data_path = "../../tests/data/";
#endif

using namespace bcg;

class TestPointCloudIoFixture : public ::testing::Test {
public:
    TestPointCloudIoFixture() = default;

    point_cloud pc;
};

TEST_F(TestPointCloudIoFixture, pts) {
    point_cloudio io(test_data_path + "test.pts", point_cloudio_flags());
    io.read(pc);
    EXPECT_EQ(pc.vertices.size(), 7);
    auto colors = pc.vertices.get<vec3f, 3>("color");
    auto intensities = pc.vertices.get<float, 1>("intensity");
    EXPECT_TRUE(colors);
    EXPECT_TRUE(intensities);
    EXPECT_EQ(pc.positions[0], vec3f(1.185287, -1.583054, -1.004562));
    EXPECT_EQ(pc.positions[1], vec3f(1.186020, -1.585068, -1.004837));
    EXPECT_EQ(pc.positions[2], vec3f(1.184158, -1.582596, -1.000168));
    EXPECT_EQ(pc.positions[3], vec3f(1.184586, -1.583145, -1.002060));
    EXPECT_EQ(pc.positions[4], vec3f(1.183914, -1.581223, -1.002609));
    EXPECT_EQ(pc.positions[5], vec3f(1.182663, -1.579544, -1.000778));
    EXPECT_EQ(pc.positions[6], vec3f(1.188553, -1.589493, -1.003494));
    EXPECT_EQ(intensities[0], -1407);
    EXPECT_EQ(intensities[1], -1458);
    EXPECT_EQ(intensities[2], -1393);
    EXPECT_EQ(intensities[3], -1429);
    EXPECT_EQ(intensities[4], -1401);
    EXPECT_EQ(intensities[5], -1387);
    EXPECT_EQ(intensities[6], -1455);
}

TEST_F(TestPointCloudIoFixture, xyz) {
    point_cloudio io(test_data_path + "test.xyz", point_cloudio_flags());
    io.read(pc);
    EXPECT_EQ(pc.vertices.size(), 5);

    EXPECT_EQ(pc.positions[0], vec3f(-72.501282, -748.387634, 23.167999));
    EXPECT_EQ(pc.positions[1], vec3f(-72.171761, -749.74646, 22.738632));
    EXPECT_EQ(pc.positions[2], vec3f(-71.282188, -748.033752, 23.631813));
    EXPECT_EQ(pc.positions[3], vec3f(-70.919182, -749.559692, 23.178967));
    EXPECT_EQ(pc.positions[4], vec3f(-69.76049, -750.247253, 23.340618));
}

TEST_F(TestPointCloudIoFixture, csv) {
    point_cloudio io(test_data_path + "test.csv", point_cloudio_flags());
    io.read(pc);
    EXPECT_EQ(pc.vertices.size(), 5);

    EXPECT_EQ(pc.positions[0], vec3f(2.567080,104.889321,-180.636716));
    EXPECT_EQ(pc.positions[1], vec3f(2.561510,104.928386,-180.411959));
    EXPECT_EQ(pc.positions[2], vec3f(2.556899,105.004835,-180.252099));
    EXPECT_EQ(pc.positions[3], vec3f(2.547219,104.880118,-179.746425));
    EXPECT_EQ(pc.positions[4], vec3f(2.548666,105.193961,-179.997706));
}


TEST_F(TestPointCloudIoFixture, tree_d) {
    point_cloudio io(test_data_path + "test.3d", point_cloudio_flags());
    io.read(pc);
    auto intensities = pc.vertices.get<float, 1>("intensity");
    EXPECT_EQ(pc.vertices.size(), 5);
    EXPECT_TRUE(intensities);

    EXPECT_EQ(pc.positions[0], vec3f(-253.651, 48.707, 307.166));
    EXPECT_EQ(pc.positions[1], vec3f(-254.041, 48.213, 307.641));
    EXPECT_EQ(pc.positions[2], vec3f(-253.829, 47.6285, 307.387));
    EXPECT_EQ(pc.positions[3], vec3f(-253.887, 47.0711, 307.459));
    EXPECT_EQ(pc.positions[4], vec3f(-254.086, 46.5395, 307.703));

    EXPECT_FLOAT_EQ(intensities[0], 22.9);
    EXPECT_FLOAT_EQ(intensities[1], 22.9);
    EXPECT_FLOAT_EQ(intensities[2], 22.9);
    EXPECT_FLOAT_EQ(intensities[3], 22.9);
    EXPECT_FLOAT_EQ(intensities[4], 22.9);
}

TEST_F(TestPointCloudIoFixture, txt) {
    point_cloudio io(test_data_path + "test.txt", point_cloudio_flags());
    io.read(pc);
    auto colors = pc.vertices.get<vec3f, 3>("color");
    auto reflectances = pc.vertices.get<float, 1>("reflectance");
    EXPECT_EQ(pc.vertices.size(), 5);
    EXPECT_TRUE(colors);
    EXPECT_TRUE(reflectances);

    EXPECT_EQ(pc.positions[0], vec3f(-275.599, -32.170, -0.009));
    EXPECT_EQ(pc.positions[1], vec3f(-275.599, -32.168, -0.205));
    EXPECT_EQ(pc.positions[2], vec3f(-275.599, -32.170, -0.393));
    EXPECT_EQ(pc.positions[3], vec3f(-281.322, -32.838, -0.009));
    EXPECT_EQ(pc.positions[4], vec3f(-281.321, -32.836, -0.209));

    EXPECT_EQ(colors[0], vec3f(277.471, 90.002, 186.658));
    EXPECT_EQ(colors[1], vec3f(277.471, 90.042, 186.658));
    EXPECT_EQ(colors[2], vec3f(277.470, 90.081, 186.658));
    EXPECT_EQ(colors[3], vec3f(283.232, 90.002, 186.658));
    EXPECT_EQ(colors[4], vec3f(283.231, 90.042, 186.658));

    EXPECT_FLOAT_EQ(reflectances[0], -0.533);
    EXPECT_FLOAT_EQ(reflectances[1], -0.223);
    EXPECT_FLOAT_EQ(reflectances[2], 1.317);
    EXPECT_FLOAT_EQ(reflectances[3], -0.113);
    EXPECT_FLOAT_EQ(reflectances[4], 0.447);

}