#ifndef RETRIEVE_SEN_DATA_H
#define RETRIEVE_SEN_DATA_H
#include "data/Abc_Sensor.hpp"

class RetrieveSenData
{
  public:
    Sensor *getRemoteSenData();
    Sensor *getLocalSenData();

  private:
    union convertSensorClassChar;
};

#endif //ifndef RETRIEVE_SEN_DATA_H