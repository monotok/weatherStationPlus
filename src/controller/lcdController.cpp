//
// Created by hammer on 21/09/2019.
//

#include "../../include/lcdController.h"

//TODO: Add check to only create page when there is no page with sensor id or type
//TODO: Set the values to blank string or zero
void LcdController::createWeatherPage(WeatherSensor* ws)
{
    lock_guard<mutex> guard(lcdcMu);
    if( ! existingWeatherPage(ws->get_sensorID()))
    {
        Pageitem sensorID = {"sensorID", 1, 0, FIXED, "SensorID:"};
        Pageitem sensorID_val = {"sensorID",1, 10, VAR, ws->get_sensorName()};
        Pageitem temp = {"temp", 2,0,FIXED, "Temp"};
        Pageitem temp_val = {"temp", 2,5,VAR, ws->get_temperature()};
        Pageitem hum;
        if(strcmp(ws->get_sensorID().c_str(), "Here") == 0)
            hum = {"hum", 2,11,FIXED, "Hum"};
        else
            hum = {"hum", 2,11,FIXED, "Bat"};
        Pageitem hum_val = {"hum", 2,15,VAR, ws->get_humidity()};

        vector<Pageitem> items{sensorID, sensorID_val, temp, temp_val, hum, hum_val};

        pages_map.insert(std::pair<string,vector<Pageitem>>(ws->get_sensorID(),items));
        LOG(INFO) << "Created a new weather page for SensorID: " << ws->get_sensorID() << " SensorName: "
                  << ws->get_sensorName() << endl;
    }
}

//TODO: Need to find a better solution for ignoring non weather pages.
string LcdController::getNextPage(string CurrentPage)
{
    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find(CurrentPage);
    if(pm_iter != pages_map.end())
    {
        pm_iter = next(pm_iter, 1);
        if(pm_iter == pages_map.end())
        {
            pm_iter = pages_map.begin();
        } else if(pm_iter->first == "date")
        {
            pm_iter = next(pm_iter, 1);
            if(pm_iter == pages_map.end())
            {
                pm_iter = pages_map.begin();
            }
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

void LcdController::drawElementToLCD(LcdDriver &lcd)
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
            pi_iter->value = Utilities::to_string_with_precision<float>(valFloat, 0);
        }
    }
}

void LcdController::checkValuesFitLcd(float newValue, LcdDriver &lcd)
{
    float valFloat = stof(pi_iter->value);
    if(valFloat > 9.99 && newValue < 10.0)
    {
        lcd.clearColumnsRowCol(pi_iter->row_start, pi_iter->col_start+5, pi_iter->col_start);
    }
}

// TODO: Need to prevent left over chars on display when writing a new smaller value
void LcdController::drawPage(string SensorName, LcdDriver &lcd)
{
    lock_guard<mutex> guard(lcdcMu);
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

void LcdController::updatePageValues(WeatherSensor* ws, LcdDriver &lcd)
{
    lock_guard<mutex> guard(lcdcMu);
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
                    if (pi_iter->value != ws->get_temperature())
                    {
                        checkValuesFitLcd(ws->get_temperature_float(), lcd);
                        pi_iter->value = ws->get_temperature();
                        checkValuesFitLcd();
                        drawElementToLCD(lcd);
                    }
                }
                if(pi_iter->id == "hum")
                {
                    if (pi_iter->value != ws->get_humidity())
                    {
                        checkValuesFitLcd(ws->get_humidity_float(), lcd);
                        pi_iter->value = ws->get_humidity();
                        checkValuesFitLcd();
                        drawElementToLCD(lcd);
                    }
                }
            }
        }
    }
}

void LcdController::createDateTimePage()
{
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string date_str(30, '\0');
    struct tm now{};
    strftime(&date_str[0], date_str.size(), "%d-%m-%y-%H-%M-%S", localtime_r(&timenow, &now));
    string delimiter = "-";
    vector<string> dateelements;

    Utilities::split_string(date_str, dateelements, '-');

    Pageitem date = {"date", 1, 0, FIXED, "Date: "};
    Pageitem date_day = {"day",1, 6, VAR, dateelements[0]};
    Pageitem date_delimiter_1 = {"delimiter",1, 8, FIXED, "-"};
    Pageitem date_month = {"month",1, 9, VAR, dateelements[1]};
    Pageitem date_delimiter_2 = {"delimiter",1, 11, FIXED, "-"};
    Pageitem date_year = {"year",1, 12, VAR, dateelements[2]};

    Pageitem time = {"time", 2,0,FIXED, "Time: "};
    Pageitem time_hour = {"hour", 2,6,VAR, dateelements[3]};
    Pageitem time_delimiter_1 = {"delimiter",2, 8, FIXED, ":"};
    Pageitem time_min = {"min", 2,9,VAR, dateelements[4]};
    Pageitem time_delimiter_2 = {"delimiter",2, 11, FIXED, ":"};
    Pageitem time_sec = {"sec", 2,12,VAR, dateelements[5]};

    vector<Pageitem> items{date, date_day, date_delimiter_1, date_month, date_delimiter_2,
                           date_year, time, time_delimiter_1, time_hour, time_delimiter_2, time_min, time_sec};

    lock_guard<mutex> guard(lcdcMu);
    pages_map.insert(std::pair<string,vector<Pageitem>>("date",items));
    LOG(INFO) << "Created a new page " << "date" << endl;
}

void LcdController::updateDateTimePage(LcdDriver &lcd)
{
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string date_str(30, '\0');
    struct tm now{};
    strftime(&date_str[0], date_str.size(), "%d-%m-%y-%H-%M-%S", localtime_r(&timenow, &now));
    string delimiter = "-";
    vector<string> dateelements;

    Utilities::split_string(date_str, dateelements, '-');

    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find("date");
    if(pm_iter != pages_map.end())
    {
        for(pi_iter = pm_iter->second.begin(); pi_iter != pm_iter->second.end(); pi_iter++)
        {
            if(pi_iter->type == VAR)
            {
                if(pi_iter->id == "sec")
                {
                    if (pi_iter->value != dateelements[5])
                    {
                        pi_iter->value = dateelements[5];
                        drawElementToLCD(lcd);
                    }
                }
                if(pi_iter->id == "min")
                {
                    if (pi_iter->value != dateelements[4])
                    {
                        pi_iter->value = dateelements[4];
                        drawElementToLCD(lcd);
                    }
                }
                if(pi_iter->id == "hour")
                {
                    if (pi_iter->value != dateelements[3])
                    {
                        pi_iter->value = dateelements[3];
                        drawElementToLCD(lcd);
                    }
                }
                if(pi_iter->id == "day")
                {
                    if (pi_iter->value != dateelements[2])
                    {
                        pi_iter->value = dateelements[2];
                        drawElementToLCD(lcd);
                    }
                }
                if(pi_iter->id == "month")
                {
                    if (pi_iter->value != dateelements[1])
                    {
                        pi_iter->value = dateelements[1];
                        drawElementToLCD(lcd);
                    }
                }
                if(pi_iter->id == "year")
                {
                    if (pi_iter->value != dateelements[0])
                    {
                        pi_iter->value = dateelements[0];
                        drawElementToLCD(lcd);
                    }
                }
            }
        }
    }
}

void LcdController::drawDateTimePage(LcdDriver &lcd)
{
    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find("date");
    if(pm_iter != pages_map.end())
    {
        for(pi_iter = pm_iter->second.begin(); pi_iter != pm_iter->second.end(); pi_iter++)
        {
            drawElementToLCD(lcd);
        }
    }
}
