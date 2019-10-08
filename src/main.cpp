#include "../include/easylogging++.hpp"
#include "../include/main.hpp"
INITIALIZE_EASYLOGGINGPP

#define LCD_ADDRESS 0x3f // I2C device address
#define ATMEGA_ADDRESS 0x04 // I2C device address
#define i2cbusno 1

//TODO: Setup threading to run all the background stuff
//TODO: Get data from arduino (restrieveSenData)
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
        usleep(3000000);
        rsd.get_RemoteWeatherSenData(dynamsensors_ptr);
        usleep(3000000);

        WeatherSensor* here = dynamsensors_ptr->getWeatherSensor_ptr("Here");
        cout << "\n\nLocal ID: " << here->get_sensorID() << "\n"
             << "Local Temp: " << here->get_temperature() << "\n"
             << "Local Humidity: " << here->get_humidity() << endl;

        WeatherSensor* remote = dynamsensors_ptr->getWeatherSensor_ptr("BackBed");
        cout << "Remote ID: " << remote->get_sensorID() << "\n"
             << "Remote Temp: " << remote->get_temperature() << "\n"
             << "Remote Humidity: " << remote->get_humidity() << endl;
    }

}
#pragma clang diagnostic pop

int main(int argc, char** argv)
{
    DynamicSensorFactory dsf = DynamicSensorFactory();
    I2cControl i2c = I2cControl(i2cbusno);

    //Start a new thread getting new sensors
    std::thread gettingSensorData (getNewSensorData, &dsf, &i2c);
    //Join the threads
    gettingSensorData.join();

}