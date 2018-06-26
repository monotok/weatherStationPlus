#include "gtest/gtest.h"
#include "../../../include/GPIOControl.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

INITIALIZE_EASYLOGGINGPP

TEST(GPIOControl, DefaultContructor)
{
    GPIOControl gpio = GPIOControl("8");
    EXPECT_STREQ("8", gpio.get_gpio_num().c_str());
}

TEST(GPIOControl, GExport)
{
    GPIOControl gpio = GPIOControl("8");
    gpio.g_export();

    struct stat myStat;
    const char *path_system = "/sys/class/gpio/gpio8";
    int result = stat(path_system, &myStat);
    bool isdir = S_ISDIR(myStat.st_mode);
    EXPECT_TRUE(isdir);
}