
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <QCoreApplication>
#include <QTimer>

#include "strato-config.h"
#include "globals.h"
#include "strato-lorawan.h"
#include "strato-sensors.h"
#include "CayenneLPP.h"
#include "logfile.h"



Globals StratoGlobals;
Sensors StratoSensors(StratoGlobals);
Lorawan StratoLorawan(StratoGlobals);


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    const auto sensorthread_timeout = std::chrono::seconds(SENSORTHREAD_TIMEOUT);
    const auto lorawanthread_timeout = std::chrono::seconds(LORAWANTHREAD_TIMEOUT);
    

    StratoSensors.start();
    StratoLorawan.start();

    std::this_thread::sleep_for(std::chrono::seconds(1));



    QTimer timer_sensorthread;

    QObject::connect(&timer_sensorthread, &QTimer::timeout, [&]()
    {
        if (StratoSensors.running == false || std::chrono::steady_clock::now() - StratoSensors.heartbeat.load() > sensorthread_timeout)
        {
            std::cerr << "SensorThread timeout" << std::endl;
            StratoSensors.stop();
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            StratoSensors.start();
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });




    QTimer timer_lorawanthread;

    QObject::connect(&timer_lorawanthread, &QTimer::timeout, [&]()
    {
        if (StratoLorawan.running == false || std::chrono::steady_clock::now() - StratoLorawan.heartbeat.load() > lorawanthread_timeout)
        {
            std::cerr << "LorawanThread timeout" << std::endl;
            StratoSensors.stop();
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            StratoSensors.start();
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });





    timer_sensorthread.start(WATCHDOG_INTERVAL);
    timer_lorawanthread.start(WATCHDOG_INTERVAL);



    return app.exec();
}




