//
// Created by hammer on 21/09/2019.
//

#include "../../include/lcdController.h"

//TODO: Add check to only create page when there is no page with sensor id or type
//TODO: Set the values to blank string or zero
// Get the unit and type from the settings file?
LcdController::LcdController(LcdDriver& lcd, Settings& weatherStationSettings): lcd(lcd), weatherStationSettings(weatherStationSettings)
{
    buildAllCustomChars();
}

void LcdController::createWeatherPage(WeatherSensor* ws, WeatherSensor::Data* reading)
{
    lock_guard<mutex> guard(lcdcMu);
    if( ! existingWeatherPage(ws->get_sensorID()))
    {
        Pageitem sensorID = {"sensorID", ws->get_Position_SensorName(), LcdConstants::FIXED, ws->get_sensorName().append(":")};
        Pageitem sensorID_val = {"pageTitle", ws->get_Position_Title(), LcdConstants::FIXED, "Current"};
        vector<Pageitem> items{sensorID, sensorID_val};
        map<string, vector<Pageitem>> sub_pages_map;

        Pageitem item(reading->readingId, reading->posName, LcdConstants::FIXED, reading->name);
        Pageitem item_val(reading->readingId, reading->posVal, LcdConstants::VAR, reading);
        items.push_back(item);
        items.push_back(item_val);

        sub_pages_map.insert(std::pair<string, vector<Pageitem>>(LcdConstants::CURRENT, items));

        pages_map.insert(std::pair<string, map<string, vector<Pageitem>>>(ws->get_sensorID(),sub_pages_map));
        LOG(INFO) << "Created a new weather page for SensorID: " << ws->get_sensorID() << " SensorName: "
                  << ws->get_sensorName() << endl;
        createWeatherAvgPage(ws, reading);
    } else
    {
//        Previous call to existing page and following call to existing page reading sets pm_iter to current page
        createNewReading(ws, reading);
        createWeatherAvgPage(ws, reading);
    }
}

void LcdController::createNewReading(WeatherSensor* ws, WeatherSensor::Data* reading)
{
    if (!existingWeatherPageReading(ws->get_sensorID(), reading->readingId))
    {
        Pageitem item(reading->readingId, reading->posName, LcdConstants::FIXED, reading->name);
        Pageitem item_val(reading->readingId, reading->posVal, LcdConstants::VAR, reading);
        auto current = pm_iter->second.find(LcdConstants::CURRENT);
        current->second.push_back(item);
        current->second.push_back(item_val);

        if (strcmp(currentPage.c_str(), ws->get_sensorID().c_str()) == 0) {
            clearDisplay();
            drawPage_NonLocking();
        }
    }
}

void LcdController::createWeatherAvgPage(WeatherSensor* ws, WeatherSensor::Data* reading)
{
    if (!existingWeatherAvgPage(ws->get_sensorID(), reading->readingId))
    {
        Pageitem sensorID = {"sensorID", ws->get_Position_SensorName(), LcdConstants::FIXED, ws->get_sensorName().append(":")};
        Pageitem sensorID_val = {"pageTitle", ws->get_Position_Title(), LcdConstants::FIXED, reading->name};
        vector<Pageitem> items{sensorID, sensorID_val};
        string id_avg = "avg_" + reading->readingId;
        string id_min = "min_" + reading->readingId;
        string id_max = "max_" + reading->readingId;
        Pageitem pi_now(reading->readingId, weatherStationSettings.getPositionByName("topleft_Name"), LcdConstants::FIXED, "Now");
        Pageitem pi_avg(id_avg, weatherStationSettings.getPositionByName("topright_Name"), LcdConstants::FIXED, "Avg");
        Pageitem pi_max(id_max, weatherStationSettings.getPositionByName("middleleft_Name"), LcdConstants::FIXED, "Max");
        Pageitem pi_min(id_min, weatherStationSettings.getPositionByName("middleright_Name"), LcdConstants::FIXED, "Min");

        Pageitem pi_now_val(reading->readingId, weatherStationSettings.getPositionByName("topleft_Val"), LcdConstants::VAR, reading);
        Pageitem pi_avg_val(id_avg, weatherStationSettings.getPositionByName("topright_Val"), LcdConstants::VAR, reading);
        Pageitem pi_max_val(id_max, weatherStationSettings.getPositionByName("middleleft_Val"), LcdConstants::VAR, reading);
        Pageitem pi_min_val(id_min, weatherStationSettings.getPositionByName("middleright_Val"), LcdConstants::VAR, reading);

        Pageitem timeframe(LcdConstants::TIMEFRAME, weatherStationSettings.getPositionByName("bottomleft_Name"), LcdConstants::FIXED, "Period: ");
        Pageitem timeframe_val(LcdConstants::TIMEFRAME, weatherStationSettings.getPositionByName("bottomright_Name"), LcdConstants::VAR, reading);

        items.push_back(pi_now); items.push_back(pi_max); items.push_back(pi_min); items.push_back(pi_avg);
        items.push_back(pi_now_val); items.push_back(pi_max_val); items.push_back(pi_min_val); items.push_back(pi_avg_val);
        items.push_back(timeframe); items.push_back(timeframe_val);

        pm_iter = pages_map.find(ws->get_sensorID());
        pm_iter->second.insert(std::pair<string, vector<Pageitem>>(reading->readingId, items));

        LOG(INFO) << "Created a new weather average subpage for SensorID: " << ws->get_sensorID() << " Reading Name: "
                  << reading->name << endl;
    }
}

