#ifndef DYNAMIC_SEN_FACTORY_H
#define DYNAMIC_SEN_FACTORY_H

#include "WeatherSensor.hpp"
#include <vector>

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
};

#endif //ifndef DYNAMIC_SEN_FACTORY_H