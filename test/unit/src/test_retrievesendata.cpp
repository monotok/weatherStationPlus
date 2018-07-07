#include "gtest/gtest.h"
#include "../../../include/RetrieveSenData.hpp"
#include "../../../include/i2cControl.hpp"
#include "../../../include/data/TempSensor.hpp"

TEST(RetrieveSensorData, Get_remote_sensor_data_from_arduino_module)
{
    I2cControl i2c(1);
    RetrieveSenData rsd;
    TemperatureSensor ts;

    // ts = rsd.getRemoteSenData();
    // EXPECT_TRUE(ts.getTemperature())
}