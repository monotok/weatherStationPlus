#include "../../include/data/WeatherSensor.hpp"


//string WeatherSensor::get_temperature()
//{
//    if(getTempUnit() == 'c')
//        return Utilities::to_string_with_precision<float>(this->temperature/100, 1)+"C";
//    return Utilities::to_string_with_precision<float>(((this->temperature/100)*9/5)+32, 1)+"F";
//}
//string WeatherSensor::get_humidity()
//{
//    return Utilities::to_string_with_precision<float>(this->humidity/100, 1)+"%";
//}

WeatherSensor::Data *WeatherSensor::getReading_ptr(string readingID)
{
    for(readingsIterator = readings_vector.begin(); readingsIterator != readings_vector.end(); ++readingsIterator)
    {
        if ((*readingsIterator)->readingId == readingID)
        {
            return (*readingsIterator);
        }
    }
    return nullptr;
}

WeatherSensor::Data *WeatherSensor::createNewSensorReading_obj(string readingID)
{
    LOG(INFO) << "Creating new sensor reading object. Reading ID: " << readingID << endl;
    Data *reading_ptr = new Data(readingID);
    addNewReadingArray(reading_ptr);
    return reading_ptr;
}

void WeatherSensor::addNewReadingArray(WeatherSensor::Data *newReading)
{
    if (readings_vector.size() == 6) {
        delete(*(readings_vector.end() -1));
        readings_vector.erase(readings_vector.end() -1);
    }
    readings_vector.push_back(newReading);
}

WeatherSensor::Data * WeatherSensor::set_reading(string readingId, float reading, ConfigParser *wss)
{
    Data *sensor = getReading_ptr(readingId);
    if (sensor == nullptr) {
        sensor = createNewSensorReading_obj(readingId);
        setLcdReadingPosition(*sensor, get_sensorID(), readingId, wss);
        if (C != nullptr) { store_weathersensormetadata_data_in_database(sensor); }
    }
    sensor->reading = reading;
    if (C != nullptr) { store_weathersensor_data_in_database(sensor); }
    return sensor;
}

vector<WeatherSensor::Data *> WeatherSensor::getAvailableReadings()
{
    return readings_vector;
}

void WeatherSensor::store_weathersensor_data_in_database(WeatherSensor::Data *reading)
{
    try {
        pqxx::work worker(*this->C);
        worker.exec_prepared("sensor_readings", get_sensorID(), reading->readingId, reading->reading);
        worker.commit();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
    }
}

void WeatherSensor::store_weathersensormetadata_data_in_database(WeatherSensor::Data *reading)
{
    try {
        pqxx::work worker(*this->C);
        worker.exec_prepared("sensor_metadata", reading->readingId, "t", "u");
        worker.commit();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
    }
}

void WeatherSensor::setLcdReadingPosition(Data& data, string sensorId, string readingId, ConfigParser* wss)
{
    wss->getSensorReadingPositions(sensorId, readingId, data.posName, data.posVal);
    data.name = wss->getSensorReadingName(sensorId, readingId);
}

string WeatherSensor::get_Reading(WeatherSensor::Data *reading)
{
    return Utilities::to_string_with_precision<float>(reading->reading, 1);
}

string WeatherSensor::get_Reading(string readingid)
{
    return Utilities::to_string_with_precision<float>(getReading_ptr(readingid)->reading, 1);
}

void WeatherSensor::update_AvgReadings()
{
    for (auto &reading : readings_vector) {
        try {
            pqxx::work worker(*this->C);
            for (auto &db_reading : reading->readings_from_db_instances) {
                auto result = worker.exec_prepared("select_past_avg_reading", db_reading->time_period,
                                                   reading->readingId);
                db_reading->average = stof(result[0][1].c_str());
                VLOG(6) << "Setting Avg for reading " << reading->name << " with time period of "
                        << db_reading->time_period << " to " << db_reading->average;
            }
        } catch (const std::exception &e)
        {
            LOG(ERROR) << e.what() << std::endl;
        }
    }
}

void WeatherSensor::update_MinReadings()
{
    for (auto &reading : readings_vector) {
        try {
            pqxx::work worker(*this->C);
            for (auto& db_reading : reading->readings_from_db_instances) {
                auto result = worker.exec_prepared("select_past_min_reading", db_reading->time_period,
                                                   reading->readingId);
                db_reading->minimum = stof(result[0][1].c_str());
                VLOG(6) << "Setting Min for reading " << reading->name << " with time period of "
                        << db_reading->time_period << " to " << db_reading->minimum;
            }
        } catch (const std::exception &e) {
            LOG(ERROR) << e.what() << std::endl;
        }
    }
}

void WeatherSensor::update_MaxReadings()
{
    for (auto &reading : readings_vector) {
        try {
            pqxx::work worker(*this->C);
            for (auto& db_reading : reading->readings_from_db_instances) {
                auto result = worker.exec_prepared("select_past_max_reading", db_reading->time_period,
                                                   reading->readingId);
                db_reading->maximum = stof(result[0][1].c_str());
                VLOG(6) << "Setting Max for reading " << reading->name << " with time period of "
                        << db_reading->time_period << " to " << db_reading->maximum;
            }
        } catch (const std::exception &e) {
            LOG(ERROR) << e.what() << std::endl;
        }
    }
}

bool WeatherSensor::checkReadingListedSettings(string sensorId, string readingId, ConfigParser *wss)
{

    return false;
}
