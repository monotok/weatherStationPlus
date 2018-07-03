#include "gtest/gtest.h"
#include "../../../include/Utilities.hpp"

TEST(Utilities, return_positive_unix_time)
{
    uint64_t time_val = Utilities::getMicrotime();
    EXPECT_GT(time_val, 0);
}