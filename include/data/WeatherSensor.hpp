#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"

class WeatherSensor : public Sensor
{
private:
  uint16_t temperature = 0;
  uint16_t humidity = 0;

public:
  ~WeatherSensor(){};
  string get_sensorID() override { return this->sensorName; }
  void set_sensorID(string sensorName) { this->sensorName = sensorName; }
  string get_sensorType() override { return this->sensorType; }
  void set_sensorType(string sensorType) override { this->sensorType = sensorType; }

  float get_temperature() { return this->temperature; }
  void set_temperature(float temperature) { this->temperature = temperature; }

  float get_humidity() { return this->humidity; }
  void set_humidity(float humidity) { this->humidity = humidity; }

  struct sensor_Data
  {
    uint16_t temperature;
    char sensorID[10];
    uint16_t perBatt;
  };

  union convertSensorClassChar {
    struct sensor_Data temporaryStruct;
    char packet[14];
  };
  union convertSensorClassChar weatherSensorUnion;
};

#endif // TEMP_SENSOR_H