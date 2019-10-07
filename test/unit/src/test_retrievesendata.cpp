#include "gtest/gtest.h"
#include <iostream>
#include "../../../include/RetrieveSenData.hpp"
#include "../../../include/i2cControl.hpp"
#include "../../../include/data/WeatherSensor.hpp"

#define I2C_ADDR 0x04

TEST(RetrieveSensorData, Get_remote_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    DynamicSensorFactory dsf = DynamicSensorFactory();
    rsd.get_RemoteWeatherSenData(&dsf);

    WeatherSensor* backbed = dsf.getWeatherSensor_ptr("BackBed");
    printf("Sensor ID: %s\n", backbed->get_sensorID().c_str());
    printf("Temp: %f\n", backbed->get_temperature());
    printf("Hum: %f\n", backbed->get_humidity());

    EXPECT_STREQ("BackBed", backbed->get_sensorID().c_str());

    delete (i2c);
}

TEST(RetrieveSensorData, Get_local_weather_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    DynamicSensorFactory dsf = DynamicSensorFactory();
    rsd.get_LocalWeatherData(&dsf);

    WeatherSensor* backbed = dsf.getWeatherSensor_ptr("Here");
    printf("Sensor ID: %s\n", backbed->get_sensorID().c_str());
    printf("Temp: %f\n", backbed->get_temperature());
    printf("Hum: %f\n", backbed->get_humidity());

    EXPECT_STREQ("Here", backbed->get_sensorID().c_str());

    delete (i2c);
}

TEST(RetrieveSensorData, Get_specified_data_from_atmega_over_i2c)
{
    union convertSensorClassChar {
        struct tempSensorData
        {
            uint16_t temperature;
            char sensorID[10];
            uint16_t perBatt;
        } tsd;
        char packet[sizeof(tsd)];
    };
    union convertSensorClassChar weatherSensorUnionLocal;
    union convertSensorClassChar weatherSensorUnionRemote;

    I2cControl *i2c = new I2cControl(1);
    i2c->writeByte(I2C_ADDR, 1);
    usleep(50000);
    i2c->readI2c(weatherSensorUnionRemote.packet, 14);

    i2c->writeByte(I2C_ADDR, 0);
    usleep(50000);
    i2c->readI2c(weatherSensorUnionLocal.packet, 14);

    EXPECT_STREQ("Here", weatherSensorUnionLocal.tsd.sensorID);
    EXPECT_STREQ("BackBed", weatherSensorUnionRemote.tsd.sensorID);
}
