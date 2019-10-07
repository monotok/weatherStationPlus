#include "../../include/RetrieveSenData.hpp"
#include <unistd.h>

using namespace std;

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, unsigned char I2C_ADDR)
{
    this->i2c_controller = i2c_controller;
    this->I2C_ADDR = I2C_ADDR;
}

void RetrieveSenData::get_LocalWeatherData(DynamicSensorFactory *ptr_dsf)
{
    i2c_controller->writeByte(I2C_ADDR, GET_LOCAL_DATA);
    usleep(50000);
    i2c_controller->readI2c(weatherSensorUnion.packet, get_temporaryStructSize());

    WeatherSensor* temp = ptr_dsf->getWeatherSensor_ptr(get_retrievedSensorName());
    temp->set_temperature(weatherSensorUnion.tsd.temperature);
    temp->set_humidity(weatherSensorUnion.tsd.perBatt);
}

void RetrieveSenData::get_RemoteWeatherSenData(DynamicSensorFactory *ptr_dsf)
{
    i2c_controller->writeByte(I2C_ADDR, GET_REMOTE_DATA);
    usleep(50000);
    i2c_controller->readI2c(weatherSensorUnion.packet, get_temporaryStructSize());

    WeatherSensor* temp = ptr_dsf->getWeatherSensor_ptr(get_retrievedSensorName());
    temp->set_temperature(weatherSensorUnion.tsd.temperature);
    temp->set_humidity(weatherSensorUnion.tsd.perBatt);
}