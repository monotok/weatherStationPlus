//
// Created by hammer on 21/09/2019.
//

#ifndef WEATHERSTATIONPLUS_LCDCONTROLLER_H
#define WEATHERSTATIONPLUS_LCDCONTROLLER_H

#include <gtest/gtest_prod.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
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
    void drawPage(string SensorName, LcdDriver &lcd);
    void updatePageValues(WeatherSensor* ws, LcdDriver &lcd);
    string getNextPage(string CurrentPage);
    void createDateTimePage();
    void drawDateTimePage(LcdDriver &lcd);
    void updateDateTimePage(LcdDriver &lcd);

private:
    struct Pageitem {
        string id;
        int row_start;
        int col_start;
        int type;
        string value;
    };

    FRIEND_TEST(LcdController, create_new_weather_page_struct_independant);
    FRIEND_TEST(LcdController, update_values_only_on_existing_page);
    FRIEND_TEST(LcdController, check_for_existing_weather_sensor);

    bool existingWeatherPage(string SensorName);
    void drawElementToLCD(LcdDriver &lcd);
    void checkValuesFitLcd();
    void checkValuesFitLcd(float newValue, LcdDriver &lcd);

    map<string, vector<Pageitem>> pages_map;
    map<string, vector<Pageitem>>::iterator pm_iter;
    vector<Pageitem>::iterator pi_iter;

    mutex lcdcMu;
};

#endif //WEATHERSTATIONPLUS_LCDCONTROLLER_H
