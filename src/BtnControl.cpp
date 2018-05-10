#include "BtnControl.hpp"

using namespace std;

BtnState::BtnState()
{
    this->lastButtonState = 0;
    this->lastDebounceTime = 0;
    this->debounceDelay = 500;
    this->interruptTime = 0;
}

bool BtnState::debounceBtn()
{
    if(this->currentButtonState == 1 && this->lastButtonState == 0 && this->interruptTime - this->lastDebounceTime > this->debounceDelay)
        return true;
    return false;
}

void BtnState::setCurrentBtnState(int btnState)
{
    this->currentButtonState = btnState;
}

void BtnState::setBtnInterruptTime()
{
    this->interruptTime = Utilities::getMicrotime();
}

void BtnState::updateLastBtnState()
{
    this->lastButtonState = this->currentButtonState;
}

void BtnState::updateLastDebounceTime()
{
    this->updateLastDebounceTime = this->interruptTime;
}