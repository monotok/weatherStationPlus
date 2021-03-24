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
#include "../include/Constants.hpp"

using namespace std;

class LcdController
{
public:
    LcdController(LcdDriver& lcd, const Settings& weatherStationSettings);
    void createWeatherPage(WeatherSensor* ws, WeatherSensor::Data* reading);
    void createWeatherAvgPage(WeatherSensor* ws, WeatherSensor::Data* reading);
    void drawPage_Locking();
    void drawPage_NonLocking();
    void drawPage();
    void updatePageValues(WeatherSensor *ws);
    void getNextPage();
    void getNextSubPage();
    void createDateTimePage();
    void drawDateTimePage();
    void updateDateTimePage();
    void clearDisplay();
    void setCurrentPage(string pageName);
    void setCurrentSubPage(string pageName);
    void getNextTimeframe();
    string getCurrentPage();
    string getCurrentSubPage();

private:
    LcdDriver lcd;
    const Settings& weatherStationSettings;

    struct Pageitem {
        string id;
        Position pos;
        int type;
        string str_val = {};
        WeatherSensor::Data* data = nullptr;
        Pageitem(string id, Position pos, int type, string data) : id(id), pos(pos), type(type), str_val(data) {}
        Pageitem(string id, Position pos, int type, WeatherSensor::Data* data) : id(id), pos(pos), type(type), data(data) {}
    };

    string currentPage = {};
    string currentSubPage = {"current"};
    short currentDBTimeframe = LcdConstants::ONE_DAY;

    FRIEND_TEST(LcdController, create_new_weather_page_struct_independant);
    FRIEND_TEST(LcdController, create_new_datetime_page_struct_independant);
    FRIEND_TEST(LcdController, update_only_changed_values_datetime);
    FRIEND_TEST(LcdController, update_values_only_on_existing_page);
    FRIEND_TEST(LcdController, update_values_only_on_existing_avg_page);
    FRIEND_TEST(LcdController, check_for_existing_weather_sensor);
    FRIEND_TEST(LcdController, draw_custom_character_battery_symbols);
    FRIEND_TEST(LcdController, get_next_timeframe);

    bool existingWeatherPage(string SensorName);
    bool existingWeatherPageReading(string SensorName, string reading);
    bool existingWeatherAvgPage(string SensorName, string readingName);
    void drawElementToLCD();
    float getCorrectReadingDataValueToDraw();
    void drawBatteryFullSymbol();
    void drawBatteryHalfSymbol();
    void drawBatteryEmptySymbol();
    string checkValuesFitLcd();
    void clearOldValuesFromLcd();
    void createNewReading(WeatherSensor* ws, WeatherSensor::Data* reading);

    //Custom Chars
    void createFullBattery();
    void createHalfBattery();
    void createEmptyBattery();

    void buildAllCustomChars();

    map<string, map<string, vector<Pageitem>>> pages_map;
    map<string, map<string, vector<Pageitem>>>::iterator pm_iter;
    map<string, vector<Pageitem>>::iterator spm_iter;
    vector<Pageitem>::iterator pi_iter;

    vector<short> timeframes = {LcdConstants::ONE_DAY, LcdConstants::ONE_WEEK, LcdConstants::ONE_MONTH, LcdConstants::ONE_YEAR};
    vector<short>::iterator tf_iter = timeframes.begin();

    mutex lcdcMu;

};

#endif //WEATHERSTATIONPLUS_LCDCONTROLLER_H
