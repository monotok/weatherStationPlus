#include "gtest/gtest.h"
#include "../../../include/yang.hpp"

TEST(YangParser, initialise_yang_parser_settings)
{
    ASSERT_NO_THROW
    (
            YangParser settings_yp("settings");
            settings_yp.parseData("test/settings.xml");
    );
}

TEST(YangParser, initialise_yang_parser_settings_without_data)
{
    ASSERT_NO_THROW
    (
            YangParser settings_yp("settings");
    );
}

TEST(YangParser, find_sensor_name_from_id)
{
    YangParser settings_yp("settings");
    settings_yp.parseData("test/settings.xml");
    string xpath_str1 = "/settings:settings/sensors/wsensor[id='1']/name";
    string sensorName1 = settings_yp.getSpecificValue(xpath_str1);
    ASSERT_STREQ(sensorName1.c_str(), "Backbed");

    string xpath_str2 = "/settings:settings/sensors/wsensor[id='2']/name";
    string sensorName2 = settings_yp.getSpecificValue(xpath_str2);
    ASSERT_STREQ(sensorName2.c_str(), "Shed");
}

TEST(YangParser, find_sensor_reading_name_via_reading_id)
{
    YangParser settings_yp("settings");
    settings_yp.parseData("test/settings.xml");
    string xpath_str = "/settings:settings/sensors/wsensor[id='1']/readings[id='1.0']/name";
    string sensorReadingName1 = settings_yp.getSpecificValue(xpath_str);
    ASSERT_STREQ(sensorReadingName1.c_str(), "Temp");
}

