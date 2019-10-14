//
// Created by hammer on 21/09/2019.
//

#include "../../include/lcdController.h"

//TODO: Add check to only create page when there is no page with sensor id or type
//TODO: Set the values to blank string or zero
void LcdController::createWeatherPage(WeatherSensor* ws)
{
    Pageitem sensorID = {"sensorID", 1, 0, FIXED, "SensorID:"};
    Pageitem sensorID_val = {"sensorID",1, 10, VAR, ws->get_sensorID()};
    Pageitem temp = {"temp", 2,0,FIXED, "Temp"};
    Pageitem temp_val = {"temp", 2,5,VAR,
                         Utilities::to_string_with_precision<float>(ws->get_temperature())};
    Pageitem hum = {"hum", 2,11,FIXED, "Hum"};
    Pageitem hum_val = {"hum", 2,15,VAR,
                        Utilities::to_string_with_precision<float>(ws->get_humidity())};

    vector<Pageitem> items{sensorID, sensorID_val, temp, temp_val, hum, hum_val};

    pages_map.insert(std::pair<string,vector<Pageitem>>(ws->get_sensorID(),items));
}

// TODO: Need to prevent left over chars on display when writing a new smaller value
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

void LcdController::updatePageValues(WeatherSensor* ws, LcdDriver lcd)
{
    pm_iter = pages_map.find(ws->get_sensorID());
    if(pm_iter != pages_map.end())
    {
        for(pi_iter = pm_iter->second.begin(); pi_iter != pm_iter->second.end(); pi_iter++)
        {
            if(pi_iter->type == VAR)
            {
                if(pi_iter->id == "temp")
                {
                    if (pi_iter->value != Utilities::to_string_with_precision<float>(ws->get_temperature()))
                    {
                        pi_iter->value = Utilities::to_string_with_precision<float>(ws->get_temperature());
                        lcd.setCursorPositionRowCol(pi_iter->row_start, pi_iter->col_start);
                        lcd.lcdString(pi_iter->value.c_str());
                    }
                }
                if(pi_iter->id == "hum")
                {
                    if (pi_iter->value != Utilities::to_string_with_precision<float>(ws->get_humidity()))
                    {
                        pi_iter->value = Utilities::to_string_with_precision<float>(ws->get_humidity());
                        lcd.setCursorPositionRowCol(pi_iter->row_start, pi_iter->col_start);
                        lcd.lcdString(pi_iter->value.c_str());
                    }
                }
            }
        }
    }
}