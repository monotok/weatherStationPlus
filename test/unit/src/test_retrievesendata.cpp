#include "gtest/gtest.h"
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
}