#include <chrono>
#include <cstdint>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "CayenneLPP.h"
#include "ook/ook_transmitter.h"
#include "strato-config.h"
#include "strato-ook.h"

namespace
{
constexpr int OOK_STATE_VERSION = 1;

struct PersistedOokState
{
    bool version_seen = false;
    int version = 0;
    bool gps_seen = false;
    bool flight_armed = false;
    bool descent_below_threshold_active = false;
    int activation_reason = 0;
    double base_altitude = 0.0;
    std::optional<int64_t> gps_failsafe_remaining_s;
    std::optional<int64_t> descent_activation_remaining_s;
};

bool isValidGpsPosition(double latitude, double longitude, double altitude, uint8_t gps_fix)
{
    return gps_fix == OOK_GPS_FIX_3D
        && std::isfinite(latitude)
        && std::isfinite(longitude)
        && std::isfinite(altitude)
        && latitude >= -90.0
        && latitude <= 90.0
        && longitude >= -180.0
        && longitude <= 180.0
        && altitude > -1000.0
        && altitude < 100000.0;
}

bool parseBoolValue(const std::string& value)
{
    return value == "1" || value == "true";
}

int64_t clampRemainingSeconds(int64_t remaining, int64_t maximum)
{
    if(remaining < 0)
        return 0;
    if(remaining > maximum)
        return maximum;
    return remaining;
}

std::chrono::steady_clock::time_point referenceTimeFromRemaining(
    std::chrono::steady_clock::time_point now,
    std::chrono::seconds total,
    int64_t remaining_seconds)
{
    const auto remaining = std::chrono::seconds(
        clampRemainingSeconds(remaining_seconds, total.count()));
    return now - (total - remaining);
}
}

Ook::Ook(Globals& globals)
    : StratoGlobals(globals),
      activation_starttime(std::chrono::steady_clock::now()),
      last_gps_time(activation_starttime),
      descent_below_threshold_starttime(activation_starttime)
{
    loadPersistedState();
}

Ook::~Ook()
{}

bool Ook::execute()
{
    try
    {
        if(!activated && !updateActivation())
            return false;
        activated = true;

        active = true;
        starttime = std::chrono::steady_clock::now();

        if(inited == false)
        {
            inited = init();
            if(inited == false)
            {
                active = false;
                return false;
            }
        }

#if OOK_PAYLOAD_FORMAT == OOK_PAYLOAD_FORMAT_TEXT
        if(!sendTextPayload(buildTextPayload()))
#elif OOK_PAYLOAD_FORMAT == OOK_PAYLOAD_FORMAT_CAYENNE_LPP
        CayenneLPP StratoPayload(255);

        StratoPayload.reset();
        StratoPayload.addGPS(2, static_cast<float>(StratoGlobals.position[0]), static_cast<float>(StratoGlobals.position[1]), static_cast<float>(StratoGlobals.position[2]));
        StratoPayload.addAnalogInput(3, static_cast<float>(StratoGlobals.battery_voltage_mean));

        if(!sendPayload(StratoPayload.getBuffer(), StratoPayload.getSize()))
#else
#error "Unsupported OOK_PAYLOAD_FORMAT"
#endif
        {
            inited = false;
            active = false;
            return false;
        }

        active = false;
        return true;
    }
    catch(...)
    {
        std::cerr << "OOK execute failed" << std::endl;
        inited = false;
        active = false;
        return false;
    }
}

bool Ook::updateActivation()
{
    if(activation_reason != ActivationReason::None)
        return true;

    const auto now = std::chrono::steady_clock::now();
    const uint64_t position_update_count = StratoGlobals.position_update_count.load();

    if(position_update_count != seen_position_update_count)
    {
        seen_position_update_count = position_update_count;

        const double latitude = static_cast<double>(StratoGlobals.position[0]);
        const double longitude = static_cast<double>(StratoGlobals.position[1]);
        const double altitude = static_cast<double>(StratoGlobals.position[2]);
        const uint8_t gps_fix = StratoGlobals.gps_fix.load();

        if(isValidGpsPosition(latitude, longitude, altitude, gps_fix))
        {
            last_gps_time = now;
            processGpsAltitude(altitude, now);
        }
    }

    const auto gps_reference_time = gps_seen ? last_gps_time : activation_starttime;
    if(now - gps_reference_time >= std::chrono::seconds(OOK_GPS_FAILSAFE_AFTER))
    {
        activation_reason = ActivationReason::GpsFailsafe;
    }

    if(flight_armed
        && descent_below_threshold_active
        && now - descent_below_threshold_starttime >= std::chrono::seconds(OOK_DESCENT_ACTIVATION_DELAY))
    {
        activation_reason = ActivationReason::PostDescentDelay;
    }

    if(activation_reason == ActivationReason::None)
    {
        savePersistedState(now);
        return false;
    }

    std::cout << "OOK sender activated: " << activationReasonText() << std::endl;
    savePersistedState(now);
    return true;
}

