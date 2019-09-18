#include "gtest/gtest.h"
#define private public

#include <iostream>
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"

TEST(DynamicSensorFactory, Add_new_weatherSensor_obj_to_vector)
{
    WeatherSensor *tempSensor = new WeatherSensor();
    tempSensor->set_sensorID("Here");
    tempSensor->set_temperature(23);
    tempSensor->set_humidity(60);

    DynamicSensorFactory dsf = DynamicSensorFactory();
    dsf.CreateNewWeatherSensor_obj(tempSensor);

    EXPECT_EQ("Here", dsf.weatherSensors_vector.at(0)->get_sensorID());
}