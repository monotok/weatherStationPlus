#include "GPIOControl.hpp"
#include <iostream>

using namespace std;

int main (void)
{
    cout << "Testing.......\n";
    
    GPIOControl gpio = GPIOControl("4");
    gpio.g_export();
}