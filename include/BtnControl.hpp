#ifndef BTN_CONTROL_H
#define BTN_CONTROL_H
#include "Utilities.hpp"

using namespace std;

class BtnState
{
    public:
        BtnState();
        ~BtnState();
        void setCurrentBtnState(int btnState);
        void setBtnInterruptTime();
        void updateLastDebounceTime();
        void updateLastBtnState();
        bool debounceBtn();


    private:
        int currentButtonState; // the current reading from the input pin
        int lastButtonState; // the previous reading from the input pin
        long lastDebounceTime; // the last time the output pin was toggled
        long debounceDelay; // the debounce time; increase if the output flickers
        long interruptTime; // a timer to calc time between button presses

};

#endif //ifndef BTN_CONTROL_H
