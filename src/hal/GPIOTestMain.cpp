#include "GPIOControl.hpp"
#include <iostream>

using namespace std;

int main (void)
{
    cout << "Testing.......\n";
    
    GPIOControl gpio = GPIOControl("4");
    gpio.g_export();
    gpio.g_unexport();

    gpio.g_setdir("out");

    GPIOControl::Value val = GPIOControl::Value::GPIO_ON;
    gpio.g_setval(val);

    return 0;
}