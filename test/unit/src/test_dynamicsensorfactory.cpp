#include "gtest/gtest.h"
#include <algorithm>

#include <iostream>
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"

TEST(DynamicSensorFactory, Add_new_weatherSensor_obj_to_vector)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();
    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);
    WeatherSensor *tempSensor = dynamic_cast<WeatherSensor*>(dsf.CreateNewSensor_obj("200"));

    std::vector<WeatherSensor*>::iterator found;
    found = std::find(dsf.weatherSensors_vector.begin(), dsf.weatherSensors_vector.end(), tempSensor);

    EXPECT_EQ("200", dsf.weatherSensors_vector.at(0)->get_sensorID());
    EXPECT_EQ((*found)->get_sensorID(), "200");

}

TEST(DynamicSensorFactory, Find_existing_sensor) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    DynamicSensorFactory dsf(conf);

    dsf.CreateNewSensor_obj("100");
    dsf.CreateNewSensor_obj("200");
    dsf.CreateNewSensor_obj("300");

    WeatherSensor* returnValAnt = dsf.getWeatherSensor_ptr("200");
    WeatherSensor* returnValBob = dsf.getWeatherSensor_ptr("100");
    WeatherSensor* unknown = dsf.getWeatherSensor_ptr("500");

    std::vector<WeatherSensor*>::iterator found;
    found = std::find(dsf.weatherSensors_vector.begin(), dsf.weatherSensors_vector.end(), returnValBob);

    EXPECT_EQ("200", returnValAnt->get_sensorID());
    EXPECT_EQ("100", returnValBob->get_sensorID());
    EXPECT_EQ(nullptr, unknown);
    EXPECT_EQ((*found)->get_sensorID(), "100");
}

TEST(DynamicSensorFactory, get_all_weathersensors) {
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    DynamicSensorFactory dsf(conf);

    dsf.CreateNewSensor_obj("100");
    dsf.CreateNewSensor_obj("200");
    dsf.CreateNewSensor_obj("300");

    vector<WeatherSensor *> returnVals = dsf.getAllWeatherSensors_ptr();

    EXPECT_EQ(returnVals.size(), 3);
}

TEST(DynamicSensorFactory, disable_db_connection_timeout)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    weatherStationSettings.db.enable = true;
    weatherStationSettings.db.host = "172.16.20.5";
    weatherStationSettings.db.port = 12345;
    weatherStationSettings.db.timeout = "1";

    DynamicSensorFactory dsf(conf);

    EXPECT_FALSE(dsf.establish_database_connection(weatherStationSettings));
}

TEST(DynamicSensorFactory, dont_connect_to_db_if_disabled)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    DynamicSensorFactory dsf(conf);

    EXPECT_FALSE(dsf.establish_database_connection(weatherStationSettings));
}

TEST(DynamicSensorFactory, store_incoming_data_in_database)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
                         +" port = " + to_string(weatherStationSettings.db.port) + " connect_timeout=1";

    weatherStationSettings.db.enable = true;

    pqxx::connection TEST_CONNECTOR(db_conn_str);

    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);

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
    pqxx::result testWorker_result = w2.exec("select * from readings;");
    EXPECT_EQ(testWorker_result.size(), 0);
    w2.commit();

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
}

TEST(DynamicSensorFactory, get_average_hour_for_reading)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
                         +" port = " + to_string(weatherStationSettings.db.port) + " connect_timeout=10";

    weatherStationSettings.db.enable = true;

    pqxx::connection TEST_CONNECTOR(db_conn_str);

    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);

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
    pqxx::result testWorker_result = w2.exec("select * from readings;");
    EXPECT_EQ(testWorker_result.size(), 0);
    w2.commit();

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

    mySen->update_AvgReadings();

    string avg_sen1 = Utilities::to_string_with_precision<float>(mySen->getReading_ptr("1.0")->day_cr.average, 1);
    string avg_sen2 = Utilities::to_string_with_precision<float>(mySen->getReading_ptr("1.1")->day_cr.average, 1);

    EXPECT_STREQ(avg_sen1.c_str(), "12.6");
    EXPECT_STREQ(avg_sen2.c_str(), "22.3");

    w.commit();
}