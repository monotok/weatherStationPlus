#include "GPIOControl.hpp"
#include <unistd.h>
#include <signal.h>

using namespace std;

void sig_handler(int sig);
bool ctrl_c_pressed = false;

int main (void)
{
    unsigned int microseconds = 3000;
    cout << "Testing.......\n\n";

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }

    string storeValue;
    GPIOControl gpio4 = GPIOControl("4");
    GPIOControl gpio17 = GPIOControl("17");
    GPIOControl::Value GPIO_ON = GPIOControl::Value::GPIO_ON;
    GPIOControl::Value GPIO_OFF = GPIOControl::Value::GPIO_OFF;
    

    //Export the values
    gpio4.g_export();
    gpio17.g_export();

    //Set the direction
    gpio4.g_setdir("in");
    gpio17.g_setdir("out");

    while(1)
    {
        usleep(microseconds);
        gpio17.g_getval(storeValue);

        if(storeValue == "0")
        {
            cout << "Button is pressed" << endl;
            gpio4.g_setval(GPIO_ON);
        }
        else{
            cout << "Button is NOT pressed" << endl;            
            gpio4.g_setval(GPIO_OFF);
        }

        if(ctrl_c_pressed)
        {
            cout << "Ctrl^C Pressed" << endl;
            cout << "unexporting pins" << endl;
            gpio4.g_unexport();
            gpio17.g_unexport();
            break;

        }

    }

    return 0;
}

void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}