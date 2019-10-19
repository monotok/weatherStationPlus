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
    RetrieveSenData rsd = RetrieveSenData(i2c_ptr, lcdc, ATMEGA_ADDRESS);
    while(true)
    {
        rsd.get_LocalWeatherData(dynamsensors_ptr);
        WeatherSensor* here = dynamsensors_ptr->getWeatherSensor_ptr("Here");
        LOG(DEBUG) << "\n\nLocal ID: " << here->get_sensorID() << "\n"
                   << "Local Temp: " << Utilities::to_string_with_precision<float>(here->get_temperature(), 2) << "\n"
                   << "Local Humidity: " << Utilities::to_string_with_precision<float>(here->get_humidity(), 2) << endl;

        usleep(3000000);

        rsd.get_RemoteWeatherSenData(dynamsensors_ptr);
        WeatherSensor* remote = dynamsensors_ptr->getWeatherSensor_ptr("BackBed");
        LOG(DEBUG) << "\n\nRemote ID: " << remote->get_sensorID() << "\n"
                   << "Remote Temp: " << Utilities::to_string_with_precision<float>(remote->get_temperature(), 2) << "\n"
                   << "Remote Humidity: " << Utilities::to_string_with_precision<float>(remote->get_humidity(), 2) << endl;

        usleep(3000000);
    }

}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void updateLcdWeatherPage(DynamicSensorFactory* dynamsensors_ptr, LcdController* lcdc_ptr, LcdDriver lcd)
{
    while(true)
    {
        lcdc_ptr->updatePageValues(dynamsensors_ptr->getWeatherSensor_ptr(currentPage), lcd);
        usleep(5000000);
    }

}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void detectbtnpress(LcdController *lcdc, LcdDriver lcd)
{
    GPIOControl gpio17 = GPIOControl("17");
    gpio17.g_setdir("in");
    BtnState bs;

    while(true)
    {
        bs.initBtnState(&gpio17);
        if(bs.debounceBtn())
        {
            LOG(DEBUG) << "Button Pressed";
            currentPage = lcdc->getNextPage(currentPage);
            lcd.clearDisplayClearMem();
            lcdc->drawPage(currentPage, lcd);
        }
        bs.reInitBtnState();
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

    //Start a new thread getting new sensors
    std::thread gettingSensorData (getNewSensorData, &dsf, i2c, &lcdc);
    LOG(INFO) << "Starting the getting sensor data thread. ID: "
         << gettingSensorData.get_id() << endl;
    usleep(5000000);
    lcdc.drawPage(currentPage, lcd);
    //Create a thread for the buttons
    std::thread listenForBtns (detectbtnpress, &lcdc, lcd);
    LOG(INFO) << "Starting the button listener thread. ID: "
              << listenForBtns.get_id() << endl;


    // Create another thread to draw to the LCD display
    std::thread updatingLcd (updateLcdWeatherPage, &dsf, &lcdc, lcd);
    LOG(INFO) << "Starting the updating lcd thread. ID: "
         << updatingLcd.get_id() << endl;
    //Join the threads
    gettingSensorData.join();
    updatingLcd.join();
    listenForBtns.join();

}