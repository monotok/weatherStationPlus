#include "../../include/configParser.hpp"

ConfigParser::ConfigParser(Settings& wsettings, const char* settingsFileLocation): wsettings(wsettings)
{
    readSettingsFile(settingsFileLocation);
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
    wsettings.version = confsettings.lookup("version");
}

void ConfigParser::getDatabaseDetails(const libconfig::Setting &root)
{
    const libconfig::Setting &db = root["database"];
    db.lookupValue("host", wsettings.db.host);
    db.lookupValue("port", wsettings.db.port);
    db.lookupValue("database", wsettings.db.database);
    db.lookupValue("user", wsettings.db.user);
    db.lookupValue("password", wsettings.db.password);
}

void ConfigParser::getGPIODetails(const libconfig::Setting &root)
{
    const libconfig::Setting &gpio = root["gpio"];
    gpio.lookupValue("gpio1", wsettings.gpio.gpio1);
    gpio.lookupValue("gpio2", wsettings.gpio.gpio2);
    gpio.lookupValue("gpio3", wsettings.gpio.gpio3);
    gpio.lookupValue("gpio4", wsettings.gpio.gpio4);
    gpio.lookupValue("gpio5", wsettings.gpio.gpio5);
}

void ConfigParser::getLogDetails(const libconfig::Setting &root)
{
    const libconfig::Setting &logg = root["logging"];
    logg.lookupValue("configFile", wsettings.logg.configFile);
}

void ConfigParser::getI2cDetails(const libconfig::Setting &root)
{
    const libconfig::Setting &i2c = root["i2c"];
    i2c.lookupValue("busno", wsettings.i2c.busno);
    i2c.lookupValue("atmega", wsettings.i2c.atmega);
    i2c.lookupValue("lcd", wsettings.i2c.lcd);
}

string ConfigParser::getSensorsName(string sensorId)
{
    string sensorName = {"NotSet"};
    const libconfig::Setting& root = confsettings.getRoot();
    const libconfig::Setting &sen = root["sensors"];
    for (const auto & sen_iter : sen)
    {
        string foundSensor = {};
        if (sen_iter.lookupValue("id", foundSensor))
        {
            if (strcmp(foundSensor.c_str(), sensorId.c_str()) == 0)
            {
                sen_iter.lookupValue("name", sensorName);
            }
        }
    }
    return sensorName;
}

string ConfigParser::getSensorReadingName(const string& sensorId, const string& readingId)
{
    const libconfig::Setting& root = confsettings.getRoot();
    const libconfig::Setting& sen = root["sensors"];
    for (const auto & sen_iter : sen)
    {
        string foundSensor = {};
        string foundReading = {};
        if (sen_iter.lookupValue("id", foundSensor))
        {
            if (strcmp(foundSensor.c_str(), sensorId.c_str()) == 0)
            {
                for (const auto & reading_iter : sen_iter.lookup("readings"))
                {
                    if (reading_iter.lookupValue("id", foundReading))
                    {
                        if (strcmp(foundReading.c_str(), readingId.c_str()) == 0) {
                            return reading_iter.lookup("name");
                        }
                    }
                }
            }
        }
    }
    return ""; //Default return
}

Position& ConfigParser::getSensorReadingPosition(const string& sensorId, const string& readingId)
{
    const libconfig::Setting& root = confsettings.getRoot();
    const libconfig::Setting& sen = root["sensors"];
    for (const auto & sen_iter : sen)
    {
        string foundSensor = {};
        string foundReading = {};
        if (sen_iter.lookupValue("id", foundSensor))
        {
            if (strcmp(foundSensor.c_str(), sensorId.c_str()) == 0)
            {
                for (const auto & reading_iter : sen_iter.lookup("readings"))
                {
                    if (reading_iter.lookupValue("id", foundReading))
                    {
                        if (strcmp(foundReading.c_str(), readingId.c_str()) == 0) {
                            return validatePosition(reading_iter.lookup("position"));
                        }
                    }
                }
            }
        }
    }
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

