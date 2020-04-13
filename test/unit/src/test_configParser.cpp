#include "gtest/gtest.h"
#include "../../../include/configParser.hpp"
#include "../../../include/settings.hpp"

TEST(ConfigParser, read_version_number)
{
    settings weatherStationSettings;
    ConfigParser conf(&weatherStationSettings, "../../../conf/settings.conf");
    EXPECT_EQ(weatherStationSettings.version, 1.0);
}

