
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <QCoreApplication>
#include <QTimer>

#include <QThread>
#include <QObject>

#include "strato-config.h"
#include "globals.h"
#include "strato-lorawan.h"
#include "strato-ook.h"
#include "strato-sensors.h"




Globals StratoGlobals;




int main(int argc, char** argv)
{

    QCoreApplication StratoApp(argc, argv);





    // ========================== Sensors ==========================

    QThread* StratoSensorsThread = new QThread;
    Sensors* StratoSensors = new Sensors(StratoGlobals);
    QTimer* StratoSensorsTimer = new QTimer(StratoSensors);

    StratoSensors->moveToThread(StratoSensorsThread);
    StratoSensorsTimer->moveToThread(StratoSensorsThread);
    StratoSensorsTimer->setTimerType(Qt::PreciseTimer);
    StratoSensorsTimer->setInterval(SENSORS_INTERVAL * 1000);

    StratoSensors->activated = true;
    StratoSensors->inited = false;

    QObject::connect(StratoSensorsTimer, &QTimer::timeout, StratoSensors, &Sensors::execute);

    QObject::connect(StratoSensorsThread, &QThread::started, [&]()
        {
            StratoSensorsTimer->start();
            QTimer::singleShot(0, StratoSensors, &Sensors::execute);
        });

    QObject::connect(StratoSensorsThread, &QThread::finished, StratoSensorsTimer, &QObject::deleteLater);
    QObject::connect(StratoSensorsThread, &QThread::finished, StratoSensors, &QObject::deleteLater);
    QObject::connect(StratoSensorsThread, &QThread::finished, StratoSensorsThread, &QObject::deleteLater);





    // ========================== Lorawan ==========================

    QThread* StratoLorawanThread = new QThread;
    Lorawan* StratoLorawan = new Lorawan(StratoGlobals);
    QTimer* StratoLorawanTimer = new QTimer(StratoLorawan);

    StratoLorawan->moveToThread(StratoLorawanThread);
    StratoLorawanTimer->moveToThread(StratoLorawanThread);
    StratoLorawanTimer->setTimerType(Qt::PreciseTimer);
    StratoLorawanTimer->setInterval(LORAWAN_INTERVAL * 1000);

    StratoLorawan->activated = true;
    StratoLorawan->inited = false;

    QObject::connect(StratoLorawanTimer, &QTimer::timeout, StratoLorawan, &Lorawan::execute);

    QObject::connect(StratoLorawanThread, &QThread::started, [&]()
        {
            StratoLorawanTimer->start();
            QTimer::singleShot(0, StratoLorawan, &Lorawan::execute);
        });

    QObject::connect(StratoLorawanThread, &QThread::finished, StratoLorawanTimer, &QObject::deleteLater);
    QObject::connect(StratoLorawanThread, &QThread::finished, StratoLorawan, &QObject::deleteLater);
    QObject::connect(StratoLorawanThread, &QThread::finished, StratoLorawanThread, &QObject::deleteLater);




    
    // ========================== OOK ==========================

    #ifdef OOK_USED
    QThread* StratoOokThread = new QThread;
    Ook* StratoOok = new Ook(StratoGlobals);
    QTimer* StratoOokTimer = new QTimer(StratoOok);

    StratoOok->moveToThread(StratoOokThread);
    StratoOokTimer->moveToThread(StratoOokThread);
    StratoOokTimer->setTimerType(Qt::PreciseTimer);
    StratoOokTimer->setInterval(OOK_INTERVAL * 1000);

    StratoOok->activated = false;
    StratoOok->inited = false;

    QObject::connect(StratoOokTimer, &QTimer::timeout, StratoOok, &Ook::execute);

    QObject::connect(StratoOokThread, &QThread::started, [&]()
        {
            StratoOokTimer->start();
            QTimer::singleShot(0, StratoOok, &Ook::execute);
        });

    QObject::connect(StratoOokThread, &QThread::finished, StratoOokTimer, &QObject::deleteLater);
    QObject::connect(StratoOokThread, &QThread::finished, StratoOok, &QObject::deleteLater);
    QObject::connect(StratoOokThread, &QThread::finished, StratoOokThread, &QObject::deleteLater);
    #endif





    // ========================== Watchdog Sensors ==========================

    QTimer* StratoSensorsWatchdog = new QTimer();

    bool sensors_cycle_missed = false;

    QObject::connect(StratoSensorsWatchdog, &QTimer::timeout, [&]()
    {
        if(!StratoSensors->active)
            return;

        auto now = std::chrono::steady_clock::now();

        if(now - StratoSensors->starttime > std::chrono::seconds(SENSORS_INTERVAL))
        {
            if(sensors_cycle_missed == false)
                std::cerr << "StratoSensorsThread cycle missed" << std::endl;
            sensors_cycle_missed = true;
            StratoSensors->inited = false;
        }
        else
            sensors_cycle_missed = false;

        if(now - StratoSensors->starttime > std::chrono::seconds(SENSORS_TIMEOUT))
        {
            if(StratoSensors->activated)
                std::cerr << "StratoSensorsThread timeout" << std::endl;
            StratoSensors->activated = false;
            StratoSensors->inited = false;

            QTimer::singleShot(SENSORS_RESTART * 1000, [&]()
            {
                if(!StratoSensors->active)
                {
                    std::cerr << "StratoSensorsThread recovered" << std::endl;
                    StratoSensors->activated = true;
                    QTimer::singleShot(0, StratoSensors, &Sensors::execute);
                    return;
                }
                else
                {
                    std::cerr << "StratoSensorsThread not responding\nKilling ..." << std::endl;
                    std::exit(1);
                }
            });
        }
    });





    // ========================== Watchdog Lorawan ==========================

    QTimer* StratoLorawanWatchdog = new QTimer();

    bool lorawan_cycle_missed = false;

    QObject::connect(StratoLorawanWatchdog, &QTimer::timeout, [&]()
    {
        if(!StratoLorawan->active)
            return;

        auto now = std::chrono::steady_clock::now();

        if(now - StratoLorawan->starttime > std::chrono::seconds(LORAWAN_INTERVAL))
        {
            if(lorawan_cycle_missed == false)
                std::cerr << "StratoLorawanThread cycle missed" << std::endl;
            lorawan_cycle_missed = true;
            StratoLorawan->inited = false;
        }
        else
            lorawan_cycle_missed = false;

        if(now - StratoLorawan->starttime > std::chrono::seconds(LORAWAN_TIMEOUT))
        {
            if(StratoLorawan->activated)
                std::cerr << "StratoLorawanThread timeout" << std::endl;
            StratoLorawan->activated = false;
            StratoLorawan->inited = false;

            QTimer::singleShot(LORAWAN_RESTART * 1000, [&]()
            {
                if(!StratoLorawan->active)
                {
                    std::cerr << "StratoLorawanThread recovered" << std::endl;
                    StratoLorawan->activated = true;
                    QTimer::singleShot(0, StratoLorawan, &Lorawan::execute);
                    return;
                }
                else
                {
                    std::cerr << "StratoLorawanThread not responding\nKilling ..." << std::endl;
                    std::exit(1);
                }
            });
        }
    });





    // ========================== Watchdog OOK ==========================

    #ifdef OOK_USED
    QTimer* StratoOokWatchdog = new QTimer();

    bool ook_cycle_missed = false;

    QObject::connect(StratoOokWatchdog, &QTimer::timeout, [&]()
    {
        if(!StratoOok->active)
            return;

        auto now = std::chrono::steady_clock::now();

        if(now - StratoOok->starttime > std::chrono::seconds(OOK_INTERVAL))
        {
            if(ook_cycle_missed == false)
                std::cerr << "StratoOokThread cycle missed" << std::endl;
            ook_cycle_missed = true;
            StratoOok->inited = false;
        }
        else
            ook_cycle_missed = false;

        if(now - StratoOok->starttime > std::chrono::seconds(OOK_TIMEOUT))
        {
            if(StratoOok->activated)
                std::cerr << "StratoOokThread timeout" << std::endl;
            StratoOok->activated = false;
            StratoOok->inited = false;

            QTimer::singleShot(OOK_RESTART * 1000, [&]()
            {
                if(!StratoOok->active)
                {
                    std::cerr << "StratoOokThread recovered" << std::endl;
                    StratoOok->activated = true;
                    return;
                }
                else
                {
                    std::cerr << "StratoOokThread not responding\nKilling ..." << std::endl;
                    std::exit(1);
                }
            });
        }
    });
    #endif





    QObject::connect(&StratoApp, &QCoreApplication::aboutToQuit, [&]()
    {
        StratoSensorsThread->quit();
        StratoSensorsThread->wait();
        #ifdef OOK_USED
        StratoOokThread->quit();
        StratoOokThread->wait();
        #endif
        StratoLorawanThread->quit();
        StratoLorawanThread->wait();
    });



    StratoSensorsThread->start();
    #ifdef OOK_USED
    StratoOokThread->start();
    #endif
    StratoLorawanThread->start();
    StratoSensorsWatchdog->start(WATCHDOG_INTERVAL);
    #ifdef OOK_USED
    StratoOokWatchdog->start(WATCHDOG_INTERVAL);
    #endif
    StratoLorawanWatchdog->start(WATCHDOG_INTERVAL);

    return StratoApp.exec();

}
