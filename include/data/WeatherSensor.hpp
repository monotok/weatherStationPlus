#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"

class WeatherSensor : public Sensor
{
private:
  float temperature;
  float humidity;
  string sensorType = "weather";
  int temporaryStructSize = sizeof(weatherSensorUnion.tsd);

public:
  ~WeatherSensor(){};
  string get_sensorID() override { return this->sensorName; }
  void set_sensorID(string sensorName) { this->sensorName = sensorName; }
  string get_sensorType() override { return this->sensorType; }

  float get_temperature() { return this->temperature; }
  void set_temperature(float temperature) { this->temperature = temperature; }

  float get_humidity() { return this->humidity; }
  void set_humidity(float humidity) { this->humidity = humidity; }

  int get_temporaryStructSize() { return this->temporaryStructSize; }

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