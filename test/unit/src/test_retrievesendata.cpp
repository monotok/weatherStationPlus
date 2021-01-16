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
    LcdController lcdc(lcd);
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
    LcdController lcdc(lcd);
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
        char sensorType[4];
        float reading;
        char unit[4];
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

TEST(RetrieveSensorData, store_incoming_data_in_database)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
                         +" port = " + to_string(weatherStationSettings.db.port);

    pqxx::connection C(db_conn_str);
    pqxx::connection REAL_C(db_conn_str);

    RetrieveSenData rsd = RetrieveSenData(nullptr, nullptr, I2C_ADDR, &REAL_C, conf);
    rsd.prepare_insert_statements(REAL_C);
    WeatherSensor* mySen = new WeatherSensor("1", "here");

    struct tempSensorData
    {
        char sensorID[4];
        char sensorType[4];
        float reading;
        char unit[4];
    } sen1, sen2;

    strcpy(sen1.sensorID, "1.0"); strcpy(sen1.sensorType, "tmp"); sen1.reading = 23.5; strcpy(sen1.unit, "cel");
    strcpy(sen2.sensorID, "1.1"); strcpy(sen2.sensorType, "hum"); sen2.reading = 55.0; strcpy(sen2.unit, "per");

    mySen->set_reading(sen1.sensorID, sen1.reading, &REAL_C, &conf);
    mySen->set_reading(sen2.sensorID, sen2.reading, &REAL_C, &conf);

    pqxx::work w(C);

    pqxx::result r = w.exec("select time,sensor_id,readings.reading_id,reading,type,unit from readings JOIN sensor_metadata on readings.reading_id = sensor_metadata.reading_id where sensor_id = 1;");

    EXPECT_EQ(r.size(), 2);

    EXPECT_STREQ(r[0][1].c_str(), "1");
    EXPECT_STREQ(r[0][2].c_str(), "1");
    EXPECT_STREQ(r[0][3].c_str(), "23.5");
    EXPECT_STREQ(r[0][4].c_str(), "tmp");
    EXPECT_STREQ(r[0][5].c_str(), "cel");

    EXPECT_STREQ(r[1][1].c_str(), "1");
    EXPECT_STREQ(r[1][2].c_str(), "1.1");
    EXPECT_STREQ(r[1][3].c_str(), "55");
    EXPECT_STREQ(r[1][4].c_str(), "hum");
    EXPECT_STREQ(r[1][5].c_str(), "per");

    // Check that the meta data table can handle duplicate keys and not throw an exception. This
    // will occur when the program restarts.
    delete (mySen->getReading_ptr("1.0"));
    delete (mySen->getReading_ptr("1.1"));
    delete (mySen);

    WeatherSensor* mySen2 = new WeatherSensor("1", "here");
    mySen2->set_reading(sen1.sensorID, sen1.reading, &REAL_C, &conf);
    pqxx::result r2 = w.exec("select reading_id,type,unit from sensor_metadata where reading_id = 1;");
    EXPECT_STREQ(r2[0][1].c_str(), "pre");

    w.commit();
    //Clean up DB
    pqxx::work w1(C);
    w1.exec(
            "delete from readings;"
    );
    w1.exec(
            "delete from sensor_metadata;"
    );
    w1.commit();

    pqxx::work w2(C);
    r = w2.exec("select * from readings;");
    EXPECT_EQ(r.size(), 0);
}
//
//TEST(RetrieveSensorData, store_battery_correctly_for_non_here_sensors)
//{
//    pqxx::connection C("dbname = weather_test user = postgres password = password \
//      hostaddr = 127.0.0.1 port = 9432");
//
//    pqxx::connection REAL_C("dbname = weather_test user = postgres password = password \
//      hostaddr = 127.0.0.1 port = 9432");
//
//    RetrieveSenData rsd = RetrieveSenData(NULL, NULL, I2C_ADDR, &REAL_C);
//    WeatherSensor* mySen = new WeatherSensor("atest", "atest","weather");
//    mySen->set_humidity(55.0);
//    mySen->set_temperature(23.5);
//
//    pqxx::work w(C);
//    rsd.store_weathersensor_data_in_database(mySen);
//
//    pqxx::result r = w.exec("select * from sensors.data where sensorid = 'atest';");
//
//    EXPECT_EQ(r.size(), 1);
//
//    for (auto row: r)
//    {
//        EXPECT_STREQ(row[0].c_str(), "atest");
//        EXPECT_STREQ(row[2].c_str(), "23.5");
//        EXPECT_STREQ(row[4].c_str(), "55");
//    }
//
//    w.commit();
//
//    //Clean up DB
//    pqxx::work w1(C);
//    w1.exec(
//            "delete FROM sensors.data where sensorid = 'atest';"
//    );
//    w1.commit();
//
//    pqxx::work w2(C);
//    r = w2.exec("select * from sensors.data where sensorid = 'atest';");
//    EXPECT_EQ(r.size(), 0);
//}