#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"

class WeatherSensor : public Sensor
{
private:
  float temperature;
  float humidity;
  int temporaryStructSize = sizeof(weatherSensorUnion.tsd);

  //Sensor Values
  void set_humidity(float humidity) { this->humidity = humidity; }
  void set_temperature(float temperature) { this->temperature = temperature; }

public:
    WeatherSensor(string SensorName, string SensorType) : Sensor(SensorName, SensorType) {};
  ~WeatherSensor(){};

  //Sensor Values
  float get_temperature() { return this->temperature; }
  float get_humidity() { return this->humidity; }

  //Other Methods
  int get_temporaryStructSize() { return this->temporaryStructSize; }
  void persistData();

  typedef struct tempSensorData
  {
    uint16_t temperature;
    char sensorID[10];
    uint16_t perBatt;
  };

  union convertSensorClassChar {
    tempSensorData tsd;
    char packet[sizeof(tsd)];
  };
  union convertSensorClassChar weatherSensorUnion;
};

#endif // TEMP_SENSOR_H