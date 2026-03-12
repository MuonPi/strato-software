
#ifndef _STRATO_SENSORS_H_
#define _STRATO_SENSORS_H_


#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <functional>

#include "globals.h"
#include "muonpi.h"

class Sensors
{
public:
    Sensors(Globals& globals);
    ~Sensors();

    bool start();
    bool stop();

    std::atomic<bool> running {false};
    std::atomic<std::chrono::steady_clock::time_point> heartbeat {std::chrono::steady_clock::now()};

private:
    Globals& StratoGlobals;
    void threadFunc();
    std::unique_ptr<MUONPI> strato_muonpi{nullptr};
    std::thread sensorThread;
};




#endif // _STRATO_SENSORS_H_