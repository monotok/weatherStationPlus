//
// Created by hammer on 13/04/2020.
//

#ifndef WEATHERSTATIONPLUS_SETTINGS_HPP
#define WEATHERSTATIONPLUS_SETTINGS_HPP

#include <string>
#include <tuple>
#include "vector"
#include "cstring"

using namespace std;

struct Database {
    bool enable;
    string host;
    int port;
    string database;
    string user;
    string password;
    string timeout;
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

struct I2c {
    int busno;
    int atmega;
    int lcd;
};

struct Position {
    string name;
    int row_start = 0;
    int col_start = 0;
    Position() = default;
    Position(int row, int col): row_start(row), col_start(col) {};

    //So it can be used in a map
    bool operator<(const Position& pos) const
    {
        return std::tie(row_start, col_start) < std::tie(pos.row_start, pos.col_start);
    }

    //So it can be used in a test to compare positions
    bool operator==(const Position& pos) const
    {
        return std::tie(row_start, col_start) == std::tie(pos.row_start, pos.col_start);
    }
};

struct Settings {
    float version;
    struct Database db;
    struct Gpio gpio;
    struct Logging logg;
    struct I2c i2c;
    vector<Position> lcd_positions;

    Position& getPositionByName(const string& name) {
        for (auto &position: lcd_positions) {
            if (strcmp(position.name.c_str(), name.c_str()) == 0)
                return position;
        }
    }
};

#endif //WEATHERSTATIONPLUS_SETTINGS_HPP
