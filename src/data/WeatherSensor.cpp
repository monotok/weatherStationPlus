#include "../../include/data/WeatherSensor.hpp"


string WeatherSensor::get_temperature()
{
    if(getTempUnit() == 'c')
        return Utilities::to_string_with_precision<float>(this->temperature/100, 1)+"C";
    return Utilities::to_string_with_precision<float>(((this->temperature/100)*9/5)+32, 1)+"F";
}
string WeatherSensor::get_humidity()
{
    return Utilities::to_string_with_precision<float>(this->humidity/100, 1)+"%";
}

void WeatherSensor::set_tempUnit_to_C()
{
    this->tempUnit = 'c';
}

void WeatherSensor::set_tempUnit_to_F()
{
    this->tempUnit = 'f';
}

void WeatherSensor::switch_tempUnit()
{
    if(this->tempUnit == 'f')
        this->tempUnit = 'c';

    else if(this->tempUnit == 'c')
        this->tempUnit = 'f';
}

char WeatherSensor::getTempUnit()
{
    return this->tempUnit;
}


