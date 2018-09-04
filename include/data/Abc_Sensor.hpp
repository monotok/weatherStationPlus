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
    virtual ~Sensor(){};
    virtual string get_sensorID() = 0;
    virtual void set_sensorID(string sensorName) = 0;
    virtual string get_sensorType() = 0;
    virtual void persistData() = 0;
};

#endif //SENSOR_H