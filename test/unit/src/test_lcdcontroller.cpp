//
// Created by hammer on 24/09/2019.
//

#include "gtest/gtest.h"
#define private public

#define lcdAdd 0x3f // I2C device address
#define i2cbusno 1

#include "../../../include/data/WeatherSensor.hpp"
#include "../../../include/lcdController.h"

//Only included to allow a sleep to show clearing display etc
#include <chrono>
#include <thread>

TEST(LcdController, create_new_weather_page_struct_independant)
{
    WeatherSensor* ws1 = new WeatherSensor("weather1", "weather");
    ws1->set_temperature(36);
    WeatherSensor* ws2 = new WeatherSensor("weather2", "weather");
    ws2->set_temperature(25);

    LcdController lcdc;
    lcdc.createWeatherPage(ws1);
    lcdc.createWeatherPage(ws2);

    auto found1 = lcdc.pages_map.find("weather1");
    auto found2 = lcdc.pages_map.find("weather2");

    EXPECT_NE(found1, lcdc.pages_map.end());
    EXPECT_NE(found2, lcdc.pages_map.end());

    EXPECT_EQ(found1->second[3].value, "36.00");
    EXPECT_EQ(found2->second[3].value, "25.00");

}

TEST(LcdController, draw_basic_page_to_lcd)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    WeatherSensor* ws1 = new WeatherSensor("mysensor", "weather");
    ws1->set_temperature(27.8);
    ws1->set_humidity(45.0);
    LcdController lcdc;
    lcdc.createWeatherPage(ws1);

    lcdc.drawPage("mysensor", lcd);
}

TEST(LcdController, update_values_only_on_existing_page)
{
    I2cControl *i2c = new I2cControl(i2cbusno);
    LcdDriver lcd(lcdAdd, i2c);

    WeatherSensor* ws1 = new WeatherSensor("weather1", "weather");
    ws1->set_temperature(36);
    ws1->set_humidity(79);

    LcdController lcdc;
    lcdc.createWeatherPage(ws1);

    lcdc.drawPage("weather1", lcd);

    auto found1 = lcdc.pages_map.find("weather1");
    EXPECT_EQ(found1->second[3].value, "36.00");
    EXPECT_EQ(found1->second[5].value, "79.00");

    ws1->set_temperature(24);
    ws1->set_humidity(56);

    lcdc.updatePageValues(ws1);
    EXPECT_EQ(found1->second[3].value, "24.00");
    EXPECT_EQ(found1->second[5].value, "56.00");

    //Sleep for 4 seconds so we can observe the display
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    lcdc.drawPage("weather1", lcd);

}