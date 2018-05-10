#include "GPIOControl.hpp"

using namespace std;

GPIOControl::GPIOControl(string selected_gpio_num)
{
    this->gpio_num = selected_gpio_num;
    this->g_export();
}

GPIOControl::~GPIOControl()
{
    this->g_unexport();
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
        LOG(ERROR) << "Could not export GPIO " << this->gpio_num;
        return -1;
    }

    VLOG(8) << "Exported GPIO " << this->gpio_num;

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
        LOG(ERROR) << "Could not unexport GPIO " << this->gpio_num;
        return -1;
    }

    VLOG(8) << "Unexported GPIO " << this->gpio_num;

    return 0;
}

int GPIOControl::g_setdir(string chosenDirection)
{
    if(GPIOControl::validateDirection(chosenDirection) == -1)
    {
        LOG(ERROR) << "Invalid direction. Should be 'out' or 'in'";
        return -1;
    }
    string directionPath = "/sys/class/gpio/gpio" + this->get_gpio_num() + "/direction";
    ofstream directionStream(directionPath, ios_base::out);
    directionStream << chosenDirection;
    directionStream.flush();
    directionStream.close();

    if(!directionStream.good())
    {
        LOG(ERROR) << "Could not set direction of GPIO " << this->gpio_num << " to " << chosenDirection;
        return -1;
    }

    VLOG(9) << "Set Direction on GPIO " << this->gpio_num << " to " << chosenDirection;

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
        LOG(ERROR) << "Could not set value on GPIO " << this->gpio_num << " to " << static_cast<int>(chosenValue);
        return -1;
    }

    VLOG(9) << "Set value on GPIO " << this->gpio_num << " to " << static_cast<int>(chosenValue);

    return 0;
}

int GPIOControl::g_getval(int& val)
{
    string valuePath = "/sys/class/gpio/gpio" + this->get_gpio_num() + "/value";
    ifstream valueStream(valuePath, ios_base::in);
    valueStream >> val;
    valueStream.close();

    if(!valueStream.good())
    {
        LOG(ERROR) << "Could not read value of GPIO " << this->gpio_num;
        return -1;
    }

    VLOG(9) << "Value of GPIO " << this->gpio_num << " is " << val;

    return 0;

}

string GPIOControl::get_gpio_num()
{
    return this->gpio_num;
}

int GPIOControl::validateDirection(string directionToValidate)
{
    if(directionToValidate.compare("out") != 0 && directionToValidate.compare("in") != 0)
    {
        return -1;
    }
    return 0;
}