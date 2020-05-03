#ifndef SENSOR_H
#define SENSOR_H

//This is an abstract class

#include <string>

using namespace std;

class Sensor
{
  protected:
    string sensorID;
    string sensorName;
    string sensorType;

  public:
    Sensor(string sensorID, string sensorName, string sensorType)
                { set_sensorID(sensorID); set_sensorName(sensorName); set_sensorType(sensorType); };
    virtual ~Sensor(){};

    void set_sensorID(string sid) { this->sensorID = sid; }
    void set_sensorName(string sname) { this->sensorName = sname; }
    void set_sensorType(string sensorType) { this->sensorType = sensorType; }
    string get_sensorID() { return this->sensorID; }
    string get_sensorName(){
        if(this->sensorName.compare("NotSet") == 0) {
            return this->sensorID;
        }
        else {
            return this->sensorName;
        }
    }
    string get_sensorType() { return this->sensorType; }
};

#endif //SENSOR_H