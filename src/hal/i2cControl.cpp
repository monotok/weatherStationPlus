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

#include "../../include/i2cControl.hpp"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

I2cControl::I2cControl(int i2cBusNumber)
{
	char i2cBus[12];
	sprintf(i2cBus, "/dev/i2c-%i", i2cBusNumber);
	//----- OPEN THE I2C BUS -----
	char *filename = (char *)i2cBus;
	if ((this->file_i2c = open(filename, O_RDWR)) < 0)
	{
		error("Failed to open the i2c bus");
		exit(1);
	}
}

void I2cControl::writeByte(unsigned char address, unsigned char byte)
{
    lock_guard<mutex> guard(mu);
//    usleep(10000);
    if (address != this->I2C_ADDR)
	{
		this->I2C_ADDR = address;
		if (ioctl(this->file_i2c, I2C_SLAVE, this->I2C_ADDR) < 0)
		{
			error("Failed to acquire bus access and/or talk to slave.\n");
			exit(1);
		}
	}
	//----- WRITE BYTES -----
	unsigned char buffer[60] = {0};
	buffer[0] = byte;
	int length = 1;										 //<<< Number of bytes to write
	if (write(this->file_i2c, buffer, length) != length) //write() returns the number of bytes actually written
	{
        ostringstream ss;
        ss << this_thread::get_id();
        string msg = "write_i2c Failed to write to the i2c bus.\n" + ss.str();
        error(msg);
	}
}
//TODO: Take in and set the i2c address and then protect with mutex
void I2cControl::readI2c(unsigned char address, char *buffer, int length)
{
	//----- READ BYTES -----
    lock_guard<mutex> guard(mu);
//    usleep(10000);
    if (address != this->I2C_ADDR)
    {
        this->I2C_ADDR = address;
        if (ioctl(this->file_i2c, I2C_SLAVE, this->I2C_ADDR) < 0)
        {
            error("Failed to acquire bus access and/or talk to slave.\n");
            exit(1);
        }
    }
	if (read(this->file_i2c, buffer, length) != length) //read() returns the number of bytes actually read
	{
	    ostringstream ss;
	    ss << this_thread::get_id();
        string msg = "readI2c Failed to read from the i2c bus.\n" + ss.str();
		error(msg);
	}
}

void I2cControl::error(string msg)
{
	LOG(ERROR) << msg << " " << strerror(errno);
}

I2cControl::~I2cControl()
{
    close(file_i2c);
}
