#ifndef DYNAMIC_SEN_FACTORY_H
#define DYNAMIC_SEN_FACTORY_H

#include <gtest/gtest_prod.h>
#include "../easylogging++.hpp"
#include "WeatherSensor.hpp"
#include <vector>
#include <mutex>

class DynamicSensorFactory
{
  public:
    WeatherSensor* getWeatherSensor_ptr(string SensorName);
    WeatherSensor* getTempWeatherSensor_ptr();
    vector<WeatherSensor *> getAllWeatherSensors_ptr();
    ~DynamicSensorFactory();

private:
    Sensor* CreateNewSensor_obj(string SensorName, string SensorType);
    vector<WeatherSensor *> weatherSensors_vector;
    vector<WeatherSensor *>::iterator weatherSensorIterator;

    FRIEND_TEST(DynamicSensorFactory, Add_new_weatherSensor_obj_to_vector);
    FRIEND_TEST(DynamicSensorFactory, Find_existing_sensor);
    FRIEND_TEST(DynamicSensorFactory, get_all_weathersensors);

    mutex dyfMu;
};

#endif //ifndef DYNAMIC_SEN_FACTORY_H