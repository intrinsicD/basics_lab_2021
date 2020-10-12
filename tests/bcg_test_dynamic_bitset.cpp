//
// Created by Alex on 07.05.20.
//

#include <gtest/gtest.h>

#include "bcg_library/utils/bcg_dynamic_bitset.h"

using namespace bcg;

TEST(TestSuiteDynamicBitset, constructor){
    DynamicBitset bitset;
    EXPECT_EQ(bitset.num_set_bits(), 0);
    EXPECT_EQ(bitset.size(), 0);
    EXPECT_EQ(bitset.capacity(), 0);

    DynamicBitset bitset1(1);
    EXPECT_EQ(bitset1.num_set_bits(), 1);
    EXPECT_EQ(bitset1.size(), 1);
    EXPECT_EQ(bitset1.capacity(), bitset1.item_size());

    DynamicBitset bitset2("1101101011");
    EXPECT_EQ(bitset2.num_set_bits(), 7);
    EXPECT_EQ(bitset2.size(), 2);
    EXPECT_EQ(bitset2.capacity(), 2 * 8);
    EXPECT_EQ(bitset2.to_string(), "1101101011");

    DynamicBitset bitset3("AABAABABAA", 'A');
    EXPECT_EQ(bitset3.num_set_bits(), 7);
    EXPECT_EQ(bitset3.size(), 2);
    EXPECT_EQ(bitset3.capacity(), 2 * 8);
    EXPECT_EQ(bitset3.to_string(), "1101101011");
}

TEST(TestSuiteDynamicBitset, equals){
    DynamicBitset bitset2("1101101011");
    DynamicBitset bitset3("AABAABABAABB", 'A');
    EXPECT_TRUE(bitset2 == bitset3);
    EXPECT_FALSE(bitset2 != bitset3);
    DynamicBitset bitset4("AABAABABAAA", 'A');
    EXPECT_FALSE(bitset2 == bitset4);
    EXPECT_TRUE(bitset2 != bitset4);
}

TEST(TestSuiteDynamicBitset, all){
    DynamicBitset bitset("1101101011");
    EXPECT_FALSE(bitset.all());
}

TEST(TestSuiteDynamicBitset, all_of){
    DynamicBitset bitset("1101101011");
    std::vector<size_t> indices = {0, 1, 3, 4, 6, 8, 9};
    EXPECT_TRUE(bitset.all_of(indices));
    indices.push_back(2);
    EXPECT_FALSE(bitset.all_of(indices));
}

TEST(TestSuiteDynamicBitset, any){
    DynamicBitset bitset("1101101011");
    EXPECT_TRUE(bitset.any());
}

TEST(TestSuiteDynamicBitset, none){
    DynamicBitset bitset("00000000");
    EXPECT_TRUE(bitset.none());
}

TEST(TestSuiteDynamicBitset, count){
    DynamicBitset bitset("00000000");
    EXPECT_EQ(bitset.count(), 0);
    EXPECT_EQ(bitset.count(), bitset.num_set_bits());
}

TEST(TestSuiteDynamicBitset, resize){
    DynamicBitset bitset(0);
    bitset.resize(10);
    EXPECT_EQ(bitset.size(), 2);
    EXPECT_EQ(bitset.capacity(), 16);
}

TEST(TestSuiteDynamicBitset, set_test_reset){
    DynamicBitset bitset(0);
    bitset.resize(10);
    EXPECT_EQ(bitset.size(), 2);
    EXPECT_EQ(bitset.capacity(), 16);

    bitset.set(0);
    EXPECT_TRUE(bitset.test(0));

    bitset.set(1);
    EXPECT_TRUE(bitset.test(1));

    bitset.set(11);
    EXPECT_TRUE(bitset.test(11));
    EXPECT_EQ(bitset.size(), 2);
    EXPECT_EQ(bitset.capacity(), 16);

    bitset.set(17);
    EXPECT_TRUE(bitset.test(17));
    EXPECT_EQ(bitset.size(), 3);
    EXPECT_EQ(bitset.capacity(), 24);

    bitset.reset(11);
    EXPECT_EQ(bitset.num_set_bits(), 3);
    EXPECT_FALSE(bitset.test(11));
    EXPECT_EQ(bitset.size(), 3);
    EXPECT_EQ(bitset.capacity(), 24);
}

TEST(TestSuiteDynamicBitset, reference_t){
    DynamicBitset bitset(0);

    bitset[0] = 1;
    EXPECT_EQ(bitset[0], 1);
    EXPECT_TRUE(bitset.test(0));
    bitset[0] = false;
    EXPECT_FALSE(bitset.test(0));

    bitset[12340] = 1;
    EXPECT_EQ(bitset[12340], 1);
    EXPECT_TRUE(bitset.test(12340));
    bitset[12340] = false;
    EXPECT_FALSE(bitset.test(12340));
}

TEST(TestSuiteDynamicBitset, bitwise_and){
    DynamicBitset smaller("110");
    DynamicBitset larger("11001011");
    EXPECT_EQ((smaller & larger).to_string(), "11");
}

TEST(TestSuiteDynamicBitset, bitwise_or){
    DynamicBitset smaller("110");
    DynamicBitset larger("10001011");
    EXPECT_EQ((smaller | larger).to_string(), "11001011");
}

TEST(TestSuiteDynamicBitset, bitwise_xor){
    DynamicBitset smaller("110");
    DynamicBitset larger("10001011");
    EXPECT_EQ((smaller ^ larger).to_string(), "01001011");
}

TEST(TestSuiteDynamicBitset, bitwise_flip){
    DynamicBitset bitset("10001011");
    EXPECT_EQ((~bitset).to_string(), "011101");
    EXPECT_EQ(bitset.flip().to_string(), "011101");
}

TEST(TestSuiteDynamicBitset, flip){
    DynamicBitset bitset("10001011");
    bitset.flip(0);
    EXPECT_EQ(bitset.to_string(), "00001011");
    bitset.flip(1);
    EXPECT_EQ(bitset.to_string(), "01001011");
}

TEST(TestSuiteDynamicBitset, l_shift){
    DynamicBitset bitset("10001011");
    EXPECT_EQ((bitset << 1).to_string(), "0001011");
    EXPECT_EQ((bitset << 4).to_string(), "1011");
}

TEST(TestSuiteDynamicBitset, r_shift){
    DynamicBitset bitset("10001011");
    EXPECT_EQ((bitset >> 1).to_string(), "01000101");
    EXPECT_EQ((bitset >> 4).to_string(), "00001");
}