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

WeatherSensor::Data* WeatherSensor::set_reading(string readingId, string type, float reading, string unit, pqxx::connection* C, ConfigParser* wss)
{
    Data *sensor = getReading_ptr(readingId);
    if (sensor == nullptr) {
        sensor = createNewSensorReading_obj(readingId);
        sensor->type = type;
        sensor->unit = unit;
        setLcdReadingPosition(*sensor, get_sensorID(), readingId, wss);
        if (C != nullptr) { store_weathersensormetadata_data_in_database(sensor, *C); }
    }
    sensor->reading = reading;
    if (C != nullptr) { store_weathersensor_data_in_database(sensor, *C); }
    return sensor;
}

vector<WeatherSensor::Data *> WeatherSensor::getAvailableReadings()
{
    return readings_vector;
}

void WeatherSensor::store_weathersensor_data_in_database(WeatherSensor::Data *reading, pqxx::connection &C)
{
    try {
        pqxx::work worker(C);
        worker.exec_prepared("sensor_readings", get_sensorID(), reading->readingId, reading->reading);
        worker.commit();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
    }
}

void WeatherSensor::store_weathersensormetadata_data_in_database(WeatherSensor::Data *reading, pqxx::connection &C)
{
    try {
        pqxx::work worker(C);
        worker.exec_prepared("sensor_metadata", reading->readingId, reading->type, reading->unit);
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

string WeatherSensor::get_Reading(string reading)
{
    return Utilities::to_string_with_precision<float>(getReading_ptr(reading)->reading, 1);
}
