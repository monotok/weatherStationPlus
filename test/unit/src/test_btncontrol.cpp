#include "gtest/gtest.h"
#define private public
#include "../../../include/BtnControl.hpp"

TEST(BTNControl, button_pressed)
{
    BtnState bs;
    // Don't call the init as it calls GPIOControl which doesn't
    // exist on this system. We can pretend.
    bs.setCurrentBtnState(1);
    bs.setBtnInterruptTime();
    EXPECT_TRUE(bs.debounceBtn());
}

TEST(BTNControl, button_pressed_and_held_only_detect_one_press)
{
    BtnState bs;
    // Don't call the init as it calls GPIOControl which doesn't
    // exist on this system. We can pretend.
    int b = 0;
    bs.setCurrentBtnState(1);
    bs.setBtnInterruptTime();
    EXPECT_TRUE(bs.debounceBtn());
    b++;
    bs.reInitBtnState();

    while (b < 10)
    {
        bs.setCurrentBtnState(1);
        bs.setBtnInterruptTime();
        EXPECT_FALSE(bs.debounceBtn());
        b++;
        bs.reInitBtnState();
    }
}

TEST(BTNControl, button_not_pressed)
{
    BtnState bs;
    // Don't call the init as it calls GPIOControl which doesn't
    // exist on this system. We can pretend.
    bs.setCurrentBtnState(0);
    bs.setBtnInterruptTime();
    EXPECT_FALSE(bs.debounceBtn());
}