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
void getNewSensorData(DynamicSensorFactory* dynamsensors_ptr, I2cControl* i2c_ptr)
{
    RetrieveSenData rsd = RetrieveSenData(i2c_ptr, ATMEGA_ADDRESS);
    while(true)
    {
        rsd.get_LocalWeatherData(dynamsensors_ptr);
        WeatherSensor* here = dynamsensors_ptr->getWeatherSensor_ptr("Here");
        LOG(INFO) << "\n\nLocal ID: " << here->get_sensorID() << "\n"
                   << "Local Temp: " << Utilities::to_string_with_precision<float>(here->get_temperature()) << "\n"
                   << "Local Humidity: " << Utilities::to_string_with_precision<float>(here->get_humidity()) << endl;

        usleep(3000000);

        rsd.get_RemoteWeatherSenData(dynamsensors_ptr);
        WeatherSensor* remote = dynamsensors_ptr->getWeatherSensor_ptr("BackBed");
        LOG(INFO) << "Remote ID: " << remote->get_sensorID() << "\n"
                   << "Remote Temp: " << Utilities::to_string_with_precision<float>(remote->get_temperature()) << "\n"
                   << "Remote Humidity: " << Utilities::to_string_with_precision<float>(remote->get_humidity()) << endl;

        usleep(3000000);
    }

}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void updateLcdWeatherPage(WeatherSensor* ws, LcdController* lcdc_ptr, LcdDriver* lcd_ptr)
{
    while(true)
    {
        lcdc_ptr->updatePageValues(ws, *lcd_ptr);
        usleep(5000000);
    }

}
#pragma clang diagnostic pop

int main(int argc, char** argv)
{
    START_EASYLOGGINGPP(argc, argv);
    el::Configurations conf("conf/logging.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    DynamicSensorFactory dsf = DynamicSensorFactory();
    I2cControl* i2c = new I2cControl(i2cbusno);
    LcdController lcdc;
    LcdDriver lcd(LCD_ADDRESS, i2c);

    //Start a new thread getting new sensors
    std::thread gettingSensorData (getNewSensorData, &dsf, i2c);
    LOG(INFO) << "Starting the getting sensor data thread. ID: "
         << gettingSensorData.get_id() << endl;
    usleep(5000000);
    lcdc.createWeatherPage(dsf.getWeatherSensor_ptr("Here"));
    lcdc.drawPage("Here", lcd);

    // Create another thread to draw to the LCD display
    std::thread updatingLcd (updateLcdWeatherPage, dsf.getWeatherSensor_ptr("Here"), &lcdc, &lcd);
    LOG(INFO) << "Starting the updating lcd thread. ID: "
         << updatingLcd.get_id() << endl;
    //Join the threads
    gettingSensorData.join();

}