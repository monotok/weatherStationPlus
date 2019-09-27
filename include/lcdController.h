//
// Created by hammer on 21/09/2019.
//

#ifndef WEATHERSTATIONPLUS_LCDCONTROLLER_H
#define WEATHERSTATIONPLUS_LCDCONTROLLER_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "data/WeatherSensor.hpp""
#include "../include/i2cControl.hpp"
#include "../include/lcdDriver.hpp"

//#define lcdAdd 0x3f // I2C device address
//#define i2cbusno 1

using namespace std;

class LcdController
{
public:
    void createWeatherPage(WeatherSensor* ws);
    void drawPage(string SensorName, LcdDriver lcd);
    void updatePageValues();
    void removePage();

private:
    struct Pageitem {
        int row_start;
        int col_start;
        string type;
        string value;
    };

    map<string, vector<Pageitem>> pages_map;
    map<string, vector<Pageitem>>::iterator pm_iter;
    vector<Pageitem>::iterator pi_iter;

};

#endif //WEATHERSTATIONPLUS_LCDCONTROLLER_H
