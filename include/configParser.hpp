//
// Created by hammer on 12/04/2020.
//

#ifndef WEATHERSTATIONPLUS_CONFIGPARSER_HPP
#define WEATHERSTATIONPLUS_CONFIGPARSER_HPP

#include <libconfig.h++>
#include "easylogging++.hpp"
#include "settings.hpp"
#include <stdlib.h>
#include <string>

using namespace std;

class ConfigParser
{
private:
    libconfig::Config confsettings;
    settings* wsettings;
    void readSettingsFile(const char* settingsFileLocation);

public:
    ConfigParser(settings* wsettings, const char* settingsFileLocation = "conf/settings.conf");
    void getVersion();
};

#endif //WEATHERSTATIONPLUS_CONFIGPARSER_HPP
