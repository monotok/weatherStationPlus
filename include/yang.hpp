//
// Created by hammer on 10/11/2021.
//

#ifndef WEATHERSTATIONPLUS_YANG_HPP
#define WEATHERSTATIONPLUS_YANG_HPP

#include <cstdio>
#include <libyang/libyang.h>
#include <cstdlib>
#include <string>

using namespace std;

class YangParser
{
public:
    YangParser(const char *module, const char *datafile);
    ~YangParser();
    string getSensorsName(string sensorID);

private:
    void loadYangModule(const char *module);
    void parseData(const char *datafile);
    struct ly_ctx *context;
    struct lyd_node *tree;
    struct lyd_node *node;

};


#endif //WEATHERSTATIONPLUS_YANG_HPP
