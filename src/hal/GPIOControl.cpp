#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "GPIOControl.hpp"

using namespace std;

GPIOControl::GPIOControl(string selected_gpio_num)
{
    this->gpio_num = selected_gpio_num;
}

int GPIOControl::g_export()
{
    string exportPath = "/sys/class/gpio/export";
    ofstream exportStream(exportPath, ios_base::out);
    if(!exportStream)
    {
        cout << "Could not export GPIO " << this->gpio_num;
        return -1;
    }
    exportStream << this-> gpio_num;
    exportStream.close();
    return 0;
}

int GPIOControl::g_unexport()
{

}

int GPIOControl::g_setdir(enum Direction)
{

}

int GPIOControl::g_setval(enum Value)
{

}

int GPIOControl::g_getval(string val)
{

}

string GPIOControl::get_gpio_num()
{
    return this->gpio_num;
}