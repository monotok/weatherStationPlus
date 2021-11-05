#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"
#include "../Utilities.hpp"
#include "../easylogging++.hpp"
#include "../configParser.hpp"
#include "../Constants.hpp"
#include <string>
#include "vector"
#include <pqxx/pqxx>

using namespace std;

class WeatherSensor : public Sensor
{
public:

    struct Data {
        string readingId = {};
        float prev_reading = {};
        float reading = {};
        struct readings_from_db {
            float average = 0.0;
            float maximum = 0.0;
            float minimum = 0.0;
            string time_period = {};
            readings_from_db(string time_period) : time_period(time_period) {};
        };
        readings_from_db day_cr, day_pr, week_cr, week_pr, month_cr, month_pr, year_cr, year_pr;
        vector<readings_from_db*> readings_from_db_instances = {&day_cr, &week_cr, &month_cr, &year_cr};
        std::map<short, readings_from_db*> timeframeConstMapping = {make_pair(LcdConstants::ONE_DAY, &day_cr),
                                                                    make_pair(LcdConstants::ONE_WEEK, &week_cr),
                                                                    make_pair(LcdConstants::ONE_MONTH, &month_cr),
                                                                    make_pair(LcdConstants::ONE_YEAR, &year_cr),
                                                                    };
        string name = {};
        Position posName = {0, 0};
        Position posVal = {0, 0};
        Data(string readingId) : readingId(readingId), day_cr("1 day"), day_pr("1 day"),
        week_cr("1 week"), week_pr("1 week"), month_cr("4 week"), month_pr("4 week"),
        year_cr("52 week"), year_pr("52 week") {}
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
    void update_AvgReadings();
    void update_MinReadings();
    void update_MaxReadings();

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

};

#endif // TEMP_SENSOR_H