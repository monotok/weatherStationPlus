#ifndef SENSOR_H
#define SENSOR_H

//This is an abstract class

#include "../settings.hpp"
#include <string>

using namespace std;

class Sensor
{
  protected:
    string sensorID;
    string sensorName;
    string sensorType;
    Position posName = {1, 0};
    Position posTitle = {1, 9};

  public:
    Sensor(string sensorID, string sensorName, string sensorType)
                { set_sensorID(sensorID); set_sensorName(sensorName); set_sensorType(sensorType); };
    virtual ~Sensor(){};

    void set_sensorID(string sid) { this->sensorID = sid; }
    void set_sensorName(string sname) { this->sensorName = sname; }
    void set_sensorType(string sensorType) { this->sensorType = sensorType; }
    Position& get_Position_SensorName() { return posName; };
    Position& get_Position_Title() { return posTitle; };

    string get_sensorID() {
        string::size_type pos = this->sensorID.find(".");
        if (pos != string::npos) {
            return this->sensorID.substr(0, pos);
        }
        return this->sensorID;
    }
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