void Ook::processGpsAltitude(double altitude, std::chrono::steady_clock::time_point now)
{
    if(!gps_seen)
    {
        gps_seen = true;
        base_altitude = altitude;
        return;
    }

    if(!flight_armed && altitude < base_altitude)
    {
        base_altitude = altitude;
    }

    const double altitude_above_base = altitude - base_altitude;

    if(!flight_armed)
    {
        if(altitude_above_base >= OOK_FLIGHT_ARM_ALTITUDE_GAIN_M)
        {
            flight_armed = true;
            descent_below_threshold_active = false;
            std::cout << "OOK sender armed at "
                      << altitude_above_base
                      << " m above base altitude" << std::endl;
        }
        return;
    }

    if(altitude_above_base >= OOK_FLIGHT_ARM_ALTITUDE_GAIN_M)
    {
        descent_below_threshold_active = false;
        return;
    }

    if(!descent_below_threshold_active)
    {
        descent_below_threshold_active = true;
        descent_below_threshold_starttime = now;
    }
}

void Ook::loadPersistedState()
{
    try
    {
        const std::filesystem::path state_path(OOK_STATE_FILE);
        std::error_code ec;
        const bool state_file_exists = std::filesystem::exists(state_path, ec);
        if(ec)
        {
            logPersistenceLoadWarning("could not check " + state_path.string() + ": " + ec.message());
            return;
        }

        if(!state_file_exists)
            return;

        std::ifstream state_file(state_path);
        if(!state_file.is_open())
        {
            logPersistenceLoadWarning("could not open " + state_path.string());
            return;
        }

        PersistedOokState state{};
        std::string line;
        while(std::getline(state_file, line))
        {
            const auto separator = line.find('=');
            if(separator == std::string::npos)
                continue;

            const std::string key = line.substr(0, separator);
            const std::string value = line.substr(separator + 1);

            if(key == "version")
            {
                state.version = std::stoi(value);
                state.version_seen = true;
            }
            else if(key == "gps_seen")
                state.gps_seen = parseBoolValue(value);
            else if(key == "flight_armed")
                state.flight_armed = parseBoolValue(value);
            else if(key == "descent_below_threshold_active")
                state.descent_below_threshold_active = parseBoolValue(value);
            else if(key == "activation_reason")
                state.activation_reason = std::stoi(value);
            else if(key == "base_altitude_m")
                state.base_altitude = std::stod(value);
            else if(key == "gps_failsafe_remaining_s")
                state.gps_failsafe_remaining_s = std::stoll(value);
            else if(key == "descent_activation_remaining_s")
                state.descent_activation_remaining_s = std::stoll(value);
        }

        if(!state.version_seen || state.version != OOK_STATE_VERSION)
        {
            logPersistenceLoadWarning("ignored unsupported OOK state file version");
            return;
        }

        if(!std::isfinite(state.base_altitude) || state.base_altitude < -1000.0 || state.base_altitude > 100000.0)
        {
            logPersistenceLoadWarning("ignored OOK state file with invalid base altitude");
            return;
        }

        gps_seen = state.gps_seen;
        flight_armed = state.flight_armed;
        descent_below_threshold_active = state.descent_below_threshold_active;
        base_altitude = state.base_altitude;

        if(descent_below_threshold_active)
            flight_armed = true;
        if(flight_armed)
            gps_seen = true;

        if(state.activation_reason == static_cast<int>(ActivationReason::PostDescentDelay))
            activation_reason = ActivationReason::PostDescentDelay;
        else if(state.activation_reason == static_cast<int>(ActivationReason::GpsFailsafe))
            activation_reason = ActivationReason::GpsFailsafe;
        else
            activation_reason = ActivationReason::None;

        const auto now = std::chrono::steady_clock::now();

        if(state.gps_failsafe_remaining_s.has_value())
        {
            const auto gps_reference_time = referenceTimeFromRemaining(
                now,
                std::chrono::seconds(OOK_GPS_FAILSAFE_AFTER),
                state.gps_failsafe_remaining_s.value());

            if(gps_seen)
                last_gps_time = gps_reference_time;
            else
                activation_starttime = gps_reference_time;
        }

        if(descent_below_threshold_active && state.descent_activation_remaining_s.has_value())
        {
            descent_below_threshold_starttime = referenceTimeFromRemaining(
                now,
                std::chrono::seconds(OOK_DESCENT_ACTIVATION_DELAY),
                state.descent_activation_remaining_s.value());
        }

        std::cout << "OOK state restored from " << state_path.string()
                  << "; remaining activation time "
                  << activationRemainingSeconds(now) << " s" << std::endl;
    }
    catch(const std::exception& exception)
    {
        logPersistenceLoadWarning(exception.what());
    }
    catch(...)
    {
        logPersistenceLoadWarning("could not restore OOK state: unknown error");
    }
}

