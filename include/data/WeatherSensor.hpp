#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"
#include "../Utilities.hpp"
#include "../easylogging++.hpp"
#include <string>
#include "vector"

using namespace std;

class WeatherSensor : public Sensor
{
public:

    struct Data {
        string readingId = {};
        float reading = {};
        string type = {};
        string unit = {};
        Data(string readingId) {
            this->readingId = readingId;
        }
    };

    //TODO: Delete these
    float temperature;
    float humidity;
    char tempUnit = 'c';

    WeatherSensor(string sensorID, string sensorName, string sensorType) : Sensor(sensorID, sensorName, sensorType) {};
    ~WeatherSensor(){};

    Data *getReading_ptr(string readingID);
    WeatherSensor::Data *createNewSensorReading_obj(string readingID);
    vector<Data *> getAvailableReadings();

    //Sensor Values
    string get_temperature();
    string get_humidity();

    float get_temperature_float() { return this->temperature/100; }
    float get_humidity_float() { return this->humidity/100; }

    //Sensor Values
    void set_reading(string readingId, string type, float reading, string unit);

    //Sensor Units
    void set_tempUnit_to_C();
    void set_tempUnit_to_F();
    void switch_tempUnit();
    char getTempUnit();

private:

    // We only support 6 sensor readings per sensor as this fits on a 20x4 display
    vector<Data *> readings_vector = {};
    vector<Data *>::iterator readingsIterator;
    void addNewReadingArray(Data *newReading);
};

#endif // TEMP_SENSOR_H