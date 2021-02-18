#include "gtest/gtest.h"
#include <algorithm>

#include <iostream>
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"

TEST(DynamicSensorFactory, Add_new_weatherSensor_obj_to_vector)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();
    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);
    WeatherSensor *tempSensor = dynamic_cast<WeatherSensor*>(dsf.CreateNewSensor_obj("myName"));

    std::vector<WeatherSensor*>::iterator found;
    found = std::find(dsf.weatherSensors_vector.begin(), dsf.weatherSensors_vector.end(), tempSensor);

    EXPECT_EQ("myName", dsf.weatherSensors_vector.at(0)->get_sensorID());
    EXPECT_EQ((*found)->get_sensorID(), "myName");

}

TEST(DynamicSensorFactory, Find_existing_sensor) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);

    dsf.CreateNewSensor_obj("bob");
    dsf.CreateNewSensor_obj("ant");
    dsf.CreateNewSensor_obj("gar");

    WeatherSensor* returnValAnt = dsf.getWeatherSensor_ptr("ant");
    WeatherSensor* returnValBob = dsf.getWeatherSensor_ptr("bob");
    WeatherSensor* unknown = dsf.getWeatherSensor_ptr("who");

    std::vector<WeatherSensor*>::iterator found;
    found = std::find(dsf.weatherSensors_vector.begin(), dsf.weatherSensors_vector.end(), returnValBob);

    EXPECT_EQ("ant", returnValAnt->get_sensorID());
    EXPECT_EQ("bob", returnValBob->get_sensorID());
    EXPECT_EQ(nullptr, unknown);
    EXPECT_EQ((*found)->get_sensorID(), "bob");
}

TEST(DynamicSensorFactory, Get_temp_weathersensor) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);

    WeatherSensor* returnVal= dsf.getTempWeatherSensor_ptr();

    EXPECT_EQ("tempWeatherSensor", returnVal->get_sensorID());
}

TEST(DynamicSensorFactory, get_all_weathersensors) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    DynamicSensorFactory dsf(conf);

    dsf.CreateNewSensor_obj("bob");
    dsf.CreateNewSensor_obj("ant");
    dsf.CreateNewSensor_obj("gar");

    vector<WeatherSensor *> returnVals = dsf.getAllWeatherSensors_ptr();

    EXPECT_EQ(returnVals.size(), 3);
}

TEST(DynamicSensorFactory, store_incoming_data_in_database)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
                         +" port = " + to_string(weatherStationSettings.db.port);

    pqxx::connection TEST_CONNECTOR(db_conn_str);

    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);

    WeatherSensor* mySen = dsf.getWeatherSensor_ptr("1");

    struct tempSensorData
    {
        char sensorID[4];
        float reading;
    } sen1, sen2;

    strcpy(sen1.sensorID, "1.0"); sen1.reading = 23.5;
    strcpy(sen2.sensorID, "1.1"); sen2.reading = 55.0;

    mySen->set_reading(sen1.sensorID, sen1.reading, &conf);
    mySen->set_reading(sen2.sensorID, sen2.reading, &conf);

    pqxx::work w(TEST_CONNECTOR);

    pqxx::result r = w.exec("select time,sensor_id,readings.reading_id,reading,type,unit from readings JOIN sensor_metadata on readings.reading_id = sensor_metadata.reading_id where sensor_id = 1;");

    EXPECT_EQ(r.size(), 2);

    EXPECT_STREQ(r[0][1].c_str(), "1");
    EXPECT_STREQ(r[0][2].c_str(), "1");
    EXPECT_STREQ(r[0][3].c_str(), "23.5");
    EXPECT_STREQ(r[0][4].c_str(), "t");
    EXPECT_STREQ(r[0][5].c_str(), "u");

    EXPECT_STREQ(r[1][1].c_str(), "1");
    EXPECT_STREQ(r[1][2].c_str(), "1.1");
    EXPECT_STREQ(r[1][3].c_str(), "55");
    EXPECT_STREQ(r[1][4].c_str(), "t");
    EXPECT_STREQ(r[1][5].c_str(), "u");

    // Check that the meta data table can handle duplicate keys and not throw an exception. This
    // will occur when the program restarts.
    delete (mySen->getReading_ptr("1.0"));
    delete (mySen->getReading_ptr("1.1"));
    mySen->set_reading(sen1.sensorID, sen1.reading, &conf);
    pqxx::result r2 = w.exec("select reading_id,type,unit from sensor_metadata where reading_id = 1;");
    EXPECT_STREQ(r2[0][1].c_str(), "t");

    w.commit();
    //Clean up DB
    pqxx::work w1(TEST_CONNECTOR);
    w1.exec(
            "delete from readings;"
    );
    w1.exec(
            "delete from sensor_metadata;"
    );
    w1.commit();

    pqxx::work w2(TEST_CONNECTOR);
    r = w2.exec("select * from readings;");
    EXPECT_EQ(r.size(), 0);
}

TEST(DynamicSensorFactory, get_average_hour_for_reading)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
                         +" port = " + to_string(weatherStationSettings.db.port);

    pqxx::connection TEST_CONNECTOR(db_conn_str);

    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);

    WeatherSensor* mySen = dsf.getWeatherSensor_ptr("1");


    mySen->set_reading("1.0", 10, &conf);
    mySen->set_reading("1.0", 12, &conf);
    mySen->set_reading("1.0", 16, &conf);
    mySen->set_reading("1.1", 22, &conf);
    mySen->set_reading("1.1", 21, &conf);
    mySen->set_reading("1.1", 24, &conf);

    pqxx::work w(TEST_CONNECTOR);

    pqxx::result r = w.exec("select time,sensor_id,readings.reading_id,reading,type,unit from readings JOIN sensor_metadata on readings.reading_id = sensor_metadata.reading_id where sensor_id = 1;");

    EXPECT_EQ(r.size(), 6);

    string avg_sen1 = mySen->get_AvgReading("1.0", "1 hour");
    string avg_sen2 = mySen->get_AvgReading("1.1", "1 hour");

    EXPECT_STREQ(avg_sen1.c_str(), "12.6");
    EXPECT_STREQ(avg_sen2.c_str(), "22.3");

    w.commit();
    //Clean up DB
    pqxx::work w1(TEST_CONNECTOR);
    w1.exec(
            "delete from readings;"
    );
    w1.exec(
            "delete from sensor_metadata;"
    );
    w1.commit();

    pqxx::work w2(TEST_CONNECTOR);
    r = w2.exec("select * from readings;");
    EXPECT_EQ(r.size(), 0);
}