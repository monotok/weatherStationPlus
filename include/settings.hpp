//
// Created by hammer on 13/04/2020.
//

#ifndef WEATHERSTATIONPLUS_SETTINGS_HPP
#define WEATHERSTATIONPLUS_SETTINGS_HPP

#include <string>

using namespace std;

struct Database {
    string host;
    int port;
    string database;
    string user;
    string password;
};

struct Gpio {
    string gpio1;
    string gpio2;
    string gpio3;
    string gpio4;
    string gpio5;
};

struct Logging {
    string configFile;
};

struct Settings {
    float version;
    struct Database db;
    struct Gpio gpio;
    struct Logging logg;
};

#endif //WEATHERSTATIONPLUS_SETTINGS_HPP
