#ifndef GPIO_Control_H
#define GPIO_Control_H

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "../logging/easylogging++.h"

using namespace std;

class GPIOControl
{
    public:
        enum class Value {GPIO_ON = 1, GPIO_OFF = 0};
        GPIOControl(string x);
        ~GPIOControl();
        int g_export();
        int g_unexport();
        int g_setdir(string direction);
        int g_setval(enum GPIOControl::Value);
        int g_getval(int& val);

        string get_gpio_num();
    
    private:
        string gpio_num;
        int validateDirection(string direction);
};

#endif //ifndef GPIOControl
