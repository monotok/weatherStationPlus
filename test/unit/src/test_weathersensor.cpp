#include "gtest/gtest.h"
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/configParser.hpp"

TEST(WeatherSensor, get_existing_reading)
{
    WeatherSensor ws1 = WeatherSensor("1", "MyTestSensor", "weather");
    ws1.createNewSensorReading_obj("1.0");
    ws1.createNewSensorReading_obj("1.1");
    ws1.createNewSensorReading_obj("1.2");

    EXPECT_TRUE(ws1.getReading_ptr("1.0") != nullptr);
    EXPECT_TRUE(ws1.getReading_ptr("1.1") != nullptr);
    EXPECT_TRUE(ws1.getReading_ptr("1.2") != nullptr);

    EXPECT_TRUE(ws1.getReading_ptr("1.7") == nullptr);
}

TEST(WeatherSensor, create_max_6_readings_on_sensor)
{
    WeatherSensor ws1 = WeatherSensor("1", "MyTestSensor", "weather");
    ws1.createNewSensorReading_obj("1.0");
    ws1.createNewSensorReading_obj("1.1");
    ws1.createNewSensorReading_obj("1.2");
    ws1.createNewSensorReading_obj("1.3");
    ws1.createNewSensorReading_obj("1.4");
    ws1.createNewSensorReading_obj("1.5");

    EXPECT_EQ(ws1.getReading_ptr("1.5")->readingId, "1.5");
    ws1.createNewSensorReading_obj("1.6");
    EXPECT_TRUE(ws1.getReading_ptr("1.5") == nullptr);
    EXPECT_EQ(ws1.getReading_ptr("1.6")->readingId, "1.6");
}

TEST(WeatherSensor, set_reading_values_correctly)
{
    // Grab settings so we can get the position information
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    ConfigParser* conf_ptr = &conf;

    //Create a reading sensor
    WeatherSensor ws1 = WeatherSensor("1", "MyTestSensor", "weather");

    //Set the reading for the first time. We should set all the values of the reading struct
    ws1.set_reading("1.0", "tmp", 24.5, "cel", nullptr, conf_ptr);
    EXPECT_EQ(ws1.getReading_ptr("1.0")->readingId, "1.0");
    EXPECT_FLOAT_EQ(ws1.getReading_ptr("1.0")->reading, 24.5);
    EXPECT_EQ(ws1.getReading_ptr("1.0")->type, "tmp");
    EXPECT_EQ(ws1.getReading_ptr("1.0")->unit, "cel");
    EXPECT_EQ(ws1.getReading_ptr("1.0")->posVal.row_start, 2);
    EXPECT_EQ(ws1.getReading_ptr("1.0")->posVal.col_start, 5);

    //Update the value and it should only change the reading
    ws1.set_reading("1.0", "hum", 40.3, "far", nullptr, conf_ptr);
    EXPECT_EQ(ws1.getReading_ptr("1.0")->readingId, "1.0");
    EXPECT_FLOAT_EQ(ws1.getReading_ptr("1.0")->reading, 40.3);
    EXPECT_EQ(ws1.getReading_ptr("1.0")->type, "tmp");
    EXPECT_EQ(ws1.getReading_ptr("1.0")->unit, "cel");

}