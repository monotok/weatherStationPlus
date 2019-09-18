#ifndef DYNAMIC_SEN_FACTORY_H
#define DYNAMIC_SEN_FACTORY_H

#include "WeatherSensor.hpp"
#include <vector>

class DynamicSensorFactory
{
  public:
    void getWeatherSensor_ptr(WeatherSensor *);

  private:
    void CreateNewWeatherSensor_obj(WeatherSensor *);
    void SearchExistingWeatherSensor_obj(WeatherSensor *);
    vector<WeatherSensor *> weatherSensors_vector;
    vector<WeatherSensor *>::iterator weatherSensors_iterator;
};

#endif //ifndef DYNAMIC_SEN_FACTORY_H