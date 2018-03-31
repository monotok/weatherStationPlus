#include "GPIOControl.hpp"
#include <iostream>
#include <unistd.h>

using namespace std;

int main (void)
{
    unsigned int microseconds = 3000;
    cout << "Testing.......\n";
    
    GPIOControl gpio = GPIOControl("4");
    gpio.g_export();

    gpio.g_setdir("out");

    GPIOControl::Value val = GPIOControl::Value::GPIO_ON;
    gpio.g_setval(val);

    usleep(microseconds);

    GPIOControl::Value valoff = GPIOControl::Value::GPIO_OFF;
    gpio.g_setval(valoff);

    string test = "Value";
    gpio.g_getval(test);

    gpio.g_unexport();

    cout << test << endl;

    return 0;
}