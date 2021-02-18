#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"
#include "../Utilities.hpp"
#include "../easylogging++.hpp"
#include "../configParser.hpp"
#include <string>
#include "vector"
#include <pqxx/pqxx>

using namespace std;

class WeatherSensor : public Sensor
{
public:

    struct Data {
        string readingId = {};
        float reading = {};
        string name = {};
        Position posName = {0, 0};
        Position posVal = {0, 0};
        Data(string readingId) {
            this->readingId = readingId;
        }
    };

    WeatherSensor(string sensorID, string sensorName) : Sensor(sensorID, sensorName) {};
    WeatherSensor(string sensorID, string sensorName, pqxx::connection* db_conn_ptr) : Sensor(sensorID, sensorName) {
        this->C = db_conn_ptr;
    };
    ~WeatherSensor(){};

    Data *getReading_ptr(string readingID);
    WeatherSensor::Data *createNewSensorReading_obj(string readingID);
    vector<Data *> getAvailableReadings();

    //Database
    void store_weathersensor_data_in_database(WeatherSensor::Data *reading);
    void store_weathersensormetadata_data_in_database(WeatherSensor::Data *reading);


    //Sensor Values
    string get_Reading(Data* reading);
    string get_Reading(string readingid);
    string get_AvgReading(string readingid, string timeperiod);
    string get_MinReading(string readingid, string timeperiod);
    string get_MaxReading(string readingid, string timeperiod);

    //Sensor Values
    WeatherSensor::Data *set_reading(string readingId, float reading, ConfigParser *wss);

private:

    // We only support 6 sensor readings per sensor as this fits on a 20x4 display
    vector<Data *> readings_vector = {};
    vector<Data *>::iterator readingsIterator;
    void addNewReadingArray(Data *newReading);
    void setLcdReadingPosition(Data& data, string sensorId, string readingId, ConfigParser* wss);

    // Database stuff
    pqxx::connection* C = nullptr;

    // Function pointer to be able to call correct db request from just name
    typedef string (WeatherSensor::*FnPtr)(string, string);
    std::map<std::string, FnPtr> db_func_map = {
            {"avg", &WeatherSensor::get_AvgReading},
            {"min", &WeatherSensor::get_MinReading},
            {"max", &WeatherSensor::get_MaxReading}
    };
};

#endif // TEMP_SENSOR_H