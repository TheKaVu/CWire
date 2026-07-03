#include "gtest/gtest.h"
#include "multidelegate.hpp"
#include "delegate.hpp"

using namespace cwr;

int doubler(int a) {
    return a * 2;
}

int tripler(int a) {
    return a * 3;
}

int quadler(int a) {
    return a * 4;
}

TEST(MultidelegateTest, InvokesMultipleFunctions) {
    multidelegate<int, int> m;
    m += delegate<int, int>::of<&doubler>();
    m += delegate<int, int>::of<&tripler>();
    m += delegate<int, int>::of<&quadler>();
    EXPECT_EQ(m.handlers().size(), 3);
    std::vector<int> r;
    m.invoke(3, r);
    ASSERT_EQ(r.size(), 3);
    EXPECT_EQ(r[0], doubler(3));
    EXPECT_EQ(r[1], tripler(3));
    EXPECT_EQ(r[2], quadler(3));
}

TEST(MultidelegateTest, ManagesFunctions) {
    multidelegate<int, int> m;
    m += delegate<int, int>::of<&doubler>();
    m += delegate<int, int>::of<&tripler>();
    m += delegate<int, int>::of<&quadler>();
    EXPECT_EQ(m.handlers().size(), 3);
    m.remove(delegate<int, int>::of<&tripler>());
    EXPECT_EQ(m.handlers().size(), 2);
    std::vector<int> r;
    m.invoke(3, r);
    ASSERT_EQ(r.size(), 2);
    EXPECT_EQ(r[0], doubler(3));
    EXPECT_EQ(r[1], quadler(3));
}
TEST(MultidelegateTest, MultidelegatesWithSameContentAreEqual) {
    multidelegate<int, int> m1, m2;
    m1 += delegate<int, int>::of<&doubler>();
    m1 += delegate<int, int>::of<&tripler>();
    m2 += delegate<int, int>::of<&doubler>();
    m2 += delegate<int, int>::of<&tripler>();
    EXPECT_EQ(m1.handlers().size(), 2);
    EXPECT_EQ(m2.handlers().size(), 2);
    EXPECT_EQ(m1, m2);
}

TEST(MultidelegateTest, MultidelegatesWithDifferentContentAreEqual) {
    multidelegate<int, int> m1, m2;
    m1 += delegate<int, int>::of<&doubler>();
    m1 += delegate<int, int>::of<&tripler>();
    m2 += delegate<int, int>::of<&tripler>();
    m2 += delegate<int, int>::of<&doubler>();
    EXPECT_NE(m1, m2);
}

TEST(MultidelegateTest, CopyContructorAndAssignmentOperatorCopiesCorrectly) {
    multidelegate<int, int> m1, m2;
    m1 += delegate<int, int>::of<&doubler>();
    m1 += delegate<int, int>::of<&tripler>();
    m2 = m1;
    EXPECT_EQ(m1, m2);
    const multidelegate<int, int> m3 = m1;
    EXPECT_EQ(m1, m3);
}