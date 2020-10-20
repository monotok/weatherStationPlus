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
        string type = {};
        string unit = {};
        string name = {};
        Position posName = {0, 0};
        Position posVal = {0, 0};
        Data(string readingId) {
            this->readingId = readingId;
        }
    };

    WeatherSensor(string sensorID, string sensorName, string sensorType) : Sensor(sensorID, sensorName, sensorType) {};
    ~WeatherSensor(){};

    Data *getReading_ptr(string readingID);
    WeatherSensor::Data *createNewSensorReading_obj(string readingID);
    vector<Data *> getAvailableReadings();

    //Database
    void store_weathersensor_data_in_database(Data *reading, pqxx::connection &C);
    void store_weathersensormetadata_data_in_database(Data *sensor, pqxx::connection &C);


    //Sensor Values
    string get_Reading(Data* reading);
    string get_Reading(string reading);

    //Sensor Values
    Data* set_reading(string readingId, string type, float reading, string unit, pqxx::connection* C, ConfigParser* wss);

private:

    // We only support 6 sensor readings per sensor as this fits on a 20x4 display
    vector<Data *> readings_vector = {};
    vector<Data *>::iterator readingsIterator;
    void addNewReadingArray(Data *newReading);
    void setLcdReadingPosition(Data& data, string sensorId, string readingId, ConfigParser* wss);
};

#endif // TEMP_SENSOR_H