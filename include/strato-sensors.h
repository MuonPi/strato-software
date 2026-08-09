
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
};




#endif // _STRATO_SENSORS_H_
