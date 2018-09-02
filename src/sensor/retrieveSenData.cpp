#include "../../include/RetrieveSenData.hpp"
#include <unistd.h>

using namespace std;

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, unsigned char I2C_ADDR)
{
    this->i2c_controller = i2c_controller;
    this->I2C_ADDR = I2C_ADDR;
}

void RetrieveSenData::get_LocalWeatherData(WeatherSensor *ptr_weatherSensorData)
{
    char requiredSensor[1] = {0};
    i2c_controller->writeByte(I2C_ADDR, requiredSensor[0]);
    usleep(50000);
    i2c_controller->readI2c(ptr_weatherSensorData->weatherSensorUnion.packet, ptr_weatherSensorData->get_temporaryStructSize());
}

void RetrieveSenData::get_RemoteWeatherSenData(WeatherSensor *ptr_weatherSensorData)
{
    char requiredSensor[1] = {1};
    i2c_controller->writeByte(I2C_ADDR, requiredSensor[0]);
    usleep(50000);
    i2c_controller->readI2c(ptr_weatherSensorData->weatherSensorUnion.packet, ptr_weatherSensorData->get_temporaryStructSize());
}