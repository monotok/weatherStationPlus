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
    Settings& wsettings;
    void readSettingsFile(const char* settingsFileLocation);
    void getVersion();
    void getDatabaseDetails(const libconfig::Setting& root);
    void getGPIODetails(const libconfig::Setting& root);
    void getLogDetails(const libconfig::Setting& root);

public:
    ConfigParser(Settings& wsettings, const char* settingsFileLocation = "conf/settings.conf");
    void ParseConfiguration();
};

#endif //WEATHERSTATIONPLUS_CONFIGPARSER_HPP
