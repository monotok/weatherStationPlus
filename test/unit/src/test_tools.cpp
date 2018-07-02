#include "gtest/gtest.h"
#include "../../../include/Utilities.hpp"

TEST(Utilities, return_positive_unix_time)
{
    long time_val = Utilities::getMicrotime();
    EXPECT_GT(time_val, 0);
}