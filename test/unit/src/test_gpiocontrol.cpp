#include "gtest/gtest.h"
#include "../../../include/GPIOControl.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

INITIALIZE_EASYLOGGINGPP

TEST(GPIOControl, DefaultContructor)
{
    GPIOControl gpio = GPIOControl("17");
    EXPECT_STREQ("17", gpio.get_gpio_num().c_str());
}

TEST(GPIOControl, DefaultDestructor)
{
    GPIOControl gpio = GPIOControl("17");
    EXPECT_STREQ("17", gpio.get_gpio_num().c_str());
    delete gpio;

    struct stat myStat;
    const char *path_system = "/sys/class/gpio/gpio17/";
    int result = stat(path_system, &myStat);
    bool isdir = S_ISDIR(myStat.st_mode);
    EXPECT_FALSE(isdir);
}

TEST(GPIOControl, GExport)
{
    GPIOControl gpio = GPIOControl("17");

    struct stat myStat;
    const char *path_system = "/sys/class/gpio/gpio17/";
    int result = stat(path_system, &myStat);
    bool isdir = S_ISDIR(myStat.st_mode);
    EXPECT_TRUE(isdir);
}