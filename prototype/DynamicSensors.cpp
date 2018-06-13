#include "../include/GPIOControl.hpp"
#include "../include/BtnControl.hpp"
#include "../include/data/TempSensor.hpp"
#include <unistd.h>
#include <signal.h>
#include <time.h>

INITIALIZE_EASYLOGGINGPP

using namespace std;

void sig_handler(int sig);
bool ctrl_c_pressed = false;

void sig_handler(int sig)
{
    write(0, "\nCtrl^C pressed in sig handler\n", 32);
    ctrl_c_pressed = true;
}

int main(int argc, char **argv)
{

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    if (sigaction(SIGINT, &sig_struct, NULL) == -1)
    {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }

    START_EASYLOGGINGPP(argc, argv);
    el::Configurations conf("../conf/logging.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    LOG(INFO) << "Testing........";

    GPIOControl gpio17 = GPIOControl("17");
    GPIOControl::Value GPIO_ON = GPIOControl::Value::GPIO_ON;
    GPIOControl::Value GPIO_OFF = GPIOControl::Value::GPIO_OFF;

    //Set the direction
    gpio17.g_setdir("in");

    BtnState bs;

    //Create a vector list of class objects
    vector<Sensor *> tempVector;
    vector<Sensor *>::iterator i;
    int b = 0;

    while (b < 10)
    {
        bs.initBtnState(&gpio17);

        if (true)
        {
            LOG(DEBUG) << "Button Pressed";
            tempVector.push_back(new TemperatureSensor());
            string sensorID = "SensorID " + to_string(b);
            tempVector[b]->set_sensorID(sensorID);
            LOG(INFO) << "Added an object of temp sensor";
            b++;
        }

        bs.reInitBtnState();

        if (ctrl_c_pressed)
        {
            cout << "Ctrl^C Pressed" << endl;
            for (i = tempVector.begin(); i != tempVector.end(); ++i)
            {
                string sensorID = (*i)->get_sensorID();
                cout << sensorID << endl;
            }
            break;
        }

        usleep(50000);
    }

    for (i = tempVector.begin(); i != tempVector.end(); ++i)
    {
        delete *i;
    }

    map<Sensor *, int> tempMap;
    map<Sensor *, int>::iterator it;

    TemperatureSensor *testSen = new TemperatureSensor();
    TemperatureSensor *testSen2 = new TemperatureSensor();

    tempMap[testSen] = 4;
    tempMap[testSen2] = 6;

    it = tempMap.find(testSen2);
    it->first->set_sensorID("SensorID testSen2");
    it = tempMap.find(testSen);
    it->first->set_sensorID("SensorID testSen1");

    for (it = tempMap.begin(); it != tempMap.end(); it++)
    {
        cout << "Found the object " << it->first->get_sensorID() << endl;
        delete (it->first);
    }

    return 0;
}
