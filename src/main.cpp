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

[[noreturn]] void getNewSensorData(DynamicSensorFactory* dynamsensors_ptr, I2cControl* i2c_ptr, LcdController* lcdc)
{
    string db_conn_str = "dbname = "+ weatherStationSettings.db.database +" user = "+ weatherStationSettings.db.user +" \
    password = "+ weatherStationSettings.db.password +" hostaddr = "+ weatherStationSettings.db.host
    +" port = " + to_string(weatherStationSettings.db.port);
    pqxx::connection C(db_conn_str);
    if (C.is_open()) {
        LOG(INFO) << "Opened database successfully: " << C.dbname() << endl;
        RetrieveSenData rsd = RetrieveSenData(i2c_ptr, lcdc, weatherStationSettings.i2c.atmega, &C, wss);
        while(true)
        {
            rsd.get_WeatherSenData(dynamsensors_ptr);
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

[[noreturn]] void updateLcdCurrentPage(DynamicSensorFactory* dynamsensors_ptr, LcdController* lcdc_ptr)
{
    while(true)
    {
        if(lcdc_ptr->getCurrentPage() == "date") {
            lcdc_ptr->updateDateTimePage();
            usleep(500000);
        } else
        {
            lcdc_ptr->updatePageValues(dynamsensors_ptr->getWeatherSensor_ptr(lcdc_ptr->getCurrentPage()));
            usleep(5000000);
        }
    }

}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

[[noreturn]] void detectbtnpress(LcdController *lcdc, DynamicSensorFactory *dsf)
{
    GPIOControl gpio1 = GPIOControl(weatherStationSettings.gpio.gpio1);
    gpio1.g_setdir("in");
    GPIOControl gpio2 = GPIOControl(weatherStationSettings.gpio.gpio2);
    gpio2.g_setdir("in");
    GPIOControl gpio3 = GPIOControl(weatherStationSettings.gpio.gpio3);
    gpio3.g_setdir("in");
    GPIOControl gpio4 = GPIOControl(weatherStationSettings.gpio.gpio4);
    gpio4.g_setdir("in");
    GPIOControl gpio5 = GPIOControl(weatherStationSettings.gpio.gpio5);
    gpio5.g_setdir("in");
    BtnState bs_1;
    BtnState bs_2;
    BtnState bs_3;
    BtnState bs_4;
    BtnState bs_5;

    while(true)
    {
        bs_1.initBtnState(&gpio1);
        bs_2.initBtnState(&gpio2);
        bs_3.initBtnState(&gpio3);
        bs_4.initBtnState(&gpio4);
        bs_5.initBtnState(&gpio5);
        if(bs_1.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio1 << " Pressed";
            lcdc->getNextPage();
            lcdc->clearDisplay();
            lcdc->drawPage_Locking();
        } else if(bs_2.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio2 << " Pressed";
//            WeatherSensor* weather_ptr = dsf->getWeatherSensor_ptr(currentPage, std::__cxx11::string());
//            weather_ptr->switch_tempUnit();
//            lcdc->updatePageValues(weather_ptr, *lcd);
        } else if(bs_3.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio3 << " Pressed";
            lcdc->clearDisplay();
            lcdc->drawDateTimePage();
            lcdc->setCurrentPage("date");
        } else if(bs_4.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio4 << " Pressed";
        } else if(bs_5.debounceBtn())
        {
            LOG(INFO) << "Button " << weatherStationSettings.gpio.gpio5 << " Pressed";
        }


        bs_1.reInitBtnState();
        bs_2.reInitBtnState();
        bs_3.reInitBtnState();
        bs_4.reInitBtnState();
        bs_5.reInitBtnState();
        usleep(50000);
    }

}
#pragma clang diagnostic pop

int main(int argc, char** argv)
{
    START_EASYLOGGINGPP(argc, argv);
    wss.ParseConfiguration();
    el::Configurations conf(weatherStationSettings.logg.configFile);
    el::Loggers::reconfigureAllLoggers(conf);

    DynamicSensorFactory dsf(wss);
    auto* i2c = new I2cControl(weatherStationSettings.i2c.busno);
    LcdDriver lcd(weatherStationSettings.i2c.lcd, i2c);
    LcdController lcdc(lcd);

    //Create the date time page
    lcdc.createDateTimePage();
    lcdc.setCurrentPage("date");
    usleep(5000000);
    lcdc.drawPage_Locking();

    //Start a new thread getting new sensors
    std::thread gettingSensorData (getNewSensorData, &dsf, i2c, &lcdc);
    LOG(INFO) << "Starting the getting sensor data thread. ID: "
         << gettingSensorData.get_id() << endl;

    //Create a thread for the buttons
    std::thread listenForBtns (detectbtnpress, &lcdc, &dsf);
    LOG(INFO) << "Starting the button listener thread. ID: "
              << listenForBtns.get_id() << endl;


    // Create another thread to draw to the LCD display
    std::thread updatingLcd (updateLcdCurrentPage, &dsf, &lcdc);
    LOG(INFO) << "Starting the updating lcd thread. ID: "
         << updatingLcd.get_id() << endl;
    //Join the threads
    gettingSensorData.join();
    updatingLcd.join();
    listenForBtns.join();

}