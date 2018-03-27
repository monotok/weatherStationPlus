#ifndef GPIOControl
#define GPIOControl

#include <string>
using namespace std;

class GPIOControl
{
    public:
        GPIOControl(string x);
        int g_export();
        int g_unexport();
        int g_setdir(enum dir);
        int g_setval(enum val);
        int g_getval(string val);

        string get_gpio_num();
    
    private:
        string current_gpio_num;
};

#endif //ifndef GPIOControl