void Ook::savePersistedState(std::chrono::steady_clock::time_point now)
{
    try
    {
        const std::filesystem::path state_path(OOK_STATE_FILE);
        const std::filesystem::path parent_path = state_path.parent_path();
        std::error_code ec;

        if(!parent_path.empty())
        {
            std::filesystem::create_directories(parent_path, ec);
            if(ec)
            {
                logPersistenceSaveWarning("could not create " + parent_path.string() + ": " + ec.message());
                return;
            }
        }

        std::filesystem::path temp_path = state_path;
        temp_path += ".tmp";

        std::ofstream state_file(temp_path, std::ios::trunc);
        if(!state_file.is_open())
        {
            logPersistenceSaveWarning("could not open " + temp_path.string());
            return;
        }

        const int64_t descent_remaining_s = descent_below_threshold_active
            ? descentActivationRemainingSeconds(now)
            : -1;

        state_file << "version=" << OOK_STATE_VERSION << '\n'
                   << "gps_seen=" << (gps_seen ? 1 : 0) << '\n'
                   << "flight_armed=" << (flight_armed ? 1 : 0) << '\n'
                   << "descent_below_threshold_active=" << (descent_below_threshold_active ? 1 : 0) << '\n'
                   << "activation_reason=" << static_cast<int>(activation_reason) << '\n'
                   << "base_altitude_m=" << base_altitude << '\n'
                   << "activation_remaining_s=" << activationRemainingSeconds(now) << '\n'
                   << "gps_failsafe_remaining_s=" << gpsFailsafeRemainingSeconds(now) << '\n'
                   << "descent_activation_remaining_s=" << descent_remaining_s << '\n';

        state_file.close();
        if(!state_file)
        {
            logPersistenceSaveWarning("could not write " + temp_path.string());
            return;
        }

        std::filesystem::rename(temp_path, state_path, ec);
        if(ec)
        {
            std::error_code remove_ec;
            std::filesystem::remove(temp_path, remove_ec);
            logPersistenceSaveWarning("could not replace " + state_path.string() + ": " + ec.message());
            return;
        }

        persistence_save_warning_logged = false;
    }
    catch(const std::exception& exception)
    {
        logPersistenceSaveWarning(exception.what());
    }
    catch(...)
    {
        logPersistenceSaveWarning("could not persist OOK state: unknown error");
    }
}

int64_t Ook::gpsFailsafeRemainingSeconds(std::chrono::steady_clock::time_point now) const
{
    if(activation_reason != ActivationReason::None)
        return 0;

    const auto gps_reference_time = gps_seen ? last_gps_time : activation_starttime;
    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - gps_reference_time);
    const auto total = std::chrono::seconds(OOK_GPS_FAILSAFE_AFTER);

    if(elapsed >= total)
        return 0;

    return (total - elapsed).count();
}

