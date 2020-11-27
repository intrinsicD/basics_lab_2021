//
// Created by alex on 14.10.20.
//

#include <gtest/gtest.h>
#include "geometry/point_cloud/bcg_point_cloudio.h"

#ifdef _WIN32
static std::string test_data_path = "..\\tests\\data\\";
#else
static std::string test_data_path = "../tests/data/";
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
    auto colors = pc.vertices.get<VectorS<3>, 3>("v_color");
    auto intensities = pc.vertices.get<bcg_scalar_t, 1>("v_intensity");
    EXPECT_TRUE(colors);
    EXPECT_TRUE(intensities);
    EXPECT_LE((pc.positions[0]- VectorS<3>(1.185287, -1.583054, -1.004562)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[1]- VectorS<3>(1.186020, -1.585068, -1.004837)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[2]- VectorS<3>(1.184158, -1.582596, -1.000168)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[3]- VectorS<3>(1.184586, -1.583145, -1.002060)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[4]- VectorS<3>(1.183914, -1.581223, -1.002609)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[5]- VectorS<3>(1.182663, -1.579544, -1.000778)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[6]- VectorS<3>(1.188553, -1.589493, -1.003494)).squaredNorm(), scalar_eps);
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

    EXPECT_LE((pc.positions[0] - VectorS<3>(-72.501282, -748.387634, 23.167999)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[1] - VectorS<3>(-72.171761, -749.74646, 22.738632)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[2] - VectorS<3>(-71.282188, -748.033752, 23.631813)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[3] - VectorS<3>(-70.919182, -749.559692, 23.178967)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[4] - VectorS<3>(-69.76049, -750.247253, 23.340618)).squaredNorm(), scalar_eps);
}

TEST_F(TestPointCloudIoFixture, csv) {
    point_cloudio io(test_data_path + "test.csv", point_cloudio_flags());
    io.read(pc);
    EXPECT_EQ(pc.vertices.size(), 5);

    EXPECT_LE((pc.positions[0] - VectorS<3>(2.567080, 104.889321, -180.636716)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[1] - VectorS<3>(2.561510, 104.928386, -180.411959)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[2] - VectorS<3>(2.556899, 105.004835, -180.252099)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[3] - VectorS<3>(2.547219, 104.880118, -179.746425)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[4] - VectorS<3>(2.548666, 105.193961, -179.997706)).squaredNorm(), scalar_eps);
}


TEST_F(TestPointCloudIoFixture, tree_d) {
    point_cloudio io(test_data_path + "test.3d", point_cloudio_flags());
    io.read(pc);
    auto intensities = pc.vertices.get<bcg_scalar_t, 1>("v_intensity");
    EXPECT_EQ(pc.vertices.size(), 5);
    EXPECT_TRUE(intensities);

    EXPECT_LE((pc.positions[0] - VectorS<3>(-253.651, 48.707, 307.166)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[1] - VectorS<3>(-254.041, 48.213, 307.641)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[2] - VectorS<3>(-253.829, 47.6285, 307.387)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[3] - VectorS<3>(-253.887, 47.0711, 307.459)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[4] - VectorS<3>(-254.086, 46.5395, 307.703)).squaredNorm(), scalar_eps);

    EXPECT_FLOAT_EQ(intensities[0], 22.9);
    EXPECT_FLOAT_EQ(intensities[1], 22.9);
    EXPECT_FLOAT_EQ(intensities[2], 22.9);
    EXPECT_FLOAT_EQ(intensities[3], 22.9);
    EXPECT_FLOAT_EQ(intensities[4], 22.9);
}

TEST_F(TestPointCloudIoFixture, txt) {
    point_cloudio io(test_data_path + "test.txt", point_cloudio_flags());
    io.read(pc);
    auto colors = pc.vertices.get<VectorS<3>, 3>("v_color");
    auto reflectances = pc.vertices.get<bcg_scalar_t, 1>("v_reflectance");
    EXPECT_EQ(pc.vertices.size(), 5);
    EXPECT_TRUE(colors);
    EXPECT_TRUE(reflectances);

    EXPECT_LE((pc.positions[0] - VectorS<3>(-275.599, -32.170, -0.009)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[1] - VectorS<3>(-275.599, -32.168, -0.205)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[2] - VectorS<3>(-275.599, -32.170, -0.393)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[3] - VectorS<3>(-281.322, -32.838, -0.009)).squaredNorm(), scalar_eps);
    EXPECT_LE((pc.positions[4] - VectorS<3>(-281.321, -32.836, -0.209)).squaredNorm(), scalar_eps);

    EXPECT_LE((colors[0] - VectorS<3>(277.471, 90.002, 186.658)).squaredNorm(), scalar_eps);
    EXPECT_LE((colors[1] - VectorS<3>(277.471, 90.042, 186.658)).squaredNorm(), scalar_eps);
    EXPECT_LE((colors[2] - VectorS<3>(277.470, 90.081, 186.658)).squaredNorm(), scalar_eps);
    EXPECT_LE((colors[3] - VectorS<3>(283.232, 90.002, 186.658)).squaredNorm(), scalar_eps);
    EXPECT_LE((colors[4] - VectorS<3>(283.231, 90.042, 186.658)).squaredNorm(), scalar_eps);

    EXPECT_FLOAT_EQ(reflectances[0], -0.533);
    EXPECT_FLOAT_EQ(reflectances[1], -0.223);
    EXPECT_FLOAT_EQ(reflectances[2], 1.317);
    EXPECT_FLOAT_EQ(reflectances[3], -0.113);
    EXPECT_FLOAT_EQ(reflectances[4], 0.447);

}