// Copyright 2018 Chris Hamer
/*    This file is part of RaspLCDDriver.

    RaspLCDDriver is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RaspLCDDriver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RaspLCDDriver.  If not, see <http://www.gnu.org/licenses/>. 
*/
#ifndef I2C_CONTROL_H
#define I2C_CONTROL_H
#include "easylogging++.hpp"

#include <unistd.h>        //Needed for I2C port
#include <fcntl.h>         //Needed for I2C port
#include <sys/ioctl.h>     //Needed for I2C port
#include <linux/i2c-dev.h> //Needed for I2C port
#include <mutex>
#include <thread>
#include <string>

class I2cControl
{
private:
  int file_i2c;
  unsigned char I2C_ADDR;
  void error(std::string msg);
  std::mutex mu;

public:
  I2cControl(int i2cNumber);
  ~I2cControl();
  void readI2c(unsigned char address, char *buffer, int length);
  void writeByte(unsigned char address, unsigned char byte);
};
#endif // i2c control