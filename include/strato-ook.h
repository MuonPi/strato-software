#ifndef _STRATO_OOK_H_
#define _STRATO_OOK_H_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <QObject>
#include <string>

#include "globals.h"

namespace MuonPi::Ook {
class Transmitter;
}

class Ook : public QObject
{
    Q_OBJECT

public:
    Ook(Globals& globals);
    ~Ook();
    std::atomic<bool> active = false;
    std::atomic<bool> activated = false;
    std::atomic<bool> inited = false;
    std::chrono::steady_clock::time_point starttime;

public slots:
    bool execute();

private:
    Globals& StratoGlobals;
    std::unique_ptr<MuonPi::Ook::Transmitter> transmitter;

    bool init();
    bool sendPayload(uint8_t* payload, uint8_t size);
    bool sendTextPayload(const std::string& payload);
    std::string buildTextPayload() const;
};

#endif // _STRATO_OOK_H_
