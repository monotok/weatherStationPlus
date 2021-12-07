#ifndef DYNAMIC_SEN_FACTORY_H
#define DYNAMIC_SEN_FACTORY_H

#include <gtest/gtest_prod.h>
#include "../easylogging++.hpp"
#include "WeatherSensor.hpp"
#include "../configParser.hpp"
#include <vector>
#include <mutex>

class DynamicSensorFactory
{
  public:
    DynamicSensorFactory(ConfigParser& wss);
    ~DynamicSensorFactory();
    bool establish_database_connection(Settings& settings);
    WeatherSensor* getWeatherSensor_ptr(string sensorID);
    vector<WeatherSensor *> getAllWeatherSensors_ptr();
    void updateAllWeatherSensorsDatabaseValues();

private:
    Sensor *CreateNewSensor_obj(string sensorID);
    vector<WeatherSensor *> weatherSensors_vector;
    vector<WeatherSensor *>::iterator weatherSensorIterator;

    FRIEND_TEST(DynamicSensorFactory, Add_new_weatherSensor_obj_to_vector);
    FRIEND_TEST(DynamicSensorFactory, Find_existing_sensor);
    FRIEND_TEST(DynamicSensorFactory, get_all_weathersensors);
    FRIEND_TEST(DynamicSensorFactory, store_incoming_data_in_database);
    FRIEND_TEST(DynamicSensorFactory, get_average_hour_for_reading);

    mutex dyfMu;
    ConfigParser& wss;
    pqxx::connection* db_conn_ptr = nullptr;

    void prepare_select_statements();
    void prepare_insert_statements();
};

#endif //ifndef DYNAMIC_SEN_FACTORY_H