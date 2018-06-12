#ifndef SENSOR_H
#define SENSOR_H

//This is an abstract class

#include <string>

using namespace std;

class Sensor
{
    protected:
        string sensorID;
        string type;        
    public:
        virtual ~Sensor() {};
        string get_sensorID() {return this->sensorID;}
        void set_sensorID(string sensorID) {this->sensorID = sensorID;}       
        virtual string get_sensorType() =0;
        virtual void set_sensorType(string type) =0;

        bool operator< (Sensor& sensorObj)
        {
            if(sensorObj.sensorID < this->sensorID)
            {
                return true;
            }
        }        
};

#endif //SENSOR_H