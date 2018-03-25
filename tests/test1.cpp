#include<iostream>

#include "gtest/gtest.h"

TEST(test1, Test) {
    EXPECT_EQ(1, 2);
}

int main() {
    std::cout << "test" << std::endl;
    std::cout << RUN_ALL_TESTS() << std::endl;
}
