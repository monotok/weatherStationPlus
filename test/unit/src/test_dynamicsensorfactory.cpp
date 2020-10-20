#include "gtest/gtest.h"
#include <algorithm>

#include <iostream>
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"

TEST(DynamicSensorFactory, Add_new_weatherSensor_obj_to_vector)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);
    WeatherSensor *tempSensor = dynamic_cast<WeatherSensor*>(dsf.CreateNewSensor_obj("myName", "weather"));

    std::vector<WeatherSensor*>::iterator found;
    found = std::find(dsf.weatherSensors_vector.begin(), dsf.weatherSensors_vector.end(), tempSensor);

    EXPECT_EQ("myName", dsf.weatherSensors_vector.at(0)->get_sensorID());
    EXPECT_EQ((*found)->get_sensorID(), "myName");

}

TEST(DynamicSensorFactory, Find_existing_sensor) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);

    dsf.CreateNewSensor_obj("bob", "weather");
    dsf.CreateNewSensor_obj("ant", "weather");
    dsf.CreateNewSensor_obj("gar", "weather");

    WeatherSensor* returnValAnt = dsf.getWeatherSensor_ptr("ant");
    WeatherSensor* returnValBob = dsf.getWeatherSensor_ptr("bob");
    WeatherSensor* unknown = dsf.getWeatherSensor_ptr("who");

    std::vector<WeatherSensor*>::iterator found;
    found = std::find(dsf.weatherSensors_vector.begin(), dsf.weatherSensors_vector.end(), unknown);

    EXPECT_EQ("ant", returnValAnt->get_sensorID());
    EXPECT_EQ("bob", returnValBob->get_sensorID());
    EXPECT_EQ("who", unknown->get_sensorID());
    EXPECT_EQ((*found)->get_sensorID(), "who");
}

TEST(DynamicSensorFactory, Get_temp_weathersensor) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);

    WeatherSensor* returnVal= dsf.getTempWeatherSensor_ptr();

    EXPECT_EQ("tempWeatherSensor", returnVal->get_sensorID());
}

TEST(DynamicSensorFactory, get_all_weathersensors) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);

    dsf.CreateNewSensor_obj("bob", "weather");
    dsf.CreateNewSensor_obj("ant", "weather");
    dsf.CreateNewSensor_obj("gar", "weather");

    vector<WeatherSensor *> returnVals = dsf.getAllWeatherSensors_ptr();

    EXPECT_EQ(returnVals.size(), 3);
}