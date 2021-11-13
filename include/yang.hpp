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
    explicit YangParser(const char *module);
    ~YangParser();
    void parseData(const char *datafile);
    string getSpecificValue(string xpath);

private:
    void loadYangModule(const char *module);

    struct ly_ctx *context = nullptr;
    struct lyd_node *tree = nullptr;
    struct lyd_node *node = nullptr;
};


#endif //WEATHERSTATIONPLUS_YANG_HPP
