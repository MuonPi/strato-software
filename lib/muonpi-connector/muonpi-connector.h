#ifndef MUONPI_H
#define MUONPI_H

#include <memory>
#include <thread>
#include "network/tcpconnection.h"
#include <boost/asio.hpp>


class MUONPI {
public:
    explicit MUONPI(const std::string ip = "127.0.0.1", std::uint16_t port = 51508);
    ~MUONPI();
    void start();
    bool getPosition(double* position);
    bool getXOR(double& rate);
    bool getAND(double& rate);
    bool isConnected();

private:
    void makeConnection();
    void scheduleReconnect();
    void decode(const TcpPacket& packet);
    boost::asio::io_context io;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard;
    boost::asio::steady_timer reconnectTimer;

    std::string ip_;
    std::uint16_t port_;
    std::thread thread;
    std::shared_ptr<TcpConnection> clientConn_{nullptr};
    std::atomic<bool> connectionHealthy = false;
    std::chrono::seconds reconnectDelay{5};

    std::atomic<double> geo_pos[3] = {-999, -999, -999};
    std::atomic<double> gpio_rate[2] = {-999, -999};
};

#endif
