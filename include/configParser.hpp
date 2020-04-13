//
// Created by hammer on 12/04/2020.
//

#ifndef WEATHERSTATIONPLUS_CONFIGPARSER_HPP
#define WEATHERSTATIONPLUS_CONFIGPARSER_HPP

#include <libconfig.h++>
#include "easylogging++.hpp"
#include <stdlib.h>
#include <string>

using namespace std;

class ConfigParser
{
private:
    libconfig::Config confsettings;
    void readSettingsFile(const char* settingsFileLocation);
    string getStringValue();

public:
    ConfigParser(const char* settingsFileLocation = "conf/settings.conf");
    string getVersion();
};

#endif //WEATHERSTATIONPLUS_CONFIGPARSER_HPP
