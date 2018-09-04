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
    WeatherSensor *ptr_remoteWeatherData = new WeatherSensor();
    rsd.get_RemoteWeatherSenData(ptr_remoteWeatherData);

    printf("Remote Sensor ID: %s\n", ptr_remoteWeatherData->weatherSensorUnion.tsd.sensorID);
    printf("Remote Temp: %i\n", ptr_remoteWeatherData->weatherSensorUnion.tsd.temperature);
    printf("Remote Battery: %i\n", ptr_remoteWeatherData->weatherSensorUnion.tsd.perBatt);

    EXPECT_STREQ("BackBed", ptr_remoteWeatherData->weatherSensorUnion.tsd.sensorID);

    delete (ptr_remoteWeatherData);
    delete (i2c);
}

TEST(RetrieveSensorData, Get_local_weather_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    WeatherSensor *ptr_localWeatherData = new WeatherSensor();
    rsd.get_LocalWeatherData(ptr_localWeatherData);

    printf("Sensor ID: %s\n", ptr_localWeatherData->weatherSensorUnion.tsd.sensorID);
    printf("Temp: %i\n", ptr_localWeatherData->weatherSensorUnion.tsd.temperature);
    printf("Hum: %i\n", ptr_localWeatherData->weatherSensorUnion.tsd.perBatt);

    EXPECT_STREQ("Here", ptr_localWeatherData->weatherSensorUnion.tsd.sensorID);

    delete (ptr_localWeatherData);
    delete (i2c);
}

TEST(RetrieveSensorData, Get_local_weather_sensor_data_and_check_data_persistence)
{
    I2cControl *i2c = new I2cControl(1);
    RetrieveSenData rsd = RetrieveSenData(i2c, I2C_ADDR);
    WeatherSensor *ptr_localWeatherData = new WeatherSensor();
    rsd.get_LocalWeatherData(ptr_localWeatherData);

    cout << "Sensor ID: " << ptr_localWeatherData->get_sensorID() << "\n"
         << endl;
    cout << "Temp: " << ptr_localWeatherData->get_temperature() << "\n"
         << endl;
    cout << "Hum: " << ptr_localWeatherData->get_humidity() << "\n"
         << endl;

    EXPECT_EQ("Here", ptr_localWeatherData->get_sensorID());

    delete (ptr_localWeatherData);
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
