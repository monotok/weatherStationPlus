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
        return -1;
    }

    cout << "Exported GPIO " << this->gpio_num << endl;

    return 0;
}

int GPIOControl::g_unexport()
{
    string unexportPath = "/sys/class/gpio/unexport";
    ofstream unexportStream(unexportPath, ios_base::out);
    unexportStream << this->gpio_num;
    unexportStream.flush();
    unexportStream.close();

    if(!unexportStream.good())
    {
        cout << "Could not unexport GPIO " << this->gpio_num << "\n" << endl;
        return -1;
    }

    cout << "Unexported GPIO " << this->gpio_num << endl;

    return 0;
}

int GPIOControl::g_setdir(string chosenDirection)
{
    if(GPIOControl::validateDirection(chosenDirection) == -1)
    {
        cout << "Invalid direction. Should be 'out' or 'in' \n" << endl;
        return -1;
    }
    string directionPath = "/sys/class/gpio/gpio" + this->get_gpio_num() + "/direction";
    ofstream directionStream(directionPath, ios_base::out);
    directionStream << chosenDirection;
    directionStream.flush();
    directionStream.close();

    if(!directionStream.good())
    {
        cout << "Could not set direction of GPIO " << this->gpio_num << " to " << chosenDirection << "\n" << endl;
        return -1;
    }

    cout << "Set Direction on GPIO " << this->gpio_num << " to " << chosenDirection << "\n" << endl;

    return 0;

}

int GPIOControl::g_setval(enum GPIOControl::Value chosenValue)
{
    string valuePath = "/sys/class/gpio/gpio" + this->get_gpio_num() + "/value";
    ofstream valueStream(valuePath, ios_base::out);
    valueStream << static_cast<int>(chosenValue);
    valueStream.flush();
    valueStream.close();

    if(!valueStream.good())
    {
        cout << "Could not set value on GPIO " << this->gpio_num << " to " << static_cast<int>(chosenValue) << "\n" << endl;
        return -1;
    }

    cout << "Set value on GPIO " << this->gpio_num << " to " << static_cast<int>(chosenValue) << "\n" << endl;

    return 0;
    cout << "Set Value on GPIO " << this->gpio_num << " to " << static_cast<int>(chosenValue) << endl;

}

int GPIOControl::g_getval(string val)
{
    string valuePath = "/sys/class/gpio/gpio" + this->get_gpio_num() + "/value";
    ifstream valueStream(valuePath, ios_base::in);
    valueStream >> val;
    valueStream.close();

    if(!valueStream.good())
    {
        cout << "Could not read value of GPIO " << this->gpio_num << endl;
        return -1;
    }

    cout << "Value of GPIO " << this->gpio_num << " is " << val << "\n" << endl;

    return 0;

}

string GPIOControl::get_gpio_num()
{
    return this->gpio_num;
}

int GPIOControl::validateDirection(string directionToValidate)
{
    if(directionToValidate.compare("out") == 0 || directionToValidate.compare("in") == 0)
    {
        return 0;
    }
    return -1;
}