//TODO: Need to find a better solution for ignoring non weather pages.
void LcdController::getNextPage()
{
    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find(currentPage);
    if(pm_iter != pages_map.end())
    {
        pm_iter = next(pm_iter, 1);
        if(pm_iter == pages_map.end())
        {
            pm_iter = pages_map.begin();
        } else if(pm_iter->first == LcdConstants::HOMEPAGE)
        {
            pm_iter = next(pm_iter, 1);
            if(pm_iter == pages_map.end())
            {
                pm_iter = pages_map.begin();
            }
        }
        currentPage = pm_iter->first;
        currentSubPage = LcdConstants::CURRENT;
    }
    else {
        currentPage = "No Pages";
    }
}

void LcdController::getNextSubPage()
{
    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find(currentPage);
    if (pm_iter != pages_map.end()) {
        spm_iter = pm_iter->second.find(currentSubPage);
        if (spm_iter != pm_iter->second.end()) {
            spm_iter = next(spm_iter, 1);
            if(spm_iter == pm_iter->second.end())
            {
                spm_iter = pm_iter->second.begin();
            }
            currentSubPage = spm_iter->first;
        }
    }

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
        for (auto &subPage: pm_iter->second)
        {
            if (strcmp(subPage.first.c_str(), LcdConstants::CURRENT.c_str()) == 0) {
                for (auto &pageItem: subPage.second)
                {
                    if (strcmp(readingid.c_str(), pageItem.id.c_str()) == 0) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool LcdController::existingWeatherAvgPage(string SensorName, string readingId)
{
    pm_iter = pages_map.find(SensorName);
    if (pm_iter != pages_map.end())
    {
        for (auto &subPage: pm_iter->second)
        {
            if (strcmp(subPage.first.c_str(), readingId.c_str()) == 0) {
                return true;
            }
        }
    }
    return false;
}

void LcdController::drawElementToLCD()
{
    lcd.setCursorPositionRowCol(pi_iter->pos.row_start, pi_iter->pos.col_start);
    if (pi_iter->str_val.empty() && pi_iter->data != nullptr) {
        if (strcmp(pi_iter->id.c_str(), LcdConstants::TIMEFRAME.c_str()) == 0) {
            lcd.lcdString(LcdConstants::timeframeText.find(currentDBTimeframe)->second.c_str());
        } else {
            clearOldValuesFromLcd();
            lcd.lcdString(checkValuesFitLcd().c_str());
        }
    } else if ( ! pi_iter->str_val.empty()) {
        lcd.lcdString(pi_iter->str_val.c_str());
    }
}

string LcdController::checkValuesFitLcd()
{
    if(pi_iter->type == LcdConstants::VAR) {
        float reading = getCorrectReadingDataValueToDraw();
        try {
            if(reading > 99.99){
                return Utilities::to_string_with_precision<float>(reading, 0);
            }
            return Utilities::to_string_with_precision<float>(reading, 1);
        }
        catch (invalid_argument& ia) {
            LOG(ERROR) << "Error occurred converting value '" << reading << "' " << endl;
        }
    }
    return "ERR";
}

float LcdController::getCorrectReadingDataValueToDraw()
{
    if (pi_iter->id.find("min_") != string::npos) {
        float minimum = pi_iter->data->timeframeConstMapping.find(currentDBTimeframe)->second->minimum;
        VLOG(6) << "Getting reading to draw. Minimum with timeframe of one day " << minimum;
        return minimum;
    }
    if (pi_iter->id.find("max_") != string::npos) {
        float maximum = pi_iter->data->timeframeConstMapping.find(currentDBTimeframe)->second->maximum;
        VLOG(6) << "Getting reading to draw. Minimum with timeframe of one day " << maximum;
        return maximum;
    }
    if (pi_iter->id.find("avg_") != string::npos) {
        float average = pi_iter->data->timeframeConstMapping.find(currentDBTimeframe)->second->average;
        VLOG(6) << "Getting reading to draw. Minimum with timeframe of one day " << average;
        return average;
    }
    return pi_iter->data->reading;
}

void LcdController::clearOldValuesFromLcd()
{
    if(pi_iter->type == LcdConstants::VAR) {
        try {
            if (pi_iter->data->prev_reading > 9.99 && pi_iter->data->reading < 10.0) {
                lcd.clearColumnsRowCol(pi_iter->pos.row_start, pi_iter->pos.col_start + 5, pi_iter->pos.col_start);
            }
        }
        catch (invalid_argument& ia) {
            LOG(ERROR) << "Error occurred converting value '" << pi_iter->data->reading << "' " << endl;
        }
    }
}

// TODO: Need to prevent left over chars on display when writing a new smaller value
void LcdController::drawPage_Locking()
{
    lock_guard<mutex> guard(lcdcMu);
    drawPage();
}

void LcdController::drawPage_NonLocking()
{
    drawPage();
}

void LcdController::drawPage()
{
    pm_iter = pages_map.find(currentPage);
    if(pm_iter != pages_map.end())
    {
        for (auto &subPage: pm_iter->second)
        {
            if (strcmp(subPage.first.c_str(), currentSubPage.c_str()) == 0) {
                for(pi_iter = subPage.second.begin(); pi_iter != subPage.second.end(); pi_iter++)
                {
                    drawElementToLCD();
                }
                break;
            }
        }
    } else
    {
        string msg = "No such sensor " + currentPage;
        LOG(WARNING) << msg << endl;
        lcd.lcdString(msg.c_str());
    }
}

void LcdController::updatePageValues(WeatherSensor *ws)
{
    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find(ws->get_sensorID());
    if(pm_iter != pages_map.end())
    {
        for (auto &subPage: pm_iter->second)
        {
            if (strcmp(subPage.first.c_str(), currentSubPage.c_str()) == 0) {
                for(pi_iter = subPage.second.begin(); pi_iter != subPage.second.end(); pi_iter++)
                {
                    if(pi_iter->type == LcdConstants::VAR)
                    {
//                        if (pi_iter->data->prev_reading != pi_iter->data->reading)
//                        {
                            drawElementToLCD();
//                        }
                    }
                }
                break;
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

    Pageitem date = {"date", Position(1, 0), LcdConstants::FIXED, "Date: "};
    Pageitem date_day = {"day", Position(1, 6), LcdConstants::VAR, dateelements[0]};
    Pageitem date_delimiter_1 = {"delimiter", Position(1, 8), LcdConstants::FIXED, "-"};
    Pageitem date_month = {"month", Position(1, 9), LcdConstants::VAR, dateelements[1]};
    Pageitem date_delimiter_2 = {"delimiter", Position(1, 11), LcdConstants::FIXED, "-"};
    Pageitem date_year = {"year", Position(1, 12), LcdConstants::VAR, dateelements[2]};

    Pageitem time = {"time", Position(2,0), LcdConstants::FIXED, "Time: "};
    Pageitem time_hour = {"hour", Position(2,6), LcdConstants::VAR, dateelements[3]};
    Pageitem time_delimiter_1 = {"delimiter", Position(2, 8), LcdConstants::FIXED, ":"};
    Pageitem time_min = {"min", Position(2,9), LcdConstants::VAR, dateelements[4]};
    Pageitem time_delimiter_2 = {"delimiter", Position(2, 11), LcdConstants::FIXED, ":"};
    Pageitem time_sec = {"sec", Position(2,12), LcdConstants::VAR, dateelements[5]};

    vector<Pageitem> items{date, date_day, date_delimiter_1, date_month, date_delimiter_2,
                           date_year, time, time_delimiter_1, time_hour, time_delimiter_2, time_min, time_sec};
    map<string, vector<Pageitem>> sub_pages_map;
    sub_pages_map.insert(std::pair<string, vector<Pageitem>>(LcdConstants::DATE, items));

    lock_guard<mutex> guard(lcdcMu);
    pages_map.insert(std::pair<string, map<string, vector<Pageitem>>>(LcdConstants::HOMEPAGE,sub_pages_map));
    LOG(INFO) << "Created a new page " << "date" << endl;
}

void LcdController::updateDateTimePage()
{
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string date_str(30, '\0');
    struct tm now{};
    strftime(&date_str[0], date_str.size(), "%d-%m-%y-%H-%M-%S", localtime_r(&timenow, &now));
    string delimiter = "-";
    vector<string> dateelements;

    Utilities::split_string(date_str, dateelements, '-');

    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find(LcdConstants::HOMEPAGE);
    if(pm_iter != pages_map.end())
    {
        for (auto &subPage: pm_iter->second)
        {
            if (strcmp(subPage.first.c_str(), LcdConstants::DATE.c_str()) == 0) {
                for(pi_iter = subPage.second.begin(); pi_iter != subPage.second.end(); pi_iter++)
                {
                    if(pi_iter->type == LcdConstants::VAR)
                    {
                        if(pi_iter->id == "sec")
                        {
                            if (pi_iter->str_val != dateelements[5])
                            {
                                pi_iter->str_val = dateelements[5];
                                drawElementToLCD();
                            }
                        }
                        if(pi_iter->id == "min")
                        {
                            if (pi_iter->str_val != dateelements[4])
                            {
                                pi_iter->str_val = dateelements[4];
                                drawElementToLCD();
                            }
                        }
                        if(pi_iter->id == "hour")
                        {
                            if (pi_iter->str_val != dateelements[3])
                            {
                                pi_iter->str_val = dateelements[3];
                                drawElementToLCD();
                            }
                        }
                        if(pi_iter->id == "day")
                        {
                            if (pi_iter->str_val != dateelements[2])
                            {
                                pi_iter->str_val = dateelements[2];
                                drawElementToLCD();
                            }
                        }
                        if(pi_iter->id == "month")
                        {
                            if (pi_iter->str_val != dateelements[1])
                            {
                                pi_iter->str_val = dateelements[1];
                                drawElementToLCD();
                            }
                        }
                        if(pi_iter->id == "year")
                        {
                            if (pi_iter->str_val != dateelements[0])
                            {
                                pi_iter->str_val = dateelements[0];
                                drawElementToLCD();
                            }
                        }
                    }
                }
                break;
            }
        }
    }
}

void LcdController::drawDateTimePage()
{
    lock_guard<mutex> guard(lcdcMu);
    pm_iter = pages_map.find(LcdConstants::HOMEPAGE);
    if(pm_iter != pages_map.end())
    {
        for (auto &subPage: pm_iter->second)
        {
            if (strcmp(subPage.first.c_str(), LcdConstants::DATE.c_str()) == 0) {
                for(pi_iter = subPage.second.begin(); pi_iter != subPage.second.end(); pi_iter++)
                {
                    drawElementToLCD();
                }
                break;
            }
        }
    }
}

void LcdController::clearDisplay()
{
    lcd.clearDisplayClearMem();
}

void LcdController::setCurrentPage(string pageName)
{
    currentPage = pageName;
}

void LcdController::setCurrentSubPage(string pageName)
{
    currentSubPage = pageName;
}

string LcdController::getCurrentPage()
{
    return currentPage;
}

string LcdController::getCurrentSubPage()
{
    return currentSubPage;
}

//Custom Char Functions

void LcdController::drawBatteryFullSymbol()
{
    lcd.lcdSendCustomChar(0);
}

void LcdController::drawBatteryHalfSymbol()
{
    lcd.lcdSendCustomChar(1);
}

void LcdController::drawBatteryEmptySymbol()
{
    lcd.lcdSendCustomChar(2);
}

void LcdController::buildAllCustomChars()
{
    createFullBattery();
    createHalfBattery();
    createEmptyBattery();
    usleep(10000);
}

void LcdController::createFullBattery()
{
    uint8_t customChar[8] = {
            0x0E,
            0x1F,
            0x1F,
            0x1F,
            0x1F,
            0x1F,
            0x1F,
            0x1F
    };
    lcd.createCustomChar(0, customChar);
//    usleep(10000);
}

void LcdController::createHalfBattery()
{
    uint8_t customChar[8] = {
            0x0E,
            0x11,
            0x11,
            0x11,
            0x1F,
            0x1F,
            0x1F,
            0x1F
    };
    lcd.createCustomChar(1, customChar);
//    usleep(10000);
}

void LcdController::createEmptyBattery()
{
    uint8_t customChar[8] = {
            0x0E,
            0x11,
            0x11,
            0x11,
            0x11,
            0x11,
            0x1F,
            0x1F
    };
    lcd.createCustomChar(2, customChar);
//    usleep(10000);
}

void LcdController::getNextTimeframe()
{
    lock_guard<mutex> guard(lcdcMu);
    tf_iter = next(tf_iter);
    if (tf_iter == timeframes.end())
    {
        tf_iter = timeframes.begin();
    }
    currentDBTimeframe = *tf_iter;
}

