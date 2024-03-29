//
// Created by hammer on 24/09/2019.
//

#include "gtest/gtest.h"
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/lcdController.h"
#include "../../../include/settings.hpp"
#include "../../../include/configParser.hpp"
#include "../../../include/data/DynamicSensorFactory.hpp"
//Only included to allow a sleep to show clearing display etc
#include <chrono>
#include <thread>

#define lcdAdd 0x27 // I2C device address
#define i2cbusno 3

TEST(LcdController, create_new_weather_page_struct_independant)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    WeatherSensor* ws1 = new WeatherSensor("1", "weather1");
    auto reading_1 = ws1->set_reading("1.1", 34.4, &conf);
    WeatherSensor* ws2 = new WeatherSensor("2", "weather2");
    auto reading_2 = ws2->set_reading("2.1", 25.46, &conf);

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(lcdAdd, i2c);
    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, reading_1);
    lcdc.createWeatherPage(ws2, reading_2);

    auto found1 = lcdc.pages_map.find("1");
    auto found2 = lcdc.pages_map.find("2");

    EXPECT_NE(found1, lcdc.pages_map.end());
    EXPECT_NE(found2, lcdc.pages_map.end());

    EXPECT_FLOAT_EQ(found1->second.find(LcdConstants::CURRENT)->second[3].data->reading, 34.4);
    EXPECT_FLOAT_EQ(found2->second.find(LcdConstants::CURRENT)->second[3].data->reading, 25.46);

}

TEST(LcdController, iterate_over_pages_onebyone)
{
    WeatherSensor* ws1 = new WeatherSensor("1", "weather1");
    WeatherSensor* ws2 = new WeatherSensor("2", "weather2");
    WeatherSensor* ws3 = new WeatherSensor("3", "weather3");
    WeatherSensor* ws4 = new WeatherSensor("4", "weather4");
    WeatherSensor::Data reading("1");

    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(lcdAdd, i2c);
    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, &reading);
    lcdc.createWeatherPage(ws2, &reading);
    lcdc.createWeatherPage(ws3, &reading);
    lcdc.createWeatherPage(ws4, &reading);
    lcdc.createDateTimePage();

    lcdc.setCurrentPage(LcdConstants::HOMEPAGE);
    lcdc.getNextPage();
    EXPECT_EQ(lcdc.getCurrentPage(), "1");
    lcdc.getNextPage();
    EXPECT_EQ(lcdc.getCurrentPage(), "2");

}

TEST(LcdController, get_next_timeframe)
{
    WeatherSensor* ws1 = new WeatherSensor("1", "weather1");
    WeatherSensor::Data reading("1");

    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(lcdAdd, i2c);
    LcdController lcdc(lcd, weatherStationSettings);

    EXPECT_EQ(lcdc.currentDBTimeframe, LcdConstants::ONE_DAY);
    lcdc.getNextTimeframe();
    EXPECT_EQ(lcdc.currentDBTimeframe, LcdConstants::ONE_WEEK);
    lcdc.getNextTimeframe();
    EXPECT_EQ(lcdc.currentDBTimeframe, LcdConstants::ONE_MONTH);
    lcdc.getNextTimeframe();
    EXPECT_EQ(lcdc.currentDBTimeframe, LcdConstants::ONE_YEAR);
    lcdc.getNextTimeframe();
    EXPECT_EQ(lcdc.currentDBTimeframe, LcdConstants::ONE_DAY);
}

TEST(LcdController, check_for_existing_weather_sensor)
{
    WeatherSensor* ws1 = new WeatherSensor("1.0", "weather1");
    WeatherSensor::Data reading("1.8");
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(lcdAdd, i2c);
    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, &reading);

    EXPECT_EQ(lcdc.existingWeatherPage("1"), true);
    EXPECT_EQ(lcdc.existingWeatherPage("2"), false);

}

TEST(LcdController, draw_basic_page_to_lcd)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    WeatherSensor* ws1 = new WeatherSensor("1.0", "mysensor");
    auto reading = ws1->set_reading("1.1", 34.4, &conf);

    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, reading);

    lcdc.setCurrentPage("1");
    lcdc.drawPage_Locking();
}

TEST(LcdController, draw_avg_weather_page_to_lcd)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    WeatherSensor* ws1 = new WeatherSensor("1", "mysensor");
    auto reading = ws1->set_reading("1.0", 34.4, &conf);

    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, reading);

    lcdc.setCurrentPage("1");
    lcdc.setCurrentSubPage(reading->readingId);
    lcdc.drawPage_Locking();
}

TEST(LcdController, draw_different_avg_weather_page_to_lcd)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
                         +" port = " + to_string(weatherStationSettings.db.port);

    pqxx::connection TEST_CONNECTOR(db_conn_str);
    pqxx::work w1(TEST_CONNECTOR);
    w1.exec(
            "delete from readings;"
    );
    w1.exec(
            "delete from sensor_metadata;"
    );
    w1.commit();

    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);

    WeatherSensor* ws1 = dsf.getWeatherSensor_ptr("1");

    auto reading = ws1->set_reading("1.0", 34, &conf);
    ws1->set_reading("1.0", 23, &conf);
    ws1->set_reading("1.0", 28, &conf);
    auto reading2 = ws1->set_reading("1.1", 2, &conf);
    ws1->set_reading("1.1", 4, &conf);
    ws1->set_reading("1.1", 8, &conf);
    ws1->set_reading("1.1", 6, &conf);

    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, reading);
    lcdc.createWeatherPage(ws1, reading2);

    lcdc.setCurrentPage("1");
    lcdc.setCurrentSubPage(reading->readingId);
    lcdc.drawPage_Locking();
    lcdc.updatePageValues(ws1);

    //Sleep for 4 seconds so we can observe the display
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    lcdc.setCurrentSubPage(reading2->readingId);
    lcdc.drawPage_Locking();
    lcdc.updatePageValues(ws1);
}

