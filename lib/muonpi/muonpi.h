#ifndef MUONPI_H
#define MUONPI_H

#include <QObject>
#include <memory>
#include <thread>
#include "tcpmessage.h"

class QThread;
class QCoreApplication;
class TcpConnection;
class TcpMessage;

class MUONPI : public QObject
{
    Q_OBJECT

public:
    explicit MUONPI(QObject* parent = nullptr);
    ~MUONPI();

    void start();
    void stop();

    bool getPosition(double* position);
    bool getXOR(double& rate);
    bool getAND(double& rate);
    bool isConnected();

private slots:
    void receivedTcpMessage(TcpMessage tcpMessage);

private:
    QThread* m_tcpThread = nullptr;
    TcpConnection* m_tcpConnection = nullptr;
    void setup();
    std::atomic<bool> connectionHealthy = false;

    std::atomic<double> geo_pos[3] = {-1, -1, -1};
    std::atomic<double> gpio_rate[2] = {-1, -1};
};

#endif
