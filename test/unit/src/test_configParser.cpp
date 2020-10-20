#include "gtest/gtest.h"
#include "../../../include/configParser.hpp"

TEST(ConfigParser, read_version_number)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.version, 2.0);
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
    EXPECT_EQ(conf.getSensorsName("1"), "Backbed");
    EXPECT_EQ(conf.getSensorsName("2"), "Shed");
}

TEST(ConfigParser, get_invalid_sensor_name)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    EXPECT_EQ(conf.getSensorsName("5"), "NotSet");
}

TEST(ConfigParser, get_sensor_reading_position)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    Position topleft = {2, 5};
    Position middleright= {3, 15};
    EXPECT_EQ(conf.getSensorReadingPosition("1", "1.0"), topleft);
    EXPECT_EQ(conf.getSensorReadingPosition("2", "2.1"), middleright);
}

TEST(ConfigParser, get_sensor_reading_position_non_existent)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    Position topleft_default = {2, 5};
    Position middleright = {3, 15};
    EXPECT_EQ(conf.getSensorReadingPosition("1", "1.4"), topleft_default);
    EXPECT_EQ(conf.getSensorReadingPosition("2", "2.1"), middleright);
}

TEST(ConfigParser, get_position_information)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.topleft_Name.row_start, 2);
    EXPECT_EQ(weatherStationSettings.topleft_Val.row_start, 2);
    EXPECT_EQ(weatherStationSettings.middleright_Val.col_start, 15);
}