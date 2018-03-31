#ifndef GPIO_Control_H
#define GPIO_Control_H

#include <string>

using namespace std;

class GPIOControl
{
    public:
        enum class Value {GPIO_ON = 1, GPIO_OFF = 0};
        GPIOControl(string x);
        int g_export();
        int g_unexport();
        int g_setdir(string direction);
        int g_setval(enum GPIOControl::Value);
        int g_getval(string val);

        string get_gpio_num();
    
    private:
        string gpio_num;
        int validateDirection(string direction);
};

#endif //ifndef GPIOControl
