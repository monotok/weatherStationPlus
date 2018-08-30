#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"

class WeatherSensor : public Sensor
{
private:
  float temperature;
  float humidity;

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
  } temporaryStruct;

  union convertSensorClassChar {
    struct sensor_Data temporaryStruct;
    char packet[14];
  };
  union convertSensorClassChar weatherSensorUnion;
  char *packet_ptr = &(weatherSensorUnion.packet[0]);

  char *get_tempSensorUnion() override { return this->packet_ptr; }
};

#endif // TEMP_SENSOR_H