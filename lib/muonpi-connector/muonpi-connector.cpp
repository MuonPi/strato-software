
#include "muonpi-connector.h"

#include "network/tcpconnection.h"
#include "network/tcpmessage_keys.h"
#include "data/events/ubx_event.h"
#include "data/events/gpio_rate_event.h"
#include "capnp/capnp_codec.h"
#include <boost/asio.hpp>
#include <iostream>

constexpr unsigned timeout_ms = 1000;

MUONPI::MUONPI(const std::string ip, std::uint16_t port)
: io{}, workGuard(boost::asio::make_work_guard(io)), reconnectTimer{io},
ip_{ip}, port_{port},
thread{[&]() { io.run(); }}
{
    start();
}

MUONPI::~MUONPI()
{
    workGuard.reset();
    io.stop();

    if(thread.joinable()) {
        thread.join();
    }
}

void MUONPI::start()
{
    boost::asio::post(io,
        [this]
        {
            makeConnection();
        });
}

void MUONPI::makeConnection()
{
    boost::system::error_code ec;

    auto socket = std::make_shared<tcp::socket>(io);

    auto server_ip =
        boost::asio::ip::make_address_v4(ip_, ec);

    if (ec)
    {
        scheduleReconnect();
        return;
    }

    tcp::endpoint endpoint(server_ip, port_);

    socket->connect(endpoint, ec);

    if (ec)
    {
        std::cerr << "Connect failed:" << ec.message();
        scheduleReconnect();
        return;
    }

    clientConn_ =
        std::make_shared<TcpConnection>(std::move(*socket));

    auto weakConn = std::weak_ptr<TcpConnection>(clientConn_);

    clientConn_->setDisconnectHandler(
        [this](const boost::system::error_code& code)
        {
            std::cerr << "Disconnected:"
                       << code.message();

            clientConn_.reset();

            scheduleReconnect();
        });

    clientConn_->setPacketHandler(
        [weakConn, this](const TcpPacket& packet)
        {
            if (auto conn = weakConn.lock())
            {
                if (static_cast<TCP_MSG_KEY>(packet.key) ==
                    TCP_MSG_KEY::MSG_PING)
                {
                    conn->sendPacket(
                        static_cast<std::uint16_t>(TCP_MSG_KEY::MSG_PONG),
                        packet.payload);
                    return;
                }

                decode(packet);
            }
        });
    connectionHealthy = true;
    clientConn_->start();
}

void MUONPI::scheduleReconnect()
{
    connectionHealthy = false;
    reconnectTimer.expires_after(reconnectDelay);

    reconnectTimer.async_wait(
        [this](const boost::system::error_code& ec)
        {
            if (!ec)
            {
                makeConnection();
            }
        });
}

void MUONPI::decode(const TcpPacket& packet) {
    TCP_MSG_KEY msgID = static_cast<TCP_MSG_KEY>(packet.key);

    if (msgID == TCP_MSG_KEY::MSG_GEO_POS)
    {
        auto pos = CapnpCodec<GnssPosStruct>::decode(packet.payload);
        geo_pos[0] = static_cast<double>(pos.lat) * 1e-7;
        geo_pos[1] = static_cast<double>(pos.lon) * 1e-7;
        geo_pos[2] = static_cast<double>(pos.height) / 1e3;
        // std::cout << "MSG_GEO_POS: " << geo_pos[0] << " " << geo_pos[1] << " " << geo_pos[2] << std::endl;
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_UBX_NAVSTATUS)
    {
        auto event = CapnpCodec<NavStatus>::decode(packet.payload);
        // emit ubxUptimeReceived(event.msss / 1000);
        // emit gpsFixReceived(event.gpsFix);
        return;
    }
    else if (msgID == TCP_MSG_KEY::MSG_GPIO_RATE)
    {
        auto event = CapnpCodec<GpioRateEvent>::decode(packet.payload);
        // gpio_rate[event.whichRate] = event.rate.at(0); // Fix
        // std::cout << "Rate " << (whichRate == 0 ? "XOR" : "AND") << " " << averageValue << std::endl;
        return;
    }
}

bool MUONPI::getPosition(double* position)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        position[i] = geo_pos[i];
        geo_pos[i] = -999;
    }
    return true;
}

bool MUONPI::getXOR(double& rate)
{
    rate = gpio_rate[0];
    gpio_rate[0] = -999;
    return true;
}

bool MUONPI::getAND(double& rate)
{
    rate = gpio_rate[1];
    gpio_rate[1] = -999;
    return true;
}

bool MUONPI::isConnected()
{
    return connectionHealthy;
}