TEST(LcdController, update_values_only_on_existing_avg_page)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
                         +" port = " + to_string(weatherStationSettings.db.port);

    pqxx::connection TEST_CONNECTOR(db_conn_str);
    pqxx::work w1(TEST_CONNECTOR);
    w1.exec(
            "delete from readings;"
    );
    w1.exec(
            "delete from sensor_metadata;"
    );
    w1.commit();

    DynamicSensorFactory dsf(conf);
    dsf.establish_database_connection(weatherStationSettings);

    WeatherSensor* ws1 = dsf.getWeatherSensor_ptr("1");

    auto reading = ws1->set_reading("1.0", 36.0, &conf);

    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, reading);

    lcdc.setCurrentPage("1");
    lcdc.setCurrentSubPage(LcdConstants::CURRENT);
    lcdc.drawPage_Locking();

    auto found1 = lcdc.pages_map.find("1");
    EXPECT_FLOAT_EQ(found1->second.find(reading->readingId)->second[7].data->day_cr.maximum, 0.0);
    EXPECT_FLOAT_EQ(found1->second.find(reading->readingId)->second[8].data->day_cr.minimum, 0.0);
    EXPECT_FLOAT_EQ(found1->second.find(reading->readingId)->second[9].data->day_cr.average, 0.0);

    //Sleep for 4 seconds so we can observe the display
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    reading->day_cr.maximum = 36.0;
    reading->day_cr.minimum = 24.0;
    reading->day_cr.average = 28.0;
    lcdc.updatePageValues(ws1);
    EXPECT_FLOAT_EQ(found1->second.find(reading->readingId)->second[7].data->day_cr.maximum, 36.0);
    EXPECT_FLOAT_EQ(found1->second.find(reading->readingId)->second[8].data->day_cr.minimum, 24.0);
    EXPECT_FLOAT_EQ(found1->second.find(reading->readingId)->second[9].data->day_cr.average, 28.0);
}

TEST(LcdController, update_values_only_on_existing_page)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    WeatherSensor* ws1 = new WeatherSensor("1", conf.getSensorsName("1"));
    auto reading = ws1->set_reading("1.0", 36.0, &conf);
    auto reading_hum = ws1->set_reading("1.1", 79.0, &conf);

    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createWeatherPage(ws1, reading);
    lcdc.createWeatherPage(ws1, reading_hum);

    lcdc.setCurrentPage("1");
    lcdc.drawPage_Locking();

    auto found1 = lcdc.pages_map.find("1");
    EXPECT_FLOAT_EQ(found1->second.find(LcdConstants::CURRENT)->second[3].data->reading, 36.0);
    EXPECT_FLOAT_EQ(found1->second.find(LcdConstants::CURRENT)->second[5].data->reading, 79.0);

    ws1->set_reading("1.0", 24.0, &conf);
    ws1->set_reading("1.1", 56.0, &conf);

    //Sleep for 4 seconds so we can observe the display
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    lcdc.updatePageValues(ws1);
    EXPECT_FLOAT_EQ(found1->second.find(LcdConstants::CURRENT)->second[3].data->reading, 24.0);
    EXPECT_FLOAT_EQ(found1->second.find(LcdConstants::CURRENT)->second[5].data->reading, 56.0);
}

TEST(LcdController, create_new_datetime_page_struct_independant)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(lcdAdd, i2c);
    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createDateTimePage();
    lcdc.drawDateTimePage();

    auto foundhome = lcdc.pages_map.find(LcdConstants::HOMEPAGE);

    EXPECT_NE(foundhome, lcdc.pages_map.end());

    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string date_str(30, '\0');
    strftime(&date_str[0], date_str.size(), "%d", std::localtime(&timenow));
    string found = foundhome->second.find(LcdConstants::DATE)->second[1].str_val;
    EXPECT_STREQ(found.c_str(), date_str.c_str());
}

TEST(LcdController, update_only_changed_values_datetime)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    LcdController lcdc(lcd, weatherStationSettings);
    lcdc.createDateTimePage();
    lcdc.drawDateTimePage();

    auto founddate = lcdc.pages_map.find(LcdConstants::HOMEPAGE);

    EXPECT_NE(founddate, lcdc.pages_map.end());
    auto oldSecond = founddate->second.find(LcdConstants::DATE)->second[11].str_val;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    lcdc.updateDateTimePage();
    auto newSecond = founddate->second.find(LcdConstants::DATE)->second[11].str_val;
    EXPECT_NE(oldSecond, newSecond);
}

TEST(LcdController, draw_custom_character_battery_symbols)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);


    LcdController lcdc(lcd, weatherStationSettings);
    lcd.setCursorPositionRowCol(1,1);
    lcdc.drawBatteryFullSymbol();
    lcd.setCursorPositionRowCol(1,2);
    lcdc.drawBatteryHalfSymbol();
    lcd.setCursorPositionRowCol(1,3);
    lcdc.drawBatteryEmptySymbol();
}

TEST(LcdController, draw_custom_characters)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "yang", "test/settings.xml");
    conf.ParseConfiguration();

    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    LcdController lcdc(lcd, weatherStationSettings);
    lcd.setCursorPositionRowCol(1,4);
    lcd.drawLeftArrow();
    lcd.setCursorPositionRowCol(1,5);
    lcd.drawRightArrow();
    lcd.setCursorPositionRowCol(1,6);
    lcd.drawDegreeSymbol();
    lcd.setCursorPositionRowCol(1,7);
    lcd.drawFullChar();
}