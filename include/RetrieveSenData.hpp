#ifndef RETRIEVE_SEN_DATA_H
#define RETRIEVE_SEN_DATA_H
#include "data/Abc_Sensor.hpp"
#include "data/WeatherSensor.hpp"
#include "data/DynamicSensorFactory.hpp"
#include "i2cControl.hpp"

static const int GET_LOCAL_DATA = 0;
static const int GET_REMOTE_DATA = 1;

class RetrieveSenData
{
public:
  RetrieveSenData(I2cControl *, unsigned char);
  void get_RemoteWeatherSenData(DynamicSensorFactory *);
  void get_LocalWeatherData(DynamicSensorFactory *);

private:
  I2cControl *i2c_controller;
  unsigned char I2C_ADDR;
  int temporaryStructSize = sizeof(weatherSensorUnion.tsd);
  int get_temporaryStructSize()  { return this->temporaryStructSize; }
  string get_retrievedSensorName() { string s(weatherSensorUnion.tsd.sensorID); return s; }
  bool check_imcoming_data();

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

#endif //ifndef RETRIEVE_SEN_DATA_H