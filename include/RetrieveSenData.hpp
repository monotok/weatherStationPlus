#ifndef RETRIEVE_SEN_DATA_H
#define RETRIEVE_SEN_DATA_H
#include <gtest/gtest_prod.h>
#include "data/Abc_Sensor.hpp"
#include "data/WeatherSensor.hpp"
#include "data/DynamicSensorFactory.hpp"
#include "i2cControl.hpp"
#include "lcdController.h"
#include <pqxx/pqxx>
#include <sstream>

#define GET_FIRST_SENSOR_READING 0
#define GET_NEXT_SENSOR_READING 1

class RetrieveSenData
{
public:
  RetrieveSenData(I2cControl *, LcdController* lcdc, unsigned char, ConfigParser& wss);
  void get_WeatherSenData(DynamicSensorFactory *ptr_dsf);

private:
  I2cControl *i2c_controller;
  LcdController *lcd_controller;
  unsigned char I2C_ADDR;
  pqxx::connection* C = nullptr;
  ConfigParser* wss = nullptr;
  int temporaryStructSize = sizeof(packet);
  int get_temporaryStructSize()  { return this->temporaryStructSize; }
  bool process_ReceivedSensor(DynamicSensorFactory *ptr_dsf);

  FRIEND_TEST(RetrieveSensorData, Get_specified_data_from_atmega_over_i2c);
  FRIEND_TEST(RetrieveSensorData, check_incoming_data_valid);
  FRIEND_TEST(RetrieveSensorData, check_incoming_data_invalid);
  FRIEND_TEST(RetrieveSensorData, get_overall_sensor_id);

    struct SensorData
    {
        char sensorID[4];
        float reading;
    } tempSensor = {};
    char packet[sizeof(tempSensor)] = {};

    //Need to declare this after the struct otherwise they would be in different scopes and therefore not the same type
    bool check_incoming_data(SensorData& sensor);
    bool check_valid_sensor(WeatherSensor *ptr_WeatherSensor);

    void get_retrievedGroupSensorID(char* sensorId) {
        strtok(sensorId, ".");
    }
//    string get_retrievedSensorType(SensorData& sensor) { return sensor.sensorType; }


};

#endif //ifndef RETRIEVE_SEN_DATA_H