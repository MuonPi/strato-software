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
    enum class ActivationReason
    {
        None,
        PostDescentDelay,
        GpsFailsafe
    };

    Globals& StratoGlobals;
    std::unique_ptr<MuonPi::Ook::Transmitter> transmitter;
    std::chrono::steady_clock::time_point activation_starttime;
    std::chrono::steady_clock::time_point last_gps_time;
    std::chrono::steady_clock::time_point descent_below_threshold_starttime;
    uint64_t seen_position_update_count = 0;
    double base_altitude = 0.0;
    bool gps_seen = false;
    bool flight_armed = false;
    bool descent_below_threshold_active = false;
    bool persistence_load_warning_logged = false;
    bool persistence_save_warning_logged = false;
    ActivationReason activation_reason = ActivationReason::None;

    bool updateActivation();
    void processGpsAltitude(double altitude, std::chrono::steady_clock::time_point now);
    void loadPersistedState();
    void savePersistedState(std::chrono::steady_clock::time_point now);
    int64_t gpsFailsafeRemainingSeconds(std::chrono::steady_clock::time_point now) const;
    int64_t descentActivationRemainingSeconds(std::chrono::steady_clock::time_point now) const;
    int64_t activationRemainingSeconds(std::chrono::steady_clock::time_point now) const;
    void logPersistenceLoadWarning(const char* message) noexcept;
    void logPersistenceSaveWarning(const char* message) noexcept;
    void logPersistenceLoadWarning(const std::string& message) noexcept;
    void logPersistenceSaveWarning(const std::string& message) noexcept;
    const char* activationReasonText() const;
    bool init();
    bool sendPayload(uint8_t* payload, uint8_t size);
    bool sendTextPayload(const std::string& payload);
    std::string buildTextPayload() const;
};

#endif // _STRATO_OOK_H_
