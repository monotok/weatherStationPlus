#ifndef BTN_CONTROL_H
#define BTN_CONTROL_H
#include <gtest/gtest_prod.h>
#include "Utilities.hpp"
#include "GPIOControl.hpp"

using namespace std;

class BtnState
{
  public:
    BtnState();
    bool debounceBtn();
    void initBtnState(GPIOControl *gpioInputPin);
    void reInitBtnState();

  private:
    int currentButtonState;    // the current reading from the input pin
    int lastButtonState;       // the previous reading from the input pin
    uint64_t lastDebounceTime; // the last time the output pin was toggled
    long debounceDelay;        // the debounce time; increase if the output flickers
    uint64_t interruptTime;    // a timer to calc time between button presses
    int storedPinValue;        // Stores the value of GPIO PIN from sys to check current state
    GPIOControl *gpioInputPin;

    void setCurrentBtnState(int btnState);
    void setBtnInterruptTime();
    void updateLastDebounceTime();
    void updateLastBtnState();

    FRIEND_TEST(BTNControl, button_pressed);
    FRIEND_TEST(BTNControl, button_pressed_and_held_only_detect_one_press);
    FRIEND_TEST(BTNControl, button_not_pressed);

};

#endif //ifndef BTN_CONTROL_H
