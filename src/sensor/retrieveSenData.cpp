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
    // struct sensor_Data
    // {
    //     uint16_t temperature;
    //     char sensorID[10];
    //     uint16_t perBatt;
    // } temporaryStruct;

    // union convertSensorClassChar {
    //     struct sensor_Data temporaryStruct;
    //     char packet[14];
    // };
    // union convertSensorClassChar sts;

    i2c_controller->writeByte(I2C_ADDR, 1);
    usleep(50000);
    i2c_controller->readI2c(sensor_data->get_tempSensorUnion(), 14);

    // printf("Sensor ID: %s\n", sts.temporaryStruct.sensorID);
    // printf("Temp: %i\n", sts.temporaryStruct.temperature);
    // printf("Hum: %i\n", sts.temporaryStruct.perBatt);

    // sensor_data->set_sensorID(sts.temporaryStruct.sensorID);
}

void RetrieveSenData::getRemoteSenData(Sensor *sensor_data)
{
    sensor_data->set_sensorID("Remote");
}