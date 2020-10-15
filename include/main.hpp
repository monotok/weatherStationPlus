//
// Created by hammer on 01/10/2019.
//

#ifndef WEATHERSTATIONPLUS_MAIN_H
#define WEATHERSTATIONPLUS_MAIN_H

#include "data/DynamicSensorFactory.hpp"
#include "RetrieveSenData.hpp"
#include "i2cControl.hpp"
#include "lcdController.h"
#include "lcdDriver.hpp"
#include "GPIOControl.hpp"
#include "BtnControl.hpp"
#include "../include/configParser.hpp"

#include <pqxx/pqxx>
#include <thread>

Settings weatherStationSettings {};
ConfigParser wss(weatherStationSettings);

#endif //WEATHERSTATIONPLUS_MAIN_H
