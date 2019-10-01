#include "../../include/RetrieveSenData.hpp"
#include <unistd.h>

using namespace std;

//TODO: No longer pass weathersensor obj to the methods. Have the temp union and struct on this class and call dynamicsensorfactory from method

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, unsigned char I2C_ADDR)
{
    this->i2c_controller = i2c_controller;
    this->I2C_ADDR = I2C_ADDR;
}

void RetrieveSenData::get_LocalWeatherData(WeatherSensor *ptr_weatherSensorData)
{
    i2c_controller->writeByte(I2C_ADDR, GET_LOCAL_DATA);
    usleep(50000);
    i2c_controller->readI2c(ptr_weatherSensorData->weatherSensorUnion.packet, ptr_weatherSensorData->get_temporaryStructSize());
    ptr_weatherSensorData->persistData();
}

void RetrieveSenData::get_RemoteWeatherSenData(WeatherSensor *ptr_weatherSensorData)
{
    i2c_controller->writeByte(I2C_ADDR, GET_REMOTE_DATA);
    usleep(50000);
    i2c_controller->readI2c(ptr_weatherSensorData->weatherSensorUnion.packet, ptr_weatherSensorData->get_temporaryStructSize());
    ptr_weatherSensorData->persistData();
}