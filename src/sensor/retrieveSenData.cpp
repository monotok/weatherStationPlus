#include "../../include/RetrieveSenData.hpp"
#include <unistd.h>

using namespace std;

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, LcdController* lcdc, unsigned char I2C_ADDR, ConfigParser& wss)
{
    this->i2c_controller = i2c_controller;
    this->lcd_controller = lcdc;
    this->I2C_ADDR = I2C_ADDR;
    this->wss = &wss;
}

RetrieveSenData::RetrieveSenData(I2cControl *i2c_controller, LcdController* lcdc, unsigned char I2C_ADDR,
        pqxx::connection* conn, ConfigParser& wss)
{
    this->i2c_controller = i2c_controller;
    this->lcd_controller = lcdc;
    this->I2C_ADDR = I2C_ADDR;
    this->C = conn;
    prepare_insert_statements(*C);
    this->wss = &wss;
}

void RetrieveSenData::get_WeatherSenData(DynamicSensorFactory *ptr_dsf)
{
    i2c_controller->writeByte(I2C_ADDR, GET_FIRST_SENSOR_READING);

    do {
        i2c_controller->readI2c(I2C_ADDR, packet, get_temporaryStructSize());
        usleep(50000);
        tempSensor = {};
        memcpy(&tempSensor, packet, sizeof(packet));
        VLOG(8) << "Retrieved Remote Data ID: " << tempSensor.sensorID << endl;
        VLOG(8) << "Retrieved Remote Data Reading: " << Utilities::to_string_with_precision<float>(tempSensor.reading, 1) << endl;
        i2c_controller->writeByte(I2C_ADDR, GET_NEXT_SENSOR_READING);

    } while (process_ReceivedSensor(ptr_dsf));
}

bool RetrieveSenData::process_ReceivedSensor(DynamicSensorFactory *ptr_dsf)
{
    if (check_incoming_data(tempSensor)) {
        char sensorId[4] = {};
        strcpy(sensorId, tempSensor.sensorID);
        get_retrievedGroupSensorID(sensorId);
        WeatherSensor *ptr_newlyCreatedWeatherSensor = ptr_dsf->getWeatherSensor_ptr(sensorId);
        //TODO: When storing data do we want to store all changed readings for a given sensor the same timestamp?
        auto reading = ptr_newlyCreatedWeatherSensor->set_reading(tempSensor.sensorID, tempSensor.reading, C, wss);
        this->lcd_controller->createWeatherPage(ptr_newlyCreatedWeatherSensor, reading);
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

void RetrieveSenData::prepare_insert_statements(pqxx::connection &c)
{
    c.prepare(
            "sensor_metadata",
            "insert into sensor_metadata (reading_id, type, unit) VALUES ($1, $2, $3) "
            "ON CONFLICT (reading_id) DO UPDATE "
            "SET type = excluded.type, unit = excluded.unit;");
    c.prepare(
            "sensor_readings",
            "insert into readings (time, sensor_id, reading_id, reading) VALUES (now(), $1, $2, $3)");
}
