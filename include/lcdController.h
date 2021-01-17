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
#include "data/WeatherSensor.hpp"
#include "../include/i2cControl.hpp"
#include "../include/lcdDriver.hpp"
#include "../include/Utilities.hpp"
#include "../include/settings.hpp"

#define FIXED 0
#define VAR 1

using namespace std;

class LcdController
{
public:
    LcdController(LcdDriver& lcd);
    void createWeatherPage(WeatherSensor* ws, WeatherSensor::Data* reading);
    void drawPage_Locking();
    void drawPage_NonLocking();
    void drawPage();
    void updatePageValues(WeatherSensor *ws);
    void getNextPage();
    void createDateTimePage();
    void drawDateTimePage();
    void updateDateTimePage();
    void clearDisplay();
    void setCurrentPage(string pageName);
    string getCurrentPage();

private:
    LcdDriver lcd;

    struct Pageitem {
        string id;
        Position pos;
        int type;
        string value;
    };

    string currentPage = {};

    FRIEND_TEST(LcdController, create_new_weather_page_struct_independant);
    FRIEND_TEST(LcdController, create_new_datetime_page_struct_independant);
    FRIEND_TEST(LcdController, update_only_changed_values_datetime);
    FRIEND_TEST(LcdController, update_values_only_on_existing_page);
    FRIEND_TEST(LcdController, check_for_existing_weather_sensor);
    FRIEND_TEST(LcdController, draw_custom_character_battery_symbols);

    bool existingWeatherPage(string SensorName);
    bool existingWeatherPageReading(string SensorName, string reading);
    void drawElementToLCD();
    void drawBatteryFullSymbol();
    void drawBatteryHalfSymbol();
    void drawBatteryEmptySymbol();
    void checkValuesFitLcd();
    void checkValuesFitLcd(string newValue);
    void createNewReading(WeatherSensor* ws, WeatherSensor::Data* reading);

    //Custom Chars
    void createFullBattery();
    void createHalfBattery();
    void createEmptyBattery();

    void buildAllCustomChars();

    map<string, vector<Pageitem>> pages_map;
    map<string, vector<Pageitem>>::iterator pm_iter;
    vector<Pageitem>::iterator pi_iter;

    mutex lcdcMu;
};

#endif //WEATHERSTATIONPLUS_LCDCONTROLLER_H
