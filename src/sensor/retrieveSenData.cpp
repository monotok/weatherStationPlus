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

void RetrieveSenData::get_LocalWeatherData(DynamicSensorFactory *ptr_dsf)
{
    i2c_controller->writeByte(I2C_ADDR, GET_LOCAL_DATA);
    usleep(50000);
    i2c_controller->readI2c(I2C_ADDR, weatherSensorUnion.packet, get_temporaryStructSize());
    VLOG(8) << "Retrieved Local Data Temp: " << weatherSensorUnion.tsd.sensorID << endl;
    VLOG(8) << "Retrieved Local Data Temp: " << to_string(weatherSensorUnion.tsd.temperature) << endl;
    VLOG(8) << "Retrieved Local Data Hum: " << to_string(weatherSensorUnion.tsd.perBatt) << endl;

    if(check_incoming_data())
    {
        WeatherSensor* temp = ptr_dsf->getWeatherSensor_ptr(get_retrievedSensorID());
        this->lcd_controller->createWeatherPage(temp);
        temp->set_temperature(weatherSensorUnion.tsd.temperature);
        temp->set_humidity(weatherSensorUnion.tsd.perBatt);
        if (C != nullptr)
            store_local_weathersensor_data_in_database(temp);
    }
}

void RetrieveSenData::get_RemoteWeatherSenData(DynamicSensorFactory *ptr_dsf)
{
    i2c_controller->writeByte(I2C_ADDR, GET_REMOTE_DATA);
    usleep(50000);
    i2c_controller->readI2c(I2C_ADDR, weatherSensorUnion.packet, get_temporaryStructSize());
    VLOG(8) << "Retrieved Remote Data Temp: " << weatherSensorUnion.tsd.sensorID << endl;
    VLOG(8) << "Retrieved Remote Data Temp: " << to_string(weatherSensorUnion.tsd.temperature) << endl;
    VLOG(8) << "Retrieved Remote Data Hum: " << to_string(weatherSensorUnion.tsd.perBatt) << endl;

    if(check_incoming_data())
    {
        WeatherSensor* temp = ptr_dsf->getWeatherSensor_ptr(get_retrievedSensorID());
        this->lcd_controller->createWeatherPage(temp);
        temp->set_temperature(weatherSensorUnion.tsd.temperature);
        temp->set_humidity(weatherSensorUnion.tsd.perBatt);
        if (C != nullptr)
            store_remote_weathersensor_data_in_database(temp);
    }
}

//TODO: Fix this because it is checking the value of I and not the sensorID
bool RetrieveSenData::check_incoming_data()
{
    for(int i = 0; i <= sizeof(weatherSensorUnion.tsd.sensorID); i++)
    {
        if(!isascii(i))
        {
            LOG(ERROR) << "Invalid Sensor ID Char detected! Not ACSII: " << i << endl;
            return false;
        }
    }
    if(weatherSensorUnion.tsd.temperature < -10000 || weatherSensorUnion.tsd.temperature > 15000)
    {
        LOG(ERROR) << "Out of range Temperature detected. " << weatherSensorUnion.tsd.temperature << endl;
        return false;
    }
    if(weatherSensorUnion.tsd.perBatt < -10000 || weatherSensorUnion.tsd.perBatt > 15000)
    {
        LOG(ERROR) << "Out of range PerBatt detected. " << weatherSensorUnion.tsd.perBatt << endl;
        return false;
    }

    return true;
}

void RetrieveSenData::store_local_weathersensor_data_in_database(WeatherSensor *ws)
{
    try {
            pqxx::work worker(*C);
            worker.exec(
                    "INSERT INTO sensors.data "
                    "(sensorid, time, temperature, humidity, battery) "
                    "VALUES "
                    "('" + worker.esc(ws->get_sensorID()) + "' , current_timestamp, " +
                    worker.esc(to_string(ws->get_temperature_float() * 100)) + ", "
                    + worker.esc(to_string(ws->get_humidity_float() * 100)) + ", null)"
            );
            worker.commit();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
    }
}

void RetrieveSenData::store_remote_weathersensor_data_in_database(WeatherSensor *ws)
{
    try {
        pqxx::work worker(*C);
        worker.exec(
                "INSERT INTO sensors.data "
                "(sensorid, time, temperature, humidity, battery) "
                "VALUES "
                "('" + worker.esc(ws->get_sensorID()) + "' , current_timestamp, " +
                worker.esc(to_string(ws->get_temperature_float() * 100)) + ", null, "
                + worker.esc(to_string(ws->get_humidity_float() * 100)) + ")"
        );
        worker.commit();
    } catch (const std::exception &e)
    {
        LOG(ERROR) << e.what() << std::endl;
    }
}