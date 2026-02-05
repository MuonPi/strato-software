
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

    std::atomic<bool> running {false};
    std::atomic<std::chrono::steady_clock::time_point> heartbeat {std::chrono::steady_clock::now()};

    std::atomic<double> XOR {0};
    std::atomic<double> XOR_mean {0};
    std::atomic<uint16_t> XOR_count {0};

    std::atomic<double> AND {0};
    std::atomic<double> AND_mean {0};
    std::atomic<uint16_t> AND_count {0};

    std::atomic<double> coordinates[3] {0};

    std::atomic<double> voltage {0};
    std::atomic<double> voltage_mean {0};
    std::atomic<uint16_t> voltage_count {0};

    std::atomic<double> magnet {0};
    std::atomic<double> magnet_mean {0};
    std::atomic<double> magnetXYZ[3] {0};
    std::atomic<uint16_t> magnet_count {0};

    // std::atomic<double> temperature {0};
    // std::atomic<double> temperature_mean {0};
    // std::atomic<uint16_t> temperature_count {0};

    std::atomic<double> uv[2] {0};
    std::atomic<double> uv_mean {0};
    std::atomic<uint16_t> uv_count {0};

    std::atomic<double> temperature {0};
    std::atomic<double> temperature_mean {0};
    std::atomic<uint16_t> temperature_count {0};

    std::atomic<double> pressure {0};
    std::atomic<double> pressure_mean {0};
    std::atomic<uint16_t> pressure_count {0};

    std::atomic<double> humidity {0};
    std::atomic<double> humidity_mean {0};
    std::atomic<uint16_t> humidity_count {0};

    std::atomic<double> ozone {0};
    std::atomic<double> ozone_mean {0};
    std::atomic<uint16_t> ozone_count {0};

private:
    void threadFunc();
    std::thread sensorThread;
};




#endif // _STRATO_SENSORS_H_