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

void ConfigParser::getDatabaseDetails(const libconfig::Setting& root)
{
    const libconfig::Setting &db = root["database"];
    db.lookupValue("host", wsettings.db.host);
}