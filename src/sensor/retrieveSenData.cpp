#include "../../include/RetrieveSenData.hpp"
#include <unistd.h>

using namespace std;

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, LcdController* lcdc, unsigned char I2C_ADDR)
{
    this->i2c_controller = i2c_controller;
    this->lcd_controller = lcdc;
    this->I2C_ADDR = I2C_ADDR;
}

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, LcdController* lcdc, unsigned char I2C_ADDR,
        pqxx::connection* conn)
{
    this->i2c_controller = i2c_controller;
    this->lcd_controller = lcdc;
    this->I2C_ADDR = I2C_ADDR;
    this->C = conn;
}

void RetrieveSenData::get_WeatherSenData(DynamicSensorFactory *ptr_dsf)
{
    i2c_controller->writeByte(I2C_ADDR, GET_FIRST_SENSOR_READING);

    do {
        i2c_controller->readI2c(I2C_ADDR, packet, get_temporaryStructSize());
        usleep(50000);
        VLOG(8) << "Retrieved Remote Data ID: " << tempSensor.sensorID << endl;
        VLOG(8) << "Retrieved Remote Data Reading: " << Utilities::to_string_with_precision<float>(tempSensor.reading, 1) << endl;
        VLOG(8) << "Retrieved Remote Data Sensor Type: " << tempSensor.sensorType << endl;

        memcpy(&tempSensor, packet, sizeof(packet));
        i2c_controller->writeByte(I2C_ADDR, GET_NEXT_SENSOR_READING);

    } while (process_ReceivedSensor(ptr_dsf));
}

bool RetrieveSenData::process_ReceivedSensor(DynamicSensorFactory *ptr_dsf)
{
    if (check_incoming_data(tempSensor)) {
        WeatherSensor *ptr_newlyCreatedWeatherSensor = ptr_dsf->getWeatherSensor_ptr(get_retrievedGroupSensorID(tempSensor));
        this->lcd_controller->createWeatherPage(ptr_newlyCreatedWeatherSensor);
        ptr_newlyCreatedWeatherSensor->set_reading(tempSensor.sensorID, tempSensor.sensorType, tempSensor.reading, tempSensor.unit);
//        if (C != nullptr)
//            store_weathersensor_data_in_database(ptr_newlyCreatedWeatherSensor);

        return true;
    }
    return false;
}

//TODO: Fix this because it is checking the value of I and not the sensorID
//TODO: Need to rewrite this func as assuming the reading is temperature
bool RetrieveSenData::check_incoming_data(SensorData& sensor)
{
//    for(int i = 0; i <= sizeof(sensor.sensorID); i++)
//    {
//        if(!isascii(i))
//        {
//            LOG(ERROR) << "Invalid Sensor ID Char detected! Not ACSII: " << i << endl;
//            return false;
//        }
//    }
//    if(sensor.reading < -10000 || sensor.reading > 15000)
//    {
//        LOG(ERROR) << "Out of range Temperature detected. " << sensor.reading << endl;
//        return false;
//    }
    if (strcmp(tempSensor.sensorID, "\0") == 0) {
        return false;
    }
    return true;
}

void RetrieveSenData::store_weathersensor_data_in_database(WeatherSensor *ws)
{
    try {
        pqxx::work worker(*C);
        for (auto &reading: ws->getAvailableReadings()) {
            worker.exec_prepared("sensor_readings", ws->get_sensorID(), reading->readingId, reading->reading);
        }
        worker.commit();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
    }
}

void RetrieveSenData::prepare_insert_statements(pqxx::connection &c)
{
    c.prepare(
            "sensor_metadata",
            "insert into sensor_metadata (reading_id, type, unit) VALUES ($1, $2, $3)");
    c.prepare(
            "sensor_readings",
            "insert into readings (time, sensor_id, reading_id, reading) VALUES (now(), $1, $2, $3)");
}

void RetrieveSenData::store_weathersensormetadata_data_in_database(WeatherSensor *ws)
{
    try {
        pqxx::work worker(*C);
        for (auto &reading: ws->getAvailableReadings()) {
            worker.exec_prepared("sensor_metadata", reading->readingId, reading->type, reading->unit);
        }
        worker.commit();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
    }
}
