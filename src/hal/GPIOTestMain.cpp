#include "GPIOControl.hpp"
#include <unistd.h>
#include <signal.h>

using namespace std;

int main (void)
{
    unsigned int microseconds = 30000;
    cout << "Testing.......\n\n";

    string storeValue;
    GPIOControl gpio4 = GPIOControl("4");
    GPIOControl gpio17 = GPIOControl("17");
    GPIOControl::Value GPIO_ON = GPIOControl::Value::GPIO_ON;
    GPIOControl::Value GPIO_OFF = GPIOControl::Value::GPIO_OFF;
    

    //Set the direction
    gpio4.g_setdir("out");
    gpio17.g_setdir("in");

    usleep(microseconds);        

    gpio17.g_getval(storeValue);

    if(storeValue == "0")
    {
        gpio4.g_setval(GPIO_ON);
    }
    else
    {
        gpio4.g_setval(GPIO_OFF);
    }    

    usleep(microseconds);    

    gpio4.g_setval(GPIO_OFF);

    return 0;
}

