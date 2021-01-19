#include "../../include/data/DynamicSensorFactory.hpp"

using namespace std;

DynamicSensorFactory::DynamicSensorFactory(ConfigParser& wss): wss(wss) {}

DynamicSensorFactory::~DynamicSensorFactory()
{
    for(weatherSensorIterator = weatherSensors_vector.begin(); weatherSensorIterator != weatherSensors_vector.end(); ++weatherSensorIterator)
    {
        delete((*weatherSensorIterator));
    }
}

vector<WeatherSensor *> DynamicSensorFactory::getAllWeatherSensors_ptr()
{
    lock_guard<mutex> guard(dyfMu);
    return weatherSensors_vector;
}

Sensor * DynamicSensorFactory::CreateNewSensor_obj(string sensorID)
{
    string sensorName = wss.getSensorsName(sensorID);
    if (!sensorName.empty() || strcmp(sensorID.c_str(), "tempWeatherSensor") == 0) {
        LOG(INFO) << "Creating new weather sensor. SensorID: " << sensorID << " SensorName: "
                  << sensorName << endl;
        WeatherSensor *newWeather_ptr = new WeatherSensor(sensorID, sensorName);
        weatherSensors_vector.push_back(newWeather_ptr);
        return newWeather_ptr;
    }
    LOG(WARNING) << "No matching sensor found. Please add sensorid " << sensorID << " to the config file." << endl;
    WeatherSensor *invalidSensor = nullptr;
    return invalidSensor;
}

WeatherSensor* DynamicSensorFactory::getWeatherSensor_ptr(string sensorID)
{
    lock_guard<mutex> guard(dyfMu);
    for(weatherSensorIterator = weatherSensors_vector.begin(); weatherSensorIterator != weatherSensors_vector.end(); ++weatherSensorIterator)
    {
        if ((*weatherSensorIterator)->get_sensorID() == sensorID)
        {
            return (*weatherSensorIterator);
        }
    }
    return dynamic_cast<WeatherSensor*>(CreateNewSensor_obj(sensorID));
}

//TODO: Can be removed
WeatherSensor* DynamicSensorFactory::getTempWeatherSensor_ptr()
{
    return getWeatherSensor_ptr("tempWeatherSensor");
}