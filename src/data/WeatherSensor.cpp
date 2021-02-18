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
    Position pos = wss->getSensorReadingPosition(sensorId, readingId);
    data.posVal = pos;
    data.posName = wss->matchNamePositionToValuePosition(pos);
    data.name = wss->getSensorReadingName(sensorId, readingId);
}

string WeatherSensor::get_Reading(WeatherSensor::Data *reading)
{
    return Utilities::to_string_with_precision<float>(reading->reading, 1);
}

string WeatherSensor::get_Reading(string readingid)
{
    if (readingid.find('_') == string::npos) {
        return Utilities::to_string_with_precision<float>(getReading_ptr(readingid)->reading, 1);
    }
    vector<string> readingid_extracted;
    Utilities::split_string(readingid, readingid_extracted, '_');
    if (std::isdigit(readingid_extracted[1][0])) {
        FnPtr fp = db_func_map[readingid_extracted[0].c_str()];
        return (this->*fp)(readingid_extracted[1], "1 day");
    }
    return "N/A";
}

string WeatherSensor::get_AvgReading(string readingid, string timeperiod)
{
    try {
        pqxx::work worker(*this->C);
        auto result = worker.exec_prepared("select_past_avg_reading", timeperiod, readingid);
        return result[0][1].c_str();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
        return "N/A";
    }
}

string WeatherSensor::get_MinReading(string readingid, string timeperiod)
{
    try {
        pqxx::work worker(*this->C);
        auto result = worker.exec_prepared("select_past_min_reading", timeperiod, readingid);
        return result[0][1].c_str();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
        return "N/A";
    }
}

string WeatherSensor::get_MaxReading(string readingid, string timeperiod)
{
    try {
        pqxx::work worker(*this->C);
        auto result = worker.exec_prepared("select_past_max_reading", timeperiod, readingid);
        return result[0][1].c_str();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
        return "N/A";
    }
}
