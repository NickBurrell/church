#include<iostream>

#include "gtest/gtest.h"

TEST(test1, Test) {
    EXPECT_EQ(1, 2);
}

int main(int argc, char** argv) {
    std::cout << "test" << std::endl;
    RUN_ALL_TESTS();
}


