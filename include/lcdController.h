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
#include "../include/Utilities.hpp"

#define FIXED 0
#define VAR 1

using namespace std;

class LcdController
{
public:
    void createWeatherPage(WeatherSensor* ws);
    void drawPage(string SensorName, LcdDriver lcd);
    void updatePageValues(WeatherSensor* ws, LcdDriver lcd);
    string getNextPage(string CurrentPage);

private:
    struct Pageitem {
        string id;
        int row_start;
        int col_start;
        int type;
        string value;
    };

    bool existingWeatherPage(string SensorName);
    void drawElementToLCD(LcdDriver lcd);
    void checkValuesFitLcd();
    void checkValuesFitLcd(float newValue, LcdDriver lcd);

    map<string, vector<Pageitem>> pages_map;
    map<string, vector<Pageitem>>::iterator pm_iter;
    vector<Pageitem>::iterator pi_iter;

};

#endif //WEATHERSTATIONPLUS_LCDCONTROLLER_H
