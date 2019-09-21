#ifndef SENSOR_H
#define SENSOR_H

//This is an abstract class

#include <string>

using namespace std;

class Sensor
{
  protected:
    string sensorName;
    string sensorType;

  public:
    Sensor(string sensorName, string sensorType){ set_sensorID(sensorName); set_sensorType(sensorType); };
    virtual ~Sensor(){};
    virtual void persistData() = 0;

    void set_sensorID(string sensorName) { this->sensorName = sensorName; }
    void set_sensorType(string sensorType) { this->sensorType = sensorType; }
    string get_sensorID() { return this->sensorName; }
    string get_sensorType() { return this->sensorType; }
};

#endif //SENSOR_H