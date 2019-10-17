//
// Created by hammer on 21/09/2019.
//

#include "../../include/lcdController.h"

//TODO: Add check to only create page when there is no page with sensor id or type
//TODO: Set the values to blank string or zero
void LcdController::createWeatherPage(WeatherSensor* ws)
{
    if( ! existingWeatherPage(ws->get_sensorID()))
    {
        Pageitem sensorID = {"sensorID", 1, 0, FIXED, "SensorID:"};
        Pageitem sensorID_val = {"sensorID",1, 10, VAR, ws->get_sensorID()};
        Pageitem temp = {"temp", 2,0,FIXED, "Temp"};
        Pageitem temp_val = {"temp", 2,5,VAR,
                             Utilities::to_string_with_precision<float>(ws->get_temperature(), 2)};
        Pageitem hum = {"hum", 2,11,FIXED, "Hum"};
        Pageitem hum_val = {"hum", 2,15,VAR,
                            Utilities::to_string_with_precision<float>(ws->get_humidity(), 2)};

        vector<Pageitem> items{sensorID, sensorID_val, temp, temp_val, hum, hum_val};

        pages_map.insert(std::pair<string,vector<Pageitem>>(ws->get_sensorID(),items));
    }
}

string LcdController::getNextPage(string CurrentPage)
{
    pm_iter = pages_map.find(CurrentPage);
    if(pm_iter != pages_map.end())
    {
        pm_iter = next(pm_iter, 1);
        if(pm_iter == pages_map.end())
        {
            pm_iter = pages_map.begin();
        }
        return pm_iter->first;
    }
    return "No Pages";
}

bool LcdController::existingWeatherPage(string SensorName)
{
    pm_iter = pages_map.find(SensorName);
    return pm_iter != pages_map.end();
}

void LcdController::drawElementToLCD(LcdDriver lcd)
{
    lcd.setCursorPositionRowCol(pi_iter->row_start, pi_iter->col_start);
    lcd.lcdString(pi_iter->value.c_str());
}

void LcdController::checkValuesFitLcd()
{
    if(pi_iter->type == VAR && ((strcmp(pi_iter->id.c_str(), "temp") == 0) || (strcmp(pi_iter->id.c_str(), "hum") == 0)))
    {
        float valFloat = stof(pi_iter->value);
        if(valFloat > 99.99){
            pi_iter->value = Utilities::to_string_with_precision<float>(valFloat, 1);
        }
    }
}

void LcdController::checkValuesFitLcd(float newValue, LcdDriver lcd)
{
    float valFloat = stof(pi_iter->value);
    if(valFloat > 9.99 && newValue < 10.0)
    {
        lcd.clearColumnsRowCol(pi_iter->row_start, pi_iter->col_start+5, pi_iter->col_start);
    }
}

// TODO: Need to prevent left over chars on display when writing a new smaller value
void LcdController::drawPage(string SensorName, LcdDriver lcd)
{
    pm_iter = pages_map.find(SensorName);
    if(pm_iter != pages_map.end())
    {
        for(pi_iter = pm_iter->second.begin(); pi_iter != pm_iter->second.end(); pi_iter++)
        {
            checkValuesFitLcd();
            drawElementToLCD(lcd);
        }
    } else
    {
        string msg = "No such sensor " + SensorName;
        lcd.lcdString(msg.c_str());
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
                    checkValuesFitLcd();
                    if (pi_iter->value != Utilities::to_string_with_precision<float>(ws->get_temperature(), 2))
                    {
                        checkValuesFitLcd(ws->get_temperature(), lcd);
                        pi_iter->value = Utilities::to_string_with_precision<float>(ws->get_temperature(), 2);
                        checkValuesFitLcd();
                        drawElementToLCD(lcd);
                    }
                }
                if(pi_iter->id == "hum")
                {
                    if (pi_iter->value != Utilities::to_string_with_precision<float>(ws->get_humidity(), 2))
                    {
                        checkValuesFitLcd(ws->get_humidity(), lcd);
                        pi_iter->value = Utilities::to_string_with_precision<float>(ws->get_humidity(), 2);
                        checkValuesFitLcd();
                        drawElementToLCD(lcd);
                    }
                }
            }
        }
    }
}
