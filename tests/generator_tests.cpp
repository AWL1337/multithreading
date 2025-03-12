#include <gtest/gtest.h>
#include "../src/generator/generator.h"

TEST(GeneratorTest, GenerateIntInRange) {
    Generator gen;
    for (int i = 0; i < 100; ++i) {
        int value = gen.generateInt(1, 10);
        EXPECT_GE(value, 1);
        EXPECT_LE(value, 10);
    }
}

TEST(GeneratorTest, GenerateDoubleInRange) {
    Generator gen;
    for (int i = 0; i < 100; ++i) {
        double value = gen.generateDouble(0.0, 100.0);
        EXPECT_GE(value, 0.0);
        EXPECT_LE(value, 100.0);
    }
}

TEST(GeneratorTest, GenerateStringLength) {
    Generator gen;
    size_t length = 5;
    std::string result = gen.generateString(length);
    EXPECT_EQ(result.length(), length);
}

TEST(GeneratorTest, GenerateNameNotEmpty) {
    Generator gen;
    std::string name = gen.generateName();
    EXPECT_FALSE(name.empty());
}