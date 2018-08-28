#include "gtest/gtest.h"
#include "../../../include/RetrieveSenData.hpp"
#include "../../../include/i2cControl.hpp"
#include "../../../include/data/TempSensor.hpp"

#define I2C_ADDR 0x04

TEST(RetrieveSensorData, Get_remote_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    TemperatureSensor *rsdata = new TemperatureSensor();
    rsd.getRemoteSenData(rsdata);
    EXPECT_EQ("Remote", rsdata->get_sensorID());
}

TEST(RetrieveSensorData, Get_local_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    TemperatureSensor *lsdata = new TemperatureSensor();
    rsd.getLocalSenData(lsdata);
    EXPECT_EQ("Here", lsdata->get_sensorID());
}