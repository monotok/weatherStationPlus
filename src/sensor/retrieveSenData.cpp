#include "../../include/RetrieveSenData.hpp"
#include <unistd.h>

using namespace std;

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, unsigned char I2C_ADDR)
{
    this->i2c_controller = i2c_controller;
    this->I2C_ADDR = I2C_ADDR;
}

void RetrieveSenData::getLocalSenData(Sensor *sensor_data)
{
    i2c_controller->writeByte(I2C_ADDR, 1);
    usleep(50000);
    i2c_controller->readI2c();
    sensor_data->set_sensorID("Here");
}

void RetrieveSenData::getRemoteSenData(Sensor *sensor_data)
{
    sensor_data->set_sensorID("Remote");
}