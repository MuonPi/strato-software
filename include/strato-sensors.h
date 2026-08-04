
#ifndef _STRATO_SENSORS_H_
#define _STRATO_SENSORS_H_


#include <chrono>
#include <QObject>

#include "globals.h"


class Sensors : public QObject
{
    Q_OBJECT

public:
    Sensors(Globals& globals);
    ~Sensors();
    std::atomic<bool> active = false;
    std::atomic<bool> activated = false;
    std::atomic<bool> inited = false;
    std::chrono::steady_clock::time_point starttime;

public slots:
    bool execute();

private:
    Globals& StratoGlobals;

    bool ads1115_inited = false;
    bool qmc5883_inited = false;
    bool veml6075_inited = false;
    bool ltr390uv01_inited = false;
    bool as7331_inited = false;
    bool as7343_inited = false;
    bool bme280_inited = false;
    bool sht31_inited = false;
    bool mpu6050_inited = false;
    bool sen0321_inited = false;
    bool ozone3click_inited = false;
};




#endif // _STRATO_SENSORS_H_
