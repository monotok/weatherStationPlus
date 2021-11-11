//
// Created by hammer on 10/11/2021.
//

#include "../../include/yang.hpp"
#include "../../include/easylogging++.hpp"

YangParser::YangParser(const char *module, const char *datafile)
{
    if (ly_ctx_new("yang", 0, &context)) {
        LOG(ERROR) << "Could not create libyang context" << endl;
        throw std::invalid_argument("Failed to create libyang context.");
    }
    loadYangModule(module);
    parseData(datafile);
}

YangParser::~YangParser()
{
    lyd_free_all(tree);
    ly_ctx_destroy(context);
}

void YangParser::loadYangModule(const char *module)
{
    if (ly_ctx_load_module(context, module, nullptr, nullptr) == nullptr) {
        LOG(ERROR) << "Could not load yang module " << module << endl;
        throw std::invalid_argument("Failed to load yang module");
    }
}

void YangParser::parseData(const char *datafile)
{
    struct ly_in *in = nullptr;
    ly_in_new_filepath(datafile, 0, &in);
    if (lyd_parse_data(context, nullptr, in, LYD_XML, 0, LYD_VALIDATE_PRESENT, &tree) != LY_SUCCESS) {
        LOG(ERROR) << "Could not parse data " << datafile << endl;
        throw std::invalid_argument("Failed to parse the data into the yang module");
    }
}

string YangParser::getSensorsName(string sensorID)
{
    string xpath_str = "/settings:settings/sensors/wsensor[id='";
    xpath_str = xpath_str + sensorID + "']/name";
    lyd_find_path(tree, xpath_str.c_str(), false, &node);
    return lyd_get_value(node);
}


