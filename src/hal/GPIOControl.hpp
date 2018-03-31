#ifndef GPIO_Control_H
#define GPIO_Control_H

#include <string>

using namespace std;

enum class Direction {in, out};
enum class Value {GPIO_ON = 1, GPIO_OFF = 0};

class GPIOControl
{
    public:
        GPIOControl(string x);
        int g_export();
        int g_unexport();
        int g_setdir(enum Direction);
        int g_setval(enum Value);
        int g_getval(string val);

        string get_gpio_num();
    
    private:
        string gpio_num;
};

#endif //ifndef GPIOControl
