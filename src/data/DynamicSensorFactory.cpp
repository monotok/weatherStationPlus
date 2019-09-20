#include "../../include/data/DynamicSensorFactory.hpp"

using namespace std;

DynamicSensorFactory::~DynamicSensorFactory()
{
    for(weatherSensorIterator = weatherSensors_vector.begin(); weatherSensorIterator != weatherSensors_vector.end(); ++weatherSensorIterator)
    {
        delete((*weatherSensorIterator));
    }
}

vector<WeatherSensor *> DynamicSensorFactory::getAllWeatherSensors_ptr()
{
    return weatherSensors_vector;
}

Sensor* DynamicSensorFactory::CreateNewSensor_obj(string SensorName, string SensorType)
{
    WeatherSensor *newWeather_ptr = new WeatherSensor(SensorName, SensorType);
    weatherSensors_vector.push_back(newWeather_ptr);
    return newWeather_ptr;
}

WeatherSensor* DynamicSensorFactory::getWeatherSensor_ptr(string SensorName)
{
    for(weatherSensorIterator = weatherSensors_vector.begin(); weatherSensorIterator != weatherSensors_vector.end(); ++weatherSensorIterator)
    {
        if ((*weatherSensorIterator)->get_sensorID() == SensorName)
        {
            return (*weatherSensorIterator);
        }
    }
    return dynamic_cast<WeatherSensor*>(CreateNewSensor_obj(SensorName, "weather"));
}

WeatherSensor* DynamicSensorFactory::getTempWeatherSensor_ptr()
{
    return getWeatherSensor_ptr("tempWeatherSensor");
}