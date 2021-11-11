#include "gtest/gtest.h"
#include "../../../include/yang.hpp"

TEST(YangParser, initialise_yang_parser_settings)
{
    ASSERT_NO_THROW(YangParser settings_yp("settings", "test/settings.xml"));
}

TEST(YangParser, find_sensor_name_from_id)
{
    YangParser settings_yp("settings", "test/settings.xml");
    string sensorName1 = settings_yp.getSensorsName("1");
    ASSERT_STREQ(sensorName1.c_str(), "Backbed");

    string sensorName2 = settings_yp.getSensorsName("2");
    ASSERT_STREQ(sensorName2.c_str(), "Shed");
}

