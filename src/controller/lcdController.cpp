//
// Created by hammer on 21/09/2019.
//

#include "../../include/lcdController.h"

void LcdController::createWeatherPage(WeatherSensor* ws)
{
    Pageitem sensorID = {1, 0, "fixed", "SensorID:"};
    Pageitem sensorID_val = {1, 10, "var", ws->get_sensorID()};
    Pageitem temp = {2,0,"fixed", "Temp"};
    Pageitem temp_val = {2,5,"var", Utilities::to_string_with_precision<float>(ws->get_temperature())};
    Pageitem hum = {2,11,"fixed", "Hum"};
    Pageitem hum_val = {2,15,"var", Utilities::to_string_with_precision<float>(ws->get_humidity())};

    vector<Pageitem> items{sensorID, sensorID_val, temp, temp_val, hum, hum_val};

    pages_map.insert(std::pair<string,vector<Pageitem>>(ws->get_sensorID(),items));
}

void LcdController::drawPage(string SensorName, LcdDriver lcd)
{
    pm_iter = pages_map.find(SensorName);
    if(pm_iter != pages_map.end())
    {
        for(pi_iter = pm_iter->second.begin(); pi_iter != pm_iter->second.end(); pi_iter++)
        {
            lcd.setCursorPositionRowCol(pi_iter->row_start, pi_iter->col_start);
            lcd.lcdString(pi_iter->value.c_str());
        }
    }
}