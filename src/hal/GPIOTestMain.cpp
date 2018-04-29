#include "GPIOControl.hpp"
#include <unistd.h>
#include <signal.h>
#include <time.h>

using namespace std;

void sig_handler(int sig);
bool ctrl_c_pressed = false;

void sig_handler(int sig)
{
    write(0,"\nCtrl^C pressed in sig handler\n",32);
    ctrl_c_pressed = true;
}

long getMicrotime()
{
  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC_RAW, &now );
  return (uint64_t)now.tv_sec * 1000000000U + (uint64_t)now.tv_nsec;
}

int main (void)
{

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }

    cout << "Testing.......\n\n";

    string storeValue;
    GPIOControl gpio4 = GPIOControl("4");
    GPIOControl gpio17 = GPIOControl("17");
    GPIOControl::Value GPIO_ON = GPIOControl::Value::GPIO_ON;
    GPIOControl::Value GPIO_OFF = GPIOControl::Value::GPIO_OFF;

    //Set the direction
    gpio4.g_setdir("out");
    gpio17.g_setdir("in");

    struct buttonState
    {
        int currentButtonState; // the current reading from the input pin
        int lastButtonState; // the previous reading from the input pin
        long lastDebounceTime; // the last time the output pin was toggled
        long debounceDelay; // the debounce time; increase if the output flickers
        long interruptTime;
    };

    struct buttonState testButton;

    testButton.lastButtonState = 1;
    testButton.lastDebounceTime = 0;
    testButton.debounceDelay = 500;
    testButton.interruptTime = 0;

    while(true)
    {
        testButton.currentButtonState = gpio17.g_getval(storeValue);
        testButton.interruptTime = getMicrotime();

        if(testButton.currentButtonState == 0 && testButton.lastButtonState == 1 && testButton.interruptTime - testButton.lastDebounceTime > testButton.debounceDelay)
        {
            cout << "Button Pressed\n" << endl;
            gpio4.g_setval(GPIO_ON);
        }
        else
        {
            gpio4.g_setval(GPIO_OFF);
        }

        testButton.lastButtonState = testButton.currentButtonState;
        testButton.lastDebounceTime = testButton.interruptTime;

        if(ctrl_c_pressed)
        {
            cout << "Ctrl^C Pressed" << endl;
            break;
        }

        usleep(50000);
    }

    return 0;
}

