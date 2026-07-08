#include "gtest/gtest.h"
#include "property.hpp"

using namespace cwr;

TEST(PropertyTest, AssignsDefaultValue) {
    property<int> intProperty;
    EXPECT_EQ(intProperty.get(), 0);
    property<char> charProperty;
    EXPECT_EQ(charProperty.get(), 0);
}

TEST(PropertyTest, WrapsValue) {
    property intProperty = 4;
    EXPECT_EQ(intProperty.get(), 4);
    const int i = intProperty;
    EXPECT_EQ(i, 4);
}

TEST(PropertyTest, SetsValue) {
    property<int> intProperty;
    intProperty.set(4);
    EXPECT_EQ(intProperty.get(), 4);
}

TEST(PropertyTest, PropertiesOfEqualValuesAreEqual) {
    property p1 = 5, p2 = 5;
    EXPECT_EQ(p1, p2);
}

TEST(PropertyTest, PropertiesOfDifferentValuesAreNotEqual) {
    property p1 = 4, p2 = 5;
    EXPECT_NE(p1, p2);
}

TEST(PropertyTest, PropertiesOfComparableValuesAreComparable) {
    property p1 = 4, p2 = 5;
    EXPECT_TRUE(p1 < p2);
}