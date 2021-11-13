//
// Created by hammer on 12/04/2020.
//

#ifndef WEATHERSTATIONPLUS_CONFIGPARSER_HPP
#define WEATHERSTATIONPLUS_CONFIGPARSER_HPP

#include <libconfig.h++>
#include "easylogging++.hpp"
#include "settings.hpp"
#include "yang.hpp"
#include <stdlib.h>
#include <string>

using namespace std;

class ConfigParser
{
private:
    libconfig::Config confsettings;
    Settings& wsettings;
    YangParser yp_settings = YangParser("settings");
    void readSettingsFile(const char* settingsFileLocation);
    void getVersion();
    void getDatabaseDetails(const libconfig::Setting& root);
    void getGPIODetails(const libconfig::Setting& root);
    void getLogDetails(const libconfig::Setting& root);
    void getI2cDetails(const libconfig::Setting& root);
    void getPositionInformation(const libconfig::Setting& root);
    Position& validatePosition(const string& position);

public:
    ConfigParser(Settings& wsettings, const char* settingsFileLocation = "conf/settings.conf",
                 const char* datafile = "conf/settings.xml");
    void ParseConfiguration();
    string getSensorsName(string sensorID);
    string getSensorReadingName(const string &sensorId, const string &readingId);
    Position& getSensorReadingPosition(const string& sensorId, const string& readingId);
    Position& matchNamePositionToValuePosition(Position& position);

};

#endif //WEATHERSTATIONPLUS_CONFIGPARSER_HPP