int64_t Ook::descentActivationRemainingSeconds(std::chrono::steady_clock::time_point now) const
{
    if(activation_reason != ActivationReason::None)
        return 0;

    if(!flight_armed || !descent_below_threshold_active)
        return OOK_DESCENT_ACTIVATION_DELAY;

    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - descent_below_threshold_starttime);
    const auto total = std::chrono::seconds(OOK_DESCENT_ACTIVATION_DELAY);

    if(elapsed >= total)
        return 0;

    return (total - elapsed).count();
}

int64_t Ook::activationRemainingSeconds(std::chrono::steady_clock::time_point now) const
{
    if(activation_reason != ActivationReason::None)
        return 0;

    if(flight_armed && descent_below_threshold_active)
        return descentActivationRemainingSeconds(now);

    return gpsFailsafeRemainingSeconds(now);
}

void Ook::logPersistenceLoadWarning(const char* message) noexcept
{
    try
    {
        if(persistence_load_warning_logged)
            return;

        std::cerr << "OOK state persistence load warning: " << (message != nullptr ? message : "")
                  << "; continuing without persisted state" << std::endl;
        persistence_load_warning_logged = true;
    }
    catch(...)
    {
        persistence_load_warning_logged = true;
    }
}

void Ook::logPersistenceSaveWarning(const char* message) noexcept
{
    try
    {
        if(persistence_save_warning_logged)
            return;

        std::cerr << "OOK state persistence save warning: " << (message != nullptr ? message : "")
                  << "; continuing with in-memory OOK state" << std::endl;
        persistence_save_warning_logged = true;
    }
    catch(...)
    {
        persistence_save_warning_logged = true;
    }
}

void Ook::logPersistenceLoadWarning(const std::string& message) noexcept
{
    logPersistenceLoadWarning(message.c_str());
}

void Ook::logPersistenceSaveWarning(const std::string& message) noexcept
{
    logPersistenceSaveWarning(message.c_str());
}

const char* Ook::activationReasonText() const
{
    switch(activation_reason)
    {
        case ActivationReason::PostDescentDelay:
            return "post-descent delay elapsed";
        case ActivationReason::GpsFailsafe:
            return "GPS unavailable/stale failsafe interval elapsed";
        case ActivationReason::None:
            break;
    }

    return "not activated";
}

bool Ook::init()
{
    MuonPi::Ook::Transmitter::Config config{};
    config.gpio = OOK_GPIO_PIN;
    config.halfBitPeriod = std::chrono::microseconds(OOK_HALF_BIT_US);
    config.repeatGap = std::chrono::milliseconds(OOK_REPEAT_GAP_MS);
    config.repeats = OOK_REPEATS;

    transmitter = std::make_unique<MuonPi::Ook::Transmitter>(config);
    if(!transmitter->open())
    {
        transmitter.reset();
        std::cerr << "Could not init OOK" << std::endl;
        return false;
    }

    std::cout << "OOK inited" << std::endl;
    return true;
}

bool Ook::sendPayload(uint8_t* payload, uint8_t size)
{
    if(transmitter == nullptr)
    {
        std::cerr << "Could not send OOK payload" << std::endl;
        return false;
    }

    std::vector<std::uint8_t> payload_bytes(payload, payload + size);
    if(!transmitter->sendBytes(payload_bytes))
    {
        std::cerr << "Could not send OOK payload" << std::endl;
        return false;
    }

    return true;
}

bool Ook::sendTextPayload(const std::string& payload)
{
    if(transmitter == nullptr)
    {
        std::cerr << "Could not send OOK text payload" << std::endl;
        return false;
    }

    if(!transmitter->sendString(payload))
    {
        std::cerr << "Could not send OOK text payload" << std::endl;
        return false;
    }

    return true;
}

std::string Ook::buildTextPayload() const
{
    std::ostringstream payload;
    payload << "lat=" << static_cast<double>(StratoGlobals.position[0])
            << ";lon=" << static_cast<double>(StratoGlobals.position[1])
            << ";alt=" << static_cast<double>(StratoGlobals.position[2])
            << ";v=" << static_cast<double>(StratoGlobals.battery_voltage_mean);
    return payload.str();
}
