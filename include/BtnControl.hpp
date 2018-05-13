#ifndef BTN_CONTROL_H
#define BTN_CONTROL_H
#include "Utilities.hpp"
#include "GPIOControl.hpp"

using namespace std;

class BtnState
{
    public:
        BtnState();
        void updateLastDebounceTime();
        void updateLastBtnState();
        bool debounceBtn();
        void reReadBtnState(GPIOControl *gpioInputPin);


    private:
        int currentButtonState; // the current reading from the input pin
        int lastButtonState; // the previous reading from the input pin
        long lastDebounceTime; // the last time the output pin was toggled
        long debounceDelay; // the debounce time; increase if the output flickers
        long interruptTime; // a timer to calc time between button presses
        int storedPinValue; // Stores the value of GPIO PIN from sys to check current state
        GPIOControl *gpioInputPin;

        void setCurrentBtnState(int btnState);
        void setBtnInterruptTime();        

};

#endif //ifndef BTN_CONTROL_H
