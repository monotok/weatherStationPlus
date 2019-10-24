#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"
#include "../Utilities.hpp"
#include <string>

using namespace std;

class WeatherSensor : public Sensor
{
private:
  float temperature = 0.0;
  float humidity = 0.0;
  char tempUnit = 'c';

public:
    WeatherSensor(string SensorName, string SensorType) : Sensor(SensorName, SensorType) {};
    ~WeatherSensor(){};

    //Sensor Values
    string get_temperature();
    string get_humidity();

    float get_temperature_float() { return this->temperature/100; }
    float get_humidity_float() { return this->humidity/100; }

    //Sensor Values
    void set_humidity(float humidity) { this->humidity = humidity; }
    void set_temperature(float temperature) { this->temperature = temperature; }

    //Sensor Units
    void set_tempUnit_to_C();
    void set_tempUnit_to_F();
    void switch_tempUnit();
    char getTempUnit();
};

#endif // TEMP_SENSOR_H