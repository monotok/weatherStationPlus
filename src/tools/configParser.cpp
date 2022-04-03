#include "../../include/configParser.hpp"

ConfigParser::ConfigParser(Settings& wsettings, const char* yangdir, const char* datafile): wsettings(wsettings), yp_settings(YangParser("settings", yangdir))
{
    yp_settings.parseData(datafile);
}

void ConfigParser::ParseConfiguration()
{
    const libconfig::Setting& root = confsettings.getRoot();
    try
    {
        getVersion();
        getDatabaseDetails(root);
        getGPIODetails(root);
        getLogDetails(root);
        getI2cDetails(root);
        cachePositions();

        LOG(INFO) << "Finished loading configuration settings from file." << endl;
    }
    catch(const libconfig::SettingNotFoundException &nfex)
    {
        LOG(WARNING) << "Setting not found" << endl;
    }
}

void ConfigParser::getVersion()
{
    wsettings.version = stof(yp_settings.getSpecificValue("/settings:settings/version"));
}

void ConfigParser::getDatabaseDetails(const libconfig::Setting &root)
{
    string base_xpath = "/settings:settings/database";
    wsettings.db.enable = Utilities::String2Bool(yp_settings.getSpecificValue(base_xpath + "/enable"));
    wsettings.db.host = yp_settings.getSpecificValue(base_xpath + "/host");
    wsettings.db.port = stoi(yp_settings.getSpecificValue(base_xpath + "/port"));
    wsettings.db.database = yp_settings.getSpecificValue(base_xpath + "/database-name");
    wsettings.db.user = yp_settings.getSpecificValue(base_xpath + "/user");
    wsettings.db.password = yp_settings.getSpecificValue(base_xpath + "/password");
    wsettings.db.timeout = yp_settings.getSpecificValue(base_xpath + "/connection-timeout");
}

void ConfigParser::getGPIODetails(const libconfig::Setting &root)
{
    string base_xpath = "/settings:settings/gpio";
    wsettings.gpio.gpio1 = yp_settings.getSpecificValue(base_xpath + "/gpio1");
    wsettings.gpio.gpio2 = yp_settings.getSpecificValue(base_xpath + "/gpio2");
    wsettings.gpio.gpio3 = yp_settings.getSpecificValue(base_xpath + "/gpio3");
    wsettings.gpio.gpio4 = yp_settings.getSpecificValue(base_xpath + "/gpio4");
    wsettings.gpio.gpio5 = yp_settings.getSpecificValue(base_xpath + "/gpio5");
}

void ConfigParser::getLogDetails(const libconfig::Setting &root)
{
    string base_xpath = "/settings:settings/logging";
    wsettings.logg.configFile = yp_settings.getSpecificValue(base_xpath + "/config-file");
}

void ConfigParser::getI2cDetails(const libconfig::Setting &root)
{
    string base_xpath = "/settings:settings/i2c";
    wsettings.i2c.busno = stoi(yp_settings.getSpecificValue(base_xpath + "/busno"));
    wsettings.i2c.atmega = stoi(yp_settings.getSpecificValue(base_xpath + "/atmega"));
    wsettings.i2c.lcd = stoi(yp_settings.getSpecificValue(base_xpath + "/lcd"));
}

string ConfigParser::getSensorsName(string sensorId)
{
    string xpath_str = "/settings:settings/sensors/wsensor[id='";
    xpath_str = xpath_str + sensorId + "']/name";
    return yp_settings.getSpecificValue(xpath_str);
}

string ConfigParser::getSensorReadingName(const string& sensorId, const string& readingId)
{
    string xpath_str = "/settings:settings/sensors/wsensor[id='";
    xpath_str = xpath_str + sensorId + "']/readings[id='" + readingId + "']/name";
    return yp_settings.getSpecificValue(xpath_str);
}

void ConfigParser::getSensorReadingPositions(const string &sensorId, const string &readingId, Position &namePosition,
                                             Position &valPosition)
{
    string xpath_str = "/settings:settings/sensors/wsensor[id='";
    xpath_str = xpath_str + sensorId + "']/readings[id='" + readingId + "']/position";
    auto value = yp_settings.getSpecificValue(xpath_str);
    if(value.empty())
        throw std::invalid_argument("Can't find reading's position. Check readings position defined in settings.");
    generatePosition(value, namePosition, valPosition);
}

void ConfigParser::generatePosition(const string &position, Position &namePosition, Position &valPosition)
{
    string xpath_str = "/settings:settings/lcd-positions[position='";
    xpath_str = xpath_str + position + "']/";
    namePosition.row_start = stoi(yp_settings.getSpecificValue(xpath_str+"name/row"));
    namePosition.col_start = stoi(yp_settings.getSpecificValue(xpath_str+"name/col"));
    valPosition.row_start = stoi(yp_settings.getSpecificValue(xpath_str+"val/row"));
    valPosition.col_start = stoi(yp_settings.getSpecificValue(xpath_str+"val/col"));
}

void ConfigParser::cachePositions()
{
    string xpath_str = "/settings:settings/lcd-positions";
    vector<string> lcd_paths_found = yp_settings.getPaths(xpath_str);
    for (auto &lcd_path: lcd_paths_found) {
        string position_name, delimiter = "'";
        Utilities::returnStringBetweenChars(lcd_path, position_name, delimiter);
        Position posName; posName.name = position_name+"_Name";
        posName.row_start = stoi(yp_settings.getSpecificValue(lcd_path+"/name/row"));
        posName.col_start = stoi(yp_settings.getSpecificValue(lcd_path+"/name/col"));
        Position posVal; posVal.name = position_name+"_Val";
        posVal.row_start = stoi(yp_settings.getSpecificValue(lcd_path+"/val/row"));
        posVal.col_start = stoi(yp_settings.getSpecificValue(lcd_path+"/val/col"));
        wsettings.lcd_positions.emplace_back(posName);
        wsettings.lcd_positions.emplace_back(posVal);
    }
}

bool ConfigParser::checkReadingExists(const string &sensorId, const string &readingId)
{
    string xpath_str = "/settings:settings/sensors/wsensor[id='";
    xpath_str = xpath_str + sensorId + "']/readings[id='" + readingId + "']/id";
    auto value = yp_settings.getSpecificValue(xpath_str);
    if(value.empty()) {
        LOG(WARNING) << "Sensor id " << sensorId << " and reading id " << readingId << " not found in settings. Ignoring." << endl;
        return false;
    }
    return true;
}


