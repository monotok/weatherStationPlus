//
// Created by hammer on 24/09/2019.
//

#include "gtest/gtest.h"
#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/lcdController.h"
//Only included to allow a sleep to show clearing display etc
#include <chrono>
#include <thread>

#define lcdAdd 0x3f // I2C device address
#define i2cbusno 3

TEST(LcdController, create_new_weather_page_struct_independant)
{
    WeatherSensor* ws1 = new WeatherSensor("weather1", "weather");
    ws1->set_temperature(3600);
    WeatherSensor* ws2 = new WeatherSensor("weather2", "weather");
    ws2->set_temperature(2500);

    LcdController lcdc;
    lcdc.createWeatherPage(ws1);
    lcdc.createWeatherPage(ws2);

    auto found1 = lcdc.pages_map.find("weather1");
    auto found2 = lcdc.pages_map.find("weather2");

    EXPECT_NE(found1, lcdc.pages_map.end());
    EXPECT_NE(found2, lcdc.pages_map.end());

    EXPECT_EQ(found1->second[3].value, "36.0C");
    EXPECT_EQ(found2->second[3].value, "25.0C");

}

TEST(LcdController, iterate_over_pages_onebyone)
{
    WeatherSensor* ws1 = new WeatherSensor("weather1", "weather");
    ws1->set_temperature(3600);
    WeatherSensor* ws2 = new WeatherSensor("weather2", "weather");
    ws2->set_temperature(2500);
    WeatherSensor* ws3 = new WeatherSensor("weather3", "weather");
    ws2->set_temperature(2500);
    WeatherSensor* ws4 = new WeatherSensor("weather4", "weather");
    ws2->set_temperature(2500);

    LcdController lcdc;
    lcdc.createWeatherPage(ws1);
    lcdc.createWeatherPage(ws2);
    lcdc.createWeatherPage(ws3);
    lcdc.createWeatherPage(ws4);

    EXPECT_EQ(lcdc.getNextPage("weather1"), "weather2");
    EXPECT_EQ(lcdc.getNextPage("weather4"), "weather1");

}

TEST(LcdController, check_for_existing_weather_sensor)
{
    WeatherSensor* ws1 = new WeatherSensor("weather1", "weather");

    LcdController lcdc;
    lcdc.createWeatherPage(ws1);

    EXPECT_EQ(lcdc.existingWeatherPage("weather1"), true);
    EXPECT_EQ(lcdc.existingWeatherPage("weather2"), false);

}

TEST(LcdController, draw_basic_page_to_lcd)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    WeatherSensor* ws1 = new WeatherSensor("mysensor", "weather");
    ws1->set_temperature(2708);
    ws1->set_humidity(4500);
    LcdController lcdc;
    lcdc.createWeatherPage(ws1);

    lcdc.drawPage("mysensor", lcd);
}

TEST(LcdController, update_values_only_on_existing_page)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    WeatherSensor* ws1 = new WeatherSensor("weather1", "weather");
    ws1->set_temperature(3600);
    ws1->set_humidity(7900);

    LcdController lcdc;
    lcdc.createWeatherPage(ws1);

    lcdc.drawPage("weather1", lcd);

    auto found1 = lcdc.pages_map.find("weather1");
    EXPECT_EQ(found1->second[3].value, "36.0C");
    EXPECT_EQ(found1->second[5].value, "79.0%");

    ws1->set_temperature(2400);
    ws1->set_humidity(5600);

    lcdc.updatePageValues(ws1, lcd);
    EXPECT_EQ(found1->second[3].value, "24.0C");
    EXPECT_EQ(found1->second[5].value, "56.0%");

    //Sleep for 4 seconds so we can observe the display
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    lcdc.drawPage("weather1", lcd);

}

TEST(LcdController, create_new_datetime_page_struct_independant)
{
    LcdController lcdc;
    lcdc.createDateTimePage();

    auto founddate = lcdc.pages_map.find("date");

    EXPECT_NE(founddate, lcdc.pages_map.end());

    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string date_str(30, '\0');
    strftime(&date_str[0], date_str.size(), "%d", std::localtime(&timenow));
    auto found = date_str.find(founddate->second[1].value);
    EXPECT_NE(found, string::npos);
}