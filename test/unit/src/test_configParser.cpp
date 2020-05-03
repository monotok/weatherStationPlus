#include "gtest/gtest.h"
#include "../../../include/configParser.hpp"
#include "../../../include/settings.hpp"

TEST(ConfigParser, read_version_number)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.version, 1.0);
}

TEST(ConfigParser, get_database_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.db.host, "172.16.20.5");
}

TEST(ConfigParser, get_gpio_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.gpio.gpio3, "22");
}

TEST(ConfigParser, get_logging_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.logg.configFile, "conf/logging.conf");
}

TEST(ConfigParser, get_sensor2_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    EXPECT_EQ(conf.getSensorsName("s1"), "Shed");
}