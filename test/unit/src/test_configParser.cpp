#include "gtest/gtest.h"
#include "../../../include/configParser.hpp"

TEST(ConfigParser, read_version_number)
{
    ConfigParser conf("../../../conf/settings.conf");
    EXPECT_EQ(conf.getVersion(), "1.0");
}

