#include "gtest/gtest.h"
#include "../../../include/BtnControl.hpp"


TEST(BTNControl, DefaultConstructor)
{
    BtnState bs;
    EXPECT_EQ(bs.lastButtonState, 0);
    EXPECT_EQ(bs.lastDebounceTime, 0);
    EXPECT_EQ(bs.debounceDelay, 0);
    EXPECT_EQ(bs.interruptTime, 0);
    EXPECT_EQ(bs.storedPinValue, 0);
    
}