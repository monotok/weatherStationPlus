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
    exportStream << this->gpio_num;
    exportStream.flush();
    exportStream.close();

    if(!exportStream.good())
    {
        cout << "Could not export GPIO " << this->gpio_num << "\n" << endl;
        cout << "Bad bit " << exportStream.bad() << "\n" << endl;
        cout << "Good bit " << exportStream.good() << "\n" << endl;
        cout << "Fail bit " << exportStream.fail() << "\n" << endl;
        cout << "EOF bit " << exportStream.eof() << "\n" << endl;
        
        
        return -1;
    }

    cout << "Exported GPIO " << this->gpio_num << endl;

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