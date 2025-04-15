// tests/add_test.cpp
#include "add.h"
#include <gtest/gtest.h>

TEST(Addition, HandlesPositives) {
  EXPECT_EQ(add(2, 3), 5);
}

TEST(Addition, HandlesNegatives) {
  EXPECT_EQ(add(-4, -6), -10);
}
