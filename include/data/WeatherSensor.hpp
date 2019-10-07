#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"

class WeatherSensor : public Sensor
{
private:
  float temperature;
  float humidity;

public:
    WeatherSensor(string SensorName, string SensorType) : Sensor(SensorName, SensorType) {};
  ~WeatherSensor(){};

  //Sensor Values
  float get_temperature() { return this->temperature; }
  float get_humidity() { return this->humidity; }

    //Sensor Values
    void set_humidity(float humidity) { this->humidity = humidity; }
    void set_temperature(float temperature) { this->temperature = temperature; }
};

#endif // TEMP_SENSOR_H