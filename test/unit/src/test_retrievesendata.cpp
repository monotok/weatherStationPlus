#include "gtest/gtest.h"
#include <iostream>
#include "../../../include/RetrieveSenData.hpp"
#include "../../../include/lcdController.h"
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"

#define I2C_ADDR 0x04
#define private public

TEST(RetrieveSensorData, Get_remote_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    DynamicSensorFactory dsf;
    rsd.get_RemoteWeatherSenData(&dsf);

    WeatherSensor* backbed = dsf.getWeatherSensor_ptr("BackBed");
    printf("Sensor ID: %s\n", backbed->get_sensorID().c_str());
    printf("Temp: %s\n", backbed->get_temperature().c_str());
    printf("Hum: %s\n", backbed->get_humidity().c_str());

    EXPECT_STREQ("BackBed", backbed->get_sensorID().c_str());

    delete (i2c);
}

TEST(RetrieveSensorData, Get_local_weather_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    DynamicSensorFactory dsf;
    rsd.get_LocalWeatherData(&dsf);

    WeatherSensor* backbed = dsf.getWeatherSensor_ptr("Here");
    printf("Sensor ID: %s\n", backbed->get_sensorID().c_str());
    printf("Temp: %s\n", backbed->get_temperature().c_str());
    printf("Hum: %s\n", backbed->get_humidity().c_str());

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
    i2c->readI2c(I2C_ADDR, weatherSensorUnionRemote.packet, 14);

    i2c->writeByte(I2C_ADDR, 0);
    usleep(50000);
    i2c->readI2c(I2C_ADDR, weatherSensorUnionLocal.packet, 14);

    EXPECT_STREQ("Here", weatherSensorUnionLocal.tsd.sensorID);
    EXPECT_STREQ("BackBed", weatherSensorUnionRemote.tsd.sensorID);
}

TEST(RetrieveSenData, check_incoming_data_valid)
{
    I2cControl *i2c = new I2cControl(1);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    string str = "Valid";
    strcpy(rsd.weatherSensorUnion.tsd.sensorID, str.c_str());
    rsd.weatherSensorUnion.tsd.temperature = 3444;
    rsd.weatherSensorUnion.tsd.perBatt = 8845;

    EXPECT_TRUE(rsd.check_imcoming_data());
}

TEST(RetrieveSenData, check_incoming_data_invalid)
{
    I2cControl *i2c = new I2cControl(1);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    string str = "Valid";
    strcpy(rsd.weatherSensorUnion.tsd.sensorID, str.c_str());
    rsd.weatherSensorUnion.tsd.temperature = 27521;
    rsd.weatherSensorUnion.tsd.perBatt = -8845;

    EXPECT_FALSE(rsd.check_imcoming_data());
}