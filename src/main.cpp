
// #include <iostream>
// #include <string>
// #include <thread>
// #include <chrono>
// #include <iomanip>
// #include <QCoreApplication>
// #include <QTimer>

// #include <QThread>
// #include <QObject>

// #include "strato-config.h"
// #include "globals.h"
// #include "strato-lorawan.h"
// #include "strato-sensors.h"
// #include "CayenneLPP.h"
// #include "logfile.h"



// Globals StratoGlobals;
// Sensors StratoSensors(StratoGlobals);
// Lorawan StratoLorawan(StratoGlobals);




// int main(int argc, char** argv)
// {
//     QCoreApplication StratoApp(argc, argv);

//     // const auto sensorthread_timeout = std::chrono::seconds(SENSORTHREAD_TIMEOUT);
//     // const auto lorawanthread_timeout = std::chrono::seconds(LORAWANTHREAD_TIMEOUT);
    

//     // StratoSensors.start();
//     // std::this_thread::sleep_for(std::chrono::seconds(1));
//     // StratoLorawan.start();

//     // std::this_thread::sleep_for(std::chrono::seconds(1));



//     // QTimer timer_sensorthread;
//     // bool timer_sensorthread_enabled = true;

//     // QObject::connect(&timer_sensorthread, &QTimer::timeout, [&]()
//     // {
//     //     if (timer_sensorthread_enabled)
//     //     {
//     //         if (StratoSensors.running == false || std::chrono::steady_clock::now() - StratoSensors.heartbeat.load() > sensorthread_timeout)
//     //         {
//     //             std::cerr << "SensorThread timeout" << std::endl;
//     //             StratoSensors.stop();
//     //             QTimer::singleShot(WATCHDOG_INTERVAL * 2, [&]()
//     //             {
//     //                 StratoSensors.start();
//     //                 timer_sensorthread_enabled = true;
//     //             });
//     //             timer_sensorthread_enabled = false;
//     //         }
//     //     }
//     // });




//     StratoLorawan.running = true;

//     QTimer StratoLorawanTimer;

//     QTimer::singleShot(0, [&](){StratoLorawan.execute();});

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



//     return StratoApp.exec();

// }



















#include <QCoreApplication>
#include <QThread>
#include <QTimer>

#include "strato-config.h"
#include "globals.h"
#include "strato-lorawan.h"
#include "strato-sensors.h"
#include "CayenneLPP.h"
#include "logfile.h"

Globals StratoGlobals;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QThread* StratoLorawanThread = new QThread;
    // QThread* StratoSensorsThread = new QThread;

    // =========================
    // WORKERS
    // =========================
    Lorawan* StratoLorawan = new Lorawan(StratoGlobals);
    // Sensors* StratoSensors = new Sensors(StratoGlobals);

    StratoLorawan->moveToThread(StratoLorawanThread);
    // StratoSensors->moveToThread(StratoSensorsThread);



    QTimer* StratoLorawanTimer = new QTimer();
    StratoLorawanTimer->setInterval(LORAWAN_INTERVAL * 1000);
    StratoLorawanTimer->moveToThread(StratoLorawanThread);

    QObject::connect(StratoLorawanTimer, &QTimer::timeout,
                     StratoLorawan, &Lorawan::execute,
                     Qt::QueuedConnection);




    // QTimer* StratoSensorsTimer = new QTimer();
    // StratoSensorsTimer->setInterval(1000);
    // StratoSensorsTimer->moveToThread(StratoSensorsThread);

    // QObject::connect(StratoSensorsTimer, &QTimer::timeout,
    //                  StratoSensors, &Sensors::tick,
    //                  Qt::QueuedConnection);





    QObject::connect(StratoLorawanThread, &QThread::started,
                     StratoLorawanTimer, qOverload<>(&QTimer::start));

    // QObject::connect(StratoSensorsThread, &QThread::started,
    //                  StratoSensorsTimer, qOverload<>(&QTimer::start));



    QObject::connect(StratoLorawanThread, &QThread::finished,
                     StratoLorawanTimer, &QObject::deleteLater);

    // QObject::connect(StratoSensorsThread, &QThread::finished,
    //                  StratoSensorsTimer, &QObject::deleteLater);

    QObject::connect(StratoLorawanThread, &QThread::finished,
                     StratoLorawan, &QObject::deleteLater);

    // QObject::connect(StratoSensorsThread, &QThread::finished,
    //                  StratoSensors, &QObject::deleteLater);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&]()
    {
        StratoLorawanThread->quit();
        // StratoSensorsThread->quit();

        StratoLorawanThread->wait();
        // StratoSensorsThread->wait();
    });

    
    
    StratoLorawanThread->start();
    // StratoSensorsThread->start();

    return app.exec();
}