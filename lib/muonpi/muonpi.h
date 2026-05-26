#ifndef MUONPI_H
#define MUONPI_H

#include <QObject>
#include <QString>
#include <memory>
#include <thread>

#include "network/tcpconnection.h"

#include <boost/asio.hpp>


class QThread;
class QCoreApplication;

class MUONPI : public QObject
{
    Q_OBJECT

public:
    explicit MUONPI(QObject* parent = nullptr);
    ~MUONPI();
    bool getPosition(double* position);
    bool getXOR(double& rate);
    bool getAND(double& rate);
    bool isConnected();

private:
    void makeConnection();
    void decode(const TcpPacket& packet);
    boost::asio::io_context io;
    std::thread thread;
    std::shared_ptr<TcpConnection> clientConn_{nullptr};
    QString ipAddress{"127.0.0.1"};
    quint16 port{51508};
    std::atomic<bool> connectionHealthy = false;

    std::atomic<double> geo_pos[3] = {-999, -999, -999};
    std::atomic<double> gpio_rate[2] = {-999, -999};
};

#endif
