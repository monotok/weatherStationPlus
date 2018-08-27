#include "gtest/gtest.h"
#include "../../../include/RetrieveSenData.hpp"
#include "../../../include/i2cControl.hpp"
#include "../../../include/data/Abc_Sensor.hpp"

#define I2C_ADDR 0x04

TEST(RetrieveSensorData, Get_remote_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    Sensor *rsdata;
    rsd.getRemoteSenData(rsdata);
    EXPECT_EQ("Remote", rsdata->get_sensorID());
}

TEST(RetrieveSensorData, Get_local_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    Sensor *lsdata;
    rsd.getLocalSenData(lsdata);
    EXPECT_EQ("Here", lsdata->get_sensorID());
}