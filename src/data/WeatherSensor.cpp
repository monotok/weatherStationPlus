#include "../../include/data/WeatherSensor.hpp"


string WeatherSensor::get_temperature()
{
    if(getTempUnit() == 'c')
        return Utilities::to_string_with_precision<float>(this->temperature/100, 1)+"C";
    return Utilities::to_string_with_precision<float>(((this->temperature/100)*9/5)+32, 1)+"F";
}
string WeatherSensor::get_humidity()
{
    return Utilities::to_string_with_precision<float>(this->humidity/100, 1)+"%";
}

void WeatherSensor::set_tempUnit_to_C()
{
    this->tempUnit = 'c';
}

void WeatherSensor::set_tempUnit_to_F()
{
    this->tempUnit = 'f';
}

void WeatherSensor::switch_tempUnit()
{
    if(this->tempUnit == 'f')
        this->tempUnit = 'c';

    else if(this->tempUnit == 'c')
        this->tempUnit = 'f';
}

char WeatherSensor::getTempUnit()
{
    return this->tempUnit;
}

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

void WeatherSensor::set_reading(string readingId, string type, float reading, string unit, pqxx::connection &C)
{
    Data *sensor = getReading_ptr(readingId);
    if (sensor == nullptr) {
        sensor = createNewSensorReading_obj(readingId);
        sensor->type = type;
        sensor->unit = unit;
        store_weathersensormetadata_data_in_database(sensor, C);
    }
    sensor->reading = reading;
    store_weathersensor_data_in_database(sensor, C);
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