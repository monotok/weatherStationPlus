#include "../../include/data/DynamicSensorFactory.hpp"

using namespace std;

void DynamicSensorFactory::CreateNewWeatherSensor_obj(WeatherSensor *ptr_newWeatherSensor)
{
    weatherSensors_vector.push_back(ptr_newWeatherSensor);
}