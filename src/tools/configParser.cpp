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
    }
    catch(const libconfig::SettingNotFoundException &nfex)
    {
        LOG(INFO) << "Setting not found" << endl;
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
}

void ConfigParser::getGPIODetails(const libconfig::Setting &root)
{
    const libconfig::Setting &gpio = root["gpio"];
    gpio.lookupValue("gpio3", wsettings.gpio.gpio3);
}

void ConfigParser::getLogDetails(const libconfig::Setting &root)
{
    const libconfig::Setting &logg = root["logging"];
    logg.lookupValue("configFile", wsettings.logg.configFile);
}

string ConfigParser::getSensorsDetails(const char *sensorID)
{
    try
    {
        const libconfig::Setting& root = confsettings.getRoot();
        const libconfig::Setting &sen = root["sensors"][sensorID];
        return sen.lookup("name");
    }
    catch(const libconfig::SettingNotFoundException &nfex)
    {
        LOG(INFO) << "Settings for sensor not found" << endl;
    }
}
