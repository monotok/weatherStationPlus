#include "../include/easylogging++.hpp"
#include "../include/main.hpp"
INITIALIZE_EASYLOGGINGPP

#define LCD_ADDRESS 0x3f // I2C device address
#define ATMEGA_ADDRESS 0x04 // I2C device address
#define i2cbusno 3

//TODO: Setup threading to run all the background stuff
//TODO: Add logging to all the functions like LCD controller.
//TODO: Define what the buttons do to control different bits eg. Change page.
//TODO: Setup lcd driver
//TODO: Build the sensors based on the incoming data from the arduino

using namespace std;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void getNewSensorData(DynamicSensorFactory* dynamsensors_ptr, I2cControl* i2c_ptr, LcdController* lcdc)
{
    pqxx::connection C("dbname = weather user = weather password = siesta_Usn_4Gulag_ \
      hostaddr = 172.16.20.5 port = 5432");
    if (C.is_open()) {
        LOG(INFO) << "Opened database successfully: " << C.dbname() << endl;
        RetrieveSenData rsd = RetrieveSenData(i2c_ptr, lcdc, ATMEGA_ADDRESS, &C);
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
    GPIOControl gpio17 = GPIOControl("17");
    gpio17.g_setdir("in");
    GPIOControl gpio27 = GPIOControl("27");
    gpio27.g_setdir("in");
    GPIOControl gpio22 = GPIOControl("22");
    gpio22.g_setdir("in");
    GPIOControl gpio18 = GPIOControl("18");
    gpio18.g_setdir("in");
    BtnState bs_17;
    BtnState bs_27;
    BtnState bs_22;
    BtnState bs_18;

    while(true)
    {
        bs_17.initBtnState(&gpio17);
        bs_27.initBtnState(&gpio27);
        bs_22.initBtnState(&gpio22);
        bs_18.initBtnState(&gpio18);
        if(bs_17.debounceBtn())
        {
            LOG(INFO) << "Button 17 Pressed";
            currentPage = lcdc->getNextPage(currentPage);
            lcd->clearDisplayClearMem();
            lcdc->drawPage(currentPage, *lcd);
        } else if(bs_27.debounceBtn())
        {
            LOG(INFO) << "Button 27 Pressed";
            WeatherSensor* weather_ptr = dsf->getWeatherSensor_ptr(currentPage);
            weather_ptr->switch_tempUnit();
            lcdc->updatePageValues(weather_ptr, *lcd);
        } else if(bs_22.debounceBtn())
        {
            LOG(INFO) << "Button 22 Pressed";
            lcd->clearDisplayClearMem();
            lcdc->drawDateTimePage(*lcd);
            currentPage = "date";
        } else if(bs_18.debounceBtn())
        {
            LOG(INFO) << "Button 18 Pressed";
            lcd->changeBacklight();
            lcdc->drawPage(currentPage, *lcd);
        }


        bs_17.reInitBtnState();
        bs_27.reInitBtnState();
        bs_22.reInitBtnState();
        bs_18.reInitBtnState();
        usleep(50000);
    }

}
#pragma clang diagnostic pop

int main(int argc, char** argv)
{
    START_EASYLOGGINGPP(argc, argv);
    el::Configurations conf("conf/logging.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    DynamicSensorFactory dsf;
    auto* i2c = new I2cControl(i2cbusno);
    LcdController lcdc;
    LcdDriver lcd(LCD_ADDRESS, i2c);

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