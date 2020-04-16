#include "../include/easylogging++.hpp"
#include "../include/main.hpp"
INITIALIZE_EASYLOGGINGPP

#define LCD_ADDRESS 0x3f // I2C device address
#define ATMEGA_ADDRESS 0x04 // I2C device address
#define i2cbusno 3

//TODO: Add logging to all the functions like LCD controller.

using namespace std;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void getNewSensorData(DynamicSensorFactory* dynamsensors_ptr, I2cControl* i2c_ptr, LcdController* lcdc)
{
    pqxx::connection C("dbname = weather user = weather password = siesta_Usn_4Gulag_ \
      hostaddr = 172.16.20.5 port = 5432");
    if (C.is_open()) {
        LOG(INFO) << "Opened database successfully: " << C.dbname() << endl;
        RetrieveSenData rsd = RetrieveSenData(i2c_ptr, lcdc, weatherStationSettings.i2c.atmega, &C);
        while(true)
        {
            rsd.get_LocalWeatherData(dynamsensors_ptr);
            WeatherSensor* here = dynamsensors_ptr->getWeatherSensor_ptr("Here");
            LOG(DEBUG) << "\n\nLocal ID: " << here->get_sensorID() << "\n"
                       << "Local Temp: " << here->get_temperature_float() << "\n"
                       << "Local Humidity: " << here->get_humidity_float() << endl;

            usleep(3000000);

            rsd.get_RemoteWeatherSenData(dynamsensors_ptr);
            WeatherSensor* remote = dynamsensors_ptr->getWeatherSensor_ptr("BackBed");
            LOG(DEBUG) << "\n\nRemote ID: " << remote->get_sensorID() << "\n"
                       << "Remote Temp: " << remote->get_temperature_float() << "\n"
                       << "Remote Humidity: " << remote->get_humidity_float() << endl;

            usleep(3000000);
        }
    }
    else
    {
        LOG(ERROR) << "Can't open database" << endl;
    }

}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void updateLcdCurrentPage(DynamicSensorFactory* dynamsensors_ptr, LcdController* lcdc_ptr, LcdDriver *lcd)
{
    while(true)
    {
        if(currentPage == "date") {
            lcdc_ptr->updateDateTimePage(*lcd);
            usleep(500000);
        } else
        {
            lcdc_ptr->updatePageValues(dynamsensors_ptr->getWeatherSensor_ptr(currentPage), *lcd);
            usleep(5000000);
        }
    }

}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void detectbtnpress(LcdController *lcdc, LcdDriver *lcd, DynamicSensorFactory *dsf)
{
    GPIOControl gpio1 = GPIOControl(weatherStationSettings.gpio.gpio1);
    gpio1.g_setdir("in");
    GPIOControl gpio2 = GPIOControl(weatherStationSettings.gpio.gpio2);
    gpio2.g_setdir("in");
    GPIOControl gpio3 = GPIOControl(weatherStationSettings.gpio.gpio3);
    gpio3.g_setdir("in");
    GPIOControl gpio4 = GPIOControl(weatherStationSettings.gpio.gpio4);
    gpio4.g_setdir("in");
    BtnState bs_1;
    BtnState bs_2;
    BtnState bs_3;
    BtnState bs_4;

    while(true)
    {
        bs_1.initBtnState(&gpio1);
        bs_2.initBtnState(&gpio2);
        bs_3.initBtnState(&gpio3);
        bs_4.initBtnState(&gpio4);
        if(bs_1.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio1 << " Pressed";
            currentPage = lcdc->getNextPage(currentPage);
            lcd->clearDisplayClearMem();
            lcdc->drawPage(currentPage, *lcd);
        } else if(bs_2.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio2 << " Pressed";
            WeatherSensor* weather_ptr = dsf->getWeatherSensor_ptr(currentPage);
            weather_ptr->switch_tempUnit();
            lcdc->updatePageValues(weather_ptr, *lcd);
        } else if(bs_3.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio3 << " Pressed";
            lcd->clearDisplayClearMem();
            lcdc->drawDateTimePage(*lcd);
            currentPage = "date";
        } else if(bs_4.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio4 << " Pressed";
            lcd->changeBacklight();
            lcdc->drawPage(currentPage, *lcd);
        }


        bs_1.reInitBtnState();
        bs_2.reInitBtnState();
        bs_3.reInitBtnState();
        bs_4.reInitBtnState();
        usleep(50000);
    }

}
#pragma clang diagnostic pop

int main(int argc, char** argv)
{
    START_EASYLOGGINGPP(argc, argv);
    ConfigParser wss(weatherStationSettings);
    wss.ParseConfiguration();
    el::Configurations conf(weatherStationSettings.logg.configFile);
    el::Loggers::reconfigureAllLoggers(conf);

    DynamicSensorFactory dsf;
    auto* i2c = new I2cControl(weatherStationSettings.i2c.busno);
    LcdController lcdc;
    LcdDriver lcd(weatherStationSettings.i2c.lcd, i2c);

    //Create the date time page
    lcdc.createDateTimePage();

    //Start a new thread getting new sensors
    std::thread gettingSensorData (getNewSensorData, &dsf, i2c, &lcdc);
    LOG(INFO) << "Starting the getting sensor data thread. ID: "
         << gettingSensorData.get_id() << endl;
    usleep(5000000);
    lcdc.drawPage(currentPage, lcd);
    //Create a thread for the buttons
    std::thread listenForBtns (detectbtnpress, &lcdc, &lcd, &dsf);
    LOG(INFO) << "Starting the button listener thread. ID: "
              << listenForBtns.get_id() << endl;


    // Create another thread to draw to the LCD display
    std::thread updatingLcd (updateLcdCurrentPage, &dsf, &lcdc, &lcd);
    LOG(INFO) << "Starting the updating lcd thread. ID: "
         << updatingLcd.get_id() << endl;
    //Join the threads
    gettingSensorData.join();
    updatingLcd.join();
    listenForBtns.join();

}