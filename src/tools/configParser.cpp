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

string ConfigParser::getSensorsName(string sensorID)
{
    try
    {
        const libconfig::Setting& root = confsettings.getRoot();
        const libconfig::Setting &sen = root["sensors"][sensorID.c_str()];
        return sen.lookup("name");
    }
    catch(const libconfig::SettingNotFoundException &nfex)
    {
        LOG(INFO) << "Settings for sensor not found" << endl;
        return "NotSet";
    }
}

