#include "../include/easylogging++.hpp"
#include "../include/main.hpp"
INITIALIZE_EASYLOGGINGPP

#define lcdAdd 0x3f // I2C device address
#define i2cbusno 1

//TODO: Setup threading to run all the background stuff
//TODO: Get data from arduino (restrieveSenData)
//TODO: Add logging to all the functions like LCD controller.
//TODO: Define what the buttons do to control different bits eg. Change page.
//TODO: Setup lcd driver
//TODO: Build the sensors based on the incoming data from the arduino

using namespace std;

void getNewSensorData(DynamicSensorFactory* dynamsensors_ptr, I2cControl* i2c)
{
    RetrieveSenData rsd(i2c, lcdAdd);

}

int main(int argc, char** argv)
{
    unique_ptr<DynamicSensorFactory> dsf_uptr (new DynamicSensorFactory);
    unique_ptr<I2cControl> i2c_uptr (new I2cControl(i2cbusno));
}