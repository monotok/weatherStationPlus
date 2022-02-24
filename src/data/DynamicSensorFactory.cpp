#include "../../include/data/DynamicSensorFactory.hpp"

using namespace std;

DynamicSensorFactory::DynamicSensorFactory(ConfigParser& wss): wss(wss) {}


bool DynamicSensorFactory::establish_database_connection(Settings &settings)
{
    if (!settings.db.enable)
        return false;
    string db_conn_str = "dbname = "+ settings.db.database +" user = "+ settings.db.user +" \
    password = "+ settings.db.password +" hostaddr = "+ settings.db.host
                         +" port = " + to_string(settings.db.port) + " connect_timeout=" + settings.db.timeout;
    try {
        db_conn_ptr = new pqxx::connection(db_conn_str);
    }
    catch (exception &broken_conn) {
        LOG(ERROR) << "Failed to connect to the DB: " << broken_conn.what() << endl;
        LOG(WARNING) << "\nDisabling DB connection." << endl;
        return false;
    }
    if (db_conn_ptr->is_open()) {
        LOG(INFO) << "Opened database successfully: " << db_conn_ptr->dbname() << endl;
        prepare_insert_statements();
        prepare_select_statements();
        return true;
    }
    LOG(ERROR) << "Can't open database" << endl;
    return false;
}

DynamicSensorFactory::~DynamicSensorFactory()
{
    for(weatherSensorIterator = weatherSensors_vector.begin(); weatherSensorIterator != weatherSensors_vector.end(); ++weatherSensorIterator)
    {
        delete((*weatherSensorIterator));
    }
    delete(db_conn_ptr);
    db_conn_ptr = nullptr;
}

vector<WeatherSensor *> DynamicSensorFactory::getAllWeatherSensors_ptr()
{
    lock_guard<mutex> guard(dyfMu);
    return weatherSensors_vector;
}

void DynamicSensorFactory::updateAllWeatherSensorsDatabaseValues()
{
    lock_guard<mutex> guard(dyfMu);
    for (auto &weathersensors : weatherSensors_vector) {
        LOG(DEBUG) << "Retrieving readings from DB for weather sensor " << weathersensors->get_sensorName();
        weathersensors->update_AvgReadings();
        weathersensors->update_MaxReadings();
        weathersensors->update_MinReadings();
    }
}

Sensor * DynamicSensorFactory::CreateNewSensor_obj(string sensorID)
{
    string sensorName = wss.getSensorsName(sensorID);
    if (!sensorName.empty() || strcmp(sensorID.c_str(), "tempWeatherSensor") == 0) {
        LOG(INFO) << "Creating new weather sensor. SensorID: " << sensorID << " SensorName: "
                  << sensorName << endl;
        WeatherSensor *newWeather_ptr = new WeatherSensor(sensorID, sensorName, db_conn_ptr);
        weatherSensors_vector.push_back(newWeather_ptr);
        return newWeather_ptr;
    }
    LOG(WARNING) << "No matching sensor found. Please add sensorid " << sensorID << " to the config file." << endl;
    WeatherSensor *invalidSensor = nullptr;
    return invalidSensor;
}

WeatherSensor* DynamicSensorFactory::getWeatherSensor_ptr(string sensorID)
{
    lock_guard<mutex> guard(dyfMu);
    for(weatherSensorIterator = weatherSensors_vector.begin(); weatherSensorIterator != weatherSensors_vector.end(); ++weatherSensorIterator)
    {
        if ((*weatherSensorIterator)->get_sensorID() == sensorID)
        {
            return (*weatherSensorIterator);
        }
    }
    return dynamic_cast<WeatherSensor*>(CreateNewSensor_obj(sensorID));
}

void DynamicSensorFactory::prepare_insert_statements()
{
    this->db_conn_ptr->prepare(
            "sensor_metadata",
            "insert into sensor_metadata (reading_id, type, unit) VALUES ($1, $2, $3) "
            "ON CONFLICT (reading_id) DO UPDATE "
            "SET type = excluded.type, unit = excluded.unit;");
    this->db_conn_ptr->prepare(
            "sensor_readings",
            "insert into readings (time, sensor_id, reading_id, reading) VALUES (now(), $1, $2, $3)");
}

void DynamicSensorFactory::prepare_select_statements()
{
    this->db_conn_ptr->prepare(
            "select_past_avg_reading",
            "SELECT time_bucket($1, time) as period, trunc(avg(reading::numeric), 1)\n"
            "from readings where reading_id = $2\n"
            "group by period\n"
            "order by period desc limit 1;");
    this->db_conn_ptr->prepare(
            "select_past_max_reading",
            "SELECT time_bucket($1, time) as period, trunc(max(reading::numeric), 1)\n"
            "from readings where reading_id = $2\n"
            "group by period\n"
            "order by period desc limit 1;");
    this->db_conn_ptr->prepare(
            "select_past_min_reading",
            "SELECT time_bucket($1, time) as period, trunc(min(reading::numeric), 1)\n"
            "from readings where reading_id = $2\n"
            "group by period\n"
            "order by period desc limit 1;");
}