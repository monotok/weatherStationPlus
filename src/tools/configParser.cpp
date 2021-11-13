#include "../../include/configParser.hpp"

ConfigParser::ConfigParser(Settings& wsettings, const char* settingsFileLocation, const char* datafile): wsettings(wsettings)
{
    readSettingsFile(settingsFileLocation);
    yp_settings.parseData(datafile);
}

void ConfigParser::readSettingsFile(const char* settingsFileLocation)
{
    try
    {
        confsettings.readFile(settingsFileLocation);
    }
    catch (const libconfig::FileIOException &fioex)
    {
        LOG(ERROR) << "Error while reading settings file: " << fioex.what() << endl;
        exit(1);
    }
    catch (const libconfig::ParseException &pex)
    {
        LOG(ERROR) << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                    << " - " << pex.getError() << endl;
    }
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
        getPositionInformation(root);

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
    wsettings.db.host = yp_settings.getSpecificValue(base_xpath + "/host");
    wsettings.db.port = stoi(yp_settings.getSpecificValue(base_xpath + "/port"));
    wsettings.db.database = yp_settings.getSpecificValue(base_xpath + "/database-name");
    wsettings.db.user = yp_settings.getSpecificValue(base_xpath + "/user");
    wsettings.db.password = yp_settings.getSpecificValue(base_xpath + "/password");
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

Position& ConfigParser::getSensorReadingPosition(const string& sensorId, const string& readingId)
{
    string xpath_str = "/settings:settings/sensors/wsensor[id='";
    xpath_str = xpath_str + sensorId + "']/readings[id='" + readingId + "']/position";
    auto value = yp_settings.getSpecificValue(xpath_str);
    if (!value.empty())
        return validatePosition(value);
    return wsettings.topleft_Val; //Default return
}

void ConfigParser::getPositionInformation(const libconfig::Setting &root)
{
    try
    {
        const libconfig::Setting &lcdPos = root["lcdPosition"];
        wsettings.topleft_Name.row_start = lcdPos.lookup("topleft_Name.row");
        wsettings.topleft_Name.col_start = lcdPos.lookup("topleft_Name.col");
        wsettings.topleft_Val.row_start = lcdPos.lookup("topleft_Val.row");
        wsettings.topleft_Val.col_start = lcdPos.lookup("topleft_Val.col");

        wsettings.topright_Name.row_start = lcdPos.lookup("topright_Name.row");
        wsettings.topright_Name.col_start = lcdPos.lookup("topright_Name.col");
        wsettings.topright_Val.row_start = lcdPos.lookup("topright_Val.row");
        wsettings.topright_Val.col_start = lcdPos.lookup("topright_Val.col");

        wsettings.middleleft_Name.row_start = lcdPos.lookup("middleleft_Name.row");
        wsettings.middleleft_Name.col_start = lcdPos.lookup("middleleft_Name.col");
        wsettings.middleleft_Val.row_start = lcdPos.lookup("middleleft_Val.row");
        wsettings.middleleft_Val.col_start = lcdPos.lookup("middleleft_Val.col");

        wsettings.middleright_Name.row_start = lcdPos.lookup("middleright_Name.row");
        wsettings.middleright_Name.col_start = lcdPos.lookup("middleright_Name.col");
        wsettings.middleright_Val.row_start = lcdPos.lookup("middleright_Val.row");
        wsettings.middleright_Val.col_start = lcdPos.lookup("middleright_Val.col");

        wsettings.bottomleft_Name.row_start = lcdPos.lookup("bottomleft_Name.row");
        wsettings.bottomleft_Name.col_start = lcdPos.lookup("bottomleft_Name.col");
        wsettings.bottomleft_Val.row_start = lcdPos.lookup("bottomleft_Val.row");
        wsettings.bottomleft_Val.col_start = lcdPos.lookup("bottomleft_Val.col");

        wsettings.bottomright_Name.row_start = lcdPos.lookup("bottomright_Name.row");
        wsettings.bottomright_Name.col_start = lcdPos.lookup("bottomright_Name.col");
        wsettings.bottomright_Val.row_start = lcdPos.lookup("bottomright_Val.row");
        wsettings.bottomright_Val.col_start = lcdPos.lookup("bottomright_Val.col");
    }
    catch(const libconfig::SettingNotFoundException &nfex)
    {
        LOG(INFO) << "Settings for lcd position not found" << endl;
    }

}

Position& ConfigParser::validatePosition(const string& position)
{
    map<string, Position> availablePositions {
            {"topleft", wsettings.topleft_Val}  ,
            {"topright", wsettings.topright_Val}  ,
            {"middleleft", wsettings.middleleft_Val}  ,
            {"middleright", wsettings.middleright_Val}  ,
            {"bottomleft", wsettings.bottomleft_Val}  ,
            {"bottomright", wsettings.bottomright_Val}
    };

    auto itr = availablePositions.find(position);
    if (itr != availablePositions.end()) {
        return itr->second;
    }
    LOG(ERROR) << "Invalid option for position." << endl;
    return wsettings.topleft_Val;
}

Position& ConfigParser::matchNamePositionToValuePosition(Position& position)
{
    map<Position, Position> matchingPositions {
            {wsettings.topleft_Val, wsettings.topleft_Name}  ,
            {wsettings.topright_Val, wsettings.topright_Name}  ,
            {wsettings.middleleft_Val, wsettings.middleleft_Name}  ,
            {wsettings.middleright_Val, wsettings.middleright_Name}  ,
            {wsettings.bottomleft_Val, wsettings.bottomleft_Name}  ,
            {wsettings.bottomright_Val, wsettings.bottomright_Name}
    };

    auto itr = matchingPositions.find(position);
    if (itr != matchingPositions.end()) {
        return itr->second;
    }
    LOG(ERROR) << "Can't match value position to name position." << endl;
    return wsettings.topleft_Name;
}

