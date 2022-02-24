#include "gtest/gtest.h"
#include "../../../include/configParser.hpp"

TEST(ConfigParser, read_version_number)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.version, 3.0);
}

TEST(ConfigParser, get_database_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    EXPECT_FALSE(weatherStationSettings.db.enable);
    EXPECT_EQ(weatherStationSettings.db.host, "172.16.20.5");
    EXPECT_EQ(weatherStationSettings.db.port, 5432);
    EXPECT_EQ(weatherStationSettings.db.database, "test_weatherdb");
    EXPECT_EQ(weatherStationSettings.db.user, "test_weather");
    EXPECT_EQ(weatherStationSettings.db.password, "testtest");
}

TEST(ConfigParser, get_gpio_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.gpio.gpio3, "22");
}

TEST(ConfigParser, get_logging_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.logg.configFile, "conf/logging.conf");
}

TEST(ConfigParser, get_sensor2_details)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    EXPECT_EQ(conf.getSensorsName("1"), "Local");
    EXPECT_EQ(conf.getSensorsName("2"), "Shed");
}

TEST(ConfigParser, get_invalid_sensor_name)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    EXPECT_TRUE(conf.getSensorsName("5").empty());
}

TEST(ConfigParser, get_sensor_reading_name)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    EXPECT_EQ(conf.getSensorReadingName("1", "1.0"), "Temp");
    EXPECT_EQ(conf.getSensorReadingName("1", "1.1"), "Hum");
}

TEST(ConfigParser, get_invalid_sensor_reading_name)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    EXPECT_EQ(conf.getSensorReadingName("1", "1.7"), "");
}

TEST(ConfigParser, get_sensor_reading_position)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    Position expected_topleft = {2, 0};
    Position expected_middleright= {3, 11};
    Position expected_topleft_val = {2, 5};
    Position expected_middleright_val= {3, 15};
    Position name_tl, name_mr;
    Position val_tl, val_mr;
    conf.getSensorReadingPositions("1", "1.0", name_tl, val_tl);
    conf.getSensorReadingPositions("2", "2.1", name_mr, val_mr);
    EXPECT_EQ(name_tl, expected_topleft);
    EXPECT_EQ(name_mr, expected_middleright);
    EXPECT_EQ(val_tl, expected_topleft_val);
    EXPECT_EQ(val_mr, expected_middleright_val);
}

TEST(ConfigParser, get_sensor_reading_position_non_existent_position_leaf)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    Position topleft_default = {2, 0};
    Position name_tl;
    Position val_tl;
    conf.getSensorReadingPositions("2", "2.3", name_tl, val_tl);
    EXPECT_EQ(name_tl, topleft_default);
}

TEST(ConfigParser, get_sensor_reading_position_val_non_existent)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    Position name_tl;
    Position val_tl;
    EXPECT_THROW(conf.getSensorReadingPositions("1", "1.4", name_tl, val_tl),
                 std::invalid_argument);
}

TEST(ConfigParser, check_readingid_against_settings)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    EXPECT_FALSE(conf.checkReadingExists("2", "2.8"));
    EXPECT_TRUE(conf.checkReadingExists("2", "2.1"));
}

TEST(ConfigParser, populate_all_lcd_positions_cache)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    EXPECT_EQ(weatherStationSettings.lcd_positions[0].name, "topleft_Name");
    EXPECT_EQ(weatherStationSettings.lcd_positions[1].name, "topleft_Val");
    EXPECT_EQ(weatherStationSettings.lcd_positions[2].name, "topright_Name");
    EXPECT_EQ(weatherStationSettings.lcd_positions[3].name, "topright_Val");
    EXPECT_EQ(weatherStationSettings.lcd_positions[4].name, "middleleft_Name");
    EXPECT_EQ(weatherStationSettings.lcd_positions[5].name, "middleleft_Val");
    EXPECT_EQ(weatherStationSettings.lcd_positions[10].name, "bottomright_Name");
    EXPECT_EQ(weatherStationSettings.lcd_positions[11].name, "bottomright_Val");
}