#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Abc_Sensor.hpp"

class TemperatureSensor: public Sensor
{
    private:
        float temperature;
    public:
        ~TemperatureSensor() {};
        string get_sensorType() override {return this->type;}
        void set_sensorType(string type) override {this->type = type;} 
};

#endif // TEMP_SENSOR_H