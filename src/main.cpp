
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
    std::this_thread::sleep_for(std::chrono::seconds(1));
    StratoLorawan.start();

    std::this_thread::sleep_for(std::chrono::seconds(1));



    QTimer timer_sensorthread;
    bool timer_sensorthread_enabled = true;

    QObject::connect(&timer_sensorthread, &QTimer::timeout, [&]()
    {
        if (timer_sensorthread_enabled)
        {
            if (StratoSensors.running == false || std::chrono::steady_clock::now() - StratoSensors.heartbeat.load() > sensorthread_timeout)
            {
                std::cerr << "SensorThread timeout" << std::endl;
                StratoSensors.stop();
                QTimer::singleShot(WATCHDOG_INTERVAL * 10, [&]()
                {
                    StratoSensors.start();
                    timer_sensorthread_enabled = true;
                });
                timer_sensorthread_enabled = false;
            }
        }
    });




    QTimer timer_lorawanthread;
    bool timer_lorawanthread_enabled = true;

    QObject::connect(&timer_lorawanthread, &QTimer::timeout, [&]()
    {
        if (timer_lorawanthread_enabled)
        {
            if (StratoLorawan.running == false || std::chrono::steady_clock::now() - StratoLorawan.heartbeat.load() > lorawanthread_timeout)
            {
                std::cerr << "LorawanThread timeout" << std::endl;
                StratoLorawan.stop();
                QTimer::singleShot(WATCHDOG_INTERVAL, [&]()
                {
                    StratoLorawan.start();
                    timer_lorawanthread_enabled = true;
                });
                timer_lorawanthread_enabled = false;
            }
        }
    });





    timer_sensorthread.start(WATCHDOG_INTERVAL);
    timer_lorawanthread.start(WATCHDOG_INTERVAL);



    return app.exec();

}

