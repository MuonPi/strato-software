
#ifndef _STRATO_SENSORS_H_
#define _STRATO_SENSORS_H_



#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <functional>


class Sensors
{
public:
    Sensors();
    ~Sensors();

    bool start();
    bool stop();

    std::atomic<double> voltage {0.0};
    std::atomic<double> voltage_mean {0.0};
    std::atomic<uint16_t> voltage_count {0};

    std::atomic<double> magnet {0};
    std::atomic<double> magnet_mean {0};
    std::atomic<double> magnetXYZ[3] {0};
    std::atomic<uint16_t> magnet_count {0};

    std::atomic<double> temperature {0};
    std::atomic<double> temperature_mean {0};
    std::atomic<uint16_t> temperature_count {0};

private:
    void threadFunc();

    std::thread sensorThread;
    std::atomic<bool> running;
    // std::mutex mtx;
};




#endif // _STRATO_SENSORS_H_