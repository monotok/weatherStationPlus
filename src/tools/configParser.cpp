#include "../../include/configParser.hpp"

ConfigParser::ConfigParser(settings* wsettings, const char* settingsFileLocation)
{
    this->wsettings = wsettings;
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

void ConfigParser::getVersion()
{
    wsettings->version = confsettings.lookup("version");
}
