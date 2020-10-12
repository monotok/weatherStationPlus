//
// Created by hammer on 21/09/2019.
//

#include "../../include/lcdController.h"

//TODO: Add check to only create page when there is no page with sensor id or type
//TODO: Set the values to blank string or zero
// Get the unit and type from the settings file?
void LcdController::createWeatherPage(WeatherSensor* ws, WeatherSensor::Data* reading)
{
    lock_guard<mutex> guard(lcdcMu);
    if( ! existingWeatherPage(ws->get_sensorID()))
    {
        Pageitem sensorID = {"sensorID", ws->get_Position_SensorName(), FIXED, ws->get_sensorName().append(":")};
        Pageitem sensorID_val = {"pageTitle", ws->get_Position_Title(), FIXED, "Current"};
        vector<Pageitem> items{sensorID, sensorID_val};

        Pageitem item = {reading->readingId, reading->posName, FIXED, reading->name};
        Pageitem item_val = {reading->readingId, reading->posVal, VAR, ws->get_Reading(reading)};
        items.push_back(item);
        items.push_back(item_val);

        pages_map.insert(std::pair<string,vector<Pageitem>>(ws->get_sensorID(),items));
        LOG(INFO) << "Created a new weather page for SensorID: " << ws->get_sensorID() << " SensorName: "
                  << ws->get_sensorName() << endl;
    } else
    {
//        Previous call to existing page and following call to existing page reading sets pm_iter to current page
        createNewReading(ws, reading);
    }
}

void LcdController::createNewReading(WeatherSensor* ws, WeatherSensor::Data* reading)
{
    if (!existingWeatherPageReading(ws->get_sensorID(), reading->readingId))
    {
        Pageitem item = {reading->readingId, reading->posName, FIXED, reading->name};
        Pageitem item_val = {reading->readingId, reading->posVal, VAR, ws->get_Reading(reading)};
        pm_iter->second.push_back(item);
        pm_iter->second.push_back(item_val);
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

bool LcdController::existingWeatherPageReading(string SensorName, string readingid)
{
    pm_iter = pages_map.find(SensorName);
    if (pm_iter != pages_map.end())
    {
        for (auto &pageItem: pm_iter->second)
        {
            if (strcmp(readingid.c_str(), pageItem.id.c_str()) == 0) {
                return true;
            }
        }
    }
    return false;
}

void LcdController::drawElementToLCD(LcdDriver &lcd)
{
    lcd.setCursorPositionRowCol(pi_iter->pos.row_start, pi_iter->pos.col_start);
    lcd.lcdString(pi_iter->value.c_str());
}

void LcdController::checkValuesFitLcd()
{
    if(pi_iter->type == VAR) {
        try {
            float valFloat = stof(pi_iter->value);
            if(valFloat > 99.99){
                pi_iter->value = Utilities::to_string_with_precision<float>(valFloat, 0);
            }
        }
        catch (invalid_argument& ia) {
            LOG(ERROR) << "Error occurred converting value '" << pi_iter->value << "' " << endl;
        }
    }
}

void LcdController::checkValuesFitLcd(string newValue, LcdDriver &lcd)
{
    if(pi_iter->type == VAR) {
        try {
            float currentVal = stof(pi_iter->value);
            if (currentVal > 9.99 && stof(newValue) < 10.0) {
                lcd.clearColumnsRowCol(pi_iter->pos.row_start, pi_iter->pos.col_start + 5, pi_iter->pos.col_start);
            }
        }
        catch (invalid_argument& ia) {
            LOG(ERROR) << "Error occurred converting value '" << pi_iter->value << "' " << endl;
        }
    }
}

// TODO: Need to prevent left over chars on display when writing a new smaller value
void LcdController::drawPage(string sensorId, LcdDriver &lcd)
{
    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find(sensorId);
    if(pm_iter != pages_map.end())
    {
        for(pi_iter = pm_iter->second.begin(); pi_iter != pm_iter->second.end(); pi_iter++)
        {
            checkValuesFitLcd();
            drawElementToLCD(lcd);
        }
    } else
    {
        string msg = "No such sensor " + sensorId;
        LOG(WARNING) << msg << endl;
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
                checkValuesFitLcd();
                auto newReadingValue = ws->get_Reading(pi_iter->id);
                if (pi_iter->value != newReadingValue)
                {
                    checkValuesFitLcd(newReadingValue, lcd);
                    pi_iter->value = newReadingValue;
                    checkValuesFitLcd();
                    drawElementToLCD(lcd);
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

    Pageitem date = {"date", Position(1, 0), FIXED, "Date: "};
    Pageitem date_day = {"day", Position(1, 6), VAR, dateelements[0]};
    Pageitem date_delimiter_1 = {"delimiter", Position(1, 8), FIXED, "-"};
    Pageitem date_month = {"month", Position(1, 9), VAR, dateelements[1]};
    Pageitem date_delimiter_2 = {"delimiter", Position(1, 11), FIXED, "-"};
    Pageitem date_year = {"year", Position(1, 12), VAR, dateelements[2]};

    Pageitem time = {"time", Position(2,0), FIXED, "Time: "};
    Pageitem time_hour = {"hour", Position(2,6), VAR, dateelements[3]};
    Pageitem time_delimiter_1 = {"delimiter", Position(2, 8), FIXED, ":"};
    Pageitem time_min = {"min", Position(2,9), VAR, dateelements[4]};
    Pageitem time_delimiter_2 = {"delimiter", Position(2, 11), FIXED, ":"};
    Pageitem time_sec = {"sec", Position(2,12), VAR, dateelements[5]};

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
