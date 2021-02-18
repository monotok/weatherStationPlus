#include "gtest/gtest.h"
#include <iostream>
#include "../../../include/RetrieveSenData.hpp"
#include "../../../include/lcdController.h"
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"

#define I2C_ADDR 0x04

TEST(RetrieveSensorData, get_overall_sensor_id)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(weatherStationSettings.i2c.lcd, i2c);
    LcdController lcdc(lcd, weatherStationSettings);
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR, conf);

    char sensorId[4] = "1.0";
    char sensorIdCopy[4] = {};
    strcpy(sensorIdCopy, sensorId);
    string result = {};
    rsd.get_retrievedGroupSensorID(sensorIdCopy);

    EXPECT_STREQ((char*)sensorIdCopy, "1");
    EXPECT_STREQ(sensorId, "1.0");
}

//TODO: Fix this test case. Does not test anything.
TEST(RetrieveSensorData, Get_sensor_data_from_arduino_module)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(weatherStationSettings.i2c.lcd, i2c);
    LcdController lcdc(lcd, weatherStationSettings);
    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR, conf);
    DynamicSensorFactory dsf(conf);
    rsd.get_WeatherSenData(&dsf);

    EXPECT_STREQ("1", dsf.getWeatherSensor_ptr("1")->get_sensorID().c_str());
    EXPECT_EQ(3, dsf.getWeatherSensor_ptr("1")->getAvailableReadings().size());

    delete (i2c);
}

TEST(RetrieveSensorData, Get_data_from_atmega_manually_i2c)
{
    struct tempSensorData
    {
        char sensorID[4];
        float reading;
    } sen1, sen2;

    I2cControl *i2c = new I2cControl(3);
    usleep(50000);
    char packet[sizeof(sen1)] = {};

    i2c->writeByte(I2C_ADDR, 0); //Get first sensor reading by passing zero
    i2c->readI2c(I2C_ADDR, packet, sizeof(packet));
    memcpy(&sen1, packet, sizeof(packet));

    i2c->writeByte(I2C_ADDR, 1); //Get second sensor reading by passing zero
    i2c->readI2c(I2C_ADDR, packet, sizeof(packet));
    memcpy(&sen2, packet, sizeof(packet));

    cout << "The tsd reading is " << sen1.reading << endl;
    cout << "The tsd reading is " << sen2.reading << endl;

    EXPECT_STREQ("1.0", sen1.sensorID);
    EXPECT_STREQ("1.1", sen2.sensorID);

    delete (i2c);
}

//TEST(RetrieveSensorData, check_incoming_data_valid)
//{
//    I2cControl *i2c = new I2cControl(3);
//    LcdController lcdc;
//    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
//    string str = "Valid";
//    strcpy(rsd.weatherSensorUnion.tsd.sensorID, str.c_str());
//    rsd.weatherSensorUnion.tsd.temperature = 3444;
//    rsd.weatherSensorUnion.tsd.perBatt = 8845;
//
//    EXPECT_TRUE(rsd.check_incoming_data());
//}

//TEST(RetrieveSensorData, check_incoming_data_invalid)
//{
//    I2cControl *i2c = new I2cControl(3);
//    LcdController lcdc;
//    RetrieveSenData rsd = RetrieveSenData(i2c, &lcdc,I2C_ADDR);
//    string str = "Valid";
//    strcpy(rsd.weatherSensorUnion.tsd.sensorID, str.c_str());
//    rsd.weatherSensorUnion.tsd.temperature = 27521;
//    rsd.weatherSensorUnion.tsd.perBatt = -8845;
//
//    EXPECT_FALSE(rsd.check_incoming_data());
//}
