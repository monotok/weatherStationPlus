#include "GPIOControl.hpp"
#include <unistd.h>
#include <signal.h>

using namespace std;

void sig_handler(int sig);
bool ctrl_c_pressed = false;

void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}

int main (void)
{
    unsigned int microseconds = 100000;
    cout << "Testing.......\n\n";

    string storeValue;
    GPIOControl gpio4 = GPIOControl("4");
    GPIOControl gpio17 = GPIOControl("17");
    GPIOControl::Value GPIO_ON = GPIOControl::Value::GPIO_ON;
    GPIOControl::Value GPIO_OFF = GPIOControl::Value::GPIO_OFF;

    //Set the direction
    gpio4.g_setdir("out");
    gpio17.g_setdir("in");

    while(true)
    {
        usleep(microseconds);        
        gpio17.g_getval(storeValue);

        if(storeValue == "0")
        {
            cout << "Button Pressed\n" << endl;
            usleep(20000);
            gpio17.g_getval(storeValue);
            if(storeValue == "0")
            {
                cout << "Certain Button Pressed\n" << endl;
                gpio4.g_setval(GPIO_ON);

                while(storeValue == "0")
                {
                    gpio17.g_getval(storeValue);                    
                }                
                cout << "Unpressed the button\n" << endl;
            }
            else
                cout << "Was just noise\n" << endl;             
        }

        gpio4.g_setval(GPIO_OFF);        

    }

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }

    return 0;
}

