#ifndef RETRIEVE_SEN_DATA_H
#define RETRIEVE_SEN_DATA_H
#include "data/Abc_Sensor.hpp"
#include "data/WeatherSensor.hpp"
#include "i2cControl.hpp"

static const int GET_LOCAL_DATA = 0;
static const int GET_REMOTE_DATA = 1;

class RetrieveSenData
{
public:
  RetrieveSenData(I2cControl *, unsigned char);
  void get_RemoteWeatherSenData(WeatherSensor *);
  void get_LocalWeatherData(WeatherSensor *);

private:
  I2cControl *i2c_controller;
  unsigned char I2C_ADDR;
};

#endif //ifndef RETRIEVE_SEN_DATA_H