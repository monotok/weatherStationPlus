#ifndef RETRIEVE_SEN_DATA_H
#define RETRIEVE_SEN_DATA_H
#include "data/Abc_Sensor.hpp"
#include "data/WeatherSensor.hpp"
#include "i2cControl.hpp"

class RetrieveSenData
{
public:
  RetrieveSenData(I2cControl *, unsigned char);
  void getRemoteSenData(Sensor *);
  void getLocalSenData(WeatherSensor *);

private:
  I2cControl *i2c_controller;
  unsigned char I2C_ADDR;
};

#endif //ifndef RETRIEVE_SEN_DATA_H