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
#include "Utilities.hpp"

using namespace std;

class ConfigParser
{
private:
    libconfig::Config confsettings;
    Settings& wsettings;
    YangParser yp_settings;
    void getVersion();
    void getDatabaseDetails(const libconfig::Setting& root);
    void getGPIODetails(const libconfig::Setting& root);
    void getLogDetails(const libconfig::Setting& root);
    void getI2cDetails(const libconfig::Setting& root);
    void generatePosition(const string &position, Position &namePosition, Position &valPosition);
    void cachePositions();

public:
    ConfigParser(Settings& wsettings, const char* yangdir = "yang",
                 const char* datafile = "conf/settings.xml");
    void ParseConfiguration();
    string getSensorsName(string sensorID);
    string getSensorReadingName(const string &sensorId, const string &readingId);
    void getSensorReadingPositions(const string &sensorId, const string &readingId, Position &namePosition,
                                   Position &valPosition);

    bool checkReadingExists(const string &sensorId, const string &readingId);
};

#endif //WEATHERSTATIONPLUS_CONFIGPARSER_HPP
