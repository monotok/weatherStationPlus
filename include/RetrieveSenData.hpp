#ifndef RETRIEVE_SEN_DATA_H
#define RETRIEVE_SEN_DATA_H
#include <gtest/gtest_prod.h>
#include "data/Abc_Sensor.hpp"
#include "data/WeatherSensor.hpp"
#include "data/DynamicSensorFactory.hpp"
#include "i2cControl.hpp"
#include "lcdController.h"
#include <pqxx/pqxx>

static const int GET_LOCAL_DATA = 0;
static const int GET_REMOTE_DATA = 1;

class RetrieveSenData
{
public:
  RetrieveSenData(I2cControl *, LcdController* lcdc, unsigned char);
  RetrieveSenData(I2cControl *, LcdController* lcdc, unsigned char, pqxx::connection* conn);
  void get_RemoteWeatherSenData(DynamicSensorFactory *);
  void get_LocalWeatherData(DynamicSensorFactory *);

private:
  I2cControl *i2c_controller;
  LcdController *lcd_controller;
  unsigned char I2C_ADDR;
  pqxx::connection* C = nullptr;
  int temporaryStructSize = sizeof(weatherSensorUnion.tsd);
  int get_temporaryStructSize()  { return this->temporaryStructSize; }
  string get_retrievedSensorID() { string s(weatherSensorUnion.tsd.sensorID); return s; }
  bool check_incoming_data();
  void store_local_weathersensor_data_in_database(WeatherSensor *ws);
  void store_remote_weathersensor_data_in_database(WeatherSensor *ws);

  FRIEND_TEST(RetrieveSensorData, Get_specified_data_from_atmega_over_i2c);
  FRIEND_TEST(RetrieveSensorData, check_incoming_data_valid);
  FRIEND_TEST(RetrieveSensorData, check_incoming_data_invalid);
  FRIEND_TEST(RetrieveSensorData, store_incoming_data_in_database);
  FRIEND_TEST(RetrieveSensorData, store_battery_correctly_for_non_here_sensors);

    typedef struct tempSensorData
    {
        int16_t temperature;
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