//
// Created by hammer on 24/09/2019.
//

#include "gtest/gtest.h"
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/lcdController.h"
#include "../../../include/settings.hpp"
#include "../../../include/configParser.hpp"
//Only included to allow a sleep to show clearing display etc
#include <chrono>
#include <thread>

#define lcdAdd 0x3f // I2C device address
#define i2cbusno 3

TEST(LcdController, create_new_weather_page_struct_independant)
{
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    WeatherSensor* ws1 = new WeatherSensor("1.0", "weather1", "weather");
    auto reading_1 = ws1->set_reading("1.1", "tmp", 34.4, "c", nullptr, &conf);
    WeatherSensor* ws2 = new WeatherSensor("2.0", "weather2","weather");
    auto reading_2 = ws2->set_reading("2.1", "tmp", 25.46, "c", nullptr, &conf);

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(63, i2c);
    LcdController lcdc(lcd);
    lcdc.createWeatherPage(ws1, reading_1);
    lcdc.createWeatherPage(ws2, reading_2);

    auto found1 = lcdc.pages_map.find("1");
    auto found2 = lcdc.pages_map.find("2");

    EXPECT_NE(found1, lcdc.pages_map.end());
    EXPECT_NE(found2, lcdc.pages_map.end());

    EXPECT_EQ(found1->second[3].value, "34.4");
    EXPECT_EQ(found2->second[3].value, "25.5");

}

TEST(LcdController, iterate_over_pages_onebyone)
{
    WeatherSensor* ws1 = new WeatherSensor("1", "weather1","weather");
    WeatherSensor* ws2 = new WeatherSensor("2", "weather2","weather");
    WeatherSensor* ws3 = new WeatherSensor("3", "weather3","weather");
    WeatherSensor* ws4 = new WeatherSensor("4", "weather4","weather");
    WeatherSensor::Data reading("1");

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(63, i2c);
    LcdController lcdc(lcd);
    lcdc.createWeatherPage(ws1, &reading);
    lcdc.createWeatherPage(ws2, &reading);
    lcdc.createWeatherPage(ws3, &reading);
    lcdc.createWeatherPage(ws4, &reading);
    lcdc.createDateTimePage();

    lcdc.setCurrentPage("date");
    lcdc.getNextPage();
    EXPECT_EQ(lcdc.getCurrentPage(), "1");
    lcdc.getNextPage();
    EXPECT_EQ(lcdc.getCurrentPage(), "2");

}

TEST(LcdController, check_for_existing_weather_sensor)
{
    WeatherSensor* ws1 = new WeatherSensor("1.0", "weather1","weather");
    WeatherSensor::Data reading("1.8");

    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(63, i2c);
    LcdController lcdc(lcd);
    lcdc.createWeatherPage(ws1, &reading);

    EXPECT_EQ(lcdc.existingWeatherPage("1"), true);
    EXPECT_EQ(lcdc.existingWeatherPage("2"), false);

}

TEST(LcdController, draw_basic_page_to_lcd)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);
    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    WeatherSensor* ws1 = new WeatherSensor("1.0", "mysensor","weather");
    auto reading = ws1->set_reading("1.1", "tmp", 34.4, "c", nullptr, &conf);

    LcdController lcdc(lcd);
    lcdc.createWeatherPage(ws1, reading);

    lcdc.setCurrentPage("mysensor");
    lcdc.drawPage_Locking();
}

TEST(LcdController, update_values_only_on_existing_page)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    Settings weatherStationSettings {};
    ConfigParser conf(weatherStationSettings, "../../settings.conf");
    conf.ParseConfiguration();

    WeatherSensor* ws1 = new WeatherSensor("1", conf.getSensorsName("1"),"weather");
    auto reading = ws1->set_reading("1.0", "tmp", 36.0, "c", nullptr, &conf);
    auto reading_hum = ws1->set_reading("1.1", "hum", 79.0, "%", nullptr, &conf);

    LcdController lcdc(lcd);
    lcdc.createWeatherPage(ws1, reading);
    lcdc.createWeatherPage(ws1, reading_hum);

    lcdc.setCurrentPage("1");
    lcdc.drawPage_Locking();

    auto found1 = lcdc.pages_map.find("1");
    EXPECT_EQ(found1->second[3].value, "36.0");
    EXPECT_EQ(found1->second[5].value, "79.0");

    ws1->set_reading("1.0", "tmp", 24.0, "c", nullptr, &conf);
    ws1->set_reading("1.1", "hum", 56.0, "%", nullptr, &conf);

    //Sleep for 4 seconds so we can observe the display
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    lcdc.updatePageValues(ws1);
    EXPECT_EQ(found1->second[3].value, "24.0");
    EXPECT_EQ(found1->second[5].value, "56.0");
}

TEST(LcdController, create_new_datetime_page_struct_independant)
{
    I2cControl *i2c = new I2cControl(3);
    LcdDriver lcd(63, i2c);
    LcdController lcdc(lcd);
    lcdc.createDateTimePage();

    auto founddate = lcdc.pages_map.find("date");

    EXPECT_NE(founddate, lcdc.pages_map.end());

    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string date_str(30, '\0');
    strftime(&date_str[0], date_str.size(), "%d", std::localtime(&timenow));
    auto found = date_str.find(founddate->second[1].value);
    EXPECT_NE(found, string::npos);
}

TEST(LcdController, update_only_changed_values_datetime)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    LcdController lcdc(lcd);
    lcdc.createDateTimePage();

    auto founddate = lcdc.pages_map.find("date");

    EXPECT_NE(founddate, lcdc.pages_map.end());
    auto oldSecond = founddate->second[11].value;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    lcdc.updateDateTimePage();
    auto newSecond = founddate->second[11].value;
    EXPECT_NE(oldSecond, newSecond);
}