#include <iostream>
#include <gtest/gtest.h>
using namespace std;

TEST(MCSGtest, HelloWorldTest) {
    // Your test code here
    EXPECT_EQ(1, 1);
    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}