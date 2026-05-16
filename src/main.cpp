
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
#include "strato-sensors.h"
#include "CayenneLPP.h"
#include "logfile.h"




Globals StratoGlobals;
// Sensors StratoSensors(StratoGlobals);
// Lorawan StratoLorawan(StratoGlobals);




int main(int argc, char** argv)
{
    QCoreApplication StratoApp(argc, argv);


    // Lorawan

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

    QObject::connect(&StratoApp, &QCoreApplication::aboutToQuit, [&]()
    {
        StratoLorawanThread->quit();
        StratoLorawanThread->wait();
    });




    // Watchdog

    QTimer* Watchdog = new QTimer();

    QObject::connect(Watchdog, &QTimer::timeout, [&]()
    {
        if(!StratoLorawan->active)
            return;

        auto now = std::chrono::steady_clock::now();
        if(now - StratoLorawan->starttime > std::chrono::seconds(LORAWAN_TIMEOUT))
        {
            std::cerr << "StratoLorawanThread timeout" << std::endl;
            StratoLorawan->activated = false;
            StratoLorawan->inited = false;

            QTimer::singleShot(LORAWAN_RESTART * 1000, [&]()
            {
                if(!StratoLorawan->active)
                    return;
                std::exit(1);
            });
        }
    });


    StratoLorawanThread->start();
    Watchdog->start(WATCHDOG_INTERVAL);

    return StratoApp.exec();

}








    // const auto sensorthread_timeout = std::chrono::seconds(SENSORTHREAD_TIMEOUT);
    // const auto lorawanthread_timeout = std::chrono::seconds(LORAWANTHREAD_TIMEOUT);
    

    // StratoSensors.start();
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // StratoLorawan.start();

    // std::this_thread::sleep_for(std::chrono::seconds(1));



    // QTimer timer_sensorthread;
    // bool timer_sensorthread_enabled = true;

    // QObject::connect(&timer_sensorthread, &QTimer::timeout, [&]()
    // {
    //     if (timer_sensorthread_enabled)
    //     {
    //         if (StratoSensors.running == false || std::chrono::steady_clock::now() - StratoSensors.heartbeat.load() > sensorthread_timeout)
    //         {
    //             std::cerr << "SensorThread timeout" << std::endl;
    //             StratoSensors.stop();
    //             QTimer::singleShot(WATCHDOG_INTERVAL * 2, [&]()
    //             {
    //                 StratoSensors.start();
    //                 timer_sensorthread_enabled = true;
    //             });
    //             timer_sensorthread_enabled = false;
    //         }
    //     }
    // });







//     QObject::connect(&StratoLorawanTimer, &QTimer::timeout, [&](){StratoLorawan.execute();});





//     // bool timer_lorawanthread_enabled = true;

//     // QObject::connect(&StratoLorawanTimer, &QTimer::timeout, [&]()
//     // {
//     //     if (timer_lorawanthread_enabled)
//     //     {
//     //         std::cout << lorawanthread_timeout - (std::chrono::steady_clock::now() - StratoLorawan.heartbeat.load()) << std::endl;
//     //         if (StratoLorawan.running == false || std::chrono::steady_clock::now() - StratoLorawan.heartbeat.load() > lorawanthread_timeout)
//     //         {
//     //             std::cerr << "LorawanThread timeout" << std::endl;
//     //             StratoLorawan.stop();
//     //             QTimer::singleShot(WATCHDOG_INTERVAL * 2, Qt::PreciseTimer, [&]()
//     //             {
//     //                 std::cout << "singleShot fired" << std::endl;
//     //                 StratoLorawan.start();
//     //                 timer_lorawanthread_enabled = true;
//     //             });
//     //             timer_lorawanthread_enabled = false;
//     //         }
//     //     }
//     // });



//     // QThread* StratoLorawanThread = new QThread;
//     // Lorawan* StratoLorawan = new Lorawan(StratoGlobals);

//     // StratoLorawan->moveToThread(StratoLorawanThread);

//     // QObject::connect(StratoLorawanThread, &QThread::started, StratoLorawan, &Lorawan::threadFunc);
//     // QObject::connect(StratoLorawan, &Lorawan::finished, StratoLorawanThread, &QThread::quit);
//     // QObject::connect(StratoLorawan, &Lorawan::finished, StratoLorawan, &QObject::deleteLater);
//     // QObject::connect(StratoLorawanThread, &QThread::finished, StratoLorawanThread, &QObject::deleteLater);

//     // StratoLorawanThread->start();

//     // timer_sensorthread.start(WATCHDOG_INTERVAL);
//     // timer_lorawanthread.start(WATCHDOG_INTERVAL);

//     StratoLorawanTimer.start(LORAWAN_INTERVAL * 1000);









