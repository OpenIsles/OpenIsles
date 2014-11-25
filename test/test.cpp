#include <gtest/gtest.h>

TEST(HelloWorld, Addition) {
    ASSERT_EQ(5, 2 + 3);
}

TEST(HelloWorld, Multiplication) {
    ASSERT_EQ(42, 2 * 3 * 7);
}