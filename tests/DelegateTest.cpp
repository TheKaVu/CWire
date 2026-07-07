#include "gtest/gtest.h"
#include "delegate.hpp"

using namespace cwr;

int times2(int a) {return a*2;}
int times3(int a) {return a*3;}

struct Struct {
    int multiplier = 2;
    int times2(int a) {return a*multiplier;}
    static int times2static(int a) {return a*2;}
};

TEST(DelegateTest, InvokesAssignedFunction) {
    const auto d = delegate<int, int>::of<&times2>();
    EXPECT_EQ(d.invoke(3), times2(3));
    EXPECT_EQ(d(3), d(3));
}

TEST(DelegateTest, StoresAnyFunctionMatchingSignature) {
    auto d = delegate<int, int>::of<&times2>();
    EXPECT_EQ(d(3), times2(3));
    d.set<&Struct::times2static>();
    EXPECT_EQ(d(3), Struct::times2static(3));
    Struct s;
    d.set<Struct, &Struct::times2>(s);
    EXPECT_EQ(d(3), s.times2(3));
}

TEST(DelegateTest, DelegatesOfSameFunctionAreEqual) {
    auto d1 = delegate<int, int>::of<&times2>();
    auto d2 = delegate<int, int>::of<&times2>();
    EXPECT_EQ(d1, d2);
    d1.set<&times3>();
    d2.set<&times3>();
    EXPECT_EQ(d1, d2);
    Struct s;
    d1.set<Struct, &Struct::times2>(s);
    d2.set<Struct, &Struct::times2>(s);
    EXPECT_EQ(d1, d2);
}

TEST(DelegateTest, DelegatesOfDifferentFunctionsAreNotEqual) {
    const auto d1 = delegate<int, int>::of<&times2>();
    const auto d2 = delegate<int, int>::of<&times3>();
    EXPECT_NE(d1, d2);
}

TEST(DelegateTest, DelegatesOfSameFunctionDifferentObjectsAreNotEqual) {
    Struct s1, s2;
    const auto d1 = delegate<int, int>::of<Struct, &Struct::times2>(s1);
    const auto d2 = delegate<int, int>::of<Struct, &Struct::times2>(s2);
    EXPECT_NE(d1, d2);
}

TEST(DelegateTest, CopyContructorAndAssignmentOperatorCopiesCorrectly) {
    auto d1 = delegate<int, int>::of<&times2>();
    auto d2 = d1;
    ASSERT_EQ(d1, d2);
    EXPECT_EQ(d1(3), d2(3));
    d1.set<&times3>();
    d2 = d1;
    ASSERT_EQ(d1, d2);
    EXPECT_EQ(d1(3), d2(3));
    Struct s;
    d1.set<Struct, &Struct::times2>(s);
    d2 = d1;
    ASSERT_EQ(d1, d2);
    EXPECT_EQ(d1(3), d2(3));
}

TEST(DelegateTest, MemberFunctionDelegateRefersToObject) {
    Struct s;
    const auto d1 = delegate<int, int>::of<Struct, &Struct::times2>(s);
    EXPECT_EQ(d1(3), s.times2(3));
    s.multiplier = 3;
    EXPECT_EQ(d1(3), s.times2(3));
}