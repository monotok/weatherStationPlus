#include "gtest/gtest.h"
#include <iostream>
#include "../../../include/RetrieveSenData.hpp"
#include "../../../include/lcdController.h"
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"

#define I2C_ADDR 0x04

//TODO: Fix this test case. Does not test anything.
TEST(RetrieveSensorData, Get_remote_sensor_data_from_arduino_module)
{
    I2cControl *i2c = new I2cControl(3);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);
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
    I2cControl *i2c = new I2cControl(3);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);
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

    I2cControl *i2c = new I2cControl(3);
    i2c->writeByte(I2C_ADDR, 1);
    usleep(50000);
    i2c->readI2c(I2C_ADDR, weatherSensorUnionRemote.packet, 14);

    i2c->writeByte(I2C_ADDR, 0);
    usleep(50000);
    i2c->readI2c(I2C_ADDR, weatherSensorUnionLocal.packet, 14);

    EXPECT_STREQ("Here", weatherSensorUnionLocal.tsd.sensorID);
    EXPECT_TRUE(strcmp("BackBed", weatherSensorUnionRemote.tsd.sensorID) == 0 || strcmp("Loft", weatherSensorUnionRemote.tsd.sensorID) == 0);
}

TEST(RetrieveSensorData, check_incoming_data_valid)
{
    I2cControl *i2c = new I2cControl(3);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    string str = "Valid";
    strcpy(rsd.weatherSensorUnion.tsd.sensorID, str.c_str());
    rsd.weatherSensorUnion.tsd.temperature = 3444;
    rsd.weatherSensorUnion.tsd.perBatt = 8845;

    EXPECT_TRUE(rsd.check_incoming_data());
}

TEST(RetrieveSensorData, check_incoming_data_invalid)
{
    I2cControl *i2c = new I2cControl(3);
    LcdController lcdc;
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
    string str = "Valid";
    strcpy(rsd.weatherSensorUnion.tsd.sensorID, str.c_str());
    rsd.weatherSensorUnion.tsd.temperature = 27521;
    rsd.weatherSensorUnion.tsd.perBatt = -8845;

    EXPECT_FALSE(rsd.check_incoming_data());
}

TEST(RetrieveSensorData, store_incoming_data_in_database)
{
    pqxx::connection C("dbname = weather_test user = postgres password = password \
      hostaddr = 127.0.0.1 port = 9432");

    pqxx::connection REAL_C("dbname = weather_test user = postgres password = password \
      hostaddr = 127.0.0.1 port = 9432");

    RetrieveSenData rsd = RetrieveSenData(NULL, NULL, I2C_ADDR, &REAL_C);
    WeatherSensor* mySen = new WeatherSensor("here", "here", "weather");
    mySen->set_humidity(55.0);
    mySen->set_temperature(23.5);

    pqxx::work w(C);
    rsd.store_local_weathersensor_data_in_database(mySen);

    pqxx::result r = w.exec("select * from sensors.data where sensorid = 'here';");

    EXPECT_EQ(r.size(), 1);

    for (auto row: r)
    {
        EXPECT_STREQ(row[0].c_str(), "here");
        EXPECT_STREQ(row[2].c_str(), "23.5");
        EXPECT_STREQ(row[3].c_str(), "55");
    }

    w.commit();

    //Clean up DB
    pqxx::work w1(C);
    w1.exec(
            "delete FROM sensors.data where sensorid = 'atest';"
    );
    w1.commit();

    pqxx::work w2(C);
    r = w2.exec("select * from sensors.data where sensorid = 'atest';");
    EXPECT_EQ(r.size(), 0);
}

TEST(RetrieveSensorData, store_battery_correctly_for_non_here_sensors)
{
    pqxx::connection C("dbname = weather_test user = postgres password = password \
      hostaddr = 127.0.0.1 port = 9432");

    pqxx::connection REAL_C("dbname = weather_test user = postgres password = password \
      hostaddr = 127.0.0.1 port = 9432");

    RetrieveSenData rsd = RetrieveSenData(NULL, NULL, I2C_ADDR, &REAL_C);
    WeatherSensor* mySen = new WeatherSensor("atest", "atest","weather");
    mySen->set_humidity(55.0);
    mySen->set_temperature(23.5);

    pqxx::work w(C);
    rsd.store_remote_weathersensor_data_in_database(mySen);

    pqxx::result r = w.exec("select * from sensors.data where sensorid = 'atest';");

    EXPECT_EQ(r.size(), 1);

    for (auto row: r)
    {
        EXPECT_STREQ(row[0].c_str(), "atest");
        EXPECT_STREQ(row[2].c_str(), "23.5");
        EXPECT_STREQ(row[4].c_str(), "55");
    }

    w.commit();

    //Clean up DB
    pqxx::work w1(C);
    w1.exec(
            "delete FROM sensors.data where sensorid = 'atest';"
    );
    w1.commit();

    pqxx::work w2(C);
    r = w2.exec("select * from sensors.data where sensorid = 'atest';");
    EXPECT_EQ(r.size(), 0);
}